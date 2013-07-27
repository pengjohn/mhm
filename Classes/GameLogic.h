#ifndef GAME_LOGIC_H__
#define GAME_LOGIC_H__

class CGameLogic
{
public:
	static void GameStart();
	static void GamePause();
	static void GameSave();
	static void GameQuit();

	static void ShowMainScene();
	static void ShowFightScene();
};
#endif