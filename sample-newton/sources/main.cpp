#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

// The "Волобуев, вот ваш кий!" game
class W4Newton : public IGame
{
public:

    sptr<Mesh> createField()
    {
        const auto physMat = w4::make::sptr<PhysicsMaterial>(.5f, .7f);

        auto floor = Mesh::create::cube({10, 1, 10});
        floor->setMaterialInst(Material::getDefault()->createInstance());
        floor->setWorldTranslation({0, -.5f, 0});
        floor->getMaterialInst()->setParam("baseColor", vec4(.918f, .816f, .686f, 1.f));
        Render::getRoot()->addChild(floor);

        auto side = Mesh::create::cylinder(8, .5f, 10);
        side->setMaterialInst(Material::getDefault()->createInstance());

        std::tuple<vec3, Rotator, std::string> sidesConfig[] = {{{-4 , 4, -2},{0, 0, 0, 1}, "vert1"},
                                                               {{4, 4, -2},{0, 0, 0, 1}, "vert2"},
                                                               {{-4, 4, 2},{0, 0, 0, 1}, "vert3"},
                                                               {{4, 4, 2},{0, 0, 0, 1}, "vert4"},
                                                               {{0, 8, -2},{0, 0, HALF_PI}, "upper1"},
                                                               {{0, 8, 2},{0, 0, HALF_PI}, "upper2"}};
        for(const auto& val: sidesConfig)
        {
            auto cloneSide = side->clone();
            cloneSide->setName(std::get<2>(val));
            cloneSide->setWorldTranslation(std::get<0>(val));
            cloneSide->setWorldRotation(std::get<1>(val));
            cloneSide->as<Mesh>()->getMaterialInst()->setParam("baseColor", vec4(.537f, .537f, .537f, 1.f));
            floor->addChild(cloneSide);
            auto& cubePhysicsComponent = cloneSide->addComponent<physics::PhysicsComponent>();
            cubePhysicsComponent.setup(m_phisSim, physics::Body::BodyType::Animated, physics::PhysicsGeometry::AutoCapsule);
        }

        auto corner = Mesh::create::sphere(.5f, 10, 10);
        corner->setMaterialInst(Material::getDefault()->createInstance());

        vec3 cornersConfig[] = {{-4 , 8, -2},
                               {-4, 8, 2},
                               {4, 8, -2},
                               {4, 8, 2}};
        for(const auto& val: cornersConfig)
        {
            auto cloneSide = corner->clone();
            cloneSide->setWorldTranslation(val);
            cloneSide->as<Mesh>()->getMaterialInst()->setParam("baseColor", vec4(.537f, .537f, .537f, 1.f));
            floor->addChild(cloneSide);
        }

        return floor;
    }

