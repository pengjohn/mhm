#ifndef CFIGHT_SCENE_H__
#define CFIGHT_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

enum
{
	WEAPON_SWORD_NULL = -1,
	WEAPON_SWORD_ZONG = 0,
	WEAPON_SWORD_TIAO = 1,
	WEAPON_SWORD_HENG = 2,
	WEAPON_SWORD_XUN = 3,
	WEAPON_SWORD_MAX,
	MONSTER_ATTACK,
};

enum
{
	HUNTER_STATE_NULL = 0,
	HUNTER_STATE_ROLLING = 1,
	HUNTER_STATE_BLOCKING = 2,
	HUNTER_STATE_ATTACKING = 3,
};

enum
{
	MONSTER_STATE_NULL = 0,
	MONSTER_STATE_ATTAKING = 1,
	MONSTER_STATE_STUN = 2
};

struct WeaponAction
{
	char *pszFileName;
	CCSprite* pSpriteArrow;
	bool bEnable;
	float cdtime;
	float movetime;
	int damage;
};

#define HUNTER_HP_MAX   1000
#define MONSTER_HP_MAX   2000

class CFightScene :public cocos2d::CCLayer
{
public:
	CFightScene(void);
	~CFightScene(void);

public:
	virtual bool init();
	static void show();

private:
	CCSprite* m_pSpriteHunter;
	CCSprite* m_pSpriteMonster;
	CCSprite* m_pSpriteActionBlock;
	CCSprite* m_pSpriteActionRoll;
	CCControlSlider* m_pSliderHunterHP;
	CCControlSlider* m_pSliderMonsterHP;

  CCSize m_sVisibleSize;
  CCPoint m_sOrigin;

	float m_fTouchBeganX;
	float m_fTouchBeganY;
	float m_fTouchEndX;
	float m_fTouchEndY;
	int m_nActionHoldTime;
  int m_nMonsterActionIndex;
  int m_nHunterState;
  int m_nMonsterState;
  int m_nHunterHP;
  int m_nMonsterHP;
  
  bool m_bGameOver;
  
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);

private :
	void clickStart(CCObject* pSender);
	void clickBack(CCObject* pSender);	
	
	void arrowMoveFinished(CCNode* sender, void* data);
	void rollFinished(CCNode* sender);
	void attackFinished(CCNode* sender);
	
	void onUpdate(float dt);
	void restartAction(int state);
	void restartMonsterAction();
	void actionHold(float dt);
	void pauseAction();
	void showGameOver();
	void clickGameOver(CCObject* pSender);	
};

#endif