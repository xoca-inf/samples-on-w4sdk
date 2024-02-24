#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

class AnimateTextureGist : public IGame
{
public:
    void onStart() override
    {
        auto mat = Material::get("animated.mat")->createInstance();
             //mat->setTexture(TextureId::TEXTURE_0, Texture::get("animation.png"));
             //mat->setBlendFunc(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA);
             mat->enableBlending(true);
             mat->setParam("startTime", Render::getElapsedTime());

        auto billboard = w4::make::sptr<Billboard>(10, 10, "animation");
             billboard->setMaterialInst(mat);
        Render::getRoot()->addChild(billboard);
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));

    }
};

W4_RUN(AnimateTextureGist)
