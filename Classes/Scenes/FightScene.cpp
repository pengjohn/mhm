#include<stdio.h>
#include<stdlib.h>
#include "FightScene.h"
#include "GameLogic.h"
#include "SimpleAudioEngine.h" 
USING_NS_CC;

WeaponAction pWeaponAction[WEAPON_SWORD_MAX] = 
{
	{"ArrowDown.png",NULL, false, 0.1f, 3.0f, 200},
	{"ArrowUp.png",NULL, false, 0.1f, 2.5f, 125},
	{"ArrowRight.png",NULL, false, 0.1f, 2.2f, 110},
	{"ArrowHold.png",NULL, false, 0.1f, 3.0f, 300}
};

WeaponAction pMonsterAction[] = 
{
	{"ArrowLeft.png",	NULL, false, 2.5f, 5.5f, 110},
	{"ArrowRight.png",NULL, false, 2.5f, 5.0f, 80},
	{"ArrowUp.png",		NULL, false, 2.5f, 6.0f, 120},
	{"ArrowDown.png",	NULL, false, 2.5f, 4.0f, 60},
};
 
#define MONSTERACTIONNUM (sizeof(pMonsterAction)/sizeof(pMonsterAction[0]))

/*
状态：
1. 未出招
  1) 纵 3.0
  2) 挑 2.5
  3) 横 2.5
  4) 蓄力 3.0
2. 纵
  1) 纵无效时间 5.0->3.0
  2) 挑 1.5
  3) 横 1.5
  4) 蓄力无效时间 5.0->3.0
3. 挑
  1) 纵 2.0
  2) 挑无效时间 5.0 ->2.5 
  3) 横 1.5
  4) 蓄力 2.0
4. 横
  1) 纵 2.0
  2) 挑 1.5
  3) 横无效时间5.0 -> 2.5
  4) 蓄力 2.0
5. 蓄力
  1) 纵无效时间2.0 -> 3.0
  2) 挑无效时间2.0 -> 2.5 
  3) 横无效时间2.0 -> 2.5
  4) 蓄无效时间2.0 -> 3.0

*/

CFightScene::CFightScene(void)
{
	m_fTouchBeganX = 0.0f;
	m_fTouchBeganY = 0.0f;
	m_fTouchEndX = 0.0f;
	m_fTouchEndY = 0.0f;
	m_nActionHoldTime = 0;
	m_nMonsterActionIndex = 0;
	m_nHunterState = HUNTER_STATE_NULL;
	m_nMonsterState = MONSTER_STATE_NULL;
  m_nHunterHP = HUNTER_HP_MAX;
  m_nMonsterHP = MONSTER_HP_MAX;
  m_pSliderHunterHP = NULL;
  m_pSliderMonsterHP = NULL;
  m_bGameOver = false;

  m_sVisibleSize = CCDirector::sharedDirector()->getVisibleSize();
  m_sOrigin = CCDirector::sharedDirector()->getVisibleOrigin();  
}

CFightScene::~CFightScene(void)
{
	removeAllChildrenWithCleanup(true);
	
	CocosDenshion::SimpleAudioEngine::sharedEngine()->end();
}

