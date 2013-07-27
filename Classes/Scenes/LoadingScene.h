#ifndef CLOADING_SCENE_H__
#define CLOADING_SCENE_H__

#include "cocos2d.h"

class CLoadingScene :public cocos2d::CCLayer
{
public:
	CLoadingScene(void);
	~CLoadingScene(void);

public:
	virtual bool init();
	static void show();

private :
	void clickStart(CCObject* pSender);
	void loadDone(float dt);
};

#endif