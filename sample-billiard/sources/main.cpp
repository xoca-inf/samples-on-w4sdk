#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

// The "Волобуев, вот ваш кий!" game
class W4Billiards : public IGame
{
public:

    sptr<Mesh> createRounding(float innerRadius, float outerRadius, float height, size_t boxCount, float angleStart, float angleFinish)
    {
        static size_t counter = 0;
        ++counter;
        auto indicesBuf = make::sptr<UserIndicesBuffer>("Indices" + std::to_string(counter));
        auto verticesBuf = make::sptr<MeshUserVerticesBuffer>("round_" + std::to_string(angleStart) + "-" + std::to_string(angleStart) + " " + std::to_string(innerRadius) + "-" + std::to_string(outerRadius) + "x" + std::to_string(height));

        resources::MeshVertexDataBuilder vdb(verticesBuf, indicesBuf);
        for(size_t i = 0; i < boxCount; ++i)
        {
            const float curAngle = angleStart + i * (angleFinish - angleStart) / (boxCount - 1);

            const auto rotating = Rotator(0, curAngle, 0).toVector();

            vdb.addVertex({rotating.x * innerRadius, - height / 2, rotating.z * innerRadius}, {0.f, 0.f}, -rotating);
            vdb.addVertex({rotating.x * innerRadius, height / 2, rotating.z * innerRadius}, {0.f, 0.f}, -rotating);
            vdb.addVertex({rotating.x * outerRadius, - height / 2, rotating.z * outerRadius}, {0.f, 0.f}, {0, -1, 0});
            vdb.addVertex({rotating.x * innerRadius, - height / 2, rotating.z * innerRadius}, {0.f, 0.f}, {0, -1, 0});
            vdb.addVertex({rotating.x * outerRadius, height / 2, rotating.z * outerRadius}, {0.f, 0.f}, rotating);
            vdb.addVertex({rotating.x * outerRadius, - height / 2, rotating.z * outerRadius}, {0.f, 0.f}, rotating);
            vdb.addVertex({rotating.x * innerRadius, height / 2, rotating.z * innerRadius}, {0.f, 0.f}, {0, 1, 0});
            vdb.addVertex({rotating.x * outerRadius, height / 2, rotating.z * outerRadius}, {0.f, 0.f}, {0, 1, 0});
        }
        for(size_t i = 1; i < boxCount; ++i)
        {
            constexpr uint32_t indexPerIter = 4 * 2; //sides*triangles
            const uint32_t curIndex = indexPerIter * i;
            vdb.addIndices({curIndex - indexPerIter + 1, curIndex - indexPerIter, curIndex + 1,
                            curIndex - indexPerIter, curIndex, curIndex + 1});
            vdb.addIndices({curIndex - indexPerIter + 3, curIndex - indexPerIter + 2, curIndex + 3,
                            curIndex - indexPerIter + 2, curIndex + 2, curIndex + 3});
            vdb.addIndices({curIndex - indexPerIter + 5, curIndex - indexPerIter + 4, curIndex + 5,
                            curIndex - indexPerIter + 4, curIndex + 4, curIndex + 5});
            vdb.addIndices({curIndex - indexPerIter + 7, curIndex - indexPerIter + 6, curIndex + 7,
                            curIndex - indexPerIter + 6, curIndex + 6, curIndex + 7});
        }

        vdb.build();

        auto result = make::sptr<Mesh>(verticesBuf->getName(), verticesBuf);
        result->addSurface("unnamed", indicesBuf);

        const float boxSize = innerRadius * (2 * sin((angleFinish - angleStart) / 2 / boxCount));
        auto& physComponent = result->addComponent<physics::PhysicsComponent>();
        physComponent.setup(m_phisSim, physics::Body::BodyType::Animated, physics::PhysicsGeometry::Custom);
        for(size_t i = 0; i < boxCount; ++i)
        {
            const float curAngle = angleStart + (i + .5f) * (angleFinish - angleStart) / boxCount;
            const auto rotation = Rotator(0, curAngle, 0);
            const auto rotating = rotation.toVector();
            auto physCube = w4::make::sptr<physics::CubeGeometry>(boxSize, height, outerRadius - innerRadius);
            physCube->setPosition(rotating * ((innerRadius + outerRadius) / 2));
            physCube->setRotation(rotation);
            physComponent.addGeometry(physCube);
        }

        return result;
    }

