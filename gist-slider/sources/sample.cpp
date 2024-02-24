#include "W4Framework.h"


W4_USE_UNSTRICT_INTERFACE

class GUISliderGist : public IGame
{
public:
    void onStart() override
    {
        auto shape = Mesh::create::cube({5,5,5});
        shape->setWorldTranslation({0.f, 0, 25.f});
        Render::getRoot()->addChild(shape);

        vec4 color(1.f);
        auto matInst = Material::getDefault()->createInstance();
        matInst->setParam("baseColor", color);
        shape->setMaterialInst(matInst);

        // --- GUI Code Here

        gui::createWidget<Label>(nullptr, "USING GUI SLIDER SNIPPET", ivec2{540, 200});
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));

        auto sliders = gui::createWidget<gui::Widget>(nullptr, "Sliders");
        sliders->setPosition({540, 1350});

        auto sliderGray  = gui::createWidget<gui::Slider>(sliders, 0.0f, TWO_PI, 0.02f, 0.f, ivec2(0, 10), "sliderBlue");
        auto sliderRed   = gui::createWidget<gui::Slider>(sliders, 0.0f, 1.0f, 0.01f, 1.f, ivec2(0, 65), "sliderRed");
        auto sliderGreen = gui::createWidget<gui::Slider>(sliders, 0.0f, 1.0f, 0.01f, 1.f, ivec2(0, 120), "sliderGreen");
        auto sliderBlue  = gui::createWidget<gui::Slider>(sliders, 0.0f, 1.0f, 0.01f, 1.f, ivec2(0, 175), "sliderBlue");

        sliderGray->setHorizontalAlign(HorizontalAlign::Center);
        sliderGray->setVerticalAlign(VerticalAlign::Center);

        sliderRed->setBackground(color::Red, w4::math::color::LightSalmon);
        sliderRed->setHorizontalAlign(HorizontalAlign::Center);
        sliderRed->setVerticalAlign(VerticalAlign::Center);

        sliderGreen->setBackground(color::Green, w4::math::color::LightGreen);
        sliderGreen->setHorizontalAlign(HorizontalAlign::Center);
        sliderGreen->setVerticalAlign(VerticalAlign::Center);

        sliderBlue->setBackground(color::Blue, w4::math::color::LightBlue);
        sliderBlue->setHorizontalAlign(HorizontalAlign::Center);
        sliderBlue->setVerticalAlign(VerticalAlign::Center);

        sliderGray->onValueChanged([=](float v)
                                   {
                                       shape->setLocalRotation({v, v, v});
                                   });

        sliderRed->onValueChanged([=](float v)
                                  {
                                      auto color = matInst->getParam<vec4>("baseColor");
                                      color.r = v;
                                      matInst->setParam("baseColor", color);
                                  });

        sliderGreen->onValueChanged([=](float v)
                                  {
                                        auto color = matInst->getParam<vec4>("baseColor");
                                        color.g = v;
                                        matInst->setParam("baseColor", color);
                                  });

        sliderBlue->onValueChanged([=](float v)
                                    {
                                        auto color = matInst->getParam<vec4>("baseColor");
                                        color.b = v;
                                        matInst->setParam("baseColor", color);
                                    });

    }
};
W4_RUN(GUISliderGist)




