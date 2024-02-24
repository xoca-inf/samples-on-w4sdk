#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct FxGist : public IGame
{
public:
    void setBlurParams(cref<MaterialInst> mat)
    {
        mat->setParam("offset", vec3(0.0, 1.3846153846, 3.2307692308));
        mat->setParam("weight", vec3(0.2270270270, 0.3162162162, 0.0702702703));
    }

    void onStart() override
    {
        gui::createWidget<Label>(nullptr, "TAP TO TOGGLE BLOOM", ivec2{540, 200});
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));

        auto matInst = Material::get("materials/simple.mat")->createInstance();
        matInst->setTexture(TextureId::TEXTURE_0, Texture::get("textures/uv_test.png"));

        m_cube = Mesh::create::cube({2.f, 2.f, 2.f});
        m_cube->setMaterialInst(matInst);

        Render::getRoot()->addChild(m_cube);

        auto& pass = Render::getFxPass();
        auto rtIdx0 = pass.createRelativeRenderTarget(vec2(0.5f, 0.5f));
        auto rtIdx1 = pass.createRelativeRenderTarget(vec2(0.25f, 0.25f));
        auto rtIdx2 = pass.createRelativeRenderTarget(vec2(0.25f, 0.25f));
        auto rtIdx3 = pass.createRelativeRenderTarget(vec2(1.0f, 1.0f));

        auto downSampleMap = FxPass::getDefaultMaterialInst();
        auto brightMat = Material::get("materials/bright.mat")->createInstance();
        brightMat->setParam("brightPassThreshold", 0.7f);

        auto blurMatVert = Material::get("materials/blurVert.mat")->createInstance();
        setBlurParams(blurMatVert);

        auto blurMatHor = Material::get("materials/blurHor.mat")->createInstance();
        setBlurParams(blurMatHor);

        auto composeMat = Material::get("materials/compose.mat")->createInstance();

        pass.createFx("bloom")
                .addFxStep({FxPass::FxPassRT_Id::Input}, rtIdx0, downSampleMap)
                .addFxStep({rtIdx0}, rtIdx1, brightMat)
                .addFxStep({rtIdx1}, rtIdx2, blurMatVert)
                .addFxStep({rtIdx2}, rtIdx1, blurMatHor)
                .addFxStep({FxPass::FxPassRT_Id::Input, rtIdx1}, FxPass::FxPassRT_Id::Screen, composeMat)
        ;

        pass.setCurrentFx("bloom");
        Render::enableFx(true);
    }

    virtual void onTouch(const event::Touch::Begin&) override
    {
        static bool isBloomEnabled = true;
        isBloomEnabled = !isBloomEnabled;
        Render::getFxPass().setEnabled(isBloomEnabled);
    }

    void onUpdate(float dt) override
    {
        m_cube->rotateLocal(Rotator{dt,dt,dt});
    }

private:
    sptr<Mesh> m_cube;
};

W4_RUN(FxGist)