void CFightScene::show()
{
	CCScene *scene = CCScene::create();
	CFightScene *layer = new CFightScene();
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


bool CFightScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

		CCSpriteFrameCache *cache=CCSpriteFrameCache::sharedSpriteFrameCache(); 
    cache->addSpriteFramesWithFile("mhm.plist","mhm.png"); 
    


	CCMenuItemImage *btn_back = CCMenuItemImage::create(
                                    "BtnBack.png",
                                    "BtnBack.png",
                                    this,menu_selector(CFightScene::clickBack));                                        

	btn_back->setPosition(ccp(m_sOrigin.x + m_sVisibleSize.width - btn_back->getContentSize().width/2 ,
                                m_sOrigin.y + m_sVisibleSize.height-btn_back->getContentSize().height/2));
	CCMenu* menu = CCMenu::create(btn_back, NULL);
	menu->setPosition(CCPointZero);
	this->addChild(menu);

    // add Hunter.png
    //m_pSpriteHunter = CCSprite::create("Hunter.png");
    m_pSpriteHunter=CCSprite::createWithSpriteFrameName("Hunter.png");
    m_pSpriteHunter->setPosition(ccp(m_sVisibleSize.width/4 + m_sOrigin.x, m_sVisibleSize.height*12/16 + m_sOrigin.y));
    this->addChild(m_pSpriteHunter, 0);

    CCActionInterval* actionHunter = CCFadeIn::create(1.0f);
    m_pSpriteHunter->runAction(actionHunter);

    // add Monster.png
    //m_pSpriteMonster = CCSprite::create("Monster.png");
    m_pSpriteMonster = CCSprite::createWithSpriteFrameName("Monster.png");
    m_pSpriteMonster->setPosition(ccp(m_sVisibleSize.width*3/4 + m_sOrigin.x, m_sVisibleSize.height*12/16 + m_sOrigin.y));
    this->addChild(m_pSpriteMonster, 0);	

		//add ActionBlock.png
    //m_pSpriteActionBlock = CCSprite::create("ActionBlock.png");
    m_pSpriteActionBlock = CCSprite::createWithSpriteFrameName("ActionBlock.png");
    m_pSpriteActionBlock->setPosition(ccp(m_sVisibleSize.width*1/4 + m_sOrigin.x, m_sVisibleSize.height*8/16 + m_sOrigin.y));
    this->addChild(m_pSpriteActionBlock, 0);	

	// add ActionRoll.png
    //m_pSpriteActionRoll = CCSprite::create("ActionRoll.png");
    m_pSpriteActionRoll = CCSprite::createWithSpriteFrameName("ActionRoll.png");
    m_pSpriteActionRoll->setPosition(ccp(m_sVisibleSize.width*3/4 + m_sOrigin.x, m_sVisibleSize.height*8/16 + m_sOrigin.y));
    this->addChild(m_pSpriteActionRoll, 0);

	//add Arrow
	for(int i=0;i<WEAPON_SWORD_MAX;i++)
	{
		//pWeaponAction[i].pSpriteArrow = CCSprite::create(pWeaponAction[i].pszFileName);
		pWeaponAction[i].pSpriteArrow = CCSprite::createWithSpriteFrameName(pWeaponAction[i].pszFileName);
		pWeaponAction[i].pSpriteArrow->setPosition(ccp(m_sVisibleSize.width*1/10 + m_sOrigin.x, m_sVisibleSize.height*(4-i)/16 + m_sOrigin.y));
		this->addChild(pWeaponAction[i].pSpriteArrow, 0);	
	}

  //怪物动作，初始位置在屏幕外，用来实现不显示效果
  for(int j=0; j<MONSTERACTIONNUM;j++)
  {
		//pMonsterAction[j].pSpriteArrow = CCSprite::create(pMonsterAction[j].pszFileName);
		pMonsterAction[j].pSpriteArrow = CCSprite::createWithSpriteFrameName(pMonsterAction[j].pszFileName);
		pMonsterAction[j].pSpriteArrow->setPosition(ccp(m_sVisibleSize.width*20/10 + m_sOrigin.x, m_sVisibleSize.height*6/16 + m_sOrigin.y));
		this->addChild(pMonsterAction[j].pSpriteArrow, 0);
  }
	restartAction(WEAPON_SWORD_NULL);
	restartMonsterAction();

	//schedule( schedule_selector(CFightScene::onUpdate)); 

  m_pSliderHunterHP = CCControlSlider::create("SliderHPTrack.png","SliderHPProgress.png" ,"SliderHPThumb.png");
  m_pSliderHunterHP->setPosition(ccp(m_sVisibleSize.width/4 + m_sOrigin.x, m_sVisibleSize.height*15/16 + m_sOrigin.y));
  m_pSliderHunterHP->setMinimumValue(0);
  m_pSliderHunterHP->setMaximumValue(HUNTER_HP_MAX);
  m_pSliderHunterHP->setValue(HUNTER_HP_MAX);
  m_pSliderHunterHP -> setTouchEnabled(false);
  this->addChild(m_pSliderHunterHP);
        
  m_pSliderMonsterHP = CCControlSlider::create("SliderHPTrack.png","SliderHPProgress.png" ,"SliderHPThumb.png");
  m_pSliderMonsterHP->setPosition(ccp(m_sVisibleSize.width*3/4 + m_sOrigin.x, m_sVisibleSize.height*15/16 + m_sOrigin.y));
  m_pSliderMonsterHP->setMinimumValue(0);
  m_pSliderMonsterHP->setMaximumValue(MONSTER_HP_MAX);
  m_pSliderMonsterHP->setValue(MONSTER_HP_MAX);
  m_pSliderMonsterHP -> setTouchEnabled(false);
  this->addChild(m_pSliderMonsterHP);
  	
	this->setTouchEnabled(true);

	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("mh3g.mp3", true); 
	return true;
}

