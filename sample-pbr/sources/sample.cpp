#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

class PbrSample : public IGame
{
public:

    PbrSample() = default;
    ~PbrSample() = default;

    void onConfig() override
    {
        Game::Config.VFSClean = true;
        Game::Config.EnableFrustumCulling = false;
    }

    void onStart() override
    {
        auto cam = Render::getScreenCamera();
        cam->setWorldTranslation({0.f, -4.f, -30.f});
        cam->setAspect(9.f / 16.f);
        cam->setClearColor({0.15, 0.15, 0.15, 1.0});

        auto arcball = make::sptr<ArcBallNode>("arcball", 12.5f);
        Render::getRoot()->addChild(arcball);

        auto light = make::sptr<PointLight>("light");
        light->setWorldTranslation({-25.f, -25.f, -25.f});
        light->setDecayRate(core::LightDecayRate::None);
        Render::getRoot()->addChild(light);

        auto matInst = Material::get("materials/pbr.mat")->createInstance();
        matInst->setTexture(TextureId::TEXTURE_0, Texture::get("textures/albedo.jpg"));
        matInst->setTexture(TextureId::TEXTURE_1, Texture::get("textures/ao.jpg"));
        matInst->setTexture(TextureId::TEXTURE_2, Texture::get("textures/metalRoughness.jpg"));
        matInst->setTexture(TextureId::TEXTURE_3, Texture::get("textures/normal.jpg"));
        matInst->setTexture(TextureId::TEXTURE_4, Texture::get("textures/emissive.jpg"));

        matInst->setParam("metallicOffset", 0.f);
        matInst->setParam("roughnessOffset", 0.f);
        matInst->setParam("emissiveFactor", 1.f);

        auto assetRoot = Asset::get("DamagedHelmet.w4a")->getFirstRoot();
        assetRoot->setWorldScale({0.1, 0.1, 0.1});
        assetRoot->traversal([matInst](Node& node)
                             {
                                    if(node.is<Mesh>())
                                    {
                                        node.as<Mesh>()->setMaterialInst(matInst);
                                    }
                             });
        arcball->addChild(assetRoot);

        arcball->setFriction(1.f);
        arcball->setVelocity({0.f, 18.f * DEG2RAD, 0.f});


        // GUI
        gui::setVirtualResolution({1080, 1920});

         gui::createWidget<Label>(nullptr, "PBR", ivec2{540, 200});

        auto sliders = createWidget<Widget>(nullptr, "Sliders");
        sliders->setPosition({520, 1430});

        auto sliderMetallic  = gui::createWidget<Slider>(sliders, 0.0f, 1.0f, 0.01f, 0.f, ivec2(0, 80),  "metallic");
        sliderMetallic->setWidth(480);
        auto metallicLabel = createWidget<Label>(sliders, "metallicLabel", ivec2({0, 0}));
        metallicLabel->setText(utils::format("Metallic: %.2f", 0.f));


        auto sliderRoughness = gui::createWidget<Slider>(sliders, 0.0f, 1.0f, 0.01f, 0.f, ivec2(0, 260),  "roughness");
        sliderRoughness->setWidth(480);
        auto roughnessLabel = createWidget<Label>(sliders, "roughnessLabel", ivec2({0, 180}));
        roughnessLabel->setText(utils::format("Roughness: %.2f", 0.f));

        auto sliderEmissive = gui::createWidget<Slider>(sliders, 0.0f, 3.0f, 0.01f, 1.f, ivec2(0, 440),  "emissive");
        sliderEmissive->setWidth(480);
        auto emissiveLabel = createWidget<Label>(sliders, "emissiveLabel", ivec2({0, 360}));
        emissiveLabel->setText(utils::format("Emissive: %.2f", 1.f));

        sliderMetallic->onValueChanged([=](float v)
        {
            auto metal = matInst->getParam<float>("metallicOffset");
            metal = v;
            matInst->setParam("metallicOffset", metal);
            metallicLabel->setText(utils::format("Metallic: %.2f", metal));
        });
        sliderRoughness->onValueChanged([=](float v)
        {
            auto rough = matInst->getParam<float>("roughnessOffset");
            rough = v;
            matInst->setParam("roughnessOffset", rough);
            roughnessLabel->setText(utils::format("Roughness: %.2f", rough));
        });
        sliderEmissive->onValueChanged([=](float v)
        {
            auto emissive = matInst->getParam<float>("emissiveFactor");
            emissive = v;
            matInst->setParam("emissiveFactor", emissive);
            emissiveLabel->setText(utils::format("Emissive: %.2f", emissive));
        });
    }
};

W4_RUN(PbrSample)