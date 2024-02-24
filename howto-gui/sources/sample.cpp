#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

class W4_GUI_Demo : public IGame
{
public:
    void onStart() override
    {
        auto cam = Render::getScreenCamera();
        cam->setWorldTranslation({0.f, 0, -25.f});
        cam->setAspect(9.f / 16.f);

        m_shape= Mesh::create::cube({5,5,5});

        w4::math::color color {color::White};
        auto matInst = Material::getDefault()->createInstance();
        matInst->setParam("baseColor", color);
        m_shape->setMaterialInst(matInst);

        Render::getRoot()->addChild(m_shape);

        // --- GUI Code Here

        gui::setVirtualResolution({1080, 1920});

        auto ui = createWidget<Widget>(nullptr, "RootUI");

        m_eulerLabel = gui::createWidget<Label>(ui, "Euler Label", ivec2(25, 140));
        m_eulerLabel->setHorizontalAlign(HorizontalAlign::Left);
        m_eulerLabel->setVerticalAlign(VerticalAlign::Top);
        m_eulerLabel->setFontSize(50);
        m_eulerLabel->setWidth(475);
        m_eulerLabel->setHorizontalPolicy(SizePolicy::Fixed);

        auto img = createWidget<gui::Image>(ui, "ui/W4Logo.png", 200, 200, 1050, 140);
             img->setHorizontalAlign(HorizontalAlign::Right);
             img->setVerticalAlign(VerticalAlign::Top);

        auto sliders = createWidget<Widget>(ui, "Sliders");
             sliders->setPosition({65, 1650});

        auto colorLabel = createWidget<Label>(sliders, "CubeColor", ivec2({100, 100}));
             colorLabel->setText(utils::format("R %.2f\nG %.2f\nB %.2f", color.r, color.g, color.b));
             colorLabel->setFontSize(50);

        auto sliderRed   = gui::createWidget<Slider>(sliders, 0.0f, 1.0f, 0.01f, 1.f, ivec2(365, 40),  "sliderRed");
        auto sliderGreen = gui::createWidget<Slider>(sliders, 0.0f, 1.0f, 0.01f, 1.f, ivec2(365, 100),  "sliderGreen");
        auto sliderBlue  = gui::createWidget<Slider>(sliders, 0.0f, 1.0f, 0.01f, 1.f, ivec2(365, 160), "sliderBlue");

        sliderRed->setBackground(color::Red, color::LightSalmon);
        sliderGreen->setBackground(color::Green, color::LightGreen);
        sliderBlue->setBackground(color::Blue, color::LightBlue);

        sliderRed->onValueChanged([=](float v)
        {
            auto color = matInst->getParam<vec4>("baseColor");
            color.r = v;
            matInst->setParam("baseColor", color);
            colorLabel->setText(utils::format("R %.2f\nG %.2f\nB %.2f", color.r, color.g, color.b));
        });

        sliderGreen->onValueChanged([=](float v)
        {
            auto color = matInst->getParam<vec4>("baseColor");
            color.g = v;
            matInst->setParam("baseColor", color);
            colorLabel->setText(utils::format("R %.2f\nG %.2f\nB %.2f", color.r, color.g, color.b));
        });

        sliderBlue->onValueChanged([=](float v)
        {
            auto color = matInst->getParam<vec4>("baseColor");
            color.b = v;
            matInst->setParam("baseColor", color);
            colorLabel->setText(utils::format("R %.2f\nG %.2f\nB %.2f", color.r, color.g, color.b));
        });


        auto rotateBtn = gui::createWidget<Button>(ui, "Stop", ivec2(920, 1750));
        rotateBtn->setSizePolicy(SizePolicy::Auto, SizePolicy::Fixed);
        rotateBtn->setWidth(270);
        rotateBtn->setFontSize(50);
        rotateBtn->onTap([this, rotateBtn]()
                         {
                             m_rotate = !m_rotate;
                             if (m_rotate)
                             {
                                 rotateBtn->setText("Stop");
                             }
                             else
                             {
                                 rotateBtn->setText("Rotate");
                             }
                         });



        auto imageButton = createWidget<ImageButton>(ui, "ui/cast.png", "ui/cast_pressed.png", ivec2(200,180), ivec2(662, 1750));
        imageButton->onTap([=]()
                           {
                               sliderRed->setValue(random(0.f, 1.f));
                               sliderGreen->setValue(random(0.f, 1.f));
                               sliderBlue->setValue(random(0.f, 1.f));
                           });

        auto selectColor = createWidget<ComboBox>(ui, "Color", ivec2(665, 240));
        selectColor->addItem("White");
        selectColor->addItems({"Red", "Green", "Blue"});
        selectColor->insertItem(0, "Black");
        selectColor->onCurrentIndexChanged([=](int32_t v)
                                           {
                                               w4::math::color color;
                                               switch(v)
                                               {
                                                   case 0:
                                                       color = color::Black;
                                                       break;
                                                   case 1:
                                                       color = color::White;
                                                       break;
                                                   case 2:
                                                       color = color::Red;
                                                       break;
                                                   case 3:
                                                       color = color::Green;
                                                       break;
                                                   case 4:
                                                   default:
                                                       color = color::Blue;
                                                       break;
                                               };

                                               sliderRed->setValue(color.r);
                                               sliderGreen->setValue(color.g);
                                               sliderBlue->setValue(color.b);
                                           });

        auto showUI = createWidget<Widget>(nullptr, "WidgetShowUI");
        showUI->setPosition({540, 70});

        auto hideTitle = gui::createWidget<Label>(showUI, "Show UI", ivec2(0, 0));
        hideTitle->setFontSize(50);
        hideTitle->setWidth(400);
        hideTitle->setHorizontalPolicy(SizePolicy::Fixed);
        hideTitle->setTextAlign(HorizontalAlign::Right);

        auto checkbox = createWidget<Checkbox>(showUI, ivec2(-125, 0));
        checkbox->setChecked(true);
        checkbox->onStateChanged([ui](bool checked) {
            ui->setVisible(checked);
        });
    }

    void onUpdate(float dt) override
    {
        if (m_rotate)
        {
            m_shape->rotateLocal({dt, dt, dt});

            const auto& rot = m_shape->getLocalRotation();
            auto pitch = rot.eulerPitch() * RAD2DEG;
            auto yaw   = rot.eulerYaw()   * RAD2DEG;
            auto roll  = rot.eulerRoll()  * RAD2DEG;
            m_eulerLabel->setText(utils::format("Pitch %.1f°\nYaw %.1f°\nRoll %.1f°", pitch, yaw, roll));
        }
    }

private:
    sptr<Mesh>           m_shape;
    sptr<gui::Label>     m_eulerLabel;
    bool                 m_rotate = true;
};
W4_RUN(W4_GUI_Demo)




