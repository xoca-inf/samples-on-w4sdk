#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

class Cell
{
public:
    enum class WALL{TOP, RIGHT, BOTTOM, LEFT}; //Clock ordered

    Cell(std::vector<std::vector<Cell>>& grid, size_t xPos, size_t yPos): m_grid(grid), m_pos(xPos, yPos) {}
    void setVisited()
    {
        m_visited = true;
    }

    bool isVisited() const
    {
        return m_visited;
    }

    bool hasWall(WALL wall) const
    {
        return m_walls[static_cast<size_t>(wall)];
    }

    Cell* checkNeighbors()
    {
        std::vector<Cell*> neighbors;

        if (m_pos.y > 0)
        {
            auto& top = m_grid[m_pos.x][m_pos.y - 1];
            if (!top.isVisited())
            {
                neighbors.push_back(&top);
            }
        }
        if (m_pos.x < m_grid.size() - 1)
        {
            auto& right = m_grid[m_pos.x + 1][m_pos.y];
            if (!right.isVisited())
            {
                neighbors.push_back(&right);
            }
        }
        if (m_pos.y < m_grid[0].size() - 1)
        {
            auto& bottom = m_grid[m_pos.x][m_pos.y + 1];
            if (!bottom.isVisited())
            {
                neighbors.push_back(&bottom);
            }
        }
        if (m_pos.x > 0)
        {
            auto& left = m_grid[m_pos.x - 1][m_pos.y];
            if (!left.isVisited())
            {
                neighbors.push_back(&left);
            }
        }

        if (!neighbors.empty())
        {
            return neighbors[std::rand() % neighbors.size()];
        }
        return nullptr;
    }

    void removeWall(Cell& otherCell)
    {
        auto& otherCellPos = otherCell.m_pos;
        if(m_pos.x > otherCellPos.x)
        {
            m_walls[3] = false;
            otherCell.m_walls[1] = false;
        }
        else if (m_pos.x < otherCellPos.x)
        {
            m_walls[1] = false;
            otherCell.m_walls[3] = false;
        }
        else if (m_pos.y > otherCellPos.y)
        {
            m_walls[0] = false;
            otherCell.m_walls[2] = false;
        }
        else
        {
            m_walls[2] = false;
            otherCell.m_walls[0] = false;
        }
    }
private:
    std::vector<std::vector<Cell>>& m_grid;
    point m_pos;
    bool m_walls[4] = {true, true, true, true};
    bool m_visited = false;
};

class MazeDemo : public IGame
{
public:

    void onConfig() override
    {}

    sptr<Mesh> createMaze(float width, float height, float corridorWidth, float wallWidth, float ballRadius)
    {
        size_t xSize = static_cast<size_t>((width - wallWidth) / (wallWidth + corridorWidth) - 1);
        size_t ySize = static_cast<size_t>((height - wallWidth) / (wallWidth + corridorWidth) - 1);

        float correctedWidth = (xSize + 1) * (wallWidth + corridorWidth) + wallWidth;
        float correctedHeight = (ySize + 1) * (wallWidth + corridorWidth) + wallWidth;

        auto maze = Mesh::create::cube({correctedWidth, correctedHeight, wallWidth});
        maze->addComponent<physics::PhysicsComponent>().setup(m_phisSim, physics::Body::BodyType::Animated, physics::PhysicsGeometry::AutoCube);

        std::vector<std::vector<Cell>> m_cells;

        for(size_t i = 0; i < xSize; ++i)
        {
            m_cells.emplace_back();
            auto& row = m_cells.back();
            for(size_t j = 0; j < ySize; ++j)
            {
                row.emplace_back(m_cells, i, j);
            }
        }

        Cell* current = &m_cells[0][0];
        std::queue<Cell*> stack;
        while(true)
        {
            current->setVisited();
            auto next = current->checkNeighbors();
            if (next)
            {
                next->setVisited();
                stack.push(current);
                current->removeWall(*next);
                current = next;
            }
            else if (!stack.empty())
            {
                current = stack.front();
                stack.pop();
            }
            else
            {
                break;
            }
        }

        for(size_t x = 0; x <= xSize; ++x)
        {
            for (size_t y = 0; y < ySize; ++y)
            {
                if(m_cells[(x == xSize) ? x - 1 : x][y].hasWall((x == xSize) ? Cell::WALL::RIGHT : Cell::WALL::LEFT))
                {
                    auto startY = y;
                    do
                    {
                        if (y == ySize - 1)
                            break;
                        ++y;
                    } while (m_cells[(x == xSize) ? x - 1 : x][y].hasWall(
                            (x == xSize) ? Cell::WALL::RIGHT : Cell::WALL::LEFT));
                    auto endY = y - 1;

                    auto wall = Mesh::create::cube({wallWidth,
                                                    (endY - startY + 1) * (corridorWidth + wallWidth) + wallWidth,
                                                    ballRadius * 2});
                    wall->setLocalTranslation({(x + .5f) * (corridorWidth + wallWidth) + wallWidth - correctedWidth / 2,
                                               ((endY + startY) / 2.f + 1) * (corridorWidth + wallWidth) + wallWidth - correctedHeight / 2,
                                               -wallWidth / 2 - ballRadius});

                    wall->addComponent<physics::PhysicsComponent>().setup(m_phisSim, physics::Body::BodyType::Animated, physics::PhysicsGeometry::AutoCube);

                    maze->addChild(wall, false);
                }
            }
        }

        for(size_t y = 0; y <= ySize; ++y)
        {
            for (size_t x = 0; x < xSize; ++x)
            {
                if(m_cells[x][(y == ySize) ? y - 1 : y].hasWall((y == ySize) ? Cell::WALL::BOTTOM : Cell::WALL::TOP))
                {
                    auto startX = x;
                    do
                    {
                        if(x == xSize - 1)
                            break;
                        ++x;
                    }while (m_cells[x][(y == ySize) ? y - 1 : y].hasWall((y == ySize) ? Cell::WALL::BOTTOM : Cell::WALL::TOP));
                    auto endX = x - 1;

                    auto wall = Mesh::create::cube({(endX - startX + 1) * (corridorWidth + wallWidth) + wallWidth,
                                                    wallWidth,
                                                    ballRadius * 2});
                    wall->setLocalTranslation(
                            {((endX + startX) / 2.f + 1) * (corridorWidth + wallWidth) + wallWidth - correctedWidth / 2,
                             (y + .5f) * (corridorWidth + wallWidth) + wallWidth - correctedHeight / 2,
                             -wallWidth / 2 - ballRadius});

                    wall->addComponent<physics::PhysicsComponent>().setup(m_phisSim, physics::Body::BodyType::Animated, physics::PhysicsGeometry::AutoCube);

                    maze->addChild(wall, false);
                }
            }
        }

        auto sphere = Mesh::create::sphere(ballRadius, 10, 10);
        maze->addChild(sphere);
        sphere->getMaterialInst()->setParam("baseColor", math::color::random());
        sphere->setWorldTranslation({(corridorWidth + wallWidth) + wallWidth - correctedWidth / 2,
                                     (corridorWidth + wallWidth) + wallWidth * 2 - correctedHeight / 2,
                                     -ballRadius - wallWidth / 2});

        auto &spherePhysicsComponent = sphere->addComponent<physics::PhysicsComponent>();
        spherePhysicsComponent.setup(m_phisSim,
                                                                                           physics::Body::BodyType::Rigid,
                                                                                           physics::PhysicsGeometry::Custom);
        spherePhysicsComponent.addGeometry(make::sptr<physics::SphereGeometry>(ballRadius * 2));
        spherePhysicsComponent.setMass(50.f);

        return maze;
    }

