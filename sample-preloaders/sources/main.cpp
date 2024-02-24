#include "W4Framework.h"
#include "W4Common.h"

W4_USE_UNSTRICT_INTERFACE

class LoadersTest : public IGame {
public:
    void onStart() override
    {
        auto btn1 = createWidget<Button>(nullptr, "SHOW LOADER",  ivec2{540, 300});
        auto btn2 = createWidget<Button>(nullptr, "CHANGE",       ivec2{540, 500});
        btn1->onTap([btn1]()
        {
            static bool visible = true;
            if(visible)
            {
                Platform::showLoader();
                btn1->setText("HIDE LOADER");
            }
            else
            {
                Platform::hideLoader();
                btn1->setText("SHOW LOADER");
            }
            visible = !visible;

        });

        btn2->onTap([btn2]()
        {
            static bool style = true;
            if(style)
                Platform::setLoaderStyle("loader1");
            else
                Platform::setLoaderStyle("loader2");
            style = !style;
        });
    }





};

W4_RUN(LoadersTest)
