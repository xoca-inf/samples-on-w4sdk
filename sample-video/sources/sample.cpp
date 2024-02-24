#include "W4Framework.h"
#include "Generation.h"

W4_USE_UNSTRICT_INTERFACE

class VideoSample : public IGame
{
public:

    void onStart() override
    {
        auto cam = Render::getScreenCamera();
        cam->setWorldTranslation({0.f, 0.f, -650.f});
        cam->setAspect(9.f / 16.f );
        cam->setFov(40.f);
        cam->setClearColor({0,0,0,1});

        constexpr auto xGrid = 20;
        constexpr auto yGrid = 20;

        constexpr float xgrid = xGrid;
        constexpr float ygrid = yGrid;

        constexpr auto ux = 1.f / xgrid;
        constexpr auto uy = 1.f / ygrid;

        vec2 units(ux, uy);

        m_video = make::sptr<platform::Video>("video/RobotDancing.mp4");
        m_root = make::sptr<RootNode>("root");

        vec2 cubeSize { 480.f / xgrid, 480.f / ygrid };

        auto xHalfGrid = xGrid / 2.f;
        auto yHalfGrid = yGrid / 2.f;


        for (auto i = 0; i < xGrid; i ++ )
        {
            for (auto j = 0; j < yGrid; j++)
            {
                auto hue = i / xGrid;
                auto saturation = 1 - j / yGrid;

                auto m_matInst = Material::get("materials/simple.mat")->createInstance();
                m_matInst->setTexture(TextureId::TEXTURE_0, m_video->getTexture());

                vec3 hsl{i / xgrid,
                     1.f - j / ygrid,
                     0.5f};

                auto color = color::hslToRgb(hsl);

                m_hsl.push_back(hsl);

                auto cube = Mesh::create::cube({cubeSize.x, cubeSize.y , cubeSize.x});

                auto x = ( i - xHalfGrid ) * cubeSize.x;
                auto y = ( j - yHalfGrid ) * cubeSize.y;

                cube->setLocalTranslation({x, y, 0.f});

                m_matInst->setParam("units", units);
                m_matInst->setParam("offset", vec2(i, j));
                m_matInst->setParam("color", color);

                vec2 delta = {0.001f * (0.5f - random(0.f, 1.f)),
                              0.001f * (0.5f - random(0.f, 1.f))};
                m_delta.push_back(delta);

                cube->setMaterialInst(m_matInst);
                m_root->addChild(cube);
                cube->setEnabled(false);
                m_meshes.push_back(cube);
            }
        }

        Render::getPass(0)->setRoot(m_root);

        gui::setVirtualResolution({1080, 1920});

        auto bigPlayBtn = gui::createWidget<gui::Image>(nullptr, "ui/play.png", 208, 256, 540, 1000, "bigPlay");
        bigPlayBtn->setHorizontalAlign(gui::HorizontalAlign::Center);
        bigPlayBtn->setVerticalAlign(gui::VerticalAlign::Center);
        bigPlayBtn->onTap([this, bigPlayBtn]()
        {
            m_video->play(true);
            bigPlayBtn->setVisible(false);
            m_started = true;

            for(auto& mesh: m_meshes)
                mesh->setEnabled(true);
        });


        event::Touch::Move::subscribe([this](auto& touch)
                                          {
                                              m_mouse.x = touch.point.x;
                                              m_mouse.y = touch.point.y;

                                              m_mouse.x -= (Platform::getSize().w / 2);
                                              m_mouse.y -= (Platform::getSize().h / 2);
                                              m_mouse.y *= 0.3f;
                                          });



        auto& pass = Render::getFxPass();
        auto rtIdx0 = pass.createRelativeRenderTarget(vec2(0.5f, 0.5f));
        auto rtIdx1 = pass.createRelativeRenderTarget(vec2(0.2f, 0.2f));
        auto rtIdx2 = pass.createRelativeRenderTarget(vec2(0.2f, 0.2f));

        auto downSampleMap = FxPass::getDefaultMaterialInst();
        auto brightMat = Material::get("materials/bright.mat")->createInstance();
        brightMat->setParam("brightPassThreshold", 0.5f);

        auto blurMatVert = Material::get("materials/blurVert.mat")->createInstance();
        blurMatVert->setParam("offset", math::vec3(0.f, 1.3846153846f, 3.2307692308f));
        blurMatVert->setParam("weight", math::vec3(0.2270270270f, 0.3162162162f, 0.0702702703f));

        auto blurMatHor = Material::get("materials/blurHor.mat")->createInstance();
        blurMatHor->setParam("offset", math::vec3(0.f, 1.3846153846f, 3.2307692308f));
        blurMatHor->setParam("weight", math::vec3(0.2270270270f, 0.3162162162f, 0.0702702703f));

        auto composeMat = Material::get("materials/compose.mat")->createInstance();

        pass.createFx("bloom")
            .addFxStep({render::FxPass::FxPassRT_Id::Input}, rtIdx0, downSampleMap)
            .addFxStep({rtIdx0}, rtIdx1, brightMat)
            .addFxStep({rtIdx1}, rtIdx2, blurMatVert)
            .addFxStep({rtIdx2}, rtIdx1, blurMatHor)
            .addFxStep({render::FxPass::FxPassRT_Id::Input, rtIdx1}, render::FxPass::FxPassRT_Id::Screen, composeMat)
            ;

        pass.setCurrentFx("bloom");

        Render::enableFx(true);
    }

    void onUpdate(float dt) override
    {
        if(m_started)
        {
            auto time = Render::getElapsedTime() * 0.05f;

            auto cam = Render::getScreenCamera();

            vec3 pos = cam->getLocalTranslation();
            pos.x += ( m_mouse.x - pos.x);
            pos.y += (-m_mouse.y - pos.y);
            cam->setLocalTranslation(pos);

            vec3 lookVec = -pos;
            auto lookTo = lookVec.toRotator(vec3(0, 1, 0));
            cam->setLocalRotation(lookTo);

            for(auto i = 0; i < m_meshes.size(); ++i)
            {
                auto cube = m_meshes[i];
                auto color = m_hsl[i];
                auto h = (static_cast<int>(360 * (color.x + time)) % 360) / 360.f;

                vec3 hsl(h, color.y, 0.5f);
                cube->getMaterialInst()->setParam("color", color::hslToRgb(hsl));
            }

            if ( counter % 1000 > 200 )
            {

                for (auto i = 0; i < m_meshes.size(); ++i)
                {
                            auto d = m_delta[i];
                            auto cube = m_meshes[i];
                            auto rot = cube->getLocalRotation().euler();
                            rot.x += 10 * d.x;
                            rot.y += 10 * d.y;
                            cube->setLocalRotation(rot);

                            auto pos = cube->getLocalTranslation();

                            pos.x += 200 * d.x;
                            pos.y += 200 * d.y;
                            pos.z += 400 * d.x;

                            cube->setLocalTranslation(pos);
                 }

            }
            if ( counter % 1000 == 0 )
            {
                for (auto& d: m_delta)
                {
                    d.x *= - 1;
                    d.y *= - 1;
                }
            }

            ++counter;
        }
    }


private:
    sptr<RootNode> m_root;
    sptr<Video> m_video;

    bool m_started = false;

    vec2 m_mouse;

    std::vector<vec2> m_delta;
    std::vector<vec3> m_hsl;
    std::vector<sptr<Mesh>> m_meshes;

    uint counter = 0;
};

W4_RUN(VideoSample)
