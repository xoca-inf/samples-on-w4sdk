#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

struct SampleObstacles : public IGame
{
    void onStart() override
    {
        m_startPoint = vec2(w4::Render::getSize().w/2, w4::Render::getSize().h/2);
        m_label = gui::createWidget<Label>(nullptr, "Click anywhere to move ", ivec2(540, 200));

        setupLight();
        setupScene();
        setupPlayer();

        event::Touch::Begin::subscribe(std::bind(&SampleObstacles::onTouchBegin, this, std::placeholders::_1));
        event::Touch::End::subscribe(std::bind(&SampleObstacles::onTouchEnd, this, std::placeholders::_1));
        event::Touch::Move::subscribe(std::bind(&SampleObstacles::onTouchMove, this, std::placeholders::_1));

    }

    void addMaterialToMesh(sptr<Mesh> mesh)
    {
        mesh->setMaterialInst(Material::getDefaultBlinn()->createInstance());
        mesh->getMaterialInst()->setTexture(w4::resources::TextureId::TEXTURE_0, Texture::get(ResourceGenerator(ColorTextureGenerator, color::random())));
    }

    void setupPlayer()
    {
        m_player = Mesh::create::sphere(1, 16, 16);
        Render::getRoot()->addChild(m_player);
        addMaterialToMesh(m_player);

        auto sphereIntersectionBeginCb = [this](const CollisionInfo& info)
        {
            const auto& source = info.source;
            const auto& target = info.target;

            m_collisionDirection = (info.source->getWorldTranslation() - info.target->getWorldTranslation()).normalizeSelf();

            //START of Debug info block
            auto posSource = info.source->getWorldTranslation();
            auto posTarget = info.target->getWorldTranslation();
            W4_LOG_DEBUG("Source collider position is %f:%f:%f", posSource.x, posSource.y, posSource.z);
            W4_LOG_DEBUG("Target collider position is %f:%f:%f", posTarget.x, posTarget.y, posTarget.z);
            W4_LOG_DEBUG("Collision Direction is %f:%f:%f", m_collisionDirection.x, m_collisionDirection.y, m_collisionDirection.z);
            //END of Debug info block

            m_label->setText("STAY AWAY!");
            m_isInIntersec = true;

        };
        auto sphereIntersectionEndCb = [this](const CollisionInfo& info)
        {
            //START of Debug info block
            const auto& source = info.source->getParent();
            const auto& target = info.target->getParent();
            W4_LOG_DEBUG("Intersection END source '%s' <-> '%s' target", source->getName().c_str(), target->getName().c_str());
            //END of Debug info block

            m_label->setText("Keep Moving");
            m_isInIntersec = false;
        };

        auto playerCollider = m_player->addCollider<core::Sphere>(1.f);
             playerCollider->setIntersectionBeginCallback(sphereIntersectionBeginCb);
             playerCollider->setIntersectionEndCallback(sphereIntersectionEndCb);
             playerCollider->setIntersecting(true);

    }

    void setupScene()
    {
        m_floor = Mesh::create::cube({25, 1, 25});
        Render::getRoot()->addChild(m_floor);
        addMaterialToMesh(m_floor);
        m_floor->setWorldTranslation({0., -1., 0.});

        for(int y = 0; y < 2; ++y)
            for(int x = 0; x < 2; ++x)
            {
                vec3 pos = vec3(10.*x - random<float>(4,6), 0., 10.*y - random<float>(4,6));

                auto size = vec3(random<float>(1,4),random<float>(1,3),random<float>(1,4));
                auto obstacle = Mesh::create::cube(size);

                m_floor->addChild(obstacle);
                obstacle->setWorldTranslation(pos);
                addMaterialToMesh(obstacle);

                sptr<Collider> obstacleCollider;
                //there are two possible way to generate a collider
                if (x%2)
                {
                    //we can add a collider to a mesh directly
                    obstacleCollider = obstacle->addCollider<core::AABB>(obstacle);
                }
                else
                {
                    //or we can use obstacle mesh only for collider generation
                    //and add the collider to root node, or level node, or whatever
                    obstacleCollider = m_floor->addCollider<core::AABB>(obstacle);
                    //in that case we need to copy a local transform of the mesh
                    obstacleCollider->setLocalTransform(obstacle->getLocalTransform());
                    //obstacle->setEbabled(false); //in the end, we can disable visible mesh
                }
                obstacleCollider->setIntersecting(true);
            }
    }

    void onTouchBegin(const Touch::Begin& evt)
    {
        auto end = vec2(evt.point.x, evt.point.y);
        m_speedVector = vec3(m_startPoint.x - end.x, 0.f,  end.y - m_startPoint.y).normalize();
    }

    void onTouchMove(const Touch::Move& evt)
    {
        auto end = vec2(evt.point.x, evt.point.y);
        m_speedVector = vec3(m_startPoint.x - end.x, 0.f,  end.y - m_startPoint.y).normalize();
    }

    void onTouchEnd(const Touch::End& evt)
    {
        m_speedVector = vec3(0.f);
    }

    void onUpdate(float dt) override
    {
        updateCamera(dt);
        updatePlayer(dt);
    }

    void updatePlayer(float dt)
    {
        if (!m_speedVector.isConsiderZero())
        {
            if (m_isInIntersec)
                if (m_collisionDirection.dot(m_speedVector) < 0)
                    m_speedVector = vec3(0.f);

            m_player->setWorldTranslation(m_player->getWorldTranslation() + m_speedVector * m_speedValue * dt);
        }
    }

    void updateCamera(float dt)
    {
        auto cam = Render::getScreenCamera();
        auto pointToLook = m_player->getWorldTranslation();

        auto newCamPos = pointToLook + camVector;
        auto lookRotator = (pointToLook - newCamPos).toRotator(vec3(0, 1, 0));

        auto currentRotator = cam->getWorldRotation();
        auto currentPos = cam->getWorldTranslation();

        cam->setWorldTranslation(math::lerp(currentPos, newCamPos, 2*dt ));
        cam->setWorldRotation(math::lerp(currentRotator, lookRotator, 2*dt ));
    }


    void setupLight()
    {
        Render::getPass(0)->getDirectionalLight()->setColor(math::vec3(1.f, 0.8f, 1.f));
        Render::getPass(0)->getDirectionalLight()->setDirection(math::vec3(0.0f, -1.f, 0.1f));
        Render::getPass(0)->getDirectionalLight()->enableShadows(true);
        Render::getPass(0)->setDirectionalLightShadowMapSize(2048);
    }

private:
    sptr<Mesh>  m_floor;
    sptr<Mesh>  m_player;
    sptr<Label> m_label;

    float camCoef   = 15.f;
    vec3  camVector = vec3(0., camCoef*1.5, camCoef);

    w4::math::vec3 m_collisionDirection;
    vec2           m_startPoint;
    bool           m_isInIntersec = false;
    float          m_speedValue   = 10.f;
    w4::math::vec3 m_speedVector  = w4::math::vec3(0.f);
};

W4_RUN(SampleObstacles)
