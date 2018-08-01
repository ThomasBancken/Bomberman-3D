//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "EndScene.h"
#include "Scenegraph\GameObject.h"
#include "Content\ContentManager.h"
#include "Components\Components.h"
#include "Physx\PhysxProxy.h"
#include "Scenegraph\SceneManager.h"
#include "Graphics\SpriteRenderer.h"
#include "Components\SpriteComponent.h"

EndScene::EndScene(void) :
	GameScene(L"EndScene")
{

}


EndScene::~EndScene(void)
{

}

void EndScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto pObj = new GameObject();
	pObj->AddComponent(new SpriteComponent(L"./Resources/Textures/GP2Exam2018_EndMenu.png", XMFLOAT2(0.f, 0.f), XMFLOAT4(1, 1, 1, 1)));
	AddChild(pObj);

	gameContext.pInput->AddInputAction(InputAction(0, Pressed, -1, VK_LBUTTON));

}

void EndScene::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (gameContext.pInput->IsActionTriggered(0))
	{
		std::cout << gameContext.pInput->GetMousePosition().x << " " << gameContext.pInput->GetMousePosition().y << std::endl;

		if ((gameContext.pInput->GetMousePosition().x > 466 && gameContext.pInput->GetMousePosition().x < 814) && (gameContext.pInput->GetMousePosition().y > 302 && gameContext.pInput->GetMousePosition().y < 415))
		{
			SceneManager::GetInstance()->SetActiveGameScene(L"StartScene");
		}
		else if ((gameContext.pInput->GetMousePosition().x > 466 && gameContext.pInput->GetMousePosition().x < 814) && (gameContext.pInput->GetMousePosition().y > 442 && gameContext.pInput->GetMousePosition().y < 558))
		{
			//QUIT
			PostQuitMessage(0);
		}
	}
}

void EndScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
