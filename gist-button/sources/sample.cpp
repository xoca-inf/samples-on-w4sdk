#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct GUILabelGist : public IGame
{
    void onStart() override
    {
        gui::createWidget<Label>(nullptr, "USING GUI BUTTON SNIPPET", ivec2{540, 200});
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));

        auto countLabel = createWidget<Label>(nullptr, "0", ivec2{540, 700});
        auto btn = createWidget<Button>(nullptr, "BUTTON",  ivec2{540, 1000});
             btn->onTap([countLabel]()
            {
                static uint count = 0;
                countLabel->setText(utils::format("%i", ++count));
            });

    }
};
W4_RUN(GUILabelGist)