void CFightScene::clickBack(CCObject* pSender)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->end();
	CGameLogic::ShowMainScene();
	//CCDirector::sharedDirector()->popScene();
}

void CFightScene::clickStart(CCObject* pSender)
{
}

void CFightScene::arrowMoveFinished(CCNode* sender, void* data)
{
	int i=(int)data;

	if( (i>WEAPON_SWORD_NULL)&&(i<WEAPON_SWORD_MAX) )
	{
		CCActionInterval* goArrow = CCScaleBy::create(0.5f, 1.5f);
		CCActionInterval* goArrowBack = goArrow->reverse();
		CCSequence* scale_seq = CCSequence::create(goArrow, goArrowBack,NULL);
		CCAction* scale_4ever = CCRepeatForever::create(scale_seq);
		pWeaponAction[i].pSpriteArrow->runAction(scale_4ever);

		pWeaponAction[i].bEnable = true;
	}
	else if(i == MONSTER_ATTACK)
	{
			//怪物往前
			CCActionInterval* goMonster = CCMoveBy::create(0.2f, ccp(-50,0));
			CCActionInterval* goMonsterBack = goMonster->reverse();
			CCSequence* seqMonster = CCSequence::create(goMonster, goMonsterBack, NULL);
			m_pSpriteMonster->runAction(seqMonster);
			
			switch(m_nHunterState)
			{
				//翻滚状态无敌
				case HUNTER_STATE_ROLLING:
					break;
				//档格状态
				case HUNTER_STATE_BLOCKING:
					//档格音效
					CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("attack.mp3");					
					break;
				default:
					//攻击音效
					CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("attack.mp3");

					//猎人被攻击闪烁
					CCFiniteTimeAction* goHunterStep1 = CCBlink::create(0.2f, 1);
					CCSequence* seqHunter = CCSequence::create(goHunterStep1, NULL);
					m_pSpriteHunter->runAction(seqHunter);

					//猎人失血，数值上升并消失
					char strDamage[20];
					sprintf(strDamage,"-%d",pMonsterAction[m_nMonsterActionIndex].damage);
					CCLabelTTF* pLabel = CCLabelTTF::create(strDamage, "Arial", 18);
					pLabel->setPosition(ccp(m_sVisibleSize.width*1/4 + m_sOrigin.x, m_sVisibleSize.height*14/16 + m_sOrigin.y));
					this->addChild(pLabel, 1);
					CCActionInterval* goLabel = CCMoveBy::create(0.2f, ccp(0,40));
					CCActionInterval* goLabelFadeOut = CCFadeOut::create(0.2f);
					CCSequence* seqLabel = CCSequence::create(goLabel, goLabelFadeOut, NULL);
					pLabel->runAction(seqLabel);

					//血槽更新
					this->m_nHunterHP -= pMonsterAction[m_nMonsterActionIndex].damage;
					if(this->m_nHunterHP <= 0)
							this->m_nHunterHP = 0;
					m_pSliderHunterHP->setValue(this->m_nHunterHP);
					//猎人被打败
					if(this->m_nHunterHP <= 0)
					{
						//CocosDenshion::SimpleAudioEngine::sharedEngine()->end();
						//CGameLogic::ShowMainScene();
						m_bGameOver = true;
						showGameOver();
						return;
					}
					
					//猎人动作被打断
			    restartAction(i);
					break;
				}
			
			//怪物攻击复位
			restartMonsterAction();
	}

}

