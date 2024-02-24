#include "W4Framework.h"
#include "Generation.h"

W4_USE_UNSTRICT_INTERFACE

class ReflectHslSample : public IGame
{
public:

    void onStart() override
    {

        auto cubemap = Cubemap::fromImages(
                                     {resources::Image::get("textures/px.jpg"),
                                      resources::Image::get("textures/nx.jpg"),
                                      resources::Image::get("textures/py.jpg"),
                                      resources::Image::get("textures/ny.jpg"),
                                      resources::Image::get("textures/pz.jpg"),
                                      resources::Image::get("textures/nz.jpg")});

        const auto& cam = Render::getScreenCamera();
        cam->setWorldTranslation({0.f, 0.f, -1200.f});
        cam->setFar(3000.f);

        auto arcball = make::sptr<ArcBallNode>("arcball", 180.f);
        Render::getRoot()->addChild(arcball);

        const int32_t xgrid = 14;
        const int32_t ygrid = 9;
        const int32_t zgrid = 14;

        for(int32_t i = 0 ; i < xgrid ; ++i)
        {
            for(int32_t k = 0 ; k < zgrid ; ++k)
            {
                auto mat = Material::get("materials/sphere.mat")->createInstance();
                mat->setCubemap(CubemapId::CUBEMAP_0, cubemap);

                for(int32_t j = 0 ; j < ygrid ; ++j)
                {
                    auto mesh = Mesh::create::sphere(60.f, 32.f, 32.f);
                    mesh->setMaterialInst(mat);

                    auto x = 200.f * (i - xgrid / 2.f);
                    auto y = 200.f * (j - ygrid / 2.f);
                    auto z = 200.f * (k - zgrid / 2.f);
                    mesh->setWorldTranslation({x, y, z});

                    arcball->addChild(mesh);
                }

                m_materials.push_back(mat);
            }
        }

        arcball->setFriction(1.f);
        arcball->setVelocity({20.f * DEG2RAD, 20.f * DEG2RAD, 0.f});
    }

    void onUpdate(float dt) override
    {
        static auto hslToRgb = [](float h, float s, float l) -> vec3
        {
            static auto hueToRgb = [](float p, float q, float t) -> float
            {
                if(t < 0.f) t += 1.f;
                if(t > 1.f) t -= 1.f;
                if(t < 1.f / 6.f) return p + (q - p) * 6.f * t;
                if(t < 1.f / 2.f) return q;
                if(t < 2.f / 3.f) return p + (q - p) * (2.f / 3.f - t) * 6.f;
                return p;
            };

            vec3 result;
            if(s == 0.f)
            {
                result.r = l;
                result.g = l;
                result.b = l;
            }
            else
            {
                auto q = l < 0.5f ? l * (1.f + s) : l + s - l * s;
                auto p = 2.f * l - q;
                result.r = hueToRgb(p, q, h + 1.f / 3.f);
                result.g = hueToRgb(p, q, h);
                result.b = hueToRgb(p, q, h - 1.f / 3.f);
            }
            return result;
        };

        static float time = 0;
        time += dt;

        for(size_t i = 0 ; i < m_materials.size() ; ++i)
        {
            auto h = (sin(time / 2.f + (360.f * i / m_materials.size()) * DEG2RAD) + 1.f) / 2.f;
            auto rgb = hslToRgb(h, 1.f, 0.5f);
            m_materials[i]->setParam("color", rgb);
        }
    }

private:
    std::vector<sptr<MaterialInst>> m_materials;
};

W4_RUN(ReflectHslSample)