    sptr<Mesh> createFloor(float width, float height, float offset)
    {
        auto indicesBuf = make::sptr<UserIndicesBuffer>("Floor indices");
        auto verticesBuf = make::sptr<MeshUserVerticesBuffer>("floor_" + std::to_string(width) + "x" + std::to_string(height) + "-" + std::to_string(offset));

        resources::MeshVertexDataBuilder vdb(verticesBuf, indicesBuf);
        vdb.addVertex({-width / 2 + offset, 0, height / 2}, {0.f, 0.f}, {0, 1, 0});
        vdb.addVertex({width / 2 - offset, 0, height / 2}, {0.f, 0.f}, {0, 1, 0});
        vdb.addVertex({-width / 2, 0, height / 2 - offset}, {0.f, 0.f}, {0, 1, 0});
        vdb.addVertex({width / 2, 0, height / 2 - offset}, {0.f, 0.f}, {0, 1, 0});

        vdb.addVertex({width / 2, 0, -height / 2 + offset}, {0.f, 0.f}, {0, 1, 0});
        vdb.addVertex({-width / 2, 0, -height / 2 + offset}, {0.f, 0.f}, {0, 1, 0});
        vdb.addVertex({width / 2 - offset, 0, -height / 2}, {0.f, 0.f}, {0, 1, 0});
        vdb.addVertex({-width / 2 + offset, 0, -height / 2}, {0.f, 0.f}, {0, 1, 0});

        vdb.addIndices({2, 1, 0,
                                2, 3, 1,
                                4, 3 ,2,
                                5, 4, 2,
                                5, 6, 4,
                                7, 6, 5});

        vdb.build();

        auto result = make::sptr<Mesh>(verticesBuf->getName(), verticesBuf);
        result->addSurface("unnamed", indicesBuf);

        result->setMaterialInst(Material::getDefault()->createInstance());

        auto& physComponent = result->addComponent<physics::PhysicsComponent>();
        physComponent.setup(m_phisSim, w4::physics::Body::BodyType::Animated, w4::physics::PhysicsGeometry::Custom);
        auto physCube1 = make::sptr<physics::CubeGeometry>(width, .2f, height - offset * 2);
        physCube1->setPosition({0, -.1f, 0});
        physComponent.addGeometry(physCube1);
        auto physCube2 = w4::make::sptr<physics::CubeGeometry>(width - offset * 2, .2f, offset);
        physCube2->setPosition({0, -.1f, (height - offset) / 2});
        physComponent.addGeometry(physCube2);
        auto physCube3 = w4::make::sptr<physics::CubeGeometry>(width - offset * 2, .2f, offset);
        physCube3->setPosition({0, -.1f, (offset - height) / 2});
        physComponent.addGeometry(physCube3);

        const auto offsetDiag = std::sqrt(2 * offset * offset);
        const Rotator rotation45(0, HALF_PI/2, 0);
        for(int i = -1; i <=1; i+=2)
        {
            for (int j = -1; j <= 1; j += 2)
            {
                auto physCubeCorner = w4::make::sptr<physics::CubeGeometry>(offsetDiag, .2f, offsetDiag);
                physCubeCorner->setRotation(rotation45);
                physCubeCorner->setPosition({i * (width / 2 - offset) , -.1f, j * (height / 2 - offset)});
                physComponent.addGeometry(physCubeCorner);
            }
        }
        return result;
    }


