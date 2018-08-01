#pragma once
#include "Scenegraph/GameScene.h"

class EndScene : public GameScene
{
public:
	EndScene(void);
	virtual ~EndScene(void);

protected:

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	EndScene(const EndScene &obj) = delete;
	EndScene& operator=(const EndScene& obj) = delete;
};

