#ifndef CMAIN_SCENE_H__
#define CMAIN_SCENE_H__

#include "cocos2d.h"

class CMainScene :public cocos2d::CCLayer
{
public:
	CMainScene(void);
	~CMainScene(void);

public:
	virtual bool init();
	static void show();

private :
	void clickStart(CCObject* pSender);
	void clickExit(CCObject* pSender);	
};

#endif