    void createField()
    {
        const auto physMat = w4::make::sptr<PhysicsMaterial>(.5f, .7f);

        auto floor = createFloor(18, 9, .75f); //Mesh::create::cube({18, .2f, 9});
        floor->setWorldTranslation({0, -.75f, 0});
        Render::getRoot()->addChild(floor);
        floor->getMaterialInst()->setParam("baseColor", vec4(.204f, .729f, .611f, 1));

        auto borderMatInst = Material::getDefault()->createInstance();
        borderMatInst->setParam("baseColor", vec4(.929f, .682f, .318f, 1));

        constexpr auto borderWidth = 1.f;
        auto side = Mesh::create::cube({borderWidth, 1.5f, 7.5f});
        side->setMaterialInst(borderMatInst);

        std::pair<vec3, Rotator> sidesConfig[] = {{{-9 - borderWidth / 2, 0, 0},{0, 0, 0, 1}},
                                                 {{9 + borderWidth / 2, 0, 0},{0, 0, 0, 1}},
                                                 {{-4.5f, 0, 4.5f + borderWidth/ 2},{0, 0.7071068f, 0, 0.7071068f}},
                                                 {{4.5f, 0, 4.5f + borderWidth / 2},{0, 0.7071068f, 0, 0.7071068f}},
                                                 {{-4.5f, 0, -4.5f - borderWidth / 2},{0, 0.7071068f, 0, 0.7071068f}},
                                                 {{4.5f, 0, -4.5f - borderWidth / 2},{0, 0.7071068f, 0, 0.7071068f}}};
        for(const auto& val: sidesConfig)
        {
            auto cloneSide = side->clone();
            cloneSide->setWorldTranslation(val.first);
            cloneSide->setWorldRotation(val.second);
            floor->addChild(cloneSide);
            auto& cubePhysicsComponent = cloneSide->addComponent<physics::PhysicsComponent>();
            cubePhysicsComponent.setup(m_phisSim, physics::Body::BodyType::Animated, physics::PhysicsGeometry::AutoCube);
            cubePhysicsComponent.setMaterial(physMat);
        }

        auto corner = createRounding(.75f, 1.75f, 1.5f, 10, HALF_PI, PI * 1.5f);
        corner->setLocalTranslation({0, 0, -4.5 - borderWidth / 2});
        corner->setMaterialInst(borderMatInst);
        floor->addChild(corner);
        corner->getFirstComponent<PhysicsComponent>().setMaterial(physMat);

        auto corner2 = createRounding(.75f, 1.75f, 1.5f, 10, -HALF_PI, HALF_PI);
        corner2->setLocalTranslation({0, 0, 4.5 + borderWidth / 2});
        corner2->setMaterialInst(borderMatInst);
        floor->addChild(corner2);
        corner2->getFirstComponent<PhysicsComponent>().setMaterial(physMat);

        auto corner3 = createRounding(.75f, 1.75f, 1.5f, 10, -HALF_PI, PI);
        corner3->setLocalTranslation({9, 0, 4.5});
        corner3->setMaterialInst(borderMatInst);
        floor->addChild(corner3);
        corner3->getFirstComponent<PhysicsComponent>().setMaterial(physMat);

        auto corner4 = createRounding(.75f, 1.75f, 1.5f, 10, -PI, HALF_PI);
        corner4->setLocalTranslation({-9, 0, 4.5});
        corner4->setMaterialInst(borderMatInst);
        floor->addChild(corner4);
        corner4->getFirstComponent<PhysicsComponent>().setMaterial(physMat);

        auto corner5 = createRounding(.75f, 1.75f, 1.5f, 10, 0, PI * 1.5f);
        corner5->setLocalTranslation({9, 0, -4.5});
        corner5->setMaterialInst(borderMatInst);
        floor->addChild(corner5);
        corner5->getFirstComponent<PhysicsComponent>().setMaterial(physMat);

        auto corner6 = createRounding(.75f, 1.75f, 1.5f, 10, -PI * 1.5f, 0);
        corner6->setLocalTranslation({-9, 0, -4.5});
        corner6->setMaterialInst(borderMatInst);
        floor->addChild(corner6);
        corner6->getFirstComponent<PhysicsComponent>().setMaterial(physMat);

        floor->setWorldRotation({0, HALF_PI, 0});
    }

