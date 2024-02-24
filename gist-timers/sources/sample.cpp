#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct TimersGist : public IGame
{
    void onStart() override
    {
        cube  = Mesh::create::cube({4, 4, 4});
        torus = Mesh::create::torus(2.5, 1, 32, 32, math::TWO_PI);

        Render::getRoot()->addChild(cube);
        shape=cube;

        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));

        Timer::addTask(1,[this]()
        {
            static uint cnt = 0;
            static auto label = gui::createWidget<Label>(nullptr, "", ivec2(540, 200));
                        label->setText(utils::format("timer#1  every 1 sec: %d ", ++cnt));
            shape->getMaterialInst()->setParam("baseColor", math::color::random());
            return true;
        }, true);

        Timer::addTask(3,[this]()
        {
            static uint cnt = 0;
            static auto label = gui::createWidget<Label>(nullptr, "", ivec2(540, 300));
                        label->setText(utils::format("timer#2 every 3 sec: %d ", ++cnt));
            if(shift) {Render::getRoot()->removeChild(cube); Render::getRoot()->addChild(torus); shape=torus;}
            else      {Render::getRoot()->removeChild(torus);Render::getRoot()->addChild(cube);  shape=cube;}
            shift = !shift;
            return true;
        }, true);
    }

    void onUpdate(float dt) override
    {
        static auto label = gui::createWidget<Label>(nullptr, "", ivec2(540, 100));
        label->setText(utils::format("elapsed time: %.2f sec.", Render::getElapsedTime()));
        shape->rotateLocal(Rotator(dt,dt,dt));
    }
private:
    sptr<Mesh> cube;
    sptr<Mesh> torus;
    sptr<Mesh> shape;
    bool shift = true;

};

W4_RUN(TimersGist)
