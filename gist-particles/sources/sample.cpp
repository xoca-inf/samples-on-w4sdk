#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct LoadParticlesGist : public IGame
{
    void addParticle(math::vec3::cref pos)
    {
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));
        auto emitter = ParticlesEmitter::get("win.part");
             emitter->setWorldScale({.1,.1,.1});
             emitter->setWorldTranslation(pos);
             emitter->setTransformMode(ParticlesEmitterParameters::TransformMode::EMITTER);
             emitter->start();
        Render::getRoot()->addChild(emitter);
    }
    void onStart() override
    {
        addParticle({-2, 0, 0});
        addParticle({ 2, 0, 0});
        addParticle({ 0,-2, 0});
        addParticle({ 0, 2, 0});
    }

    void onUpdate(float dt) override
    {
        Render::getRoot()->rotateLocal(Rotator(0,0,dt));
    }

};

W4_RUN(LoadParticlesGist)
