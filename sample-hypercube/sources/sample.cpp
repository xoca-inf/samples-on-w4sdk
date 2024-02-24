#include "W4Framework.h"
#include "Asset.h"

W4_USE_UNSTRICT_INTERFACE

class HypercubeSample : public IGame
{
public:

    HypercubeSample() = default;
    ~HypercubeSample() = default;

    void onConfig() override
    {
        Game::Config.VFSClean = true;
        Game::Config.UseDefaultRenderPass = false;
    }

    void onStart() override
    {
        auto cubemap = Cubemap::fromImages({
                                      resources::Image::get("textures/right.png"),
                                      resources::Image::get("textures/left.png"),
                                      resources::Image::get("textures/top.png"),
                                      resources::Image::get("textures/bottom.png"),
                                      resources::Image::get("textures/front.png"),
                                      resources::Image::get("textures/back.png"),
                                     });

        m_camera = Render::getScreenCamera();
        m_camera->setFov(90.f);
        m_camera->setClearMask(ClearMask::Color | ClearMask::Depth | ClearMask::Skybox);
        m_camera->getSkybox()->setCubemap(cubemap);

        m_textureCamera = make::sptr<Camera>("texture cam", m_camera->getFov(), m_camera->getAspect(), m_camera->getNear(), m_camera->getFar());
        m_textureCamera->setWorldTransform(m_camera->getWorldTransform());
        auto renderTarget = make::sptr<TextureRenderTarget>(math::size{1024, 1024});
        m_texture = renderTarget->getColorTexture();
        m_textureCamera->setRenderTarget(renderTarget);
        m_textureCamera->setFov(90.f);
        m_textureCamera->setClearMask(ClearMask::Color | ClearMask::Depth | ClearMask::Skybox);
        m_textureCamera->getSkybox()->setCubemap(cubemap);

        m_unlit = Material::get("materials/unlit.mat")->createInstance();

        auto cube = Mesh::create::cube({5.f, 5.f, 5.f});
        cube->setMaterialInst(m_unlit);
        m_root->addChild(cube);

        m_texturePassIdx = Render::addPass();
        auto texturePass = Render::getPass(m_texturePassIdx);
        texturePass->setCamera(m_textureCamera);
        texturePass->setRoot(m_root);
        texturePass->setPreRenderHandler(
                [this]()
                {
                    m_unlit->setTexture(TextureId::TEXTURE_0, Texture::get("textures/obsidian2.jpg"));
                }
        );

        texturePass->setPostRenderHandler(
                [this]()
                {
                    m_unlit->setTexture(TextureId::TEXTURE_0, m_texture);
                }
        );

        m_mainPassIdx = Render::addPass();
        Render::getPass(m_mainPassIdx)->setRoot(m_root);
    }

    void onUpdate(float dt) override
    {
        auto rot = m_root->getWorldRotation();
        rot += math::Rotator(dt, dt, dt);
        m_root->setWorldRotation(rot);

        rot = m_camera->getWorldRotation();
        rot += math::Rotator(0.f, 0.f, dt);
        m_camera->setWorldRotation(rot);
        m_textureCamera->setWorldTransform(m_camera->getWorldTransform());
    }

private:
    sptr<RootNode> m_root = make::sptr<RootNode>("root");
    sptr<Camera> m_camera;
    sptr<Camera> m_textureCamera;
    sptr<MaterialInst> m_unlit;
    sptr<Texture> m_texture;

    size_t m_texturePassIdx;
    size_t m_mainPassIdx;
};

W4_RUN(HypercubeSample)
