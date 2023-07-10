#include "Game.h"
#include<exception>

Game::Game()
	: m_hwnd(nullptr)
	, m_width(0)
	, m_height(0)
	, m_isRunning(false)
	, m_startTime({})
	, m_endTime({})
	, m_freqTime({})
	, m_gamepad(0)
{

}

Game::~Game()
{
	for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it)
	{
		delete(*it);
	}
	m_enemies.clear();

	for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		delete(*it);
	}
	m_blocks.clear();
}

void Game::initialize(HWND hwnd, int width, int height)
{
	m_hwnd = hwnd;
	m_width = (width > 0) ? width : 1;
	m_height = (height > 0) ? height : 1;

	// レンダラー
	m_renderer = std::make_unique<Renderer>(this, 0.5f, 0.5f, 0.5f);
	if (!m_renderer->initialize()) throw std::exception();
	m_renderer->setLight(XMFLOAT3(0.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));


	// 時間計測
	QueryPerformanceFrequency(&m_freqTime);
	QueryPerformanceCounter(&m_startTime);

	// キー入力、マウス入力、ゲームパッド入力
	m_keyboard.initialize();
	m_mouse.initialize();
	m_gamepad.initialize();

	// 地形
	m_terrain = std::make_unique<TerrainActor>(this, 0, ZeroVec3d, L"Images\\grass_loop.png");

	// スカイボックス
	std::vector<const wchar_t*> skyFilenames;
	skyFilenames.push_back(L"Images\\skybox_x_p.png");
	skyFilenames.push_back(L"Images\\skybox_y_p.png");
	skyFilenames.push_back(L"Images\\skybox_x_n.png");
	skyFilenames.push_back(L"Images\\skybox_y_n.png");
	skyFilenames.push_back(L"Images\\skybox_z_p.png");
	skyFilenames.push_back(L"Images\\skybox_z_n.png");
	m_skybox = std::make_unique<BoxActor>(this, 1, ZeroVec3d,
		skyFilenames, false,1000.0f, 1000.0f, 1000.0f);

	// スカイドーム
	m_skydome = std::make_unique<HemisphereActor>(this, 0, XMFLOAT3(0.0f, 0.0f, -10.0f),
		L"Images\\skydome.png", true, 1500.0f, 20, 40, 1920.0f, 1920.0f, 960.0f, 960.0f,
		740.0f, 0.8f, true);

	// プレイヤー
	//m_player = std::make_unique<PlayerActor>(this, 3, ZeroVec3d);
	m_player = std::make_unique<PlayerActor>(this, 3, XMFLOAT3(8.0f,0.0f,0.0f));
	m_player->setForwardSpeed(5.0f);
	m_player->setYawSpeed(90.0f);

	// カメラアクター
	m_camActor = std::make_unique<CameraActor>(this, ZeroVec3d, false);
	m_camActor->setFPSCameraParam(1.4f);
	//m_camActor->setTPSCameraParam(4.0f, 2.5f, 1.0f, 1.0f);
	m_camActor->setTPSCameraParam(4.0f, 2.5f, 2.0f, 1.0f);
	m_camActor->attach(m_player.get());

	// エネミー
	EnemyActor* enemy = new EnemyActor(this, 1, XMFLOAT3(11.25f, -3.5f, 0.0f), L"Blue", 1.0f);
	m_enemies.push_back(enemy);
	enemy = new EnemyActor(this, 1, XMFLOAT3(11.25f, 8.0f, 0.0f), L"Blue", 1.0f);
	m_enemies.push_back(enemy);
	enemy = new EnemyActor(this, 1, XMFLOAT3(20.25f, -3.5f, 0.0f), L"Blue", 1.0f);
	m_enemies.push_back(enemy);
	enemy = new EnemyActor(this, 1, XMFLOAT3(20.25f, 3.5f, 0.0f), L"Blue", 1.0f);
	m_enemies.push_back(enemy);
	
	// ボール
	m_ball = std::make_unique<BallActor>(this, 3, XMFLOAT3(30.0f, 0.0f, 30.0f), WHITE_TEXTRE, 1.0f);
	m_ball->setUpSpeed(-20.0f);

	m_phyBall = std::make_unique<PhysicsBallActor>(this, 3, XMFLOAT3(30.0f, 5.0f, 30.0f), WHITE_TEXTRE, 1.0f);

	// FBX model
	m_model = std::make_unique<FileMeshActor>(this, 3, XMFLOAT3(10.0f, -20.0f, 0.0f),
		"Images\\model.gmesh", "Images\\");
	m_model->setOriginalMaterial(MeshMaterial(XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.3f, 0.3f, 0.3f)));

	// ブロック（壁など）
	BlockActor* block = new BlockActor(this, 1, XMFLOAT3(10.0f, 0.0f, 0.0f),
		L"Red", XMFLOAT3(0.5f, 20.0f, 2.0f));
	m_blocks.push_back(block);
	block = new BlockActor(this, 1, XMFLOAT3(20.0f, 9.75f, 0.0f),
		L"Blue", XMFLOAT3(19.5f, 0.5f, 2.0f));
	m_blocks.push_back(block);
	block = new BlockActor(this, 1, XMFLOAT3(29.5f, -0.05f, 0.0f),
		L"Yellow", XMFLOAT3(0.5f, 19.5f, 2.0f));
	m_blocks.push_back(block);
	block = new BlockActor(this, 1, XMFLOAT3(21.0f, -9.75f, 0.0f),
		L"Green", XMFLOAT3(17.5f, 0.5f, 2.0f));
	m_blocks.push_back(block);

	block = new BlockActor(this, 1, XMFLOAT3(12.5f, -7.0f, 0.0f),
		L"Black", XMFLOAT3(0.5f, 5.0f, 2.0f));
	m_blocks.push_back(block);
	block = new BlockActor(this, 1, XMFLOAT3(12.5f, 0.0f, 0.0f),
		L"Black", XMFLOAT3(0.5f, 5.0f, 2.0f));
	m_blocks.push_back(block);
	block = new BlockActor(this, 1, XMFLOAT3(12.5f, 7.0f, 0.0f),
		L"Black", XMFLOAT3(0.5f, 5.0f, 2.0f));
	m_blocks.push_back(block);

	block = new BlockActor(this, 1, XMFLOAT3(17.5f, -4.75f, 0.0f),
		L"Cyan", XMFLOAT3(9.5f, 0.5f, 2.0f));
	m_blocks.push_back(block);
	block = new BlockActor(this, 1, XMFLOAT3(17.5f, -2.25f, 0.0f),
		L"Cyan", XMFLOAT3(9.5f, 0.5f, 2.0f));
	m_blocks.push_back(block);
	block = new BlockActor(this, 1, XMFLOAT3(17.5f, 2.25f, 0.0f),
		L"Cyan", XMFLOAT3(9.5f, 0.5f, 2.0f));
	m_blocks.push_back(block);
	block = new BlockActor(this, 1, XMFLOAT3(17.5f, 4.75f, 0.0f),
		L"Cyan", XMFLOAT3(9.5f, 0.5f, 2.0f));
	m_blocks.push_back(block);

	block = new BlockActor(this, 1, XMFLOAT3(22.5, -3.5f, 0.0f),
		L"Magenta", XMFLOAT3(0.5f, 3.0f, 2.0f));
	m_blocks.push_back(block);
	block = new BlockActor(this, 1, XMFLOAT3(22.5, 3.5f, 0.0f),
		L"Magenta", XMFLOAT3(0.5f, 3.0f, 2.0f));
	m_blocks.push_back(block);



	// ゲーム駆動状態
	m_isRunning = true;
}

