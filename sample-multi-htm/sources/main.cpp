#include "W4Framework.h"
#include "W4Common.h"

W4_USE_UNSTRICT_INTERFACE

class LoadersTest : public IGame {
public:
    void onStart() override
    {
        auto btn = createWidget<Button>(nullptr, "SHOW LOADER",  ivec2{540, 300});
        btn->onTap([&btn]()
        {
            static bool visible = true;
            if(visible) Platform::showLoader();
            else        Platform::hideLoader();
            visible = !visible;
            btn->setText((visible) ? "HIDE LOADER" : "SHOW LOADER");

        });
    }





};

W4_RUN(LoadersTest)
