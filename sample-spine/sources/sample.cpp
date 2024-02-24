#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

void callback(spine::AnimationState *state, spine::EventType type, spine::TrackEntry *entry, spine::Event *event) {
    SP_UNUSED(state);
    const spine::String &animationName = (entry && entry->getAnimation()) ? entry->getAnimation()->getName() : spine::String("");

    switch (type) {
        case spine::EventType_Start:
          //  printf("%d start: %s\n", entry->getTrackIndex(), animationName.buffer());
            break;
            case spine::EventType_Interrupt:
          //  printf("%d interrupt: %s\n", entry->getTrackIndex(), animationName.buffer());
            break;
            case spine::EventType_End:
         //   printf("%d end: %s\n", entry->getTrackIndex(), animationName.buffer());
            break;
            case spine::EventType_Complete:
          //  printf("%d complete: %s\n", entry->getTrackIndex(), animationName.buffer());
            break;
            case spine::EventType_Dispose:
         //   printf("%d dispose: %s\n", entry->getTrackIndex(), animationName.buffer());
            break;
            case spine::EventType_Event:
         //   printf("%d event: %s, %s: %d, %f, %s %f %f\n", entry->getTrackIndex(), animationName.buffer(), event->getData().getName().buffer(), event->getIntValue(), event->getFloatValue(),
         //       event->getStringValue().buffer(), event->getVolume(), event->getBalance());
                break;
    }
    fflush(stdout);
}

class SpineGist : public IGame
{
public:
    void onStart() override
    {
        auto cam = Render::getScreenCamera();
            cam->setWorldTranslation({0.f, 0, -25.f});
            cam->setFov(45.f);

        {
            auto spine = w4::make::sptr<Spine>("spine", "owl-pma.atlas", "owl-pro.json");

            spine->timeScale = 1;
            spine->setUsePremultipliedAlpha(false);

            spine->skeleton->setPosition(0, 300);

            spine->state->setListener(callback);
            spine->state->setAnimation(0, "idle", true);
            spine->state->setAnimation(1, "blink", true);

            spine->setWorldScale(vec3(10, 10, 10));
            spine->translateWorld(vec3(0, 0, 10));

            Render::getRoot()->addChild(spine);
            m_spines.push_back(spine);
        }

        {
            auto spine = w4::make::sptr<Spine>("spine", "spineboy-pma.atlas", "spineboy-pro.skel");

            spine->timeScale = 1;
            spine->setUsePremultipliedAlpha(false);

            spine->skeleton->setPosition(0, -600);
            spine->skeleton->setScaleX(-1.0f);

            spine->state->setListener(callback);
            spine->state->addAnimation(0, "walk", true, 0);
            spine->state->addAnimation(0, "jump", true, 3);
            spine->state->addAnimation(0, "run", true, 0);

            spine->setWorldScale(vec3(5, 5, 5));

            spine->translateWorld(vec3(0, 0, -10));

            Render::getRoot()->addChild(spine);
            m_spines.push_back(spine);
        }

        auto unlit = Material::getDefault()->createInstance();

        m_cyl = Mesh::create::cylinder(5.f, 3.f, 20);
        m_cyl->setMaterialInst(unlit);
        Render::getRoot()->addChild(m_cyl);

        m_cube = Mesh::create::cube({5.f, 5.f, 5.f});
        m_cube->setMaterialInst(unlit);
        m_cube->setWorldTranslation({0.0f, -7.0f, 0.0f});
        Render::getRoot()->addChild(m_cube);

        m_sphere = Mesh::create::sphere(2.5f, 10, 10);
        m_sphere->setMaterialInst(unlit);
        m_sphere->setWorldTranslation({5.0f, -7.0f, 0.0f});
        Render::getRoot()->addChild(m_sphere);
    }

    void onUpdate(float dt) override
    {
        for (auto& s : m_spines)
        {
            s->onUpdate(dt);
            //s->translateWorld({0.0f, 0.0f, dt*0.1f});
            //s->rotateLocal(Rotator(0, 0, dt));
        }

        //m_spines[1]->rotateLocal(Rotator(0, dt, 0));
        m_cyl->rotateLocal(Rotator(dt, dt, dt));
    }

private:
    std::vector<w4::sptr<Spine>> m_spines;
    w4::sptr<Mesh> m_cube;
    w4::sptr<Mesh> m_cyl;
    w4::sptr<Mesh> m_sphere;
};


W4_RUN(SpineGist)




