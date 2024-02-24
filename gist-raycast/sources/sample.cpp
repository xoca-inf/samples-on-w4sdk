#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

class GistRaycast: public IGame
{
    void createTestZone(float verticalPosition, ScreencastEvent event, const char * text)
    {
        auto mesh = Mesh::create::plane({4, 2});
        mesh->setWorldTranslation({0, 6.0f - 12 * verticalPosition, 0});

        auto screenCastCb = [](const CollisionInfo& info)
        {
            const auto* node = info.target->getParent();
            if(node->is<Mesh>())
            {
                node->as<Mesh>()->getMaterialInst()->setParam("baseColor", math::color::random());
            }
        };

        mesh->addCollider<core::AABB>(text, math::vec3(4.f, 2.f, 1.f))->setScreencastCallback(event, screenCastCb);

        Render::getRoot()->addChild(mesh);
        auto label = createWidget<Label>(nullptr, text, ivec2{540, (int)(2060 * verticalPosition) - 300});
    }

    void onStart()
    {
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));
        createTestZone(.2, ScreencastEvent::Down,   "TOUCH DOWN TEST ZONE");
        createTestZone(.5, ScreencastEvent::Move, "TOUCH MOVE TEST ZONE");
        createTestZone(.8, ScreencastEvent::Up,     "TOUCH UP TEST ZONE");
    }
};

W4_RUN(GistRaycast);

