#include "W4Framework.h"
W4_USE_UNSTRICT_INTERFACE

struct SkyboxGist : public IGame
{
    void onStart() override
    {
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));
        auto cubemap = Cubemap::fromImages(
        {
            resources::Image::get("cube.png"),
            resources::Image::get("cube.png"),
            resources::Image::get("cube.png"),
            resources::Image::get("cube.png"),
            resources::Image::get("cube.png"),
            resources::Image::get("cube.png"),
        });
        cube = Mesh::create::cube({2,2,2});
        Render::getScreenCamera()->addChild(cube);
        Render::getRoot()->addChild(Render::getScreenCamera());
        Render::getScreenCamera()->setClearMask(ClearMask::Color | ClearMask::Depth | ClearMask::Skybox);
        Render::getScreenCamera()->getSkybox()->setCubemap(cubemap);
    }

    void onUpdate(float dt) override
    {
        cube->rotateLocal(Rotator(dt,dt,dt));
        Render::getScreenCamera()->rotateLocal(Rotator{sin(Render::getElapsedTime())/350, dt/3, 0});
    }
private:
    sptr<Node> cube;
};
W4_RUN(SkyboxGist)