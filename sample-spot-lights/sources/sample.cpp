#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

std::vector<vec3> spotLightsColors =
{
    vec3(0.0f, 1.0f, 0.5f),
    vec3(0.5f, 0.0f, 1.0f),
    vec3(0.0f, 0.5f, 1.0f)
};

int xSize = 40;
int ySize = 40;
float cubeDim = 2.5;

struct CubeHelper
{
    sptr<Mesh> m_cube;
    float m_time;
    float m_speed;
    int m_i, m_j;

    static sptr<MaterialInst> sharedMat()
    {
        static sptr<MaterialInst> res = []()
        {
            auto mat = MaterialInst::predefined::lambert();
            mat->setTexture(TextureId::TEXTURE_0, Texture::predefined::white());
            return mat;
        }();
        return res;
    }

    CubeHelper(int i, int j)
        : m_i(i)
        , m_j(j)
    {
        m_cube = Mesh::create::cube(vec3 (cubeDim, cubeDim, cubeDim));
        m_time = math::random<float>(0, math::PI);
        m_speed = math::random<float>(0.1, 2.0);

        m_cube->setMaterialInst(sharedMat());
        m_cube->setWorldTranslation(vec3((-xSize / 2 + i) * cubeDim, cubeDim * sin(m_time * m_speed) * 0.3, (-ySize / 2 + j) * cubeDim));

        auto root = Render::getRoot();
        root->addChild(m_cube);
    }

    void update(float dt)
    {
        m_time += dt;
        //m_time = 0.0f;
        m_cube->setWorldTranslation(vec3((-xSize / 2 + m_i) * cubeDim, cubeDim * sin(m_time * m_speed) * 0.3, (-ySize / 2 + m_j) * cubeDim));
    }
};

struct SpotLightHelper
{
    sptr<SpotLight> m_light;
    sptr<Tween<vec3>> m_lightTween;
    sptr<Tween<float>> m_lightTweenAngle;
    sptr<Tween<float>> m_lightTweenDecay;

    SpotLightHelper(const std::string& name, vec3::cref color)
    {
        m_light = w4::make::sptr<SpotLight>(name);
        m_light->setColor(color);
        m_light->setDecayRate(LightDecayRate::Linear);
        m_light->setAngle(HALF_PI / 2.0f);
        m_light->setDecayFactor(0.5f);
        m_light->setWorldTranslation(vec3(5.0f, 20.f, 0.0f));
        m_light->setLocalRotation(Rotator( HALF_PI, 0.0f, 0.0f));
        m_light->enableShadows();
        m_light->setDebugViewColor({1,1,1,1}).enableDebugView(true);


        Render::getRoot()->addChild(m_light);
    }

    void initTweens()
    {
        m_lightTween = make::sptr<Tween<vec3>>(m_light->getWorldTranslation(), 0.0f);
        m_lightTween->add(vec3(math::random<float>() * 35 - 15, math::random<float>() * 15 + 10, math::random<float>() * 35 - 15), 5.0f, easing::SineInOut<vec3>);

        m_lightTweenAngle = make::sptr<Tween<float>>(m_light->getAngle(), 0.0f);
        m_lightTweenAngle->add(math::random<float>() * 0.7 + 0.1, 5.0f, easing::SineInOut<float>);

        m_lightTweenDecay = make::sptr<Tween<float>>(m_light->getDecayFactor(), 0.0f);
        m_lightTweenDecay->add(math::random<float>(), 5.0f, easing::SineInOut<float>);
    }

    void onUpdate(float tweenTime)
    {
        m_light->setWorldTranslation(m_lightTween->getValue(tweenTime));
        auto pointToLook = vec3(0.0f, 0.0f, 0.0f);
        auto lookTo = (pointToLook - m_light->getWorldTranslation()).toRotator(vec3(0, 1, 0));
        m_light->setWorldRotation(lookTo);

        m_light->setAngle(m_lightTweenAngle->getValue(tweenTime));

        m_light->setDecayFactor(m_lightTweenDecay->getValue(tweenTime));
    }
};


class SpotLightSample : public IGame {
public:

    SpotLightSample() = default;
    ~SpotLightSample() = default;

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

        cam->setClearMask(cam->getClearMask() | ClearMask::Background);
        cam->getBackground()->setMaterial(resources::Material::get("materials/background.mat")->createInstance());
        cam->getBackground()->setTexture(resources::Texture::get("back.jpg"));

        m_light1 = w4::make::sptr<PointLight>("light1");
        m_light1->setColor(vec3(1.0f, 0.5f, 0.0f));
        m_light1->setWorldTranslation(vec3(0.0f, 10.f, 0.0f));
        m_light1->setDebugViewColor({1,1,0,1}).enableDebugView(true);

        Render::getPass(0)->getDirectionalLight()->setColor(math::vec3(1.0f, 0.5f, 0.0f));
        Render::getPass(0)->getDirectionalLight()->setDirection(math::vec3(1.0f, -1.0f, -1.0f));
        Render::getPass(0)->getDirectionalLight()->enableShadows();

        auto box = Mesh::create::cube(vec3(3, 1, 2));
        auto boxMat = MaterialInst::predefined::lambert();
        boxMat->setTexture(TextureId::TEXTURE_0,Texture::predefined::white());
        box->setMaterialInst(boxMat);
        box->setWorldTranslation(vec3(0, 5, 0));
        //box->setShadowCaster(false);

        Render::getRoot()->addChild(m_light1);
        Render::getRoot()->addChild(box);

        for (int i = 0; i < spotLightsColors.size(); ++i)
            m_helpers.emplace_back(utils::format("sp%d", i), spotLightsColors[i]);

        m_field.reserve(xSize);
        for (int i = 0; i < xSize; ++i)
        {
            m_field.emplace_back();
            m_field[i].reserve(ySize);
            for (int j = 0; j < ySize; ++j)
            {
                m_field[i].emplace_back(i, j);
//                if ((i+j) % 2 ==0)
//                {
//                    m_field[i][j].m_cube->setShadowReceiver(false);
//                }
            }
        }
        createTweens();
    }

    void createTweens()
    {
        for(auto& h : m_helpers) h.initTweens();
        m_tweenTime = 0.0f;
    }

    void onUpdate(float dt) override
    {
        m_tweenTime += dt;

        for (auto& h : m_helpers) h.onUpdate(m_tweenTime);
        if (m_tweenTime > 5.0f) createTweens();
        for (int i = 0; i < xSize; ++i)
            for (int j = 0; j < ySize; ++j)
                m_field[i][j].update(dt);
    }



private:
    sptr<PointLight> m_light1;
    std::vector<SpotLightHelper> m_helpers;

    std::vector<std::vector<CubeHelper>> m_field;

    float m_tweenTime = 0.0f;
};

W4_RUN(SpotLightSample)