#include "W4Framework.h"
W4_USE_UNSTRICT_INTERFACE

const std::string PLAYER_RUN  = "A_PaperBoy_Run";
const std::string PLAYER_DOWN = "A_PaperBoy_Jump_Down";
const std::string PLAYER_UP   = "A_PaperBoy_Jump_Up";

static const float finishRocksX = -380.f;

const char* (animationNames)[] = {
    "A_PaperBoy_Jump_Down",
    "A_PaperBoy_Jump_Up",
    "A_PaperBoy_Run",
};


class PaperRun: public IGame
{
    void onConfig() override
    {
        Game::Config.VFSClean = true;
        Game::Config.EnableFrustumCulling = false;
    }

    void onStart() override
    {
        auto asset = Asset::get("SK_PaperBoy_T_Pose.w4a");

        playerRunPos  = {-100.0f, -370.f, 0.f};
        playerJumpPos = playerRunPos + vec3({0.f, 50.f, 0.f});

        m_player = asset->getFirstRoot();
        m_player->setWorldRotation(Rotator{0, PI, 0});
        m_player->setWorldTranslation(playerRunPos);
        m_player->rotateLocal({0, -PI * 0.5f, 0});
        Render::getRoot()->addChild(m_player);


        m_rocks = makeRocks();
        Render::getRoot()->addChild(m_rocks);

        m_background = Mesh::create::plane({1920.f, 1080.f});
        m_background->setWorldTranslation({0.0f, 0.f, 63.f});
        backMat = Material::get("materials/background.mat")->createInstance();
        backMat->setTexture(TextureId::TEXTURE_0, Texture::get("textures/layer_1.png"));
        backMat->setTexture(TextureId::TEXTURE_1, Texture::get("textures/layer_2.png"));
        m_background->setMaterialInst(backMat);
        Render::getRoot()->addChild(m_background);

        m_road = Mesh::create::plane({1920.f, 170.f});
        m_road->setWorldTranslation({318, -386.0f, 22.f});
        m_road->setWorldRotation({1.099557, 0.0, 0.0});
        roadMat = Material::get("materials/road.mat")->createInstance();
        roadMat->setTexture(TextureId::TEXTURE_0, Texture::get("textures/layer_trail.png"));
        m_road->setMaterialInst(roadMat);
        Render::getRoot()->addChild(m_road);

        Render::getScreenCamera()->setWorldTranslation({0.f, 0.f, -450.f});
        Render::getScreenCamera()->setFov(60);

        playAnim(PLAYER_RUN, true);

        setAnimEvent(PLAYER_UP, 0.25f, [&](Animator& a)
        {
            m_currentTime = 0.f;
            m_jumpUp = true;
        });

        restartRocks();
    }

    sptr<Node> makeRocks()
    {
        auto rocks = Asset::get("rocks.w4a")->getFirstRoot();
        {
            rocks->setLocalTranslation({0.f, -386.0f, -1022.f});
            rocks->setLocalScale(vec3(0.2f));
            rocks->setLocalRotation({-0.387350, 0.128162, 0.040123});

            {
                auto rock1 = rocks->findChildren("Rock1").front()->as<Mesh>();
                auto backMat = Material::get("materials/rocks.mat")->createInstance();
                backMat->setTexture(TextureId::TEXTURE_0, Texture::get("textures/Rock1Texture.png"));
                rock1->setMaterialInst(backMat);
                rock1->setEnabled(false);
            }
            {
                auto rock2 = rocks->findChildren("Rock2").front()->as<Mesh>();
                auto backMat = Material::get("materials/rocks.mat")->createInstance();
                backMat->setTexture(TextureId::TEXTURE_0, Texture::get("textures/Rock2Texture.png"));
                rock2->setMaterialInst(backMat);
                rock2->setEnabled(true);
            }
            {
                auto rock3 = rocks->findChildren("Rock3").front()->as<Mesh>();
                auto backMat = Material::get("materials/rocks.mat")->createInstance();
                backMat->setTexture(TextureId::TEXTURE_0, Texture::get("textures/Rock3Texture.png"));
                rock3->setMaterialInst(backMat);
                rock3->setEnabled(true);
            }
            {
                auto rock4 = rocks->findChildren("Rock4").front()->as<Mesh>();
                auto backMat = Material::get("materials/rocks.mat")->createInstance();
                backMat->setTexture(TextureId::TEXTURE_0, Texture::get("textures/Rock4Texture.png"));
                rock4->setMaterialInst(backMat);
                rock4->setEnabled(false);
            }
        }
        return rocks;
    }


