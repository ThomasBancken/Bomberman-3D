#pragma once
#include "Scenegraph/GameScene.h"
#include "Materials/SkinnedDiffuseMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Prefabs\Data.h"
#include "Graphics/SpriteFont.h"

class ModelComponent;
class ControllerComponent;
class ParticleEmitterComponent;

class ExamScene : public GameScene
{
public:
	ExamScene(void);
	virtual ~ExamScene(void);

	void CreateCrate(Tile &tile);
	void CreateBomb(Tile &tile, int playerColGroupID, int size, int id);
	Tile GetTile(GameObject* obj);
	void CreateParticle(Tile &tile);
	void SpawnPickUp(Tile &tile, PowerUpTypes type);
	void SetExplosionTilesRight(Bomb bomb, int it);
	void SetExplosionTilesLeft(Bomb bomb, int it);
	void SetExplosionTilesUp(Bomb bomb, int it);
	void SetExplosionTilesDown(Bomb bomb, int it);
	void IncreaseSize(int id);
	void IncreaseCount(int id);
	void DecreaseCount(int id);
	void ResetScene(const GameContext& gameContext);
	void CleanResetScene(const GameContext& gameContext);

protected:

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

private:
	FMOD::System* m_pFMODSystem = nullptr;
	FMOD::Channel* m_pChannel = nullptr;
	FMOD::Channel* m_pThemeChannel = nullptr;
	FMOD::Sound *m_pExp1Sound = nullptr;
	FMOD::Sound *m_pExp2Sound = nullptr;
	FMOD::Sound *m_pExp3Sound = nullptr;
	FMOD::Sound *m_pThemeSound = nullptr;

	GameObject * m_Player1Obj = nullptr;
	ModelComponent* m_pPlayerModel1 = nullptr;
	ControllerComponent* m_pController1;
	int m_CurrAnimIndex1 = 0;
	int m_Lives1 = 3;
	int m_BombCount1 = 1;
	int m_BombSize1 = 2;
	int m_LiveBombs1 = 0;

	GameObject * m_Player2Obj = nullptr;
	ModelComponent* m_pPlayerModel2 = nullptr;
	ControllerComponent* m_pController2;
	int m_CurrAnimIndex2 = 0;
	int m_Lives2 = 3;
	int m_BombCount2 = 1;
	int m_BombSize2 = 2;
	int m_LiveBombs2 = 0;

	GameObject * m_pCamera = nullptr;
	GameObject* m_pPauseScreen = nullptr;
	GameObject* m_pControlScreen = nullptr;
	GameObject* m_pSprite1 = nullptr;
	GameObject* m_pSprite2 = nullptr;

	vector<Tile> m_Tiles;
	vector<ParticleStr> m_Particles;

	SpriteFont * m_Font;

	bool m_GameOver = false;
	bool m_Paused = false;
	bool m_Sepia = false;

	float m_StartDelay = 5.0f;
	float m_RemainingTime = 300.0f;

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	ExamScene(const ExamScene &obj) = delete;
	ExamScene& operator=(const ExamScene& obj) = delete;
};