void CFightScene::rollFinished(CCNode* sender)
{
	m_nHunterState = HUNTER_STATE_NULL;
}

void CFightScene::attackFinished(CCNode* sender)
{
	m_nHunterState = HUNTER_STATE_NULL;
}

void CFightScene::onUpdate(float dt)
{
	
}

void CFightScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
		CCTouch *touch = (CCTouch*)pTouches->anyObject();
		CCPoint location = touch->getLocation();
		m_fTouchBeganX = location.x;
		m_fTouchBeganY = location.y;
    
    //游戏结束
    if(m_bGameOver)
    	return;
    	
    //翻滚动作处理, 设置无敌时间
		CCRect rect = m_pSpriteActionRoll->boundingBox();
		if( rect.containsPoint(location) )
		{
			if(m_nHunterState == HUNTER_STATE_NULL)
			{
				m_nHunterState = HUNTER_STATE_ROLLING;
				CCActionInterval* goHunterStep11 = CCRotateBy::create(1.0f, -360);
				CCActionInterval* goHunterStep12 = CCMoveBy::create(0.9f, ccp(-150,0));
				CCActionInterval* goHunterStep1 = CCSpawn::create(goHunterStep11, goHunterStep12, NULL);
				CCActionInterval* goHunterStep2 = CCMoveBy::create(0.1f, ccp(150,0));
					
	      CCSequence* seqHunter = CCSequence::create(goHunterStep1, goHunterStep2, CCCallFuncN::create(this,callfuncN_selector(CFightScene::rollFinished)), NULL);
	      m_pSpriteHunter->runAction(seqHunter);
				restartAction(WEAPON_SWORD_NULL);
		  }
			return;
		}

		//档格			
		CCRect rect2 = m_pSpriteActionBlock->boundingBox();
		if( rect2.containsPoint(location) )
		{
			if(m_nHunterState == HUNTER_STATE_NULL)
			{
				m_nHunterState = HUNTER_STATE_BLOCKING;
				//档格动作
				CCActionInterval* goHunterStep1 = CCRotateBy::create(0.1f, -30);
	      CCSequence* seqHunter = CCSequence::create(goHunterStep1, NULL);
	      m_pSpriteHunter->runAction(seqHunter);

				//攻击动作停止移动
				pauseAction();
		  }
			return;
		}
		
		m_nActionHoldTime = 0;
		schedule( schedule_selector(CFightScene::actionHold), 1.0f); 
}

void CFightScene::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{

}

