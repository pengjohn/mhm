#include "MainScene.h"
#include "GameLogic.h"
USING_NS_CC;

CMainScene::CMainScene(void)
{
}

CMainScene::~CMainScene(void)
{
	removeAllChildrenWithCleanup(true);
}

void CMainScene::show()
{
	CCScene *scene = CCScene::create();
	CMainScene *layer = new CMainScene();
	layer->init();
	scene->addChild(layer);
	CCScene* pScene = CCDirector::sharedDirector()->getRunningScene();
	if(!pScene)
		CCDirector::sharedDirector()->runWithScene(scene);
	else
	{
		CCDirector::sharedDirector()->replaceScene(scene);
	}
}


bool CMainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    // add Loading.png
    CCSprite* pSprite = CCSprite::create("Background_2.png");
    pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    this->addChild(pSprite, 0);
    
    //
    CCMenuItemImage *btn_start = CCMenuItemImage::create(
                                        "BtnStart.png",
                                        "BtnStartPress.png",
                                        this,menu_selector(CMainScene::clickStart));

    CCMenuItemImage *btn_setting = CCMenuItemImage::create(
                                        "BtnExit.png",
                                        "BtnExitPress.png",
                                        this,menu_selector(CMainScene::clickExit));                                        

		CCMenu* menu = CCMenu::create(btn_start, btn_setting, NULL);
		menu->alignItemsVerticallyWithPadding(10);
		menu->setPosition(ccp(visibleSize.width/2, visibleSize.height/4));
		this->addChild(menu);
		
		return true;
}
void CMainScene::clickStart(CCObject* pSender)
{
	CGameLogic::ShowFightScene();
}

void CMainScene::clickExit(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();
}
