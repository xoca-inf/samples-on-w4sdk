#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct MinimalAppGist : public IGame
{
    void onStart() override
    {
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));

        for(int y = 0; y < 16; ++y)
            for(int x = 0; x < 9; ++x)
            {
                auto shape = (x%2)
                    ? Mesh::create::cube({1,1,1})
                    : Mesh::create::torus(.6, 0.3, 32, 32, TWO_PI);

                shape->rotateLocal(Rotator(random<float>(-5,5),random<float>(-5,5), random<float>(-5,5)));
                Render::getRoot()->addChild(shape);
                shape->setWorldTranslation({2.0f * x - 8.0f ,2.0f * y - 15.5f, 20});
                shape->getMaterialInst()->setParam("baseColor", color::random());
                m_shapes.emplace_back(shape);
            }
    }

    void onUpdate(float dt) override
    {
        static float time = 0; time += dt;
        for(auto i=0; i< m_shapes.size();++i)
           m_shapes[i]->rotateLocal(Rotator(i/12*dt*sin(time),i/12*dt*sin(time),dt));
    }
private:
   std::vector<sptr<Mesh>> m_shapes;
};

W4_RUN(MinimalAppGist)