void CFightScene::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	CCTouch *touch = (CCTouch*)pTouches->anyObject();
	CCPoint location = touch->getLocation();
	int i = WEAPON_SWORD_NULL;

    //游戏结束
    if(m_bGameOver)
    	return;

	unschedule( schedule_selector(CFightScene::actionHold) );

	m_fTouchEndX = location.x;
	m_fTouchEndY = location.y;

	//先处理档格的情况
	if(m_nHunterState == HUNTER_STATE_BLOCKING)
	{
		m_nHunterState = HUNTER_STATE_NULL;
		//档格动作恢复
		CCActionInterval* goHunterStep1 = CCRotateBy::create(0.1f, 30);
		CCSequence* seqHunter = CCSequence::create(goHunterStep1, NULL);
		m_pSpriteHunter->runAction(seqHunter);

		//攻击动作恢复移动
		restartAction(WEAPON_SWORD_NULL);
		return;
	}

	float absX = fabsf(m_fTouchEndX - m_fTouchBeganX);
	float absY = fabsf(m_fTouchEndY - m_fTouchBeganY);
	if( (absX<50) && (absY<50) )
	{
		//if Hold
		if(m_nActionHoldTime <1)
			return;
		i = WEAPON_SWORD_XUN;
	}
	else
	{
		if(absX > absY)
		{
			if(m_fTouchEndX > m_fTouchBeganX)
				i = WEAPON_SWORD_HENG; //left->right
			else
				; //right->left
		}
		else
		{
			if(m_fTouchEndY > m_fTouchBeganY)
				i = WEAPON_SWORD_TIAO; //down->up
			else
				i = WEAPON_SWORD_ZONG;//up->down
		}
	}
    //for(int i=0;i<WEAPON_SWORD_MAX;i++)
	if(i != WEAPON_SWORD_NULL)
	{
		//循环判断是否点击了某个攻击滑块
		//CCRect rect = pWeaponAction[i].pSpriteArrow->boundingBox();
		//if( rect.containsPoint(location) )
		{
			//如果动作还不可用，则跳过
			if(pWeaponAction[i].bEnable == false)
				return;
			//攻击音效
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("attack.mp3");
			//猎人砍刀动作
			m_nHunterState = HUNTER_STATE_ATTACKING;
			CCActionInterval* goHunterStep1 = CCRotateBy::create(0.1f, -30);
			CCActionInterval* goHunterStep2 = CCRotateBy::create(0.2f, 60);
			CCActionInterval* goHunterStep3 = CCRotateBy::create(0.1f, -30);
			CCSequence* seqHunter = CCSequence::create(goHunterStep1, goHunterStep2, goHunterStep3, CCCallFuncN::create(this,callfuncN_selector(CFightScene::attackFinished)), NULL);
			m_pSpriteHunter->runAction(seqHunter);

			//怪物往后退
			CCActionInterval* goMonster = CCMoveBy::create(0.1f, ccp(10,0));
			CCActionInterval* goMonsterBack = goMonster->reverse();
			CCSequence* seqMonster = CCSequence::create(goMonster, goMonsterBack, NULL);
			m_pSpriteMonster->runAction(seqMonster);

			//怪物失血，数值上升并消失
			char strDamage[20];
			if(i == WEAPON_SWORD_XUN)
			{
				sprintf(strDamage,"-%d",pWeaponAction[i].damage*m_nActionHoldTime);
			}
			else
			{
				sprintf(strDamage,"-%d",pWeaponAction[i].damage);
			}
			CCLabelTTF* pLabel = CCLabelTTF::create(strDamage, "Arial", 18);
			pLabel->setPosition(ccp(m_sVisibleSize.width*3/4 + m_sOrigin.x, m_sVisibleSize.height*14/16 + m_sOrigin.y));
			this->addChild(pLabel, 1);
			CCActionInterval* goLabel = CCMoveBy::create(0.2f, ccp(0,40));
			CCActionInterval* goLabelFadeOut = CCFadeOut::create(0.2f);
			CCSequence* seqLabel = CCSequence::create(goLabel, goLabelFadeOut, NULL);
			pLabel->runAction(seqLabel);

			//血槽更新
			pMonsterAction[m_nMonsterActionIndex].damage;
			if(i == WEAPON_SWORD_XUN)
			{
				this->m_nMonsterHP -= pWeaponAction[i].damage*m_nActionHoldTime;
			}
			else
			{
				this->m_nMonsterHP -= pWeaponAction[i].damage;
			}
			if(this->m_nMonsterHP <= 0)
				this->m_nMonsterHP = 0;
			m_pSliderMonsterHP->setValue(this->m_nMonsterHP);
			//怪物被打败
			if(this->m_nMonsterHP <= 0)
			{
				//CocosDenshion::SimpleAudioEngine::sharedEngine()->end();
				//CGameLogic::ShowMainScene();
				m_bGameOver = true;
				showGameOver();
				return;
			}	
			//复位动作滑块
			restartAction(i);
			//怪物只有被打出硬直的时候才会复位动作
			if(m_nMonsterState == MONSTER_STATE_STUN)
			{
				restartMonsterAction();
			}
		}
	}
	return;
  
}