    void onConfig() override
    {
        Game::Config.EnableFrustumCulling = false;
    }
    void onStart() override
    {
        m_phisSim = make::sptr<physics::Simulator>(1000, 1000, 20, vec3(0.0f, -9.8f, 0.0f));
        w4::physics::SimulatorCollector::addSimulator(m_phisSim);

        auto cam = Render::getScreenCamera();
             cam->setWorldTranslation({0.f, 15, -15.f});
             cam->setWorldRotation({HALF_PI / 2, 0 ,0});
             cam->setFov(60.f);
             cam->setAspect(9.f/16.f);
             cam->setClearMask(ClearMask::Color | ClearMask::Depth |ClearMask::Background);
             cam->getBackground()->setMaterial(Material::get("materials/clouds.mat")->createInstance());

        const auto field = createField();

        constexpr float ballRadius = .8f;

        const auto physMat = make::sptr<PhysicsMaterial>(.5f, 1.1f);
        for(int i = -2; i < 3; ++i)
        {
            auto sphere = Mesh::create::sphere(ballRadius, 10, 10);
            sphere->setWorldTranslation({i * ballRadius * 2, 2, 0});
            Render::getRoot()->addChild(sphere);
            sphere->getMaterialInst()->setParam("baseColor", math::color::random());

            auto &spherePhysicsComponent = sphere->addComponent<physics::PhysicsComponent>();
            spherePhysicsComponent.setup(m_phisSim,
                                                                                               physics::Body::BodyType::Rigid,
                                                                                               physics::PhysicsGeometry::Custom);
            spherePhysicsComponent.addGeometry(make::sptr<SphereGeometry>(ballRadius * 2));


            spherePhysicsComponent.setMass(.5f);
            spherePhysicsComponent.setMaterial(physMat);

            const auto& spherePos = sphere->getWorldTranslation();

            vec3 joint1Pos = {spherePos.x, 8, -2};
            auto joint1 = make::sptr<physics::BallSocketJoint>(m_phisSim,
                                                           spherePhysicsComponent,
                                                           field->getChild<render::Mesh>("upper1")->getFirstComponent<PhysicsComponent>(),
                                                           joint1Pos);
            m_joints.push_back(joint1);

            vec3 joint2Pos = {spherePos.x, 8, 2};
            auto joint2 = make::sptr<physics::BallSocketJoint>(m_phisSim,
                                                               spherePhysicsComponent,
                                                               field->getChild<render::Mesh>("upper2")->getFirstComponent<PhysicsComponent>(),
                                                               joint2Pos);
            m_joints.push_back(joint2);

            const auto plotter1 = make::sptr<Plotter>("rope" + std::to_string(i + 3) + "_1");
            const auto plotter2 = make::sptr<Plotter>("rope" + std::to_string(i + 3) + "_2");
            m_ropes.emplace_back(sphere, joint1Pos + vec3(0, 0, .5f), plotter1, joint2Pos - vec3(0, 0, .5f), plotter2);
            Render::getRoot()->addChild(plotter1);
            Render::getRoot()->addChild(plotter2);

            if(!m_sphere)
            {
                m_sphere = sphere;
            }
        }

        m_sphere->getFirstComponent<physics::PhysicsComponent>().setVelocity({-7, 0, 0});

        event::Touch::Begin::subscribe(std::bind(&W4Newton::onTouchBegin, this, std::placeholders::_1));
        event::Touch::Move::subscribe(std::bind(&W4Newton::onTouchMove, this, std::placeholders::_1));
        event::Touch::End::subscribe(std::bind(&W4Newton::onTouchEnd, this, std::placeholders::_1));
    }

    void onTouchBegin(const event::Touch::Begin &evt)
    {
        const Plain plain({0, 0, 0}, {0, 1, 0});
        const auto ray = Render::createRayFromScreen(evt.point);
        m_touchBegin = plain.vectorIntersect(ray.origin, ray.direction);
    }

    void onTouchMove(const event::Touch::Move &evt)
    {
        const Plain plain({0, 0, 0}, {0, 1, 0});
        const auto ray = Render::createRayFromScreen(evt.point);
        const auto touchMoved = plain.vectorIntersect(ray.origin, ray.direction);

        if(!m_plotter)
        {

        }
        const auto& spherePos = m_sphere->getWorldTranslation();
        std::vector<LinesVertexFormat> vertices = {{spherePos, {0,1,0,1}},
                                                   {spherePos + (touchMoved - m_touchBegin), {1, 0, 0, 1}}};
        std::vector<uint32_t> indices = {1, 0};
        m_plotter->setLines(vertices, indices);
    }

    void onTouchEnd(const event::Touch::End &evt)
    {
        Render::getRoot()->removeChild(m_plotter);
        m_plotter.reset();
        const Plain plain({0, 0, 0}, {0, 1, 0});
        const auto ray = Render::createRayFromScreen(evt.point);
        const auto touchEnd = plain.vectorIntersect(ray.origin, ray.direction);
        m_sphere->getFirstComponent<physics::PhysicsComponent>().setVelocity(m_touchBegin - touchEnd);
    }

    void onUpdate(float dt) override
    {
        const std::vector<uint32_t> indices = {1, 0};
        for(const auto& val: m_ropes)
        {
            const auto& spherePos = std::get<0>(val)->getWorldTranslation();
            std::vector<LinesVertexFormat> vertices1 = {{spherePos, {0,0,0,1}},
                                                        {std::get<1>(val), {0, 0, 0, 1}}};
            std::get<2>(val)->setLines(vertices1, indices);
            std::vector<LinesVertexFormat> vertices2 = {{spherePos, {0,0,0,1}},
                                                        {std::get<3>(val), {0, 0, 0, 1}}};
            std::get<4>(val)->setLines(vertices2, indices);
        }
    }
private:
    sptr<physics::Simulator> m_phisSim;
    sptr<Mesh> m_sphere;
    sptr<Plotter> m_plotter;
    std::vector<std::tuple<sptr<Mesh>, vec3, sptr<Plotter>, vec3, sptr<Plotter>>> m_ropes;
    vec3 m_touchBegin;

    std::vector<physics::BallSocketJoint::sptr> m_joints;
};
W4_RUN(W4Newton)
