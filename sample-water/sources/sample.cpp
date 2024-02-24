#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

float cubeDim = 1000;

std::vector<vec3> spotLightsColors = {
        vec3(0.0f, 1.0f, 0.5f),
        vec3(0.5f, 0.0f, 1.0f),
        vec3(0.0f, 0.5f, 1.0f)
};

class WaterSample : public IGame {
public:

    WaterSample() = default;
    ~WaterSample() = default;

    void onConfig() override
    {
        Game::Config.VFSClean = true;
        Game::Config.EnableFrustumCulling = false;
    }

    void onStart() override
    {
        auto cubemap = Cubemap::fromImages({resources::Image::get("textures/right.png"),
                                            resources::Image::get("textures/left.png"),
                                            resources::Image::get("textures/top.png"),
                                            resources::Image::get("textures/bottom.png"),
                                            resources::Image::get("textures/front.png"),
                                            resources::Image::get("textures/back.png"),
                                    });

        auto cam = Render::getScreenCamera();
        cam->setAspect(9.f / 16.f);
        cam->setFov(90.f);
        cam->setClearColor({0.25,0.25,0.25,1});
        cam->setClearMask(ClearMask::Color | ClearMask::Depth | ClearMask::Skybox);
        cam->getSkybox()->setCubemap(cubemap);

        cam->setWorldTranslation({0, 20.f, -100});
        auto pointToLook = vec3(0.0f, 0.0f, 0.0f);
        auto lookTo = (pointToLook - cam->getWorldTranslation()).toRotator(vec3(0, 1, 0));
        cam->setWorldRotation(lookTo);

        m_light1 = make::sptr<PointLight>("light1");
        m_light1->setColor(vec3(1.0f, 1.0f,  1.0f));
        m_light1->setWorldTranslation(vec3(0.0f, -50.f, 400.0f));
        //m_light1->addComponent<DebugViewComponent>(vec3(1.0f, 1.0f, 0.0f));

        auto root = Render::getPass(0)->getRoot();
        root->addChild(m_light1);

        //m_light1->getComponent<DebugViewComponent>().enable(true);


        auto plane = Mesh::create::plane(vec2(cubeDim, cubeDim));

        auto mat = Material::get("materials/water.mat")->createInstance();
        mat->setCubemap(CubemapId::CUBEMAP_0, cubemap);
        plane->setMaterialInst(mat);
        plane->setWorldTranslation(vec3(0.0f, -100.f, 0.0));
        plane->setWorldRotation(Rotator(HALF_PI, 0.f, 0.0f));
        root->addChild(plane);

        m_spotLight = make::sptr<SpotLight>("spotLight");
        m_spotLight->setColor(vec3(0.5f, 0.5f, 0.0f));
        m_spotLight->setDecayRate(LightDecayRate::Linear);
        m_spotLight->setAngle(HALF_PI / 6.0f);
        m_spotLight->setDecayFactor(0.5f);
        m_spotLight->setWorldTranslation(vec3(40.0f, 40.f, 0.0f));
        //m_spotLight->addComponent<DebugViewComponent>(vec3(1.0f, 1.0f, 1.0f));

        auto lookToPL = (pointToLook - m_spotLight->getWorldTranslation()).toRotator(vec3(0, 1, 0));
        m_spotLight->setWorldRotation(lookToPL);

        root->addChild(m_spotLight);

        auto yAngle = m_spotLight->getWorldRotation().euler().y;
        m_lightRotationTween = make::sptr<Tween<float>>(yAngle + HALF_PI / 2.0f, 0.0f);
        m_lightRotationTween->add(yAngle - HALF_PI / 2.0f, 4.0f, easing::SineInOut<float>);
        m_lightRotationTween->add(yAngle + HALF_PI / 2.0f, 8.0f, easing::SineInOut<float>);

        m_lightAngleTween = make::sptr<Tween<float>>(HALF_PI / 8.0f);
        m_lightAngleTween->add(HALF_PI / 6.0f, 4.0f, easing::SineInOut<float>);
        m_lightAngleTween->add(HALF_PI / 8.0f, 8.0f, easing::SineInOut<float>);

        auto r = m_spotLight->getWorldRotation().euler();
        rx = r.x;
        rz = r.z;
    }

    void onUpdate(float dt) override
    {
        static float t = 0.f;
        t += dt;
        auto t2 = fmod(t, m_lightRotationTween->getDuration());
        auto angle = m_lightRotationTween->getValue(t2);

        m_spotLight->setWorldRotation(Rotator(rx, angle, rz));
        m_spotLight->setAngle(m_lightAngleTween->getValue(t2));
    }


    void onKey(const event::Keyboard::Down& evt) override
    {
        switch (evt.key) {
            case event::Keyboard::Key::A:
            {
                m_light1->setWorldTranslation(m_light1->getWorldTranslation() + vec3(-0.5, 0, 0));
                break;
            }
            case event::Keyboard::Key::S:
            {
                m_light1->setWorldTranslation(m_light1->getWorldTranslation() + vec3(0, 0, -0.5));
                break;
            }
            case event::Keyboard::Key::D:
            {
                m_light1->setWorldTranslation(m_light1->getWorldTranslation() + vec3(0.5, 0, 0));
                break;
            }
            case event::Keyboard::Key::W:
            {
                m_light1->setWorldTranslation(m_light1->getWorldTranslation() + vec3(0, 0, 0.5));
                break;
            }
            default:
                break;
        }
    }

private:
    sptr<PointLight> m_light1;
    sptr<SpotLight> m_spotLight;
    sptr<Tween<float>> m_lightRotationTween;
    sptr<Tween<float>> m_lightAngleTween;
    float rx, rz;
};

W4_RUN(WaterSample)