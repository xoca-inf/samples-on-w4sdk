#include "W4Framework.h"

W4_USE_UNSTRICT_INTERFACE

enum EBoneType {
    BT_BODY,
    BT_HEAD,
    BT_RIGHT_ARM,
    BT_LEFT_ARM     ,
    BT_RIGHT_FOREARM,
    BT_LEFT_FOREARM ,
    BT_RIGHT_THIGH,
    BT_LEFT_THIGH,
    BT_RIGHT_LEG,
    BT_LEFT_LEG,
};

enum EBonesJointType {
    BJT_BALL,
    BJT_HINGE,
};

struct SBone {
    float zRotation;
    vec3 position, size;
};

struct SBonesJoint {
    EBoneType boneA, boneB;
    vec3 position;
    EBonesJointType type;
    vec2 limits;
    bool enableLimit, enableTwistLimit;
    vec2 twistLimit;
};

class W4Newton : public IGame
{
private:
    std::vector<SBone> m_bones = {
            {0.0f,  {0.0f,   0.0f,  0.0f}, {0.55f, 0.7f,  0.3f }}, //BT_BODY
            {0.0f,  {0.0f,   0.55f, 0.0f}, {0.4f,  0.35f, 0.2f }}, //BT_HEAD
            {-PI/2, {-0.45f, 0.28f, 0.0f}, {0.25f, 0.4f,  0.2f }}, //BT_RIGHT_ARM
            {PI/2,  {0.45f,  0.28f, 0.0f}, {0.25f, 0.4f,  0.2f }}, //BT_LEFT_ARM
            {-PI/2, {-0.9f,  0.28f, 0.0f}, {0.24f, 0.6f,  0.24f}}, //BT_RIGHT_FOREARM
            {PI/2,  {0.9f,   0.28f, 0.0f}, {0.24f, 0.6f,  0.24f}}, //BT_LEFT_FOREARM
            {0.0f,  {-0.2f,  -0.6f, 0.0f}, {0.27f, 0.7f,  0.2f }}, //BT_RIGHT_THIGH
            {0.0f,  {0.2f,   -0.6f, 0.0f}, {0.27f, 0.7f,  0.2f }}, //BT_LEFT_THIGH
            {0.0f,  {-0.2f,  -1.3f, 0.0f}, {0.3f,  0.8f,  0.3f }}, //BT_RIGHT_LEG
            {0.0f,  {0.2f,   -1.3f, 0.0f}, {0.3f,  0.8f,  0.3f }}  //BT_LEFT_LEG
    };
    std::vector<SBonesJoint> m_joints = {
            {BT_HEAD,          BT_BODY,        {0.0f,   0.35f,  0.0f}, BJT_HINGE, {-PI/4, PI/4},   true, false, {0.0f, 0.f}},
            {BT_RIGHT_ARM,     BT_BODY,        {-0.22f, 0.28f,  0.0f}, BJT_BALL,  {0.0f,  PI/2.5}, true, true,  {0.1f, 0.f}},
            {BT_LEFT_ARM,      BT_BODY,        {0.22f,  0.28f,  0.0f}, BJT_BALL,  {0.0f,  PI/2.5}, true, true,  {0.1f, 0.f}},
            {BT_RIGHT_FOREARM, BT_RIGHT_ARM,   {-0.65f, 0.28f,  0.0f}, BJT_HINGE, {0.0f,  PI/2},   true, false, {0.0f, 0.f}},
            {BT_LEFT_FOREARM,  BT_LEFT_ARM,    {0.65f,  0.28f,  0.0f}, BJT_HINGE, {0.0f,  PI/2},   true, false, {0.0f, 0.f}},
            {BT_RIGHT_THIGH,   BT_BODY,        {-0.2f,  -0.32f, 0.0f}, BJT_BALL,  {0.0f,  PI/4},   true, true,  {0.8f, 0.f}},
            {BT_LEFT_THIGH,    BT_BODY,        {0.2f,   -0.32f, 0.0f}, BJT_BALL,  {0.0f,  PI/4},   true, true,  {0.8f, 0.f}},
            {BT_RIGHT_LEG,     BT_RIGHT_THIGH, {-0.2f,  -0.95f, 0.0f}, BJT_HINGE, {-PI/2, 0.0f},   true, false, {0.0f, 0.f}},
            {BT_LEFT_LEG,      BT_LEFT_THIGH,  {0.2f,   -0.95f, 0.0f}, BJT_HINGE, {-PI/2, 0.0f},   true, false, {0.0f, 0.f}}
    };

    static constexpr float m_scale = 1.5f;