    void restartRocks()
    {
        m_rocks->setWorldTranslation({318, -386.0f, 22.f});
        m_readyToJump = true;
    }

    void jump()
    {
        playAnim(PLAYER_UP);
    }

    void playAnim(const std::string& animationName,  bool loop = false)
    {
        Render::getRoot()->traversal([&, loop](Node &node)
        {
            if (node.is<SkinnedMesh>())
            {
                auto &smesh = *node.asRaw<SkinnedMesh>();
                smesh.play(animationName, 0.1f);
                smesh.getAnimator(animationName).setIsLooped(loop);
                auto dur = smesh.getAnimator(animationName).getDuration();
            }
        });
    }

    void setAnimEvent(const std::string& animationName, float time, const std::function<void(Animator&)>& func)
    {
        Render::getRoot()->traversal([&,func](Node &node)
                                     {
                                         if (node.is<SkinnedMesh>())
                                         {
                                             auto &smesh = *node.asRaw<SkinnedMesh>();
                                             smesh.getAnimator(animationName).setTimeHandler(time, func);
                                         }
                                     });
    }

    sptr<Node> getPlayer() const
    {
        return m_player;
    }

    void onUpdate(float dt) override
    {
        if (m_jumpUp)
        {
            m_currentTime += dt;

            if (m_currentTime > m_jump_up_time)
            {
                m_currentTime -= m_jump_up_time;
                m_jumpDown = true;
                m_jumpUp   = false;
                playAnim(PLAYER_DOWN);
            }
            else
            {
                auto v = math::lerp(playerRunPos, playerJumpPos, m_currentTime / m_jump_up_time);
                getPlayer()->setLocalTranslation(v);
            }
        }
        else if (m_jumpDown)
        {
            m_currentTime += dt;
            if (m_currentTime > m_jump_down_time)
            {
                getPlayer()->setLocalTranslation(playerRunPos);
                m_jumpDown = false;
                playAnim(PLAYER_RUN, true);
            }
            else
            {
                auto v = math::lerp(playerJumpPos, playerRunPos, m_currentTime / m_jump_down_time);
                getPlayer()->setLocalTranslation(v);
            }
        }
        // m_rocks
        {
            auto t = m_rocks->getLocalTranslation();
            t.x -= dt * m_playerSpeed;
            m_rocks->setLocalTranslation(t);
            if(m_readyToJump && (t.x < 30.f))
            {
                m_readyToJump = false;
                jump();
            }
            else if (t.x < finishRocksX)
            {
                restartRocks();
            }
        }
    }

private:

    vec3 playerRunPos;
    vec3 playerJumpPos;

    sptr<Node> m_player;
    sptr<Node> m_rocks;
    sptr<Mesh> m_background;
    sptr<Mesh> m_road;

    sptr<MaterialInst> roadMat;
    sptr<MaterialInst> backMat;

    float m_jump_up_time   = 0.233333f;
    float m_jump_down_time = 0.186667f;
    float m_currentTime    = 0.f;
    float m_playerSpeed    = 267.f;

    bool m_jumpUp      = false;
    bool m_jumpDown    = false;
    bool m_readyToJump = false;
};

W4_RUN(PaperRun)

