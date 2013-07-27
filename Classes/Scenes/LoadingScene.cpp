#include "LoadingScene.h"
#include "GameLogic.h"
USING_NS_CC;

CLoadingScene::CLoadingScene(void)
{
}

CLoadingScene::~CLoadingScene(void)
{
	removeAllChildrenWithCleanup(true);
}

void CLoadingScene::show()
{
	CCScene *scene = CCScene::create();
	CLoadingScene *layer = new CLoadingScene();
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


bool CLoadingScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    //Close Icon
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(CLoadingScene::clickStart));
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);
    
	//add Loading
    CCLabelTTF* pLabel = CCLabelTTF::create("Loading", "Arial", 18);
    pLabel->setPosition(ccp(origin.x + visibleSize.width/2,origin.y + visibleSize.height - pLabel->getContentSize().height));
    this->addChild(pLabel, 1);

    // add Loading.png
    CCSprite* pSprite = CCSprite::create("HelloWorld.png");
    pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    this->addChild(pSprite, 0);

	schedule( schedule_selector(CLoadingScene::loadDone), 1.0f); 
	return true;
}

void CLoadingScene::clickStart(CCObject* pSender)
{
	unschedule(schedule_selector(CLoadingScene::loadDone));
	CGameLogic::ShowMainScene();
}
void CLoadingScene::loadDone(float dt)
{
	unschedule(schedule_selector(CLoadingScene::loadDone));
	CGameLogic::ShowMainScene();
}

       
