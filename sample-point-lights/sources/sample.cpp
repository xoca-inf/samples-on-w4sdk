#include "W4Framework.h"
#include "Asset.h"
#include <random>

W4_USE_UNSTRICT_INTERFACE;

// disclaimer: code of this particular sample is awful and author knows about it
class PointLightsSample : public IGame {
public:

    void onConfig() override
    {
        Game::Config.VFSClean = true;
        Game::Config.UseSimpleInput = false;
        Game::Config.EnableFrustumCulling = false;
        Platform::setWindowStyle("landscape");
    }
    void onStart() override
    {
        auto cam = Render::getScreenCamera();
        cam->setWorldTranslation({0.f, VSIZE / 2.f, -HSIZE});

        auto pointToLook = vec3(0.0f, VSIZE / 2.f, HSIZE / 2.f);
        auto lookTo = (pointToLook - cam->getWorldTranslation()).toRotator(vec3(0, 1, 0));
        cam->setWorldRotation(lookTo);

        auto mat = Material::get("materials/bump.mat")->createInstance();

        auto color = Texture::get("textures/color.png");
        color->setWrapping(Wrapping::Repeat, Wrapping::Repeat);
        mat->setTexture(TextureId::TEXTURE_0, color);

        auto normal = Texture::get("textures/normal.png");
        normal->setWrapping(Wrapping::Repeat, Wrapping::Repeat);
        mat->setTexture(TextureId::TEXTURE_1, normal);

        auto floor = Mesh::create::plane({HSIZE, HSIZE}, true);
        floor->setMaterialInst(mat);
        floor->setWorldRotation({90.f * DEG2RAD, 0.f, 0.f});
        Render::getRoot()->addChild(floor);

        auto back = Mesh::create::plane({HSIZE, VSIZE}, true);
        back->setMaterialInst(mat);
        back->setWorldTranslation({0.f, VSIZE / 2.f, HSIZE / 2.f});
        Render::getRoot()->addChild(back);

        auto ceiling = Mesh::create::plane({HSIZE, HSIZE}, true);
        ceiling->setMaterialInst(mat);
        ceiling->setWorldRotation({-90.f * DEG2RAD, 0.f, 0.f});
        ceiling->setWorldTranslation({0.f, VSIZE, 0.f});
        Render::getRoot()->addChild(ceiling);

        initLights();
    }

    void onUpdate(float dt) override
    {
        updateLights(dt);
        updateRenderOrder();
    }

    void initLights()
    {
        auto lambert = Material::getDefaultLambert()->createInstance();
        lambert->setTexture(TextureId::TEXTURE_0, Texture::predefined::white());

        auto addLight = [this, lambert](const vec3& color, float deg) -> sptr<ParticlesEmitter>
        {
            auto addParticle = [] (math::vec3::cref pos) -> sptr<ParticlesEmitter>
            {
                auto emitter = ParticlesEmitter::load("particles/particle.part");
                emitter->setWorldScale({.001,.001,.001});
                emitter->setWorldTranslation(pos);
                emitter->setTransformMode(ParticlesEmitterParameters::TransformMode::EMITTER);
                emitter->start();
                return emitter;
            };

            auto light = make::sptr<PointLight>("light");
            light->setColor(color);
            light->setDecayRate(LightDecayRate::Linear);
            light->setIntensity(0.5f);

            auto particle = addParticle({0.f, 0.f, 0.f});
            particle->setStartColor({color.r, color.g, color.b, 1.0});
            particle->addChild(light);

            auto plane = make::sptr<Node>("plane");
            plane->addChild(particle, false);
            plane->setWorldTranslation({0.f, VSIZE / 2.f, 0.f});
            plane->setWorldRotation({0.f, 0.f, deg * DEG2RAD});

            Render::getRoot()->addChild(plane, false);
            return particle;
        };

        vec3 red = {1.0, 0.0, 0.0};
        vec3 green = {0.0, 1.0, 0.0};
        vec3 blue = {0.0, 0.0, 1.0};
        vec3 yellow = {1.0, 1.0, 0.0};
        vec3 magenta = {1.0, 0.0, 1.0};
        vec3 cyan = {0.0, 1.0, 1.0};

        m_intensity = make::sptr<Tween<float>>(0.25f);
        m_intensity->add(0.5f, 1.f, &easing::BounceInOut<float>);
        m_intensity->add(0.75f, 2.f, &easing::BounceInOut<float>);
        m_intensity->add(0.5f, 3.f, &easing::BounceInOut<float>);
        m_intensity->add(0.25f, 4.f, &easing::BounceInOut<float>);

        m_spheres.push_back(addLight(red, 25.f));
        m_spheres.push_back(addLight(blue, -25.f));
        m_spheres.push_back(addLight(magenta, 50.f));
        m_spheres.push_back(addLight(cyan, -50.f));
        m_spheres.push_back(addLight(yellow, 75.f));
        m_spheres.push_back(addLight(green, -75.f));
    }

    void updateRenderOrder()
    {
        using i2z = std::pair<size_t, float>;
        std::vector<i2z> i2zv;
        for(size_t i = 0, end = m_spheres.size() ; i < end ; ++i)
        {
            i2zv.emplace_back(i, m_spheres[i]->getWorldTranslation().z);
        }

        std::sort(i2zv.begin(), i2zv.end(), [](const i2z& a, const i2z& b)
        {
            return a.second > b.second;
        });

        for(size_t i = 0, end = i2zv.size() ; i < end ; ++i)
        {
            m_spheres[i2zv[i].first]->setRenderOrder(i);
        }
    }

    void updateLights(float dt)
    {
        static float time = 0.f;
        time += dt;

        static auto rotateSphere = [this](cref<Node> node, float delta, float time)
        {
            const float RADIUS = VSIZE / 4.f;
            auto dx = RADIUS * sin(delta * DEG2RAD + time);
            auto dy = RADIUS * cos(delta * DEG2RAD + time);
            node->setLocalTranslation({dx, 0.f, dy});
        };

        for(size_t i = 0, cnt = m_spheres.size() ; i < cnt ; ++i)
        {
            float delta = static_cast<float>(i) / cnt;
            rotateSphere(m_spheres[i], 360.f * delta, time);

            {
                auto duration = m_intensity->getDuration();
                auto t = fmod(time * 0.75 + duration * delta, duration);
                auto intensity = m_intensity->getValue(t);
                m_spheres[i]->getChild<PointLight>("light")->setIntensity(intensity);
//                auto col = m_spheres[i]->getStartColor();
//                m_spheres[i]->setStartColor(col.r, col.g, col.b, intensity);
            }
        }
    }

private:
    constexpr static const float HSIZE = 5.f;
    constexpr static const float VSIZE = 3.f;

    sptr<Tween<float>> m_intensity;
    sptr<Tween<vec3>> m_color;
    std::vector<sptr<ParticlesEmitter>> m_spheres;
};

W4_RUN(PointLightsSample)
