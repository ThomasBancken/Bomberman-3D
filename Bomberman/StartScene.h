#pragma once
#include "Scenegraph/GameScene.h"

class StartScene : public GameScene
{
public:
	StartScene(void);
	virtual ~StartScene(void);

protected:

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	StartScene(const StartScene &obj) = delete;
	StartScene& operator=(const StartScene& obj) = delete;
};

