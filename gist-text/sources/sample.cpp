#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct TextGist : public IGame
{
public:
    void onStart() override
    {
        std::srand(std::time(nullptr));

        gui::createWidget<Label>(nullptr, "TAP TO START", ivec2{540, 200});
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));

        auto mat = w4::resources::Material::get("materials/unlit.mat");//Material::getDefaultLambert();

        m_cube = Mesh::create::box({2.f, 2.f, 2.f});

        const std::string sides[] = {"It is certain.", "Very doubtful.", "Cannot predict now.", "Don't count on it.", "Better not tell you now.", "You may rely on it."};

        int i = 0;
        m_cube->foreachSurface([this, &mat, &i, &sides](Surface& surface){
            const auto matInst = mat->createInstance();

            variant::VariantObject texTexParams;

            // Possible params and types
            //-----------------------------------------------------------------------------------------------------------
            // text        | std::string | Text to render
            // font        | std::string | Font to render. If skipped use "sans-serif"
            // fillColor    | vec4        | RGBA color of text body. If not defined - not filled.
            // strokeColor | vec4        | RGBA color of text stroke. If not defined - not stroked.
            // lineWidth   | int32_t     | Stroke line width if strokeColor was specified. by default - 2.
            // size        | int32_t     | Font size in canvas pixel (greater - text is more smoothest). If skipped use "10px"
            // style       | int32_t     | Possible values in enum TextTextureFontStyle. If skipped use "normal".
            // weight      | int32_t     | Possible values in enum TextTextureFontWeight. If skipped use "normal".
            //
            // ! Textures, generated from texts using similar font may have different height. But if you align it top edges,
            // ! you will be able to align the text шт texture with each other...

            texTexParams["text"] = sides[i];
            texTexParams["size"] = 100;
            const auto colorIdx = i % 3;
            texTexParams["fillColor"] = w4::math::vec4((colorIdx == 0) ? 1 : 0, (colorIdx == 1) ? 1 : 0, (colorIdx == 2) ? 1 : 0, 1);
            texTexParams["strokeColor"] = w4::math::vec4(0, 0, 0, 1);
            texTexParams["style"] = static_cast<int32_t>(TextTextureFontStyle::ITALIC);
            texTexParams["weight"] = static_cast<int32_t>(TextTextureFontWeight::BOLD);
            texTexParams["lineWidth"] = 10;
            auto texTex = w4::resources::Texture::get(
                    w4::resources::ResourceGenerator(w4::resources::TextTextureGenerator, texTexParams));
            matInst->setParam("texture0", texTex);
            matInst->setParam("texture1", Texture::get("textures/stone.jpeg"));
            matInst->setParam("aspect", static_cast<float>(texTex->getSize()->w) / texTex->getSize()->h);
            surface.setMaterialInst(matInst);
            ++i;
        });

        Render::getRoot()->addChild(m_cube);
    }

    virtual void onTouch(const event::Touch::Begin&) override
    {
        m_timeLeft = std::rand() % 100 / 10.f;
        for(size_t i = 0; i < 3; ++i)
        {
            m_directions[i] = (std::rand() % 60 - 30) / 10.f;
        }
    }

    void onUpdate(float dt) override
    {
        if(m_timeLeft > 0)
        {
            float resultDt = std::min(dt * 2, m_timeLeft);
            m_cube->rotateLocal(Rotator(resultDt * m_directions));
            m_timeLeft -= resultDt;
        }
    }

private:
    float m_timeLeft = 0;
    vec3 m_directions;
    sptr<Mesh> m_cube;
};

W4_RUN(TextGist)