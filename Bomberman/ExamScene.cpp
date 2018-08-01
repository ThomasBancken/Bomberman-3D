//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ExamScene.h"
#include "Scenegraph\GameObject.h"
#include "Prefabs\Prefabs.h"
#include "Content\ContentManager.h"
#include "Components\Components.h"
#include <Materials/Shadow/DiffuseMaterial_Shadow.h>
#include "Physx\PhysxManager.h"
#include <Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h>
#include "Physx\PhysxProxy.h"
#include "Components\ParticleEmitterComponent.h"
#include "Scenegraph\SceneManager.h"
#include "Base/SoundManager.h"
#include "Graphics/ShadowMapRenderer.h"
#include "Graphics\ModelAnimator.h"
#include "Components\SpriteComponent.h"
#include "CourseObjects/Exam/StartScene.h"
#include "Graphics/TextRenderer.h"
#include <Materials/Post/PostSepia.h>

ExamScene::ExamScene(void) :
	GameScene(L"ExamScene")
{

}

ExamScene::~ExamScene(void)
{
	m_Tiles.clear();
	m_Particles.clear(); 
}

void ExamScene::CreateCrate(Tile &tile)
{
	if (!tile.stone)
	{
		auto crate = new GameObject();
		auto crateModel = new ModelComponent(L"./Resources/Meshes/Crate.ovm");
		crateModel->SetMaterial(2);
	
		crate->AddComponent(crateModel);
	
		auto physX = PhysxManager::GetInstance()->GetPhysics();
	
		auto PxMaterial = physX->createMaterial(1, 1, 0);
	
		crate->GetTransform()->Translate(tile.position.x, 0.1f, tile.position.y);
	
		auto rigidBody = new RigidBodyComponent(true);
		crate->AddComponent(rigidBody);
	
		auto pConvexMesh = ContentManager::Load<PxTriangleMesh>(L"Resources\\Meshes\\Crate.ovpt");
		std::shared_ptr<PxGeometry> geom(new PxTriangleMeshGeometry(pConvexMesh));
		crate->AddComponent(new ColliderComponent(geom, *PxMaterial));
	
		AddChild(crate);
	
		tile.crate = true;
		tile.crateObj = crate;
	}
}

void ExamScene::CreateBomb(Tile &tile, int playerColGroupID, int size, int id)
{	
	if (!tile.hasBomb)
	{
		auto physX = PhysxManager::GetInstance()->GetPhysics();

		auto material = physX->createMaterial(0.5f, 0.5f, 0.5f);

		Bomb bomb;

		auto m_Bomb = new GameObject();
		auto pBombModel = new ModelComponent(L"./Resources/Meshes/Bomb.ovm");
		pBombModel->SetMaterial(0);

		m_Bomb->AddComponent(pBombModel);

		for (size_t i = 0; i < m_Tiles.size(); ++i)
		{
			if (tile.row == m_Tiles[i].row && tile.col == m_Tiles[i].col)
			{
				m_Bomb->GetTransform()->Translate(tile.position.x, 0.1f, tile.position.y);

				bomb.obj = m_Bomb;
				bomb.row = m_Tiles[i].row;
				bomb.col = m_Tiles[i].col;
				bomb.size = size;
				bomb.id = id;

				m_Tiles[i].bomb = bomb;
				m_Tiles[i].hasBomb = true;

				if (id == 1)
				{
					m_LiveBombs1++;
				}
				else
				{
					m_LiveBombs2++;
				}

				break;
			}
		}

		auto rigidBody = new RigidBodyComponent();	

		if (playerColGroupID == 1)
		{
			rigidBody->SetCollisionGroup(CollisionGroupFlag::Group2);
			rigidBody->SetCollisionIgnoreGroups(static_cast<CollisionGroupFlag>(Group1));
		}
		else if(playerColGroupID == 2)
		{
			rigidBody->SetCollisionGroup(CollisionGroupFlag::Group1);
			rigidBody->SetCollisionIgnoreGroups(static_cast<CollisionGroupFlag>(Group2));
		}

		rigidBody->SetKinematic(true);

		m_Bomb->AddComponent(rigidBody);

		auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Resources\\Meshes\\Crate.ovpc");
		std::shared_ptr<PxGeometry> geom(new PxConvexMeshGeometry(pConvexMesh));

		ColliderComponent* collider = new ColliderComponent(geom, *material);

		m_Bomb->AddComponent(collider);

		AddChild(m_Bomb);
	}
}

Tile ExamScene::GetTile(GameObject * obj)
{
	XMFLOAT3 objPos = obj->GetTransform()->GetPosition();

	XMFLOAT2 testPos = XMFLOAT2(objPos.x, objPos.z);
	testPos.x += 90.0f + 7.5f;
	testPos.y += 60.0f - 7.5f;

	int row = int(8 - testPos.y / 15);
	int col = int(testPos.x / 15);

	Tile tile;
	tile = m_Tiles[0];

	for (size_t i = 0; i < m_Tiles.size(); ++i)
	{
		if (m_Tiles[i].row == row)
		{
			if (m_Tiles[i].col == col)
			{
				tile = m_Tiles[i];
				//Logger::LogWarning(L"Tile pos: " + to_wstring(tile.position.x) + L" " + to_wstring(tile.position.y) + L" Player pos: " + to_wstring(obj->GetTransform()->GetPosition().x) + L" " + to_wstring(obj->GetTransform()->GetPosition().z) + L" Row/Col: " + to_wstring(row) + L" " + to_wstring(col));
				break;
			}
		}
	}

	return tile;
}

void ExamScene::CreateParticle(Tile & tile)
{
	auto pObj = new GameObject();

	auto pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/Fireball.png", 10);
	pParticleEmitter->SetVelocity(XMFLOAT3(0, 0, 0));
	pParticleEmitter->SetMinSize(5.0f);
	pParticleEmitter->SetMaxSize(10.0f);
	pParticleEmitter->SetMinEnergy(1.5f);
	pParticleEmitter->SetMaxEnergy(3.0f);
	pParticleEmitter->SetMinSizeGrow(2.f);
	pParticleEmitter->SetMaxSizeGrow(2.5f);
	pParticleEmitter->SetMinEmitterRange(0.f);
	pParticleEmitter->SetMaxEmitterRange(5.f);
	pParticleEmitter->SetColor(XMFLOAT4(1.f, 0.6f, 0.f, 0.6f));
	pObj->AddComponent(pParticleEmitter);
	AddChild(pObj);

	pObj->GetTransform()->Translate(tile.position.x, 5.f, tile.position.y);
	//pObj->GetTransform()->Translate(tile.position.x, 3.f, 0);
	//pObj->GetTransform()->Translate(0, tile.position.y, 1);
	//pObj->GetTransform()->Translate(tile.position.x, tile.position.y, 1);

	//RemoveChild(tile.particle.obj);

	tile.particle = ParticleStr();

	ParticleStr particle;
	particle.obj = pObj;
	particle.timer = tile.fireTimer;

	tile.particle = particle;
	m_Particles.push_back(particle);
}

