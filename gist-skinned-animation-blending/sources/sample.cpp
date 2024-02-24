#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

class GistSkinnedAnimationBlending : public IGame
{
    void onStart() override
    {
        Render::getScreenCamera()->setWorldTranslation({0,50,-150});
        Render::getRoot()->addChild(Asset::get("SK_PaperBoy_T_Pose.w4a")->getFirstRoot());
        Render::getRoot()->rotateLocal({0, PI * .8, 0});
        Render::getRoot()->traversalTyped<SkinnedMesh>([this](cref<SkinnedMesh> node)
        {
            m_skinnedMeshes.emplace_back(node);
            node->getAnimator(0).setIsLooped(true);
            node->getAnimator(1).setIsLooped(true);
            node->play("Walk");
          });

        Timer::addTask(blendingTime + 1.533333f, [this, isWalk = true]() mutable
        {
            for (auto & smesh: m_skinnedMeshes)
            {
                smesh->play(isWalk ? "Run" : "Walk", blendingTime);
            }
            isWalk = !isWalk;
            return true;
        }, true);

        m_label = gui::createWidget<gui::Label>(nullptr, "walk: 100%, run: 100%", ivec2{540, 200});
        gui::createWidget<Label>(nullptr, "CLICK ON [?] FOR CODE VIEW ", ivec2(540, 1800));
    }

    void onUpdate(float) override
    {
        m_label->setText(utils::format("walk: %.1f%%, run: %.1f%%",
                m_skinnedMeshes.front()->getAnimator("Walk").getWeight() * 100,
                m_skinnedMeshes.front()->getAnimator("Run").getWeight() * 100));
    }

    static constexpr float blendingTime = 3.833333f;
    sptr<gui::Label> m_label;
    std::vector<sptr<SkinnedMesh>> m_skinnedMeshes;
};

W4_RUN(GistSkinnedAnimationBlending);