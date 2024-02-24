#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct PlayAudioGist : public IGame
{
    void onStart() override
    {
        auto audio = make::sptr<Audio>("tururu.mp3");
             audio->setLooped(true);

        auto label = createWidget<gui::Label>(nullptr, "Touch to play", ivec2{540,200});
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));

        event::Touch::Begin::subscribe([audio, label](auto unused)
        {
            if (audio->isPlaying())
            {
                audio->stop();
                label->setText("Touch to play");
            }
            else
            {
                audio->play();
                label->setText("Touch to stop");
            }
        });
    }
};
W4_RUN(PlayAudioGist)
