#include "GameLogic.h"
#include "cocos2d.h"
#include "Scenes/LoadingScene.h"
#include "Scenes/MainScene.h"
#include "Scenes/FightScene.h"

void CGameLogic::GameStart()
{
	CLoadingScene::show();
}

void CGameLogic::GamePause()
{

}

void CGameLogic::GameSave()
{

}

void CGameLogic::GameQuit()
{

}

void CGameLogic::ShowMainScene()
{
	CMainScene::show();
}

void CGameLogic::ShowFightScene()
{
	CFightScene::show();
}