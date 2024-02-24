#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

class CubefallDemo : public IGame
{
public:

    void onConfig() override
    {}

    void onStart() override
    {
        auto root = Render::getRoot();

        auto cubeMat = Material::get("materials/cube.mat");
      //  auto cubeMat = Material::getDefault();

        m_materials[0] = cubeMat->createInstance(); m_materials[0]->setParam("mainColor", vec3(0.61, 0.81, 0.27));
        m_materials[1] = cubeMat->createInstance(); m_materials[1]->setParam("mainColor", vec3(0.60, 0.29, 0.94));
        m_materials[2] = cubeMat->createInstance(); m_materials[2]->setParam("mainColor", vec3(0.91, 0.27, 0.57));
        m_materials[3] = cubeMat->createInstance(); m_materials[3]->setParam("mainColor", vec3(0.96, 0.58, 0.3));
        m_selectedMaterial = cubeMat->createInstance(); m_selectedMaterial->setParam("mainColor", vec3(2., 2., 2.));

// camera tune
        auto cam = Render::getScreenCamera();
//            cam->setWorldTranslation({0, 100, -100});
            cam->setWorldRotation(cam->getWorldRotation() + Rotator({1, 0, 0}, PI / 180 * 20));
            cam->setFov(112.f);
            cam->setFar(300.f);
            cam->setClearColor({.055f, .231f, .498f, 1.f});
            cam->setClearMask(ClearMask::Color | ClearMask::Depth |ClearMask::Background);
            cam->getBackground()->setMaterial(Material::get("materials/clouds.mat")->createInstance());

        InitPhysics();
    }

    void InitPhysics()
    {
        m_phisSim = make::sptr<physics::Simulator>(800, 10, 0, vec3(0.0f, -9.8f, 0.0f));
        w4::physics::SimulatorCollector::addSimulator(m_phisSim);

        m_floor = make::sptr<Node>("Floor");
        auto& component = m_floor->addComponent<physics::PhysicsComponent>();
        component.setup(m_phisSim, physics::Body::BodyType::Animated, physics::PhysicsGeometry::Custom);
        auto floorGeometry = make::sptr<physics::CubeGeometry>(100.f, 0.2f, 100.f);
        component.addGeometry(floorGeometry);
        m_floor->setWorldTranslation({0.0f, -8.0f, 0.0f});
    }

    void onUpdate(float dt) override
    {
        constexpr float boxSizeMin = .3f;
        constexpr float boxSizeMax = 1.6f;

        constexpr size_t cubesCount = 750;
        constexpr size_t spawnSimultaneous = 50;

        constexpr float respawnTimeout = .03f;

        if(m_timeFromLastSpawn > respawnTimeout)
        {
            m_timeFromLastSpawn = 0.f;
            if(!m_cubes.empty())
            {
                Render::getRoot()->removeChild(m_cubes.front());
                m_cubes.pop();
            }
            const auto cubes2Spawn = std::min(spawnSimultaneous, cubesCount - m_cubes.size());
            for(auto i = 0; i < cubes2Spawn; ++i)
            {
                const float boxSize = boxSizeMin + std::rand() % 100 * (boxSizeMax - boxSizeMin) / 100.f;

                auto cube = Mesh::create::cube({boxSize - 0.05f, boxSize - 0.05f, boxSize - 0.05f});

                cube->setWorldTranslation({std::rand() % 20 - 10.f, 10, std::rand() % 10 - 10.f});
                Render::getRoot()->addChild(cube);
                cube->setMaterialInst(m_materials[std::rand() % (sizeof m_materials / sizeof(m_materials[0]))]);

                auto& physicsComponent = cube->addComponent<physics::PhysicsComponent>();
                physicsComponent.setup(m_phisSim, w4::physics::Body::BodyType::Rigid, w4::physics::PhysicsGeometry::Custom, 5.0f);
                physicsComponent.addGeometry(make::sptr<physics::CubeGeometry>(boxSize, boxSize, boxSize));

                cube->addCollider<core::AABB>("unnamed", cube)->setScreencastCallback(
                                                          [this](const core::CollisionInfo& info)
                                                          {
                                                              info.target->getParent()->as<Mesh>()->setMaterialInst(m_selectedMaterial);
                                                              info.target->getParent()->getFirstComponent<PhysicsComponent>().setAngularMomentum({25, 0, 0});
                                                          });
                m_cubes.push(cube);
            }
        }
        else
        {
            m_timeFromLastSpawn += dt;
        }
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
    sptr<Node> m_floor;
    std::queue<sptr<Mesh>> m_cubes;
    sptr<MaterialInst> m_materials[4];
    sptr<MaterialInst> m_selectedMaterial;
    sptr<Simulator> m_phisSim;

    float m_timeFromLastSpawn = std::numeric_limits<float>::max();
};

W4_RUN(CubefallDemo)
