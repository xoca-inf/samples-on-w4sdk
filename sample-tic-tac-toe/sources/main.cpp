#include "W4Framework.h"
W4_USE_UNSTRICT_INTERFACE

struct RotateAction
{
    RotateAction(sptr<Node> node, const Rotator& begin, const Rotator& finish, float duration, const std::function<void()>& onFinish)
    {
        m_node = node;
        m_tween = make::sptr<Tween<Rotator>>(begin, 0.0f);
        m_tween->add(finish, duration, easing::Linear<Rotator>);
        m_onFinish = onFinish;
    }

    bool isActive() const
    {
        return m_time > 0.f;
    }

    void onUpdate(float dt)
    {
        if (m_time > m_tween->getDuration())
        {
            m_time = -1.f;
            m_node->setLocalRotation(m_tween->getValue(m_tween->getDuration()));
            if(m_onFinish) m_onFinish();
        }
        else if (m_time > -0.01f)
        {
            m_time += dt;
            m_node->setLocalRotation(m_tween->getValue(m_time));
        }
    }

    sptr<Tween<Rotator>> m_tween;
    sptr<Node>           m_node;
    float                     m_time = 0.f;
    std::function<void()>    m_onFinish;
};

enum class FieldState
{
    Empty,
    X,
    O
};

class TicTacToe: public IGame
{
public:
    void onStart() override
    {
        Render::getScreenCamera()->setWorldTranslation({0, 0, -25});

        auto cubeMat = Material::get("materials/cube.mat");
        auto textureCubeMat = Material::get("materials/textureCube.mat");

        m_matX = textureCubeMat->createInstance();
        m_matO = textureCubeMat->createInstance();
        m_matWinX = textureCubeMat->createInstance();
        m_matWinO = textureCubeMat->createInstance();
        m_matVoid = cubeMat->createInstance();

        m_matX->setParam("mainColor", vec3(1.0, 0.0, 1.0));
        m_matX->setTexture(TextureId::TEXTURE_0, Texture::get("textures/X.png"));

        m_matO->setParam("mainColor", vec3(1.0, 0.0, 1.0));
        m_matO->setTexture(TextureId::TEXTURE_0, Texture::get("textures/O.png"));

        m_matWinX->setParam("mainColor", vec3(0.0, 1.0, 0.0));
        m_matWinX->setTexture(TextureId::TEXTURE_0, Texture::get("textures/X.png"));

        m_matWinO->setParam("mainColor", vec3(1.0, 0.0, 0.0));
        m_matWinO->setTexture(TextureId::TEXTURE_0, Texture::get("textures/O.png"));

        m_matVoid->setParam("mainColor", vec3(1.0, 0.0, 1.0));


        vec3 step  { 4.f, 4.f, 0.f};
        vec3 start { (-fieldSize  * step.x) / 2.f,
                     (-fieldSize * step.y) / 2.f,
                         0.f};
        start += step / 2.f;

        for (int i = 0; i < fieldSize; ++i)
        {
            for (int j = 0; j < fieldSize; ++j)
            {
                auto box = Mesh::create::box({3, 3, 3});
                auto pos = start + step * vec3(i, j, 0);
                box->setLocalTranslation(pos);
                Render::getRoot()->addChild(box);

                box->foreachSurface([this](Surface& surface)
                {
                   surface.setMaterialInst(m_matVoid);
                });

                auto number = m_boxes.size();

                box->addCollider<core::AABB>("unnamed", box)->setScreencastCallback([this, box, number](const auto&)
                {
                    onBox(number);
                });

                m_boxes.push_back(box);

            }
        }
    }
    void onUpdate(float dt) override
    {
        if (m_action) m_action->onUpdate(dt);

        for (auto& action: m_resetActions)
            action->onUpdate(dt);
    }

