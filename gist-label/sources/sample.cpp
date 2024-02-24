#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct GUILabelGist : public IGame
{
    void onStart() override
    {
        createWidget<Label>(nullptr, "TAP ON ELAPSED TIME FOR RESET", ivec2(540, 200));
        createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));

        label = createWidget<Label>(nullptr, "", ivec2(540, 800));
        label->setHorizontalAlign(HorizontalAlign::Center);
        label->setVerticalAlign(VerticalAlign::Center);
        label->setFontSize(100);
        label->setFont(Font::Copperplate);
        label->setTextColor(color::Yellow);
        label->onTap([this] { elapsedTime = 0;});
    }

    void onUpdate(float dt) override
    {
        elapsedTime += dt;
        label->setText(utils::format("%.2f sec", elapsedTime));
    }

    sptr<gui::Label> label;
    float elapsedTime = 0;

};
W4_RUN(GUILabelGist)