void CFightScene::restartAction(int state)
{
    //游戏结束
    if(m_bGameOver)
    	return;

	switch(state)
	{
		case WEAPON_SWORD_ZONG:
			pWeaponAction[WEAPON_SWORD_ZONG].cdtime = 5.0f;
			pWeaponAction[WEAPON_SWORD_ZONG].movetime = 3.0f;
			pWeaponAction[WEAPON_SWORD_TIAO].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_TIAO].movetime = 1.5f;
			pWeaponAction[WEAPON_SWORD_HENG].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_HENG].movetime = 1.5f;
			pWeaponAction[WEAPON_SWORD_XUN].cdtime = 5.0f;
			pWeaponAction[WEAPON_SWORD_XUN].movetime = 3.0f;
			break;

		case WEAPON_SWORD_TIAO:
			pWeaponAction[WEAPON_SWORD_ZONG].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_ZONG].movetime = 2.0f;
			pWeaponAction[WEAPON_SWORD_TIAO].cdtime = 5.0f;
			pWeaponAction[WEAPON_SWORD_TIAO].movetime = 2.5f;
			pWeaponAction[WEAPON_SWORD_HENG].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_HENG].movetime = 1.5f;
			pWeaponAction[WEAPON_SWORD_XUN].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_XUN].movetime = 2.0f;
			break;

		case WEAPON_SWORD_HENG:
			pWeaponAction[WEAPON_SWORD_ZONG].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_ZONG].movetime = 2.0f;
			pWeaponAction[WEAPON_SWORD_TIAO].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_TIAO].movetime = 1.5f;
			pWeaponAction[WEAPON_SWORD_HENG].cdtime = 5.0f;
			pWeaponAction[WEAPON_SWORD_HENG].movetime = 2.5f;
			pWeaponAction[WEAPON_SWORD_XUN].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_XUN].movetime = 2.0f;
			break;

		case WEAPON_SWORD_XUN:
			pWeaponAction[WEAPON_SWORD_ZONG].cdtime = 2.0f;
			pWeaponAction[WEAPON_SWORD_ZONG].movetime = 3.0f;
			pWeaponAction[WEAPON_SWORD_TIAO].cdtime = 2.0f;
			pWeaponAction[WEAPON_SWORD_TIAO].movetime = 2.5f;
			pWeaponAction[WEAPON_SWORD_HENG].cdtime = 2.0f;
			pWeaponAction[WEAPON_SWORD_HENG].movetime = 2.5f;
			pWeaponAction[WEAPON_SWORD_XUN].cdtime = 2.0f;
			pWeaponAction[WEAPON_SWORD_XUN].movetime = 3.0f;
			break;

		default:
			pWeaponAction[WEAPON_SWORD_ZONG].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_ZONG].movetime = 3.0f;
			pWeaponAction[WEAPON_SWORD_TIAO].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_TIAO].movetime = 2.5f;
			pWeaponAction[WEAPON_SWORD_HENG].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_HENG].movetime = 2.5f;
			pWeaponAction[WEAPON_SWORD_XUN].cdtime = 0.1f;
			pWeaponAction[WEAPON_SWORD_XUN].movetime = 3.0f;
			break;
	}

	for(int i=0;i<WEAPON_SWORD_MAX;i++)
	{
			//停止之前的动作
			pWeaponAction[i].pSpriteArrow->stopAllActions();
			//置位并重新开始移动
			pWeaponAction[i].pSpriteArrow->setPosition(ccp(m_sVisibleSize.width*1/10 + m_sOrigin.x, m_sVisibleSize.height*(4-i)/16 + m_sOrigin.y));
			pWeaponAction[i].pSpriteArrow->setScale(1);

			//如果是翻滚动作，cdtime增加2秒
      if(m_nHunterState == HUNTER_STATE_ROLLING)
      {
      	pWeaponAction[i].cdtime +=2.0f;
      }
      
			CCDelayTime* delay = CCDelayTime::create(pWeaponAction[i].cdtime);
			CCActionInterval* goArrow = CCMoveTo::create(pWeaponAction[i].movetime, ccp(m_sVisibleSize.width*9/10 + m_sOrigin.x, m_sVisibleSize.height*(4-i)/16 + m_sOrigin.y) );
			CCSequence* seq = CCSequence::create(delay, goArrow, CCCallFuncND::create(this,callfuncND_selector(CFightScene::arrowMoveFinished),(void*)i), NULL);
			pWeaponAction[i].pSpriteArrow->runAction(seq);
			pWeaponAction[i].bEnable = false;
	}
}