    void onBox(uint i)
    {
        if (!m_action && m_active)
        {
            if (m_fields[i] == FieldState::Empty)
            {
                auto box = m_boxes[i];
                if(m_state == GameState::PlayerX)
                {
                    box->getSurface("back").setMaterialInst(m_matX);
                    m_state = GameState::PlayerO;
                    m_fields[i] = FieldState::X;
                }
                else if(m_state == GameState::PlayerO)
                {
                    box->getSurface("back").setMaterialInst(m_matO);
                    m_state = GameState::PlayerX;
                    m_fields[i] = FieldState::O;
                }

                auto rot = box->getLocalRotation();
                Rotator finish(rot.eulerPitch() + PI, rot.eulerYaw(), rot.eulerRoll());
                m_action = make::sptr<RotateAction>(box, rot, finish, 0.3f, [this]()
                {
                    m_action.reset();
                    FieldState state = FieldState::Empty;
                    bool reset = checkWin(state, true) || !hasEmptyField(); //checkWin is not pure function, it has side effect
                    if(reset)
                    {
                        m_active = false;
                        Timer::addTask(1.f,[this]()
                        {
                            onReset();
                            return false;
                        }, false);
                    }
                    else
                    {
                        if (m_state == GameState::PlayerO)
                        {
                            uint move = findBestMove();
                            onBox(move);
                        }
                    }
                });
            }
        }
    }
    void onReset()
    {
        m_resetActions.clear();
        for (auto i = 0; i < m_fields.size(); ++i)
        {
            if (m_fields[i] != FieldState::Empty)
            {
                auto box = m_boxes[i];
                auto rot = box->getLocalRotation();
                Rotator finish(rot.eulerPitch() + PI, rot.eulerYaw(), rot.eulerRoll());
                m_resetActions.push_back(make::sptr<RotateAction>(box, rot, Rotator(0.f, 0.f, 0.f), 0.3f, nullptr));
            }
        }


        Timer::addTask(0.3f, [this]()
        {
            m_resetActions.clear();
            onGame();
            return false;
        }, false);
    }

    void onGame()
    {
        m_active = true;
        for(auto& f: m_fields)
        {
            f = FieldState::Empty;
        }
        m_state = GameState::PlayerX;
    }

    void onWinBox(uint i)
    {
        if (m_fields[i] == FieldState::X)
        {
            m_boxes[i]->getSurface("back").setMaterialInst(m_matWinX);
        }
        else if (m_fields[i] == FieldState::O)
        {
            m_boxes[i]->getSurface("back").setMaterialInst(m_matWinO);
        }
    }

    // is not pure function, it has side effect by onWinBox
    bool checkWin(FieldState& state, bool onBox = false)
    {
        bool win = false;
        for (int i = 0; i < fieldSize; ++i)
        {
            state = m_fields[i];
            if (state == FieldState::Empty) continue;
            win = true;
            for (int j = 1; j < fieldSize; ++j)
            {
                if (state != m_fields[i + j * fieldSize])
                {
                    win = false;
                    break;
                }
            }
            if (win)
            {
                if(onBox)
                {
                    for(int j = 0; j < fieldSize; ++j)
                    {
                        onWinBox(i + j * fieldSize);
                    }
                }
                break;
            }
        }
        if(!win)
        {
            for(int i = 0; i < fieldSize; ++i)
            {
                state = m_fields[i * fieldSize];
                if(state == FieldState::Empty) continue;
                win = true;
                for(int j = 1; j < fieldSize; ++j)
                {
                    if(state != m_fields[i * fieldSize + j])
                    {
                        win = false;
                        break;
                    }
                }
                if(win)
                {
                    if(onBox)
                    {
                        for(int j = 0; j < fieldSize; ++j)
                        {
                            onWinBox(i * fieldSize + j);
                        }
                    }
                    break;
                }
            }
        }
        if (!win)
        {
            state = m_fields[0];
            win = true;
            if(state == FieldState::Empty) win = false;

            for(int i = 1; i < fieldSize; ++i)
            {
                if(state != m_fields[i + i * fieldSize])
                {
                    win = false;
                    break;
                }
            }

            if(win)
            {
                if(onBox)
                {
                    for(int i = 0; i < fieldSize; ++i)
                    {
                        onWinBox(i * fieldSize + i);
                    }
                }
            }
        }
        if (!win)
        {
            uint size = fieldSize-1;
            state = m_fields[size];
            win = true;
            if(state == FieldState::Empty) win = false;

            for(int i = 1; i < fieldSize; ++i)
            {
                if(state != m_fields[size - i + i * fieldSize])
                {
                    win = false;
                    break;
                }
            }

            if(win)
            {
                if(onBox)
                {
                    for(int i = 0; i < fieldSize; ++i)
                    {
                        onWinBox(size - i + i * fieldSize);
                    }
                }
            }
        }
        if(!win) state = FieldState::Empty;
        return win;
    }