    void onConfig() override
    {
        Game::Config.EnableFrustumCulling = false;
    }
    void onStart() override
    {
        m_phisSim = make::sptr<physics::Simulator>(1000, 1000, 0, vec3(0.0f, -9.8f, 0.0f));
        physics::SimulatorCollector::addSimulator(m_phisSim);

        auto cam = Render::getScreenCamera();
             cam->setWorldTranslation({0.f, 15, -15.f});
             cam->setWorldRotation({HALF_PI / 2, 0 ,0});
             cam->setFov(45.f);
             cam->setAspect(9.f/16.f);
             cam->setClearMask(ClearMask::Color | ClearMask::Depth | ClearMask::Background);
             cam->getBackground()->setMaterial(Material::get("materials/clouds.mat")->createInstance());

        constexpr float boxSize = 1.f;
        constexpr int boxesPerBelt = 36;
        constexpr int towerHeight = 10;
        const float wallRadius = boxSize / (2 * sin(PI / boxesPerBelt)) + boxSize / 2;

        auto matInst = Material::getDefault()->createInstance();

        constexpr float ballRadius = .4f;

        std::vector<vec3> ballPositions = {{0, 0, -4.5f}};
        for(int i = 1; i < 6; ++i)
        {
            for(int j = 0; j < i; ++j)
            {
                ballPositions.emplace_back(vec3((j - static_cast<float>(i) / 2) * ballRadius * 2, 0, 4.5f + (i - 1.f) * ballRadius * 2));
            }
        }

        const auto physMat = make::sptr<PhysicsMaterial>(.5f, 1.3f);

        for(const auto& ball: ballPositions)
        {
            auto sphere = Mesh::create::sphere(ballRadius, 10, 10);
            Render::getRoot()->addChild(sphere);
                sphere->getMaterialInst()->setParam("baseColor", color::random());
                sphere->setWorldTranslation(ball);


//            auto &spherePhysicsComponent = sphere->addComponent<w4::physics::PhysicsComponent>(m_phisSim,
//                                                                                               w4::physics::Body::BodyType::Rigid,
//                                                                                               w4::physics::PhysicsGeometry::AutoSphere);
            auto &spherePhysicsComponent = sphere->addComponent<physics::PhysicsComponent>();
            spherePhysicsComponent.setup(m_phisSim,
                                                                                                 physics::Body::BodyType::Rigid,
                                                                                                 physics::PhysicsGeometry::Custom);
            spherePhysicsComponent.addGeometry(make::sptr<physics::SphereGeometry>(ballRadius * 2));
            spherePhysicsComponent.setMass(.5f);
            spherePhysicsComponent.setMaterial(physMat);

            if(!m_sphere)
            {
                m_sphere = sphere;
            }
        }

        createField();

        event::Touch::Begin::subscribe(std::bind(&W4Billiards::onTouchBegin, this, std::placeholders::_1));
        event::Touch::Move::subscribe(std::bind(&W4Billiards::onTouchMove, this, std::placeholders::_1));
        event::Touch::End::subscribe(std::bind(&W4Billiards::onTouchEnd, this, std::placeholders::_1));
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
            m_plotter = make::sptr<Plotter>("cue");
            Render::getRoot()->addChild(m_plotter);
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
        m_sphere->getFirstComponent<physics::PhysicsComponent>().setVelocity((m_touchBegin - touchEnd) * 3);
    }

    void onUpdate(float dt) override
    {
    }
private:
    sptr<physics::Simulator> m_phisSim;
    sptr<Mesh> m_sphere;
    sptr<Plotter> m_plotter;
    vec3 m_touchBegin;
};
W4_RUN(W4Billiards)