void CFightScene::pauseAction()
{
	for(int i=0;i<WEAPON_SWORD_MAX;i++)
	{
			//停止之前的动作
			pWeaponAction[i].pSpriteArrow->stopAllActions();
			//置位并重新开始移动
			pWeaponAction[i].pSpriteArrow->setPosition(ccp(m_sVisibleSize.width*1/10 + m_sOrigin.x, m_sVisibleSize.height*(4-i)/16 + m_sOrigin.y));
			pWeaponAction[i].pSpriteArrow->setScale(1);
	}	
}

void CFightScene::restartMonsterAction()
{
	//游戏结束
	if(m_bGameOver)
		return;

	//停止之前的动作，并将动作图标显示到屏幕外
	pMonsterAction[m_nMonsterActionIndex].pSpriteArrow->stopAllActions();
	pMonsterAction[m_nMonsterActionIndex].pSpriteArrow->setPosition(ccp(m_sVisibleSize.width*20/10 + m_sOrigin.x, m_sVisibleSize.height*6/16 + m_sOrigin.y));
	
	//随机怪物攻击
	m_nMonsterActionIndex = rand()%MONSTERACTIONNUM;
	
	//置位并重新开始移动
	pMonsterAction[m_nMonsterActionIndex].pSpriteArrow->setPosition(ccp(m_sVisibleSize.width*9/10 + m_sOrigin.x, m_sVisibleSize.height*6/16 + m_sOrigin.y));
	pMonsterAction[m_nMonsterActionIndex].pSpriteArrow->setScale(1);

	CCDelayTime* delay = CCDelayTime::create(pMonsterAction[m_nMonsterActionIndex].cdtime);
	CCActionInterval* goArrow = CCMoveTo::create(pMonsterAction[m_nMonsterActionIndex].movetime, ccp(m_sVisibleSize.width*1/10 + m_sOrigin.x, m_sVisibleSize.height*6/16 + m_sOrigin.y) );
	CCSequence* seq = CCSequence::create(delay, goArrow, CCCallFuncND::create(this,callfuncND_selector(CFightScene::arrowMoveFinished),(void*)MONSTER_ATTACK), NULL);
	pMonsterAction[m_nMonsterActionIndex].pSpriteArrow->runAction(seq);
	pMonsterAction[m_nMonsterActionIndex].bEnable = false;

}


void CFightScene::actionHold(float dt)
{
	if(m_nActionHoldTime <3)
	{
		m_nActionHoldTime ++;
		if(pWeaponAction[WEAPON_SWORD_XUN].bEnable == true)
		{

		}
	}
}

void CFightScene::showGameOver()
{
  CCLayerColor *background = CCLayerColor::create(ccc4(150,150,150,150));
  this->addChild(background);

  CCMenuItemImage *btn_gameover = CCMenuItemImage::create(
                                      "GameOver.png",
                                      "GameOver.png",
                                      this,menu_selector(CFightScene::clickGameOver));

	CCMenu* menu = CCMenu::create(btn_gameover, NULL);
	menu->alignItemsVerticallyWithPadding(10);
	menu->setPosition(ccp(m_sVisibleSize.width/2, m_sVisibleSize.height/2));
	this->addChild(menu);

  //停止动作
	pMonsterAction[m_nMonsterActionIndex].pSpriteArrow->stopAllActions();
	pMonsterAction[m_nMonsterActionIndex].pSpriteArrow->setPosition(ccp(m_sVisibleSize.width*20/10 + m_sOrigin.x, m_sVisibleSize.height*6/16 + m_sOrigin.y));
	for(int i=0;i<WEAPON_SWORD_MAX;i++)
	{
			//停止之前的动作
			pWeaponAction[i].pSpriteArrow->stopAllActions();
			//置位并重新开始移动
			pWeaponAction[i].pSpriteArrow->setPosition(ccp(m_sVisibleSize.width*1/10 + m_sOrigin.x, m_sVisibleSize.height*(4-i)/16 + m_sOrigin.y));
			pWeaponAction[i].pSpriteArrow->setScale(1);
	}	    
}    
       	
       	
void CFightScene::clickGameOver(CCObject* pSender)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->end();
	CGameLogic::ShowMainScene();	
}