#pragma once
#include "Scenegraph\GameObject.h"
#include "Prefabs\Data.h"

class ModelComponent;

class Bomb : public GameObject
{
public:
	Bomb(Tile tile, int size);
	~Bomb(void);

protected:

	virtual void Initialize(const GameContext& gameContext);
	virtual void Update(const GameContext& gameContext);

private:
	GameObject * m_Bomb = nullptr;
	
	Tile m_Tile;
	
	float m_Timer = 3.0f;
	int m_Size = 1;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	Bomb(const Bomb& yRef);
	Bomb& operator=(const Bomb& yRef);
};