void ExamScene::SpawnPickUp(Tile & tile, PowerUpTypes type)
{
	if (!tile.powerUp)
	{
		auto powerUp = new GameObject();
		auto BombModel = new ModelComponent(L"./Resources/Meshes/Bomb.ovm");
		BombModel->SetMaterial(0);

		powerUp->AddComponent(BombModel);

		if (type == PowerUpTypes::BombCount)
		{
			auto PowerUpModel = new ModelComponent(L"./Resources/Meshes/BombCount.ovm");
			PowerUpModel->SetMaterial(7);
			powerUp->AddComponent(PowerUpModel);
		}
		else if (type == PowerUpTypes::BombSize)
		{
			auto PowerUpModel = new ModelComponent(L"./Resources/Meshes/BombSize.ovm");
			PowerUpModel->SetMaterial(7);
			powerUp->AddComponent(PowerUpModel);
		}

		//auto physX = PhysxManager::GetInstance()->GetPhysics();

		//auto PxMaterial = physX->createMaterial(1, 1, 0);

		powerUp->GetTransform()->Translate(tile.position.x, 0.1f, tile.position.y);

		//auto rigidBody = new RigidBodyComponent(true);
		//powerUp->AddComponent(rigidBody);

		//auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Resources\\Meshes\\Bomb.ovpc");
		//std::shared_ptr<PxGeometry> geom(new PxConvexMeshGeometry(pConvexMesh));
		//
		//auto col = new ColliderComponent(geom, *PxMaterial);
		//col->EnableTrigger(true);

		//powerUp->AddComponent(col);

		//powerUp->SetOnTriggerCallBack([&](GameObject* trigger, GameObject* other, GameObject::TriggerAction action)
		//{
		//	if (action == GameObject::TriggerAction::ENTER)
		//	{
		//		if (other->GetTag() == L"Player1")
		//		{
		//			if (type == PowerUpTypes::BombCount)
		//			{
		//				IncreaseCount(1);
		//			}
		//			else if (type == PowerUpTypes::BombSize)
		//			{
		//				IncreaseSize(1);
		//			}
		//		}
		//		if (other->GetTag() == L"Player2")
		//		{
		//			if (type == PowerUpTypes::BombCount)
		//			{
		//				IncreaseCount(2);
		//			}
		//			else if (type == PowerUpTypes::BombSize)
		//			{
		//				IncreaseSize(2);
		//			}
		//		}
		//
		//		RemoveChild(trigger);
		//	}
		//});

		AddChild(powerUp);

		tile.powerUp = true;
		tile.powerObj = powerUp;
		tile.type = type;
	}
}