bool Game::loop()
{
	float deltaTime = 0.0f;
	if (tick(deltaTime))
	{
		input();
		update(deltaTime);
		draw();
	}
	return m_isRunning;
}

void Game::input()
{
	m_keyboard.input();
	m_mouse.input(m_hwnd);
	m_gamepad.input();
}

void Game::update(float deltaTime)
{
	m_renderer->update();

	m_terrain->update(deltaTime);
	m_skybox->update(deltaTime);
	m_skydome->update(deltaTime);
	m_ball->update(deltaTime);
	m_phyBall->update(deltaTime);
	m_model->update(deltaTime);

	// これが先
	m_player->update(deltaTime);

	// これが後
	m_camActor->update(deltaTime);

	{
		auto it = m_enemies.begin();
		while (it != m_enemies.end())
		{
			if ((*it)->isDead())
			{
				delete(*it);
				it = m_enemies.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it)
	{
		(*it)->update(deltaTime);
	}

	for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		(*it)->update(deltaTime);
	}

	if (m_enemies.size() == 0)exit(0);

}

void Game::draw()
{
	ID3D12GraphicsCommandList* cmdList = nullptr;
	cmdList = m_renderer->begin();

	m_terrain->draw(cmdList);
	m_ball->draw(cmdList);
	m_phyBall->draw(cmdList);
	m_skydome->draw(cmdList);

	m_player->draw(cmdList);

	m_model->draw(cmdList);

	//m_test->draw(cmdList);

	for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it)
	{
		(*it)->draw(cmdList);
	}

	for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		(*it)->draw(cmdList);
	}

	m_renderer->end();
}

bool Game::tick(float& deltaTime)
{
	QueryPerformanceCounter(&m_endTime);
	if (m_endTime.QuadPart - m_startTime.QuadPart == 0) return false;

	deltaTime = (float)(m_endTime.QuadPart - m_startTime.QuadPart)
		/ (float)m_freqTime.QuadPart;

	m_startTime = m_endTime;
	deltaTime = (deltaTime > MaxDeltaTime) ? MaxDeltaTime : deltaTime;

	return true;

}