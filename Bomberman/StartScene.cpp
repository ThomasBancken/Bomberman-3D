//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "StartScene.h"
#include "Scenegraph\GameObject.h"
#include "Content\ContentManager.h"
#include "Components\Components.h"
#include "Physx\PhysxProxy.h"
#include "Scenegraph\SceneManager.h"
#include "Graphics\SpriteRenderer.h"
#include "Components\SpriteComponent.h"
#include "Scenegraph\SceneManager.h"
#include "CourseObjects/Exam/ExamScene.h"

StartScene::StartScene(void) :
	GameScene(L"StartScene")
{

}

StartScene::~StartScene(void)
{

}

void StartScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	auto pObj = new GameObject();
	pObj->AddComponent(new SpriteComponent(L"./Resources/Textures/GP2Exam2018_MainMenu.png", XMFLOAT2(0.f, 0.f), XMFLOAT4(1, 1, 1, 1)));
	AddChild(pObj);

	gameContext.pInput->AddInputAction(InputAction(0, Pressed, -1, VK_LBUTTON));
}

void StartScene::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if (gameContext.pInput->IsActionTriggered(0))
	{
		std::cout << gameContext.pInput->GetMousePosition().x << " " << gameContext.pInput->GetMousePosition().y << std::endl;

		if ((gameContext.pInput->GetMousePosition().x > 466 && gameContext.pInput->GetMousePosition().x < 814) && (gameContext.pInput->GetMousePosition().y > 262 && gameContext.pInput->GetMousePosition().y < 378))
		{
			SceneManager::GetInstance()->SetActiveGameScene(L"ExamScene");
		}
		else if((gameContext.pInput->GetMousePosition().x > 466 && gameContext.pInput->GetMousePosition().x < 814) && (gameContext.pInput->GetMousePosition().y > 418 && gameContext.pInput->GetMousePosition().y < 534))
		{
			//QUIT
			PostQuitMessage(0);
		}
	}
}

void StartScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