    void createRagDoll(const vec3& position)
    {
        constexpr float mass = 8.0f;
        vec3 xAxis(1.0f, 0.0f, 0.0f);
        vec3 zAxis(0.0f, 0.0f, 1.0f);

        std::vector<sptr<Mesh>> bodyParts;

        for (auto& bone: m_bones)
        {
            auto body = Mesh::create::cube(bone.size * m_scale);
            body->addComponent<PhysicsComponent>().setup(m_phisSim, Body::BodyType::Rigid, PhysicsGeometry::AutoCube, mass);
            body->setWorldTranslation(bone.position * m_scale + position);
            body->setWorldRotation({forwardVector, bone.zRotation});

            Render::getRoot()->addChild(body);

            body->getMaterialInst()->setParam("baseColor", math::color::random());

            bodyParts.push_back(body);
        }

        for (auto& joint: m_joints)
        {
            physics::Joint::sptr jointPtr;
            const auto jointPos = joint.position * m_scale + position;
            switch(joint.type)
            {
                case BJT_BALL:
                {
                    jointPtr.reset(new BallSocketJoint(m_phisSim,
                                                           bodyParts[joint.boneA]->getFirstComponent<PhysicsComponent>(),
                                                           bodyParts[joint.boneB]->getFirstComponent<PhysicsComponent>(),
                                                           jointPos,
                                                           0.f,
                                                           joint.enableLimit ? std::make_optional(joint.limits) : std::nullopt,
                                                           joint.enableTwistLimit ? std::make_optional(joint.twistLimit) : std::nullopt));
                    break;
                }
                case BJT_HINGE:
                {
                    jointPtr = make::sptr<HingeJoint>(m_phisSim,
                                                      bodyParts[joint.boneA]->getFirstComponent<PhysicsComponent>(),
                                                      bodyParts[joint.boneB]->getFirstComponent<PhysicsComponent>(),
                                                      jointPos,
                                                      (-rightVector).rotate({0, 0, m_bones[joint.boneA].zRotation}),
                                                      0.f,
                                                      joint.enableLimit ? std::make_optional(joint.limits) : std::nullopt);
                    break;
                }
                default:
                    FATAL_ERROR("Invalid joint type");
            }
            m_jointsStorage.push_back(jointPtr);
        }
    }

public:

    void onConfig() override
    {
        Game::Config.EnableFrustumCulling = false;
    }
    void onStart() override
    {
        m_phisSim = make::sptr<physics::Simulator>(1000, 1000, 1000, vec3(0.0f, -9.8f, 0.0f));
        physics::SimulatorCollector::addSimulator(m_phisSim);

        auto cam = Render::getScreenCamera();
             cam->setWorldTranslation({-12.f, 15.f, -3.f});
             cam->setWorldRotation(Rotator(HALF_PI / 4, 0, 0) + Rotator(0, HALF_PI / 3, 0));
             cam->setFov(80.f);
             cam->setAspect(9.f/16.f);
             cam->setClearColor({0.0f, 0.0f, 0.0f, 1.0f});

        constexpr auto floorHeight = 2.0f;
        auto floor = Mesh::create::cube({30.f, floorHeight, 10.0f});
        floor->setWorldTranslation({0.0f, -floorHeight / 2.0f, 0.0f});
        floor->addComponent<PhysicsComponent>().setup(m_phisSim, Body::BodyType::Animated, PhysicsGeometry::AutoCube);

        Render::getRoot()->addChild(floor);

        floor->getMaterialInst()->setParam("baseColor", math::color::random() * .3f + .7f);

        constexpr auto stepHeight = .9f;
        constexpr auto stepDepth = .9f;

        for(int i = 0; i < 40; ++i)
        {
            auto step = Mesh::create::cube({30.f, stepHeight, stepDepth});
            step->setWorldTranslation({0.0f, stepHeight * (i + .5f), 5.f + stepDepth * (i + .5f)});
            step->addComponent<PhysicsComponent>().setup(m_phisSim, Body::BodyType::Animated, PhysicsGeometry::AutoCube);

            Render::getRoot()->addChild(step);

            step->getMaterialInst()->setParam("baseColor", math::color::random() * .3f + .7f);
        }

        constexpr float cubeMass = 1.5f;

        for (int i=0; i<50; i++)
        {
            auto cube = Mesh::create::cube({.75f, .75f, .75f});
            cube->setWorldTranslation({(rand()%50 - 25) / 2.f, .375f, (rand()%50 - 25) / 5.f});

            cube->addComponent<PhysicsComponent>().setup(m_phisSim, Body::BodyType::Animated, PhysicsGeometry::AutoCube, cubeMass);

            Render::getRoot()->addChild(cube);

            cube->getMaterialInst()->setParam("baseColor", math::color::random());
        }

        for(int i =0; i < 5; ++i)
        {
            createRagDoll({(i - 2) * 5.f, 35, 35});
        }
    }

    void onUpdate(float dt) override
    {
        if(rand()%200 == 0 && !m_jointsStorage.empty())
        {
            m_jointsStorage.erase(m_jointsStorage.begin() + rand()%m_jointsStorage.size());
        }
    }
private:
    sptr<physics::Simulator> m_phisSim;
    std::vector<physics::Joint::sptr> m_jointsStorage;
};
W4_RUN(W4Newton)
