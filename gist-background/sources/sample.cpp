#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct BackgroundGist : public IGame
{
    void onStart() override
    {
        mats.emplace_back(Material::get("back1.mat")->createInstance());
        mats.emplace_back(Material::get("back2.mat")->createInstance());
        mats.emplace_back(Material::get("back3.mat")->createInstance());
        mats.emplace_back(Material::get("back4.mat")->createInstance());
        mats.emplace_back(Material::get("back5.mat")->createInstance());

        cam = Render::getScreenCamera();
            cam->setClearMask(ClearMask::Color | ClearMask::Depth |ClearMask::Background);
            cam->getBackground()->setMaterial(mats.front());
        Render::getRoot()->addChild(Mesh::create::cube({2,2,2}));
        gui::createWidget<Label>(nullptr, "TAP FOR CHANGE BACKGROUND", ivec2(540, 200));
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));
    }

    void onTouch(const event::Touch::Begin& unused) override
    {
        std::rotate(mats.begin(),mats.begin()+1,mats.end());
        cam->getBackground()->setMaterial(mats.front());
    }

    void onUpdate(float dt) override
    {
        Render::getRoot()->rotateLocal(Rotator(dt,dt,dt));
    }

    sptr<Camera> cam;
    std::vector<sptr<MaterialInst>> mats;
};

W4_RUN(BackgroundGist)