    void onStart() override
    {
        m_phisSim = make::sptr<physics::Simulator>(10, 2000, 0, vec3(0.0f, 0.0f, 9.8f));
        physics::SimulatorCollector::addSimulator(m_phisSim);

        auto root = Render::getRoot();

        std::srand(std::time(nullptr));

        m_maze = createMaze(35, 60, 2, .3f, .75f);
        Render::getRoot()->addChild(m_maze);

// camera tune
        auto cam = Render::getScreenCamera();
        cam->setWorldTranslation({0, 0, -25});
        cam->setFov(80.f);
        cam->setFar(300.f);
        cam->setClearColor({.055f, .231f, .498f, 1.f});
        cam->setClearMask(ClearMask::Color | ClearMask::Depth |ClearMask::Background);
        cam->getBackground()->setMaterial(Material::get("materials/clouds.mat")->createInstance());

        event::Touch::Begin::subscribe(std::bind(&MazeDemo::onTouchBegin, this, std::placeholders::_1));
        event::Touch::Move::subscribe(std::bind(&MazeDemo::onTouchMove, this, std::placeholders::_1));

        setVirtualResolution({1080, 1920});
        auto titleLabel = createWidget<Label>(nullptr, "Click and move to tilt the field", ivec2(540, 1830));
        titleLabel->setHorizontalAlign(HorizontalAlign::Center);
    }

    void onUpdate(float dt) override
    {
    }

    void onTouchBegin(const event::Touch::Begin &evt)
    {
        m_startPoint = evt.point;
    }

    void onTouchMove(const event::Touch::Move &evt)
    {
        const auto& screenSize = Platform::getSize();
        auto vAngle = (static_cast<int>(m_startPoint.y) - static_cast<int>(evt.point.y)) / static_cast<float>(screenSize.h);
        auto hAngle = (static_cast<int>(m_startPoint.x) - static_cast<int>(evt.point.x)) / static_cast<float>(screenSize.w);
        m_maze->setWorldRotation(m_maze->getWorldRotation() + Rotator(vAngle * HALF_PI / 2, hAngle * HALF_PI / 2, 0));
        m_startPoint = evt.point;
    }


#ifndef NDEBUG

    void onKey(const event::Keyboard::Down& evt) override
    {
        static bool shift = true;
        static float fov = Render::getScreenCamera()->getFov();

        switch(evt.key)
        {
            case event::Keyboard::Key::Space:
            {
                Platform::setWindowStyle(shift ? "portrait" : "landscape");
                shift = !shift;
            }
            case event::Keyboard::Key::ArrowLeft:  fov -= 1.0; break;
            case event::Keyboard::Key::ArrowRight: fov += 1.0; break;

            default:
                break;
        }
        Render::getScreenCamera()->setFov(fov);
        W4_LOG_ERROR("FOV: %f", fov);

    }
#endif


private:
    sptr<Mesh> m_maze;

    point m_startPoint;
    sptr<Simulator> m_phisSim;
};

W4_RUN(MazeDemo)
