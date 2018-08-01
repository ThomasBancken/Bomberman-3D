//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "Bomb.h"
#include "Components/Components.h"
#include "../OverlordProject/Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "../OverlordProject/Materials/DiffuseMaterial.h"
#include "Content\ContentManager.h"

Bomb::Bomb(Tile tile, int size) : m_Tile(tile), m_Size(size)
{

}


Bomb::~Bomb(void)
{
	delete m_Bomb;
}

void Bomb::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	m_Bomb = new GameObject();
	auto pBombModel = new ModelComponent(L"./Resources/Meshes/Bomb.ovm");
	pBombModel->SetMaterial(0);

	m_Bomb->AddComponent(pBombModel);

	m_Bomb->GetTransform()->Scale(2, 2, 2);
	m_Bomb->GetTransform()->Translate(m_Tile.position.x, 10, m_Tile.position.y);
}

void Bomb::Update(const GameContext & gameContext)
{
	m_Timer -= gameContext.pGameTime->GetElapsed();

	if (m_Timer <= 0.0f)
	{
		//m_Bomb->GetParent()->RemoveChild(m_Bomb);
	}
}
