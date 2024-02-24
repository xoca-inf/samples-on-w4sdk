#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct LoadAssetGist : public IGame
{
    void onStart() override
    {
        gui::createWidget<Label>(nullptr, "TAP FOR CHANGE COLOR", ivec2(540, 200));
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));
        Render::getScreenCamera()->setWorldTranslation({0, 0, -75});
        auto asset = Asset::get("utah-teapot.w4a")->getFirstRoot();
        asset->traversal([](auto & node)
        {
            W4_LOG_DEBUG("node %s", node.getName().data());
        });
        teapot = asset->getChild<Mesh>("Utah Teapot Quads");
        Render::getRoot()->addChild(teapot);
    }

    virtual void onTouch(const event::Touch::Begin&) override
    {
        teapot->foreachSurface([](Surface & surface)
        {
            surface.getMaterialInst()->setParam("baseColor", math::color::random());
        });
    }

    void onUpdate(float dt) override
    {
        teapot->rotateLocal(Rotator(0, dt, 0));
    }
private:
    sptr<Mesh> teapot;

};
W4_RUN(LoadAssetGist)