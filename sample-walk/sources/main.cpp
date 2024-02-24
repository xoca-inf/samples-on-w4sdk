#include "W4Framework.h"
#include <iostream>

W4_USE_UNSTRICT_INTERFACE

struct GridLocation
{
    int x, y;
};

// Helpers for GridLocation

bool operator == (GridLocation a, GridLocation b)
{
    return a.x == b.x && a.y == b.y;
}

bool operator != (GridLocation a, GridLocation b)
{
    return !(a == b);
}

bool operator < (GridLocation a, GridLocation b)
{
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

std::basic_iostream<char>::basic_ostream& operator<<(std::basic_iostream<char>::basic_ostream& out, const GridLocation& loc)
{
    out << '(' << loc.x << ',' << loc.y << ')';
    return out;
}

namespace std {
/* implement hash function so we can put GridLocation into an unordered_set */
template <> struct hash<GridLocation> {
    typedef GridLocation argument_type;
    typedef std::size_t result_type;
    std::size_t operator()(const GridLocation& id) const noexcept {
        return std::hash<int>()(id.x ^ (id.y << 4));
    }
};
}

struct GridWithWeights
{
    static std::array<GridLocation, 4> DIRS;

    int width, height;
    std::unordered_set<GridLocation> blocks;
    std::unordered_set<GridLocation> foes;
    std::unordered_set<GridLocation> gifts;
    std::unordered_set<GridLocation> roads;
    std::unordered_map<GridLocation, double> costs;

    GridWithWeights(int width_, int height_)
        : width(width_), height(height_) {}

    bool in_bounds(GridLocation id) const
    {
        return 0 <= id.x && id.x < width && 0 <= id.y && id.y < height;
    }

    bool passable(GridLocation id) const
    {
        return blocks.find(id) == blocks.end();
    }

    bool isRoad(GridLocation id) const
    {
        return roads.find(id) != roads.end();
    }
    bool isGift(GridLocation id) const
    {
        return gifts.find(id) != gifts.end();
    }
    bool isFoe(GridLocation id) const
    {
        return foes.find(id) != foes.end();
    }

    std::vector<GridLocation> neighbors(GridLocation id) const
    {
        std::vector<GridLocation> results;

        for (GridLocation dir : DIRS)
        {
            GridLocation next{id.x + dir.x, id.y + dir.y};
            if (in_bounds(next) && passable(next))
                results.push_back(next);
        }

        if ((id.x + id.y) % 2 == 0)
            std::reverse(results.begin(), results.end());
        // aesthetic improvement on square grids

        return results;
    }
};

std::array<GridLocation, 4> GridWithWeights::DIRS =
    {
        GridLocation{1, 0}, GridLocation{0, -1},
        GridLocation{-1, 0}, GridLocation{0, 1}
    };

template<typename T, typename priority_t>
struct PriorityQueue
{
    typedef std::pair<priority_t, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>,
        std::greater<PQElement>> elements;

    inline bool empty() const
    {
        return elements.empty();
    }

    inline void put(T item, priority_t priority)
    {
        elements.emplace(priority, item);
    }

    T get()
    {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};

inline double heuristic(GridLocation a, GridLocation b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

void a_star_search
    (GridWithWeights graph,
     GridLocation start,
     GridLocation goal,
     std::unordered_map<GridLocation, GridLocation>& came_from,
     std::unordered_map<GridLocation, double>& cost_so_far)
{
    PriorityQueue<GridLocation, double> frontier;
    frontier.put(start, 0);

    came_from[start] = start;
    cost_so_far[start] = 0;

    while (!frontier.empty())
    {
        GridLocation current = frontier.get();

        if (current == goal)
            break;

        for (GridLocation next : graph.neighbors(current))
        {
            double new_cost = cost_so_far[current] + graph.costs[next];
            if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next])
            {
                cost_so_far[next] = new_cost;
                double priority = new_cost + heuristic(next, goal);
                frontier.put(next, priority);
                came_from[next] = current;
            }
        }
    }
}

std::vector<GridLocation> reconstruct_path(
    GridLocation start, GridLocation goal,
    std::unordered_map<GridLocation, GridLocation> came_from
)
{
    std::vector<GridLocation> path;

    if (came_from.find(goal)!=came_from.end())
    {
        GridLocation current = goal;
        while(current != start)
        {
            path.push_back(current);
            current = came_from[current];
        }
    }
    path.push_back(start); // optional
    std::reverse(path.begin(), path.end());
    return path;
}


struct SampleWalk : public IGame

{
public:
    const int xSize = 20;
    const int ySize = 20;
    const float cubeDim = 50.;
    GridWithWeights generatedMap = GridWithWeights(xSize, ySize);
    GridLocation currentLoca;

    void onStart() override
    {
        setupCamera();
        setupLight();

        generateMap();

        setupFloor();
        setupBuddy();

        gui::createWidget<Label>(nullptr, "TAP ANYWHERE TO GO", ivec2(540, 200));
    }

    void restoreColor(GridLocation cubeLoca)
    {
        auto cube = m_cubes[cubeLoca.x + cubeLoca.y * ySize];
        auto cost = generatedMap.costs[{cubeLoca.x, cubeLoca.y}];
        if (generatedMap.passable(cubeLoca))
        {
            if(generatedMap.isRoad(cubeLoca))
            {
                cube->getMaterialInst()->setTexture(resources::TextureId::TEXTURE_0,
                                                    Texture::get("textures/bricks2.png"));
                cube->getMaterialInst()->setTexture(resources::TextureId::TEXTURE_1,
                                                    Texture::get("textures/bricks2_gray.png"));
            }
            else
            {
                cube->getMaterialInst()->setTexture(resources::TextureId::TEXTURE_0,
                                                    Texture::get(ResourceGenerator(
                                                        ColorTextureGenerator,
                                                        math::vec4(.0,
                                                                   cost, .0,
                                                                   1.))));
            }
        }
        else
        {
            cube->getMaterialInst()->setTexture(resources::TextureId::TEXTURE_0,
                                                Texture::get(ResourceGenerator(ColorTextureGenerator, math::vec4(cost, .0, .0, 1.))));

        }

    }

    void showPath(GridLocation goal)
    {
        for(int y = 0; y < ySize; ++y)
            for(int x = 0; x < xSize; ++x)
                restoreColor({x,y});

        std::unordered_map<GridLocation, GridLocation> came_from;
        std::unordered_map<GridLocation, double> cost_so_far;
        a_star_search(generatedMap, currentLoca, goal, came_from, cost_so_far);

        std::vector<GridLocation> path = reconstruct_path(currentLoca, goal, came_from);

        W4_LOG_INFO("hello we ok with reconstruct");

        m_buddy->traversal([this](Node& node)
                           {
                               if(node.is<SkinnedMesh>())
                               {
                                   node.as<SkinnedMesh>()->play(0);
                               }
                           });

        isMoved = true;

        m_buddyTweenTime = 0.f;
        m_buddyTween = make::sptr<Tween<vec3>>(m_buddy->getWorldTranslation(), 0.f);

        for(auto const& point: path)
        {
            W4_LOG_DEBUG("so the point is %d, %d", point.x, point.y);
            auto cube_in_path = m_cubes[point.x + point.y * ySize];
            cube_in_path->getMaterialInst()->setTexture(resources::TextureId::TEXTURE_0,
                                                        Texture::get(ResourceGenerator(ColorTextureGenerator,
                                                                                       math::vec4(.0, .0,
                                                                                                  generatedMap.costs[{
                                                                                                      point.x,
                                                                                                      point.y}],
                                                                                                  1.))));

            if(point != currentLoca)
            {
                auto destination = cube_in_path->getWorldTranslation();
                destination.y = -cubeDim / 2.f;
                m_buddyTween->add(destination, m_buddyTween->getDuration() + generatedMap.costs[{point.x, point.y}],
                                  easing::Linear<vec3>);
            }

        }

        if (std::find(path.begin(), path.end(), goal) != path.end())
            currentLoca = goal;

    }

    void generateMap()
    {
//        auto def_map = std::array<std::array<uint8_t,20>, 20>{
//            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
//            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
//            1, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1
//            1, 1, 4, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 4, 4, 4, 1, 1
//            1, 1, 4, 4, 4, 4, 4, 3, 3, 2, 2, 3, 3, 3, 2, 2, 2, 4, 1, 1
//            1, 1, 4, 4, 4, 4, 4, 3, 3, 2, 3, 3, 3, 3, 3, 2, 2, 4, 1, 1
//            1, 1, 4, 4, 3, 3, 3, 3, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 1, 1
//            1, 1, 4, 4, 3, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 1, 1
//            1, 1, 4, 4, 3, 3, 3, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 4, 1, 1
//            1, 1, 4, 4, 3, 3, 3, 2, 3, 2, 2, 2, 3, 3, 3, 3, 4, 4, 1, 1
//            1, 1, 4, 4, 3, 3, 2, 2, 3, 3, 3, 2, 2, 2, 3, 3, 4, 4, 1, 1
//            1, 1, 4, 4, 3, 3, 2, 3, 3, 3, 3, 3, 3, 2, 2, 3, 3, 4, 1, 1
//            1, 1, 4, 4, 3, 2, 2, 3, 3, 3, 3, 3, 3, 2, 2, 3, 4, 4, 1, 1
//            1, 1, 4, 4, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 4, 4, 1, 1
//            1, 1, 4, 4, 2, 2, 3, 3, 4, 4, 4, 4, 3, 3, 3, 2, 4, 4, 1, 1
//            1, 1, 4, 4, 2, 2, 3, 3, 4, 4, 4, 4, 3, 3, 3, 3, 4, 4, 1, 1
//            1, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1
//            1, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1
//            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
//            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
//        };

        int giftsHere = 5;
        int foesHere = 3;


        //std::unordered_set<GridLocation> blocks;

        currentLoca = {10,10};

        for(int y = 0; y < ySize; ++y)
            for(int x = 0; x < xSize; ++x)
            {
                auto noise = random<int>(3, 8);

//                if (noise % 3 == 0 && currentLoca!=GridLocation{x,y})
//                    generatedMap.blocks.insert(GridLocation{x, y});

                if (currentLoca!=GridLocation{x,y})
                {
                    if (x < 1 || x > 18 || y < 1 || y > 18)
                        generatedMap.blocks.insert(GridLocation{x, y});
                    else if(noise % 3 == 0 && (x < 4 || x > 15 || y < 4 || y > 15))
                        generatedMap.blocks.insert(GridLocation{x, y});
                    else
                    {
                        if(x == 6 || x == 13 || y == 6 || y == 13)
                        {
                            generatedMap.roads.insert(GridLocation{x, y});
                        }
                        else if(noise % 3 == 0 && x % 2 == 0)
                        {
                            generatedMap.roads.insert(GridLocation{x, y});
                        }
                        else
                        {
                            auto add_noise = random<float>();
                            if (add_noise<.25f)
                                generatedMap.blocks.insert(GridLocation{x, y});
                            else if (add_noise<.30f)
                                generatedMap.gifts.insert(GridLocation{x, y});
                            else if (add_noise<.40f)
                                generatedMap.foes.insert(GridLocation{x, y});

                        }
                    }
                }



                float cost = noise/10.f;
                generatedMap.costs[{x,y}] = cost;
            }

        generatedMap.costs[currentLoca] = .7;
    }

//    void createGift()
//    {
//        auto shapeGift = Mesh::create::cube({cubeDim/2,cubeDim/2,cubeDim/2});
//        shape->addChild(shapeGift);
//        shapeGift->setLocalTranslation({0, cubeDim/2, 0});
//    }

    void setupCamera()
    {
        auto cubemap = Cubemap::fromImages({
                                        resources::Image::get("textures/bluecloud_rt.png"),
                                        resources::Image::get("textures/bluecloud_lf.png"),
                                        resources::Image::get("textures/bluecloud_up.png"),
                                        resources::Image::get("textures/bluecloud_dn.png"),
                                        resources::Image::get("textures/bluecloud_bk.png"),
                                        resources::Image::get("textures/bluecloud_ft.png"),
                                    });

        auto cam = Render::getScreenCamera();
            cam->setFov(55.f);
            cam->setFar(10000.f);
            cam->setNear(5.f);
            cam->setClearColor({0.25,0.25,0.25,1});
            cam->setWorldTranslation({-500.f, 500.f, -500.f});
            cam->rotateLocal(Rotator(PI/3.f, PI/4.f, 0 ));
            cam->setClearMask(ClearMask::Color | ClearMask::Depth | ClearMask::Skybox);
            cam->getSkybox()->setCubemap(cubemap);
    }

    void setupLight()
    {
        auto addLight = [](const vec3& pos, float intensity)
        {
            auto light = make::sptr<PointLight>("light");
                light->setWorldTranslation(pos);
                light->setColor({1.f, 1.f, 1.f});
                light->setDecayRate(LightDecayRate::None);
                light->setIntensity(intensity);
                light->setDebugViewColor({1,0,1,1}).enableDebugView(true);
            Render::getRoot()->addChild(light);
        };

        addLight({cubeDim*xSize, cubeDim*5, 0.f}, 0.225f);
        addLight({-cubeDim*xSize, cubeDim*5, 0.f}, 0.225f);
        addLight({0.f, cubeDim*5, cubeDim*xSize}, 0.225f);
        addLight({0.f, cubeDim*5, -cubeDim*xSize}, 0.225f);

        addLight({0.f, cubeDim*10, 0.f}, 0.325f);
    }

    void setupFloor()
    {
        auto material = Material::get("materials/mixText.mat");

        for(int y = 0; y < ySize; ++y)
            for(int x = 0; x < xSize; ++x)
            {
                auto shape = Mesh::create::cube({cubeDim,cubeDim,cubeDim});
                    Render::getRoot()->addChild(shape);
                    shape->setWorldTranslation({(-xSize / 2.f + x) * cubeDim, -cubeDim, (-ySize / 2.f + y) * cubeDim});
                    shape->setMaterialInst(material->createInstance());
                m_cubes.emplace_back(shape);

                if (generatedMap.passable({x,y}))
                {
                    shape->addCollider<core::AABB>("unnamed", shape)->setScreencastCallback(
                                                  [this, shape, x, y](const CollisionInfo& info)
                                                  {
                                                      if (not isMoved)
                                                          showPath(GridLocation{x, y});
                                                  });

                    if(generatedMap.isGift({x,y}))
                    {
                        auto shapeGift = Mesh::create::cube({cubeDim/2,cubeDim/2,cubeDim/2});
                        shape->addChild(shapeGift);
                        shapeGift->setLocalTranslation({0, cubeDim/2, 0});
                        shapeGift->setMaterialInst(material->createInstance());
                        shapeGift->getMaterialInst()->setTexture(TextureId::TEXTURE_0, Texture::get("textures/acacia_leaves.png"));
                        shapeGift->getMaterialInst()->setTexture(TextureId::TEXTURE_1, Texture::get("textures/cube.png"));
                    }

                    if(generatedMap.isFoe({x,y}))
                    {
                        auto shapeFoe = Mesh::create::cube({cubeDim/2,cubeDim,cubeDim/2});
                        shape->addChild(shapeFoe);
                        shapeFoe->setLocalTranslation({0, cubeDim, 0});
                        shapeFoe->setMaterialInst(material->createInstance());
                        shapeFoe->getMaterialInst()->setTexture(TextureId::TEXTURE_0, Texture::get("textures/acacia_door_bottom.png"));
                        shapeFoe->getMaterialInst()->setTexture(TextureId::TEXTURE_1, Texture::get("textures/acacia_log.png"));
                    }

                }
                else
                {
                    auto pos = shape->getWorldTranslation();
                    pos.y = -cubeDim*generatedMap.costs[{x,y}];
                    shape->setWorldTranslation(pos);
                }

                restoreColor({x,y});
            }
    }

    void setupBuddy()
    {
        m_buddy = Asset::get("SK_PaperBoy_T_Pose.w4a")->getFirstRoot();

        Render::getRoot()->addChild(m_buddy);
        m_buddy->log();
        m_buddy->setWorldTranslation({0.f, -cubeDim/2.f, 0.f});
        m_buddy->rotateLocal(Rotator(0, PI, 0 ));
        m_buddy->traversal([this](Node & node)
                           {
                               if (node.is<SkinnedMesh>())
                               {
                                   node.as<SkinnedMesh>()->getAnimator("Walk").setIsLooped(true);
                                   node.as<SkinnedMesh>()->getAnimator("PlatformerStart").setIsLooped(true);
                                   node.as<SkinnedMesh>()->play(1);
                               }
                           });
    }

    void onUpdate(float dt) override
    {
        static float time = 0; time += dt;
        m_buddyTweenTime += dt;

        if (isMoved)
        {
            m_buddy->setWorldTranslation(m_buddyTween->getValue(m_buddyTweenTime));
            if (m_buddyTweenTime+dt>m_buddyTween->getDuration())
            {
                isMoved = false;
                m_buddy->traversal([this](Node& node)
                                   {
                                       if (node.is<SkinnedMesh>())
                                           node.as<SkinnedMesh>()->play(1);
                                   });
            }
        }
    }

private:
    std::vector<sptr<Mesh>> m_cubes;
    sptr<Node> m_buddy;

    sptr<Tween<vec3>> m_buddyTween;

    float m_buddyTweenTime = 0.0f;
    bool isMoved = false;
};

W4_RUN(SampleWalk)