    // This is the evaluation function as discussed
    int evaluate()
    {
        auto state = FieldState::Empty;
        checkWin(state);

        if (state == FieldState::O)
            return +10;
        else if (state == FieldState::X)
            return -10;

        return 0;
    }

    // This is the minimax function. It considers all
    // the possible ways the game can go and returns
    // the value of the board
    int minimax(int depth, bool isMax)
    {
        int score = evaluate();

        // If Maximizer has won the game return his/her
        // evaluated score
        if (score == 10)
            return score;

        // If Minimizer has won the game return his/her
        // evaluated score
        if (score == -10)
            return score;

        // If there are no more moves and no winner then
        // it is a tie
        if (hasEmptyField()==false)
            return 0;

        // If this maximizer's move
        if (isMax)
        {
            int best = -1000;

            // Traverse all cells
            for (int i = 0; i<3; i++)
            {
                for (int j = 0; j<3; j++)
                {
                    // Check if cell is empty
                    if ( m_fields[j + i * fieldSize]==FieldState::Empty)
                    {
                        // Make the move
                        m_fields[j + i * fieldSize] = FieldState::O;

                        // Call minimax recursively and choose
                        // the maximum value
                        best = std::max( best,
                                    minimax(depth+1, !isMax) );

                        // Undo the move
                        m_fields[j + i * fieldSize] = FieldState::Empty;
                    }
                }
            }
            return best;
        }

            // If this minimizer's move
        else
        {
            int best = 1000;

            // Traverse all cells
            for (int i = 0; i<3; i++)
            {
                for (int j = 0; j<3; j++)
                {
                    // Check if cell is empty
                    if (m_fields[j + i * fieldSize]==FieldState::Empty)
                    {
                        // Make the move
                        m_fields[j + i * fieldSize] = FieldState::X;

                        // Call minimax recursively and choose
                        // the minimum value
                        best = std::min(best,
                                   minimax(depth+1, !isMax));

                        // Undo the move
                        m_fields[j + i * fieldSize] = FieldState::Empty;
                    }
                }
            }
            return best;
        }
    }

    uint findBestMove()
    {
        int bestVal = -1000;
        uint bestMove = -1;

        // Traverse all cells, evaluate minimax function for
        // all empty cells. And return the cell with optimal
        // value.
        for (int i = 0; i<fieldSize; i++)
        {
            for (int j = 0; j<fieldSize; j++)
            {
                // Check if cell is empty
                if (m_fields[j + i * fieldSize]==FieldState::Empty)
                {
                    // Make the move
                    m_fields[j + i * fieldSize] = FieldState::O;

                    // compute evaluation function for this
                    // move.
                    int moveVal = minimax(0, false);

                    // Undo the move
                    m_fields[j + i * fieldSize] = FieldState::Empty;

                    // If the value of the current move is
                    // more than the best value, then update
                    // best/
                    if (moveVal > bestVal)
                    {
                        bestMove = j + i * fieldSize;
                        bestVal = moveVal;
                    }
                }
            }
        }

        return bestMove;
    }

    bool hasEmptyField()
    {
        return std::find(m_fields.begin(), m_fields.end(), FieldState::Empty) != m_fields.end();
    }

    static constexpr int fieldSize = 3;

    std::array<FieldState, fieldSize * fieldSize> m_fields{};

    sptr<RotateAction>  m_action;

    std::vector<sptr<RotateAction>>  m_resetActions;

    std::vector<sptr<Mesh>> m_boxes;
    sptr<MaterialInst>  m_matX;
    sptr<MaterialInst>  m_matO;
    sptr<MaterialInst>  m_matWinX;
    sptr<MaterialInst>  m_matWinO;
    sptr<MaterialInst>  m_matVoid;

    bool m_active = true;


    enum class GameState
    {
        PlayerX,
        PlayerO
    } m_state = GameState::PlayerX;
};

W4_RUN(TicTacToe)

