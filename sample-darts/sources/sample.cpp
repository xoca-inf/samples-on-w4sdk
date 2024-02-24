#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

class DartsDemo : public IGame
{
public:

    void onConfig() override
    {}

    void onStart() override
    {
        m_phisSim = make::sptr<physics::Simulator>(800, 10, 0, vec3(0.0f, -9.8f, 0.0f));
        w4::physics::SimulatorCollector::addSimulator(m_phisSim);

        auto root = Render::getRoot();

        auto sampleMat = Material::get("materials/simple.mat");
        m_target = Mesh::create::cube({80, 80, .2f});
        m_target->addComponent<physics::PhysicsComponent>().setup(m_phisSim, physics::Body::BodyType::Animated, physics::PhysicsGeometry::AutoCube);
        auto sampleMatInst = sampleMat->createInstance();
        sampleMatInst->setTexture(TextureId::TEXTURE_0, Texture::get("textures/archery_target.png"));
        m_target->setMaterialInst(sampleMatInst);
        Render::getRoot()->addChild(m_target);

// camera tune
        auto cam = Render::getScreenCamera();
            cam->setWorldTranslation({0, 0, -25});
            cam->setFov(112.f);
            cam->setFar(300.f);
            cam->setClearColor({.055f, .231f, .498f, 1.f});
            cam->setClearMask(ClearMask::Color | ClearMask::Depth |ClearMask::Background);
            cam->getBackground()->setMaterial(Material::get("materials/clouds.mat")->createInstance());

        event::Touch::Begin::subscribe(std::bind(&DartsDemo::onTouchBegin, this, std::placeholders::_1));
        event::Touch::End::subscribe(std::bind(&DartsDemo::onTouchEnd, this, std::placeholders::_1));

        setVirtualResolution({1080, 1920});
        auto titleLabel = createWidget<Label>(nullptr, "Click to throw dart. Throwing force depends on click duration.", ivec2(540, 1720));
        titleLabel->setHorizontalAlign(HorizontalAlign::Center);
    }

    sptr<Mesh> createDart()
    {
        auto dart = Mesh::create::cone(.05f, 0.f, 2.25f,10, 1);
        auto& phisComponent = dart->addComponent<PhysicsComponent>();
        phisComponent.setup(m_phisSim, physics::Body::BodyType::Rigid, physics::PhysicsGeometry::Custom);

        auto body = Mesh::create::cylinder(4.f, .25f, 10);
        body->setLocalTranslation({0, 3.125f, 0});
        dart->addChild(body, false);
        body->getMaterialInst()->setParam("baseColor", color::random());
        auto bodyGeometry = w4::make::sptr<physics::CapsuleGeometry>(.5f, 4.f);
        bodyGeometry->setPosition(body->getLocalTranslation());
        phisComponent.addGeometry(bodyGeometry);

        auto tail = Mesh::create::cylinder(4.f, .15f, 10);
        tail->setLocalTranslation({0, 5.125f, 0});
        dart->addChild(tail, false);
        tail->getMaterialInst()->setParam("baseColor", color::random());
        auto tailGeometry = w4::make::sptr<physics::CapsuleGeometry>(.3f, 4.f);
        tailGeometry->setPosition(tail->getLocalTranslation());
        phisComponent.addGeometry(tailGeometry);

        for(int i = 0; i < 4; ++i)
        {
            const vec3 cubeSize(.05f, 2.0f, .7f);
            auto plumage = Mesh::create::cube(cubeSize);
            plumage->setLocalTranslation(vec3(0, 7.625f, 0.45f).rotate({0, i * HALF_PI, 0}));
            plumage->setLocalRotation({HALF_PI / 5, i * HALF_PI, 0});
            dart->addChild(plumage, false);
            plumage->getMaterialInst()->setParam("baseColor", color::random());

            auto plumageGeometry = w4::make::sptr<physics::CubeGeometry>(cubeSize);
            plumageGeometry->setPosition(plumage->getLocalTranslation());
            plumageGeometry->setRotation(plumage->getLocalRotation());
            phisComponent.addGeometry(plumageGeometry);
        }
        dart->setWorldTranslation({0, 35, -15});
        dart->setWorldRotation({-HALF_PI, 0, 0});
        Render::getRoot()->addChild(dart);
        dart->getMaterialInst()->setParam("baseColor", color::random());

        m_phisSim->setCollisionCallback({m_target->getFirstComponent<PhysicsComponent>()}, {phisComponent}, false, [this, dart](physics::Body& a, physics::Body& b){
            m_dartsHits.push_back(dart);
        });

        return dart;
    }

    void onUpdate(float dt) override
    {
        m_touchDuration += dt;
        for(auto& mesh: m_dartsHits)
        {
            mesh->removeFirstComponent<PhysicsComponent>();
        }
        m_dartsHits.clear();
    }

    void onTouchBegin(const event::Touch::Begin &evt)
    {
        m_touchDuration = 0.f;
    }

    void onTouchEnd(const event::Touch::End &evt)
    {
        const auto ray = Render::createRayFromScreen(evt.point);
        auto dart = createDart();
        dart->setWorldTranslation(ray.origin);
        const float velocity = m_touchDuration * 75.f;
        auto& phisComponent = dart->getFirstComponent<physics::PhysicsComponent>();
        phisComponent.setVelocity((ray.direction + vec3(0, .35f, 0)) * velocity);
        phisComponent.setAngularMomentum({0, -.5f, 10});
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
    sptr<Mesh> m_target;
    std::vector<sptr<Mesh>> m_dartsHits;
    sptr<Simulator> m_phisSim;

    float m_touchDuration = 0.f;
};

W4_RUN(DartsDemo)
