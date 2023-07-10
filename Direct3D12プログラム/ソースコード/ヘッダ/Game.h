#pragma once

#include<Windows.h>
#include"Keyboard.h"
#include"Mouse.h"
#include"Gamepad.h"

#include"Renderer.h"
#include<memory>

#include"CameraActor.h"
#include "PlaneActor.h"
#include "BoxActor.h"
#include"SphereActor.h"
#include"HemisphereActor.h"
#include"FileMeshActor.h"

#include"PlayerActor.h"

#include"EnemyActor.h"
#include<vector>

#include"TerrainActor.h"
#include"BallActor.h"

#include"PhysicsBallActor.h"

#include"BlockActor.h"


class Game
{
public:
    Game();
    ~Game();

    void initialize(HWND hwnd, int width, int height);
    bool loop();

    HWND getHwnd() { return m_hwnd; }
    int getWidth()const { return m_width; }
    int getHeight()const { return m_height; }
    Renderer* getRenderer() { return m_renderer.get(); }

    const Keyboard& getKeyboard() const { return m_keyboard; }
    const Mouse& getMouse()const { return m_mouse; }
    const Gamepad& getGamepad()const { return m_gamepad; }

    std::vector<EnemyActor*>& getEnemies() { return m_enemies; }

    TerrainActor* getTerrain() { return m_terrain.get(); }

    std::vector<BlockActor*> getBlocks() { return m_blocks; }

private:
    void input();
    void update(float deltaTime);
    void draw();

    bool tick(float& deltaTime);

    static constexpr float MaxDeltaTime = 0.05f;

    HWND m_hwnd;
    int m_width;
    int m_height;

    bool m_isRunning;

    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_endTime;
    LARGE_INTEGER m_freqTime;

    std::unique_ptr<Renderer> m_renderer;

    Keyboard m_keyboard;
    Mouse m_mouse;
    Gamepad m_gamepad;

    std::unique_ptr<CameraActor> m_camActor;

    std::unique_ptr<TerrainActor> m_terrain;
    std::unique_ptr<BallActor> m_ball;
    std::unique_ptr<PhysicsBallActor> m_phyBall;
    std::unique_ptr<BoxActor> m_skybox;
    std::unique_ptr<HemisphereActor> m_skydome;

    std::unique_ptr<PlayerActor> m_player;

    std::vector<EnemyActor*>m_enemies;

    //std::unique_ptr<FileMesh>m_test;
    //std::unique_ptr<SphereActor>m_test;

    std::unique_ptr<FileMeshActor>m_model;

    std::vector<BlockActor*> m_blocks;
};
