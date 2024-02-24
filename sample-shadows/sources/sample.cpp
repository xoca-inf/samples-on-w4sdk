#include "W4Framework.h"

#include "KeyboardControllerComponent.h"

W4_USE_UNSTRICT_INTERFACE

class ShadowSample : public IGame {
public:

    ShadowSample() = default;
    ~ShadowSample() = default;

    void onConfig() override
    {
        Game::Config.VFSClean = true;
        Game::Config.EnableFrustumCulling = false;
    }

    void onStart() override
    {
        Render::enableDebugView(true);
        auto cam = Render::getScreenCamera();
        cam->setAspect(9.f / 16.f);
        cam->setFov(35.f);
        cam->setClearColor({0.25,0.25,0.25,1});

        cam->setWorldTranslation({46.f, 22.f, -21.f});
        auto pointToLook = vec3(0.0f, 0.0f, 0.0f);
        auto lookTo = (pointToLook - cam->getWorldTranslation()).toRotator(vec3(0, 1, 0));
        cam->setWorldRotation(lookTo);
        cam->getBackground()->setTexture(resources::Texture::predefined::white());

        cam->setClearMask(cam->getClearMask() | ClearMask::Background);

        auto box = Mesh::create::cube(vec3(3, 1, 2));
        auto boxMat = MaterialInst::predefined::lambert();
        boxMat->setTexture(TextureId::TEXTURE_0,Texture::predefined::white());
        box->setMaterialInst(boxMat);
        box->setWorldTranslation(vec3(0, 5, 0));

        auto light1 = w4::make::sptr<SpotLight>("light");
        light1->setColor(vec3(0.0f, 1.0f, 0.5f));
        light1->setDecayRate(LightDecayRate::Linear);
        light1->setAngle(HALF_PI / 2.0f);
        light1->setDecayFactor(0.5f);
        light1->setWorldTranslation(vec3(15.0f, 20.f, 0.0f));
        light1->setLocalRotation(Rotator( HALF_PI, 0.0f, 0.0f));
        light1->enableShadows();
        light1->setDebugViewColor({1,1,1,1}).enableDebugView(true);

        Render::getPass(0)->getDirectionalLight()->setColor(math::vec3(0.5f, 0.5f, 0.5f));
        //Render::getPass(0)->getDirectionalLight()->setDirection(math::vec3(0.0f, -1.0f, 1.0f));
        Render::getPass(0)->getDirectionalLight()->setDirection(math::vec3(0.0f, -1.0f, 0.0f));
        Render::getPass(0)->getDirectionalLight()->enableShadows();


        auto lookToL1 = (pointToLook - light1->getWorldTranslation()).toRotator(vec3(0, 1, 0));
        light1->setWorldRotation(lookToL1);

        auto plane = Mesh::create::plane(vec2 (100, 100));
        auto planeMat = Material::getDefaultBlinn()->createInstance();
        planeMat->setTexture(TextureId::TEXTURE_0, Texture::predefined::white());
        plane->setMaterialInst(planeMat);
        plane->setLocalRotation(Rotator( HALF_PI, 0.0f, 0.0f));
        plane->setWorldTranslation(vec3(0, - 0.05, 0));

        auto pBoy = Asset::get("SK_PaperBoy_T_Pose.w4a")->getFirstRoot();
        pBoy->setWorldScale(math::vec3(0.1, 0.1, 0.1));
        pBoy->setWorldTranslation(math::vec3(4.0, 0.0, 4.0));
        pBoy->rotateLocal({0, PI * .8, 0});

        Render::getRoot()->addChild(pBoy);
        //Render::getRoot()->rotateLocal({0, PI * .8, 0});
        Render::getRoot()->traversalTyped<SkinnedMesh>([this](cref<SkinnedMesh> node)
                                                       {
                                                           node->getAnimator("A_PaperBoy_Run").setIsLooped(true);
                                                           node->play("A_PaperBoy_Run");
                                                           auto mat = Material::getDefaultBlinn()->createInstance();
                                                           mat->setTexture(TextureId::TEXTURE_0, Texture::predefined::white());
                                                           node->setMaterialInst(mat);
                                                           //node->setShadowReceiver(false);
                                                           //node->setShadowCaster(false);
                                                       });

        Render::getRoot()->addChild(box);
        Render::getRoot()->addChild(light1);
        Render::getRoot()->addChild(plane);

/*
        auto &moveComponent = light1->addComponent<MovementComponent>();
        auto moveSettings = moveComponent.getSettings();
        moveSettings.speed.forward = 20;
        moveSettings.speed.backward = 10;
        moveSettings.speed.left = 15;
        moveSettings.speed.right = 15;
        moveSettings.speed.up = 10;
        moveSettings.speed.down = 10;
        moveComponent.setSettings(moveSettings);

        light1->addComponent<KeyboardControllerComponent>();*/

    }

    void createTweens()
    {

    }

    void onUpdate(float dt) override
    {
        static float t = 0.0f;
        t += dt;
        Render::getPass(0)->getDirectionalLight()->setDirection(math::vec3(0.0f, -1.0f, sin(t) * 4.0f));
    }

private:

};

W4_RUN(ShadowSample)