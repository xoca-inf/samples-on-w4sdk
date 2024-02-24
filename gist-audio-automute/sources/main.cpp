#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct emptyApp : public IGame
{
    void onStart() override
    {
        auto sz = Render::getSize();
        gui::createWidget<Label>(nullptr, "OLOLO, ITS ONLY STUB", ivec2(sz.w / 2.0, sz.h/2.0));
        bg_audio = make::sptr<Audio>("background.mp3");
        bg_audio->setLooped(true);
        bg_audio->play();

    }

    void onPause() override {
        mute(true);
    }

    void onResume() override {
        mute(false);
    }


private:
    void mute(bool flag) {
        if (flag)
        {
            if (bg_audio && bg_audio->isPlaying())
                bg_audio->stop();

        }
        else
        {
            if (bg_audio && !bg_audio->isPlaying())
                bg_audio->play();

        }
    }
    sptr<Audio> bg_audio;

};

W4_RUN(emptyApp)
