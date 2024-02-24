#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

class W4PhysicsDemo : public IGame
{
public:
    void onConfig() override
    {
        // todo: configure application behavior
    }
    void onStart() override
    {
        m_phisSim = make::sptr<physics::Simulator>(500, 10, 0, vec3(0.0f, -9.8f, 0.0f));
        w4::physics::SimulatorCollector::addSimulator(m_phisSim);

        auto cam = Render::getScreenCamera();
             cam->setWorldTranslation({0.f, 25, -25.f});
             cam->setWorldRotation({HALF_PI / 2, 0 ,0});
             cam->setFov(45.f);

        constexpr float boxSize = 1.f;
        constexpr int boxesPerBelt = 36;
        constexpr int towerHeight = 10;
        const float wallRadius = boxSize / (2 * sin(PI / boxesPerBelt)) + boxSize / 2;

        const auto matInst = Material::getDefault()->createInstance();

        const float stepAngle = TWO_PI / boxesPerBelt;
        const size_t bricksPerLine = 0;
        for(auto i = 0; i < towerHeight; ++i)
        {
            const vec3 beltOffset = {0, (i + .5f) * boxSize - (towerHeight / 2.f) * boxSize, 0}; //Вертикальное смещение текущего уровня башни
            for (auto j = 0; j < boxesPerBelt; ++j)
            {
                const vec3 wallVector = {wallRadius, 0, 0}; //Вектор радиуса, который мы будем вращать

                auto cube = Mesh::create::cube({boxSize, boxSize, boxSize});
                const auto curWallVector = wallVector.rotate(
                        {{0, 1, 0},
                         stepAngle * j + ((i % 2) ? stepAngle / 2 : 0)}); //Смещаем каждый чётный уровень на половину кубика

                cube->setWorldTranslation(curWallVector + beltOffset);
                cube->setWorldRotation(
                        curWallVector.toRotator({0, 1, 0})); //Поворачиваем куб в направлении вектора радиуса
                cube->setMaterialInst(matInst);
                Render::getRoot()->addChild(cube);

                cube->addComponent<physics::PhysicsComponent>().setup(m_phisSim, physics::Body::BodyType::Rigid, physics::PhysicsGeometry::AutoCube);
            }
        }
        constexpr float floorHeight = 1.f;
        auto floor = Mesh::create::cube({50, floorHeight, 50});
        floor->setWorldTranslation({0, - (towerHeight / 2.f) * boxSize - floorHeight / 2, 0});
        floor->setMaterialInst(matInst);
        Render::getRoot()->addChild(floor);

        floor->addComponent<physics::PhysicsComponent>().setup(m_phisSim, physics::Body::BodyType::Animated, physics::PhysicsGeometry::AutoCube);

        m_sphere = Mesh::create::sphere(.5f, 10, 10);
        m_sphere->setMaterialInst(matInst);
        Render::getRoot()->addChild(m_sphere);

        auto& spherePhysicsComponent = m_sphere->addComponent<physics::PhysicsComponent>();
        spherePhysicsComponent.setup(m_phisSim, physics::Body::BodyType::Rigid, physics::PhysicsGeometry::AutoSphere);
        spherePhysicsComponent.setMass(3.f);

        event::Touch::Begin::subscribe(std::bind(&W4PhysicsDemo::onTouchBegin, this, std::placeholders::_1));
        event::Touch::End::subscribe(std::bind(&W4PhysicsDemo::onTouchEnd, this, std::placeholders::_1));
    }

    void onTouchBegin(const event::Touch::Begin &evt)
    {
        m_touchDuration = 0.f;
    }

    void onTouchEnd(const event::Touch::End &evt)
    {
        const auto ray = Render::createRayFromScreen(evt.point);
        m_sphere->setWorldTranslation(ray.origin);
        const float velocity = m_touchDuration * 50.f;
        m_sphere->getFirstComponent<physics::PhysicsComponent>().setVelocity(ray.direction * velocity);
    }

    void onUpdate(float dt) override
    {
        m_touchDuration += dt;
    }
private:
    sptr<physics::Simulator> m_phisSim;
    sptr<Mesh> m_sphere;
    float m_touchDuration = 0.f;
};
W4_RUN(W4PhysicsDemo)
