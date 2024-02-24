#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct ArcBallGist : public IGame
{
    void onStart() override
    {
        auto arcball = make::sptr<ArcBallNode>("arcball", 2.5f);
             arcball->addChild(Mesh::create::cube({2.5f, 2.5f, 2.5f}));
             arcball->setFriction(1.f);
             arcball->setVelocity({0.f, DEG2RAD * 24.f, 0.f});
        Render::getRoot()->addChild(arcball);
        gui::createWidget<Label>(nullptr, "SWIPE TO ROTATE", ivec2{540, 200});
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));
    }
};

W4_RUN(ArcBallGist)
