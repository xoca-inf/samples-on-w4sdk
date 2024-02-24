#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct ShadertoyDemo : public IGame
{
public:
    void onStart() override
    {
        m_cube = Mesh::create::cube({2.f, 2.f, 2.f});
        m_matInst = Material::get("materials/topologica.mat")->createInstance();
        m_matInst->setParam("specColor", vec4(0.f, 0.f, 1.f, 1.f));
        m_cube->setMaterialInst(m_matInst);
        Render::getRoot()->addChild(m_cube);

        event::Touch::Move::subscribe(std::bind(&ShadertoyDemo::onMove, this, std::placeholders::_1));
    }

    void onMove(const event::Touch::Move& evt)
    {
        const auto& screenSize = Platform::getSize();
        m_matInst->setParam("mousePos", vec2(static_cast<float>(evt.point.x) / screenSize.w, static_cast<float>(evt.point.y) / screenSize.h));
    }

    void onUpdate(float dt) override
    {
        m_cube->rotateLocal(Rotator{dt,dt,dt});
    }

private:
    sptr<Mesh> m_cube;
    sptr<MaterialInst> m_matInst;
};

W4_RUN(ShadertoyDemo)