void ExamScene::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//GetPhysxProxy()->EnablePhysxDebugRendering(true);

	m_pFMODSystem = SoundManager::GetInstance()->GetSystem();
	
	auto m_FMODResult = m_pFMODSystem->createSound("./Resources/Sounds/Explosion1.mp3", FMOD_2D, 0, &m_pExp1Sound);
	if (m_FMODResult != FMOD_OK) {
		cout << "ExpSound1 Not OK" << endl;
	}

	m_FMODResult = m_pFMODSystem->createSound("./Resources/Sounds/Explosion2.mp3", FMOD_2D, 0, &m_pExp2Sound);
	if (m_FMODResult != FMOD_OK) {
		cout << "ExpSound2 Not OK" << endl;
	}

	m_FMODResult = m_pFMODSystem->createSound("./Resources/Sounds/Explosion3.mp3", FMOD_2D, 0, &m_pExp3Sound);
	if (m_FMODResult != FMOD_OK) {
		cout << "ExpSound3 Not OK" << endl;
	}

	m_FMODResult = m_pFMODSystem->createSound("./Resources/Sounds/Bomberman_Theme.mp3", FMOD_LOOP_NORMAL, 0, &m_pThemeSound);
	if (m_FMODResult != FMOD_OK) {
		cout << "ThemeSound Not OK" << endl;
	}

	m_pChannel->setVolume(.5f);
	m_pThemeChannel->setVolume(0.1f);

	m_Font = ContentManager::Load<SpriteFont>(L"Resources/Fonts/Consolas_32.fnt");

	//GROUND PLANE
	//************
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto material = physX->createMaterial(1, 1, 0);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<PxGeometry> geom(new PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom, *material, PxTransform(PxQuat(XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(ground);

	//Camera
	//******
	m_pCamera = new FixedCamera();
	AddChild(m_pCamera);
	m_pCamera->GetTransform()->Translate(0.f, 160.f, -100.f);
	m_pCamera->GetTransform()->Rotate(60.f, 0.f, 0.f);
	SetActiveCamera((m_pCamera->GetComponent<CameraComponent>()));

	//Materials
	//*********
	auto bombMat = new DiffuseMaterial_Shadow();
	bombMat->SetDiffuseTexture(L"./Resources/Textures/Bomb2.jpg");

	gameContext.pMaterialManager->AddMaterial(bombMat, 0);

	auto playerMat = new SkinnedDiffuseMaterial_Shadow();
	playerMat->SetDiffuseTexture(L"./Resources/Textures/Tex_Bomberman1.jpg");
	
	gameContext.pMaterialManager->AddMaterial(playerMat, 1);

	auto stoneMat = new DiffuseMaterial_Shadow();
	stoneMat->SetDiffuseTexture(L"./Resources/Textures/Crate+Stone2.jpg");

	gameContext.pMaterialManager->AddMaterial(stoneMat, 2);

	auto arenaMat = new DiffuseMaterial_Shadow();
	arenaMat->SetDiffuseTexture(L"./Resources/Textures/Arena2.jpg");

	gameContext.pMaterialManager->AddMaterial(arenaMat, 3);

	auto stone2Mat = new DiffuseMaterial_Shadow();
	stone2Mat->SetDiffuseTexture(L"./Resources/Textures/Tex_Stone.jpg");

	gameContext.pMaterialManager->AddMaterial(stone2Mat, 4);

	auto floorMat = new DiffuseMaterial();
	floorMat->SetDiffuseTexture(L"./Resources/Textures/Floor.png");

	gameContext.pMaterialManager->AddMaterial(floorMat, 5);

	auto player2Mat = new SkinnedDiffuseMaterial_Shadow();
	player2Mat->SetDiffuseTexture(L"./Resources/Textures/Tex_Bomberman_Black.jpg");

	gameContext.pMaterialManager->AddMaterial(player2Mat, 6);

	auto greenMat = new DiffuseMaterial_Shadow();
	greenMat->SetDiffuseTexture(L"./Resources/Textures/Green.jpg");

	gameContext.pMaterialManager->AddMaterial(greenMat, 7);

	//Arena Mesh
	//***********
	auto pArenaObj = new GameObject();
	auto pArenaModel = new ModelComponent(L"./Resources/Meshes/Arena.ovm");
	pArenaModel->SetMaterial(3);

	pArenaObj->AddComponent(pArenaModel);

	auto rigidBody = new RigidBodyComponent(true);
	pArenaObj->AddComponent(rigidBody);

	auto pTriangleArenaMesh = ContentManager::Load<PxTriangleMesh>(L"Resources\\Meshes\\Arena.ovpt");
	std::shared_ptr<PxGeometry> ArenaGeom(new PxTriangleMeshGeometry(pTriangleArenaMesh));
	pArenaObj->AddComponent(new ColliderComponent(ArenaGeom, *material));

	AddChild(pArenaObj);

	//Tile Array
	//************
	int rows = 9;
	int cols = 13;

	for (int i = 0; i < (rows * cols); ++i)
	{
		float x = -90.0f + ((i % 13) * 15);
		float z = 60.0f - ((i / 13) * 15);

		int row = (i / 13);
		int col = (i % 13);

		Tile tile;

		m_Tiles.push_back(tile);

		if (row % 2 == 1)
		{
			if (col % 2 == 1)
			{
				m_Tiles[i].stone = true;
			}
		}

		m_Tiles[i].position = XMFLOAT2(x, z);
		m_Tiles[i].row = row;
		m_Tiles[i].col = col;
		m_Tiles[i].particle = ParticleStr();

		//CreateBomb(m_Tiles[i]);
	}

	//Stone Meshes
	//************
	for (int i = 0; i < 24; ++i) //4 ROWS, 6 COLS
	{
		float x = -75.0f + ((i % 6) * 30);
		float z = 45.0f - ((i / 6) * 30);

		auto pStoneObj = new GameObject();
		auto pStoneModel = new ModelComponent(L"./Resources/Meshes/Stone2.ovm");
		//auto pStoneModel = new ModelComponent(L"./Resources/Meshes/Stone2.ovm");
		pStoneModel->SetMaterial(4);

		pStoneObj->AddComponent(pStoneModel);
		pStoneObj->GetTransform()->Translate(x, 1.f, z);
		pStoneObj->GetTransform()->Rotate(90, 90, 0);

		//pStoneObj->GetTransform()->Scale(0.15f, 0.15f, 0.15f);

		auto stoneRigidBody = new RigidBodyComponent(true);
		pStoneObj->AddComponent(stoneRigidBody);

		auto pConvexMesh = ContentManager::Load<PxTriangleMesh>(L"Resources\\Meshes\\Stone2.ovpt");
		//auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Resources\\Meshes\\Stone2.ovpc");

		std::shared_ptr<PxGeometry> stoneGeom(new PxTriangleMeshGeometry(pConvexMesh));
		//std::shared_ptr<PxGeometry> stoneGeom(new PxConvexMeshGeometry(pConvexMesh));

		auto collider = new ColliderComponent(stoneGeom, *material);

		pStoneObj->AddComponent(collider);

		AddChild(pStoneObj);
	}

	//Crates
	//******
	for (size_t i = 0; i < m_Tiles.size(); ++i)
	{
		switch (m_Tiles[i].row)
		{
		case 0: //
			if (m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 5 || m_Tiles[i].col == 7 || m_Tiles[i].col == 10 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 1: //
			if (m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 7 || m_Tiles[i].col == 10 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 2: //
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 1 || m_Tiles[i].col == 2 || m_Tiles[i].col == 3 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 8 || m_Tiles[i].col == 9 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 3: //
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 8 || m_Tiles[i].col == 10 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 4: //
			if (m_Tiles[i].col == 2 || m_Tiles[i].col == 3 || m_Tiles[i].col == 7 || m_Tiles[i].col == 8 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 5: //
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 8 || m_Tiles[i].col == 10 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 6://
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 1 || m_Tiles[i].col == 2 || m_Tiles[i].col == 3 || m_Tiles[i].col == 6 || m_Tiles[i].col == 7 || m_Tiles[i].col == 8 || m_Tiles[i].col == 9 || m_Tiles[i].col == 10 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 7://
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 2 || m_Tiles[i].col == 10)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 8://
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 3 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 7 || m_Tiles[i].col == 8 || m_Tiles[i].col == 9 || m_Tiles[i].col == 10)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		default:
			break;
		}
	}

	//GROUND
	//******
	auto pFloorObj = new GameObject();
	auto pGroundModel = new ModelComponent(L"./Resources/Meshes/UnitPlane.ovm");
	pGroundModel->SetMaterial(5);

	pFloorObj->AddComponent(pGroundModel);
	pFloorObj->GetTransform()->Translate(0, -.5f, 0);
	pFloorObj->GetTransform()->Scale(50, 1.0f, 50);

	AddChild(pFloorObj);


	//Characters
	//****************
	m_Player1Obj = new GameObject();
	auto playerChild1 = new GameObject();

	m_pPlayerModel1 = new ModelComponent(L"./Resources/Meshes/Player.ovm");
	m_pPlayerModel1->SetMaterial(1);

	playerChild1->AddComponent(m_pPlayerModel1);
	//m_pPlayerModel1->GetTransform()->Scale(0.1f, 0.1f, 0.1f);

	auto playerPXMat = physX->createMaterial(1, 1, 0);
	m_pController1 = new ControllerComponent(playerPXMat, 5, 20);

	m_pController1->SetCollisionGroup(CollisionGroupFlag::Group1);
	m_pController1->SetCollisionIgnoreGroups(static_cast<CollisionGroupFlag>(Group2));

	m_Player1Obj->AddComponent(m_pController1);

	playerChild1->GetTransform()->Translate(0, -10, 0);
	//m_Player1Obj->GetTransform()->Translate(-90, 15, 60);

	AddChild(m_Player1Obj);
	m_Player1Obj->AddChild(playerChild1);
	m_pController1->Translate(-90, 15, 60);
	m_pPlayerModel1->GetAnimator()->SetAnimation(0);
	m_pPlayerModel1->GetAnimator()->Play();

	//PLAYER2
	m_Player2Obj = new GameObject();
	auto playerChild2 = new GameObject();

	m_pPlayerModel2 = new ModelComponent(L"./Resources/Meshes/Player.ovm");
	m_pPlayerModel2->SetMaterial(6);

	playerChild2->AddComponent(m_pPlayerModel2);
	//m_pPlayerModel2->GetTransform()->Scale(0.1f, 0.1f, 0.1f);

	auto playerMat2 = physX->createMaterial(1, 1, 0);
	m_pController2 = new ControllerComponent(playerMat2, 5, 20);

	m_pController2->SetCollisionGroup(CollisionGroupFlag::Group2);
	m_pController2->SetCollisionIgnoreGroups(static_cast<CollisionGroupFlag>(Group1));

	m_Player2Obj->AddComponent(m_pController2);

	playerChild2->GetTransform()->Translate(0, -10, 0);
	//m_Player2Obj->GetTransform()->Translate(90, 15, -60);

	AddChild(m_Player2Obj);
	m_Player2Obj->AddChild(playerChild2);
	m_pController2->Translate(90, 15, -60);
	m_pPlayerModel2->GetAnimator()->SetAnimation(0);
	m_pPlayerModel2->GetAnimator()->Play();

	m_Player1Obj->SetTag(L"Player1");

	m_Player2Obj->SetTag(L"Player2");

	//SPRITES
	//*******
	m_pSprite1 = new GameObject();
	m_pSprite1->AddComponent(new SpriteComponent(L"./Resources/Textures/BombermanLeftWhite.png", XMFLOAT2(0.f, 0.f), XMFLOAT4(1, 1, 1, 1)));
	AddChild(m_pSprite1);

	m_pSprite1->GetTransform()->Scale(0.6f, 0.6f, 0.6f);

	m_pSprite2 = new GameObject();
	m_pSprite2->AddComponent(new SpriteComponent(L"./Resources/Textures/BombermanRightBlack.png", XMFLOAT2(0.f, 0), XMFLOAT4(1, 1, 1, 1)));
	AddChild(m_pSprite2);

	m_pSprite2->GetTransform()->Translate(1280 - (256 * 0.6f), 0, 0);
	m_pSprite2->GetTransform()->Scale(0.6f, 0.6f, 0.6f);

	//PAUSE SCREEN
	//************
	//m_pPauseScreen = new GameObject();
	//m_pPauseScreen->AddComponent(new SpriteComponent(L"./Resources/Textures/PauseScreen.png", XMFLOAT2(0.f, 0.f), XMFLOAT4(1, 1, 1, 0)));
	//m_pPauseScreen->GetTransform()->Translate(320, 180, 0);
	//AddChild(m_pPauseScreen);

	//ControlScreen
	//*************
	m_pControlScreen = new GameObject();
	m_pControlScreen->AddComponent(new SpriteComponent(L"./Resources/Textures/GP2Exam2018_Controls.png", XMFLOAT2(0.f, 0.f), XMFLOAT4(1, 1, 1, 1)));
	AddChild(m_pControlScreen);

	//Input
	//*****
	gameContext.pInput->AddInputAction(InputAction(0, Down, 'W'));
	gameContext.pInput->AddInputAction(InputAction(1, Down, 'A'));
	gameContext.pInput->AddInputAction(InputAction(2, Down, 'S'));
	gameContext.pInput->AddInputAction(InputAction(3, Down, 'D'));
	gameContext.pInput->AddInputAction(InputAction(4, Pressed, VK_SPACE));

	gameContext.pInput->AddInputAction(InputAction(5, Down, VK_UP));
	gameContext.pInput->AddInputAction(InputAction(6, Down, VK_DOWN));
	gameContext.pInput->AddInputAction(InputAction(7, Down, VK_LEFT));
	gameContext.pInput->AddInputAction(InputAction(8, Down, VK_RIGHT));
	gameContext.pInput->AddInputAction(InputAction(9, Pressed, VK_RCONTROL));

	gameContext.pInput->AddInputAction(InputAction(10, Pressed, 'B')); //DEBUG BREAK

	gameContext.pInput->AddInputAction(InputAction(11, Pressed, 'P'));
	gameContext.pInput->AddInputAction(InputAction(12, Pressed, -1, VK_LBUTTON));
	gameContext.pInput->AddInputAction(InputAction(13, Pressed, 'E')); //Effect

	gameContext.pShadowMapRenderer->SetLight({ 0, 122, -10 }, { 0, -0.9f, 0.1f });

	gameContext.pMaterialManager->AddMaterial_PP(new PostSepia(), 0);
}

void ExamScene::Update(const GameContext& gameContext)
{
	if (gameContext.pInput->IsActionTriggered(13))
	{
		if (m_Sepia)
		{
			m_Sepia = false;
			RemovePostProcessingMaterial(0);
		}
		else
		{
			m_Sepia = true;
			AddPostProcessingMaterial(0);
		}
	}

	cout << gameContext.pGameTime->GetFPS() << endl;

	UNREFERENCED_PARAMETER(gameContext);
	if (m_StartDelay > 0)
	{
		bool playing = false;
		m_pThemeChannel->isPlaying(&playing);

		if (playing)
		{
			m_pThemeChannel->stop();
		}

		m_StartDelay -= gameContext.pGameTime->GetElapsed();

		if (m_StartDelay <= 0.0f)
		{
			RemoveChild(m_pControlScreen);
			//m_pControlScreen->GetComponent<SpriteComponent>()->SetColor(XMFLOAT4(0, 0, 0, 0));
			m_pFMODSystem->playSound(m_pThemeSound, 0, false, &m_pThemeChannel);
		}
	}
	else
	{
		if (m_Paused)
		{
			if (gameContext.pInput->IsActionTriggered(12))
			{
				std::cout << gameContext.pInput->GetMousePosition().x << " " << gameContext.pInput->GetMousePosition().y << std::endl;

				if ((gameContext.pInput->GetMousePosition().x > 230 + 320 && gameContext.pInput->GetMousePosition().x < 410 + 320) && (gameContext.pInput->GetMousePosition().y > 75 + 180 && gameContext.pInput->GetMousePosition().y < 130 + 180))
				{
					m_Paused = false;
					RemoveChild(m_pPauseScreen);
					//m_pPauseScreen->GetComponent<SpriteComponent>()->SetColor(XMFLOAT4(0, 0, 0, 0));
					CleanResetScene(gameContext);
					SceneManager::GetInstance()->SetActiveGameScene(L"StartScene");
				}
				else if ((gameContext.pInput->GetMousePosition().x > 230 + 320 && gameContext.pInput->GetMousePosition().x < 410 + 320) && (gameContext.pInput->GetMousePosition().y > 144 + 180 && gameContext.pInput->GetMousePosition().y < 200 + 180))
				{
					m_Paused = false;
					RemoveChild(m_pPauseScreen);
					//m_pPauseScreen->GetComponent<SpriteComponent>()->SetColor(XMFLOAT4(0, 0, 0, 0));
					CleanResetScene(gameContext);
				}
				else if ((gameContext.pInput->GetMousePosition().x > 230 + 320 && gameContext.pInput->GetMousePosition().x < 410 + 320) && (gameContext.pInput->GetMousePosition().y > 215 + 180 && gameContext.pInput->GetMousePosition().y < 270 + 180))
				{
					PostQuitMessage(0);
				}
			}
		}

		if (gameContext.pInput->IsActionTriggered(11)) //PAUSE
		{
			if (!m_Paused)
			{
				m_pThemeChannel->setPaused(true);

				m_Paused = true;
				m_pPauseScreen = new GameObject();
				m_pPauseScreen->AddComponent(new SpriteComponent(L"./Resources/Textures/PauseScreen.png", XMFLOAT2(0.f, 0.f), XMFLOAT4(1, 1, 1, 1)));
				m_pPauseScreen->GetTransform()->Translate(320, 180, 0);
				AddChild(m_pPauseScreen);
				//m_pPauseScreen->GetComponent<SpriteComponent>()->SetColor(XMFLOAT4(1, 1, 1, 1));
			}
			else
			{
				m_pThemeChannel->setPaused(false);

				m_Paused = false;
				RemoveChild(m_pPauseScreen);
				//m_pPauseScreen->GetComponent<SpriteComponent>()->SetColor(XMFLOAT4(0, 0, 0, 0));
			}
		}

		Tile player1Tile = GetTile(m_Player1Obj);
		Tile player2Tile = GetTile(m_Player2Obj);

		for (size_t i = 0; i < m_Tiles.size(); i++)
		{
			if (player1Tile.row == m_Tiles[i].row && player1Tile.col == m_Tiles[i].col)
			{
				if (m_Tiles[i].powerUp)
				{
					switch (m_Tiles[i].type)
					{
					case PowerUpTypes::BombCount:
						m_BombCount1++;
						RemoveChild(m_Tiles[i].powerObj);
						break;
					case PowerUpTypes::BombSize:
						m_BombSize1++;
						RemoveChild(m_Tiles[i].powerObj);
						break;
					default:
						break;
					}

					m_Tiles[i].powerUp = false;
					m_Tiles[i].type = PowerUpTypes::None;
				}

				if (m_Tiles[i].onFire)
				{
					Logger::LogWarning(L"Player 1 DIED");

					//RemoveChild(m_Player1Obj);

					--m_Lives1;

					if (m_Lives1 < 0)
					{
						m_GameOver = true;
					}
					else
					{
						ResetScene(gameContext);
					}
				}
			}
			
			if (player2Tile.row == m_Tiles[i].row && player2Tile.col == m_Tiles[i].col)
			{
				if (m_Tiles[i].powerUp)
				{
					switch (m_Tiles[i].type)
					{
					case PowerUpTypes::BombCount:
						m_BombCount2++;
						RemoveChild(m_Tiles[i].powerObj);
						break;
					case PowerUpTypes::BombSize:
						m_BombSize2++;
						RemoveChild(m_Tiles[i].powerObj);
						break;
					default:
						break;
					}

					m_Tiles[i].powerUp = false;
					m_Tiles[i].type = PowerUpTypes::None;
				}

				if (m_Tiles[i].onFire)
				{
					Logger::LogWarning(L"Player 2 DIED");

					//RemoveChild(m_Player2Obj);

					--m_Lives2;

					if (m_Lives2 < 0)
					{
						m_GameOver = true;
					}
					else
					{
						ResetScene(gameContext);
					}
				}
			}
		}

		if (!m_GameOver && !m_Paused)
		{
			m_RemainingTime -= gameContext.pGameTime->GetElapsed();

			if (m_RemainingTime <= 0.0f)
			{
				m_GameOver = true;
				return;
			}

			for (size_t i = 0; i < m_Tiles.size(); ++i)
			{
				if (m_Tiles[i].hasBomb)
				{
					m_Tiles[i].bomb.timer -= gameContext.pGameTime->GetElapsed();

					if (m_Tiles[i].bomb.timer <= 0.0f)
					{
						SetExplosionTilesRight(m_Tiles[i].bomb, 0);
						SetExplosionTilesLeft(m_Tiles[i].bomb, 1);
						SetExplosionTilesUp(m_Tiles[i].bomb, 1);
						SetExplosionTilesDown(m_Tiles[i].bomb, 1);

						DecreaseCount(m_Tiles[i].bomb.id);

						RemoveChild(m_Tiles[i].bomb.obj);

						auto random = rand() % 3;

						switch (random)
						{
						case 0:
							m_pFMODSystem->playSound(m_pExp1Sound, 0, false, &m_pChannel);
							break;
						case 1:
							m_pFMODSystem->playSound(m_pExp2Sound, 0, false, &m_pChannel);
							break;
						case 2:
							m_pFMODSystem->playSound(m_pExp3Sound, 0, false, &m_pChannel);
							break;
						default:
							break;
						}
					}
				}
				if ( m_Tiles[i].onFire)
				{
					if (m_Tiles[i].hasBomb)
					{
						m_Tiles[i].bomb.timer = 0.0f;
					}

					m_Tiles[i].fireTimer -= gameContext.pGameTime->GetElapsed();

					if (m_Tiles[i].fireTimer <= 0.0f)
					{
						m_Tiles[i].onFire = false;

						//m_Tiles[i].particle.obj->GetComponent<ParticleEmitterComponent>()->SetColor(XMFLOAT4(0, 0, 0, 0));
						//m_Tiles[i].particle.obj->RemoveComponent(m_Tiles[i].particle.obj->GetComponent<ParticleEmitterComponent>());
						//RemoveChild(m_Tiles[i].particle.obj);
						//m_Tiles[i].particle = Particle();
						m_Tiles[i].fireTimer = 3.0f;
					}
				}
			}

			float speed = 50.0f;

			//PLAYER 1
			//********
			XMFLOAT3 zeroFloat3 = XMFLOAT3(0.f, 0.f, 0.f);
			XMFLOAT3 forwFloat3 = XMFLOAT3(0.f, 0.f, 1.f);
			XMFLOAT3 rightFloat3 = XMFLOAT3(1.f, 0.f, 0.f);
			XMFLOAT3 gravFloat3 = XMFLOAT3(0.f, -1.f, 0.f);

			XMVECTOR movementVec = XMLoadFloat3(&zeroFloat3);
			XMVECTOR sceneForwardVec = XMLoadFloat3(&forwFloat3);
			XMVECTOR sceneRightVec = XMLoadFloat3(&rightFloat3);
			XMVECTOR sceneGravVec = XMLoadFloat3(&gravFloat3);

			if (gameContext.pInput->IsActionTriggered(0)) //W
			{
				movementVec = sceneForwardVec;

				m_Player1Obj->GetTransform()->Rotate(0.f, 180.f, 0.f);
			}
			else if (gameContext.pInput->IsActionTriggered(1)) //A
			{
				movementVec = -sceneRightVec;

				m_Player1Obj->GetTransform()->Rotate(0.f, 90.f, 0.f);
			}
			else if (gameContext.pInput->IsActionTriggered(2)) //S
			{
				movementVec = -sceneForwardVec;

				m_Player1Obj->GetTransform()->Rotate(0.f, 0.f, 0.f);
			}
			else if (gameContext.pInput->IsActionTriggered(3)) //D
			{
				movementVec = sceneRightVec;

				m_Player1Obj->GetTransform()->Rotate(0.f, 270.f, 0.f);
			}

			movementVec += sceneGravVec;

			movementVec *= speed * gameContext.pGameTime->GetElapsed();

			XMFLOAT3 displacement;
			XMStoreFloat3(&displacement, movementVec);

			if (displacement.x != 0 || displacement.z != 0)
			{
				if (m_CurrAnimIndex1 != 1)
				{
					m_CurrAnimIndex1 = 1;
					m_pPlayerModel1->GetAnimator()->SetAnimation(m_CurrAnimIndex1);
				}
			}
			else
			{
				if (m_CurrAnimIndex1 != 0)
				{
					m_CurrAnimIndex1 = 0;
					m_pPlayerModel1->GetAnimator()->SetAnimation(m_CurrAnimIndex1);
				}
			}

			m_pController1->Move(displacement);

			if (gameContext.pInput->IsActionTriggered(4) && m_LiveBombs1 < m_BombCount1) //SPACE
			{

				CreateBomb(GetTile(m_Player1Obj), 1, m_BombSize1, 1);
			}

			movementVec = XMLoadFloat3(&zeroFloat3);

			if (gameContext.pInput->IsActionTriggered(5)) //Up
			{
				movementVec = sceneForwardVec;

				m_Player2Obj->GetTransform()->Rotate(0.f, 180.f, 0.f);
			}
			else if (gameContext.pInput->IsActionTriggered(7)) //Left
			{
				movementVec = -sceneRightVec;

				m_Player2Obj->GetTransform()->Rotate(0.f, 90.f, 0.f);
			}
			else if (gameContext.pInput->IsActionTriggered(6)) //Down
			{
				movementVec = -sceneForwardVec;

				m_Player2Obj->GetTransform()->Rotate(0.f, 0.f, 0.f);
			}
			else if (gameContext.pInput->IsActionTriggered(8)) //Right
			{
				movementVec = sceneRightVec;

				m_Player2Obj->GetTransform()->Rotate(0.f, 270.f, 0.f);
			}

			movementVec += sceneGravVec;

			movementVec *= speed * gameContext.pGameTime->GetElapsed();

			XMStoreFloat3(&displacement, movementVec);

			if (displacement.x != 0 || displacement.z != 0)
			{
				if (m_CurrAnimIndex2 != 1)
				{
					m_CurrAnimIndex2 = 1;
					m_pPlayerModel2->GetAnimator()->SetAnimation(m_CurrAnimIndex2);
				}
			}
			else
			{
				if (m_CurrAnimIndex2 != 0)
				{
					m_CurrAnimIndex2 = 0;
					m_pPlayerModel2->GetAnimator()->SetAnimation(m_CurrAnimIndex2);
				}
			}

			m_pController2->Move(displacement);

			if (gameContext.pInput->IsActionTriggered(9) && m_LiveBombs2 < m_BombCount2) //RControl
			{
				CreateBomb(GetTile(m_Player2Obj), 2, m_BombSize2, 2);
			}
			 
			for (size_t i = 0; i < m_Particles.size(); ++i)
			{
				m_Particles[i].timer -= gameContext.pGameTime->GetElapsed();

				if (m_Particles[i].timer <= 0.0f)
				{
					RemoveChild(m_Particles[i].obj);
					m_Particles.erase(m_Particles.begin() + i);
				}
			}
		}
		else if (!m_Paused)
		{
			m_pThemeChannel->stop();
			CleanResetScene(gameContext);
			SceneManager::GetInstance()->SetActiveGameScene(L"EndScene");
		}
	}
}

void ExamScene::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if (m_StartDelay <= 0)
	{
		TextRenderer::GetInstance()->DrawText(m_Font, to_wstring(int(m_RemainingTime)), { 630,20 }, { 1,1,0,1 });
		TextRenderer::GetInstance()->DrawText(m_Font, to_wstring(int(m_Lives1)), { 30, 130 }, { 1,1,1,1 });
		TextRenderer::GetInstance()->DrawText(m_Font, to_wstring(int(m_Lives2)), { 1250, 130 }, { 1,1,1,1 });
	}
}

void ExamScene::SetExplosionTilesRight(Bomb bomb, int it)
{
	if (it == 0)
	{
		for (size_t i = 0; i < m_Tiles.size(); ++i)
		{
			if (bomb.row == m_Tiles[i].row && bomb.col == m_Tiles[i].col)
			{
				m_Tiles[i].onFire = true;
				m_Tiles[i].hasBomb = false;

				CreateParticle(m_Tiles[i]);

				SetExplosionTilesRight(bomb, it + 1);
			}
		}
	}
	else if(it <= bomb.size)
	{
		for (size_t i = 0; i < m_Tiles.size(); ++i)
		{
			if (m_Tiles[i].col == bomb.col + it && m_Tiles[i].row == bomb.row)
			{
				if (!m_Tiles[i].stone)
				{
					if (m_Tiles[i].crate)
					{
						RemoveChild(m_Tiles[i].crateObj);
						m_Tiles[i].crate = false;

						CreateParticle(m_Tiles[i]);

						m_Tiles[i].onFire = true;

						int random = rand() % 15;

						if (random == 1 || random == 3)
						{
							SpawnPickUp(m_Tiles[i], PowerUpTypes::BombCount);
						}
						else if (random == 2)
						{
							SpawnPickUp(m_Tiles[i], PowerUpTypes::BombSize);
						}
					}
					else
					{
						m_Tiles[i].onFire = true;

						CreateParticle(m_Tiles[i]);

						SetExplosionTilesRight(bomb, it + 1);
					}
				}
			}
		}
	}
}

void ExamScene::SetExplosionTilesLeft(Bomb bomb, int it)
{
	if (it == 0)
	{
		for (size_t i = 0; i < m_Tiles.size(); ++i)
		{
			if (bomb.row == m_Tiles[i].row && bomb.col == m_Tiles[i].col)
			{
				m_Tiles[i].onFire = true;
				m_Tiles[i].hasBomb = false;

				CreateParticle(m_Tiles[i]);

				SetExplosionTilesLeft(bomb, it + 1);
			}
		}
	}
	else if (it <= bomb.size)
	{
		for (size_t i = 0; i < m_Tiles.size(); ++i)
		{
			if (m_Tiles[i].col == bomb.col - it && m_Tiles[i].row == bomb.row)
			{
				if (!m_Tiles[i].stone)
				{
					if (m_Tiles[i].crate)
					{
						RemoveChild(m_Tiles[i].crateObj);
						m_Tiles[i].crate = false;

						CreateParticle(m_Tiles[i]);

						m_Tiles[i].onFire = true;	

						int random = rand() % 15;

						if (random == 1 || random == 3)
						{
							SpawnPickUp(m_Tiles[i], PowerUpTypes::BombCount);
						}
						else if (random == 2)
						{
							SpawnPickUp(m_Tiles[i], PowerUpTypes::BombSize);
						}
					}
					else
					{
						m_Tiles[i].onFire = true;
						
						CreateParticle(m_Tiles[i]);

						SetExplosionTilesLeft(bomb, it + 1);
					}
				}
			}
		}
	}
}

void ExamScene::SetExplosionTilesUp(Bomb bomb, int it)
{
	if (it == 0)
	{
		for (size_t i = 0; i < m_Tiles.size(); ++i)
		{
			if (bomb.row == m_Tiles[i].row && bomb.col == m_Tiles[i].col)
			{
				m_Tiles[i].onFire = true;				
				m_Tiles[i].hasBomb = false;

				CreateParticle(m_Tiles[i]);

				SetExplosionTilesUp(bomb, it + 1);
			}
		}
	}
	else if (it <= bomb.size)
	{
		for (size_t i = 0; i < m_Tiles.size(); ++i)
		{
			if (m_Tiles[i].row == bomb.row + it && m_Tiles[i].col == bomb.col)
			{
				if (!m_Tiles[i].stone)
				{
					if (m_Tiles[i].crate)
					{
						RemoveChild(m_Tiles[i].crateObj);
						m_Tiles[i].crate = false;

						CreateParticle(m_Tiles[i]);

						m_Tiles[i].onFire = true;	

						int random = rand() % 15;

						if (random == 1 || random == 3)
						{
							SpawnPickUp(m_Tiles[i], PowerUpTypes::BombCount);
						}
						else if (random == 2)
						{
							SpawnPickUp(m_Tiles[i], PowerUpTypes::BombSize);
						}
					}
					else
					{
						m_Tiles[i].onFire = true;

						CreateParticle(m_Tiles[i]);

						SetExplosionTilesUp(bomb, it + 1);
					}
				}
			}
		}
	}
}

void ExamScene::SetExplosionTilesDown(Bomb bomb, int it)
{
	if (it == 0)
	{
		for (size_t i = 0; i < m_Tiles.size(); ++i)
		{
			if (bomb.row == m_Tiles[i].row && bomb.col == m_Tiles[i].col)
			{
				m_Tiles[i].onFire = true;
				m_Tiles[i].hasBomb = false;

				CreateParticle(m_Tiles[i]);

				SetExplosionTilesDown(bomb, it + 1);
			}
		}
	}
	else if (it <= bomb.size)
	{
		for (size_t i = 0; i < m_Tiles.size(); ++i)
		{
			if (m_Tiles[i].row == bomb.row - it && m_Tiles[i].col == bomb.col)
			{
				if (!m_Tiles[i].stone)
				{
					if (m_Tiles[i].crate)
					{
						RemoveChild(m_Tiles[i].crateObj);
						m_Tiles[i].crate = false;

						CreateParticle(m_Tiles[i]);

						m_Tiles[i].onFire = true;

						int random = rand() % 15;

						if (random == 1 || random == 3)
						{
							SpawnPickUp(m_Tiles[i], PowerUpTypes::BombCount);
						}
						else if (random == 2)
						{
							SpawnPickUp(m_Tiles[i], PowerUpTypes::BombSize);
						}
					}
					else
					{
						m_Tiles[i].onFire = true;						

						CreateParticle(m_Tiles[i]);

						SetExplosionTilesDown(bomb, it + 1);
					}
				}
			}
		}
	}
}

void ExamScene::IncreaseSize(int id)
{
	if (id == 1)
	{
		m_BombSize1++;
	}
	else if(id == 2)
	{
		m_BombSize2++;
	}
}

void ExamScene::IncreaseCount(int id)
{
	if (id == 1)
	{
		m_BombCount1++;
	}
	else if (id == 2)
	{
		m_BombCount2++;
	}
}

void ExamScene::DecreaseCount(int id)
{
	if (id == 1)
	{
		m_LiveBombs1--;
	}
	else if (id == 2)
	{
		m_LiveBombs2--;
	}
}

void ExamScene::ResetScene(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	for (size_t i = 0; i < m_Tiles.size(); ++i)
	{
		if (m_Tiles[i].crate)
		{
			RemoveChild(m_Tiles[i].crateObj);
			m_Tiles[i].crate = false;
		}
		if (m_Tiles[i].onFire)
		{
			m_Tiles[i].onFire = false;
			m_Tiles[i].fireTimer = 3;
			RemoveChild(m_Tiles[i].particle.obj);
			m_Tiles[i].particle = ParticleStr();
		}
		if (m_Tiles[i].hasBomb)
		{
			m_Tiles[i].hasBomb = false;
			RemoveChild(m_Tiles[i].bomb.obj);
			m_Tiles[i].bomb = Bomb();
		}
		if (m_Tiles[i].powerUp)
		{
			m_Tiles[i].powerUp = false;
			RemoveChild(m_Tiles[i].powerObj);
			m_Tiles[i].type = PowerUpTypes::None;
		}
	}

	for (size_t i = 0; i < m_Tiles.size(); ++i)
	{
		switch (m_Tiles[i].row)
		{
		case 0: //
			if (m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 5 || m_Tiles[i].col == 7 || m_Tiles[i].col == 10 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 1: //
			if (m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 7 || m_Tiles[i].col == 10 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 2: //
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 1 || m_Tiles[i].col == 2 || m_Tiles[i].col == 3 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 8 || m_Tiles[i].col == 9 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 3: //
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 8 || m_Tiles[i].col == 10 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 4: //
			if (m_Tiles[i].col == 2 || m_Tiles[i].col == 3 || m_Tiles[i].col == 7 || m_Tiles[i].col == 8 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 5: //
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 8 || m_Tiles[i].col == 10 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 6://
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 1 || m_Tiles[i].col == 2 || m_Tiles[i].col == 3 || m_Tiles[i].col == 6 || m_Tiles[i].col == 7 || m_Tiles[i].col == 8 || m_Tiles[i].col == 9 || m_Tiles[i].col == 10 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 7://
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 2 || m_Tiles[i].col == 10)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 8://
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 3 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 7 || m_Tiles[i].col == 8 || m_Tiles[i].col == 9 || m_Tiles[i].col == 10)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		default:
			break;
		}
	}

	m_pController1->Translate(-90, 15, 60);
	m_pPlayerModel1->GetAnimator()->SetAnimation(0);
	m_Player1Obj->GetTransform()->Rotate(0.f, 0.f, 0.f);

	m_pController2->Translate(90, 15, -60);
	m_pPlayerModel2->GetAnimator()->SetAnimation(0);
	m_Player2Obj->GetTransform()->Rotate(0.f, 0.f, 0.f);

	m_StartDelay = 5;

	m_RemainingTime = 300.0f;

	m_pControlScreen = new GameObject();
	m_pControlScreen->AddComponent(new SpriteComponent(L"./Resources/Textures/GP2Exam2018_Controls.png", XMFLOAT2(0.f, 0.f), XMFLOAT4(1, 1, 1, 1)));
	AddChild(m_pControlScreen);
	m_pControlScreen->GetComponent<SpriteComponent>()->SetColor(XMFLOAT4(1, 1, 1, 1));

	m_GameOver = false;

	//m_Lives1 = 3;
	m_BombCount1 = 1;
	m_BombSize1 = 2;
	m_LiveBombs1 = 0;

	//m_Lives2 = 3;
	m_BombCount2 = 1;
	m_BombSize2 = 2;
	m_LiveBombs2 = 0;

	for (ParticleStr part : m_Particles)
	{
		RemoveChild(part.obj);
	}

	m_Particles.clear();
}

void ExamScene::CleanResetScene(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	for (size_t i = 0; i < m_Tiles.size(); ++i)
	{
		if (m_Tiles[i].crate)
		{
			RemoveChild(m_Tiles[i].crateObj);
			m_Tiles[i].crate = false;
		}
		if (m_Tiles[i].onFire)
		{
			m_Tiles[i].onFire = false;
			m_Tiles[i].fireTimer = 3;
			RemoveChild(m_Tiles[i].particle.obj);
			m_Tiles[i].particle = ParticleStr();
		}
		if (m_Tiles[i].hasBomb)
		{
			m_Tiles[i].hasBomb = false;
			RemoveChild(m_Tiles[i].bomb.obj);
			m_Tiles[i].bomb = Bomb();
		}
		if (m_Tiles[i].powerUp)
		{
			m_Tiles[i].powerUp = false;
			RemoveChild(m_Tiles[i].powerObj);
			m_Tiles[i].type = PowerUpTypes::None;
		}
	}

	for (size_t i = 0; i < m_Tiles.size(); ++i)
	{
		switch (m_Tiles[i].row)
		{
		case 0: //
			if (m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 5 || m_Tiles[i].col == 7 || m_Tiles[i].col == 10 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 1: //
			if (m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 7 || m_Tiles[i].col == 10 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 2: //
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 1 || m_Tiles[i].col == 2 || m_Tiles[i].col == 3 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 8 || m_Tiles[i].col == 9 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 3: //
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 8 || m_Tiles[i].col == 10 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 4: //
			if (m_Tiles[i].col == 2 || m_Tiles[i].col == 3 || m_Tiles[i].col == 7 || m_Tiles[i].col == 8 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 5: //
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 2 || m_Tiles[i].col == 4 || m_Tiles[i].col == 8 || m_Tiles[i].col == 10 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 6://
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 1 || m_Tiles[i].col == 2 || m_Tiles[i].col == 3 || m_Tiles[i].col == 6 || m_Tiles[i].col == 7 || m_Tiles[i].col == 8 || m_Tiles[i].col == 9 || m_Tiles[i].col == 10 || m_Tiles[i].col == 11 || m_Tiles[i].col == 12)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 7://
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 2 || m_Tiles[i].col == 10)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		case 8://
			if (m_Tiles[i].col == 0 || m_Tiles[i].col == 3 || m_Tiles[i].col == 4 || m_Tiles[i].col == 6 || m_Tiles[i].col == 7 || m_Tiles[i].col == 8 || m_Tiles[i].col == 9 || m_Tiles[i].col == 10)
			{
				CreateCrate(m_Tiles[i]);
			}
			break;
		default:
			break;
		}
	}

	m_pController1->Translate(-90, 15, 60);
	m_pPlayerModel1->GetAnimator()->SetAnimation(0);
	m_Player1Obj->GetTransform()->Rotate(0.f, 0.f, 0.f);

	m_pController2->Translate(90, 15, -60);
	m_pPlayerModel2->GetAnimator()->SetAnimation(0);
	m_Player2Obj->GetTransform()->Rotate(0.f, 0.f, 0.f);

	m_StartDelay = 5;

	m_RemainingTime = 300.0f;

	m_pControlScreen = new GameObject();
	m_pControlScreen->AddComponent(new SpriteComponent(L"./Resources/Textures/GP2Exam2018_Controls.png", XMFLOAT2(0.f, 0.f), XMFLOAT4(1, 1, 1, 1)));
	AddChild(m_pControlScreen);
	m_pControlScreen->GetComponent<SpriteComponent>()->SetColor(XMFLOAT4(1, 1, 1, 1));

	m_GameOver = false;

	m_Lives1 = 3;
	m_BombCount1 = 1;
	m_BombSize1 = 2;
	m_LiveBombs1 = 0;

	m_Lives2 = 3;
	m_BombCount2 = 1;
	m_BombSize2 = 2;
	m_LiveBombs2 = 0;

	for (ParticleStr part : m_Particles)
	{
		RemoveChild(part.obj);
	}

	m_Particles.clear();
}
