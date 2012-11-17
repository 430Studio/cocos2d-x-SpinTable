//
//  SpinTable.h
//  slots
//
//  Created by YANGSHAOQUN on 12-11-16.
//
//

#ifndef __slots__SpinTable__
#define __slots__SpinTable__

#include "cocos2d.h"

class SpinTableCell : public cocos2d::CCNode {
    
public:
    static SpinTableCell * create();
    virtual bool init();
};




class SpinTable;

class SpinTableDelegate {
    
public:
    virtual float cellsSpaceWidthWithSpinTable(SpinTable * table);
    virtual float cellsSpaceHeightWithSpinTable(SpinTable * table);
    virtual SpinTableCell * getCell(SpinTable * table, int row, int column);
    virtual float spinVelocity(int column);
    virtual float spinVelocityShowResult(int column);
    virtual float spinDuration(int column);
    virtual void cellWillSpinInTable(SpinTable * table, SpinTableCell * cell);
    virtual void cellWillSpinInTableAndShowResult(SpinTable * table, SpinTableCell * cell, int rowIndex, int columnIndex);
};



class SpinTable : public cocos2d::CCNode {
    
    cocos2d::CCSize m_szTableFrame;
    cocos2d::CCSize m_szCellFrame;
    
    int m_iRowNum;
    int m_iColumnNum;
    float m_fSpaceWidth;
    float m_fSpaceHeight;
    
    float m_fSpinVelocity;
    float m_fSpinDuration;
    float m_fSpinTime;
    
    bool m_bShowResult;
    bool m_bStartSpin;
    
    int * m_iarrRowIndex;
    bool * m_barrShowResult;
    
    cocos2d::CCArray * m_arrTableColumns;
    
    SpinTableDelegate * m_oDelegate;
    
protected:
    SpinTable();
    ~SpinTable();
    
    void startSpin();
    
    float realSpinVelocity(int column);
    
    void spinEnded();
    
    virtual void visit();
    
    cocos2d::CCRect currentTableFrameRect();
    
    virtual void onEnter();
    virtual void onExit();
    
    bool cellOutOfBottomBound(SpinTableCell * cell);
    
    void update(float dt);
    
public:
    static SpinTable * create(cocos2d::CCSize tableFrame, cocos2d::CCSize cellFrame, int row, int column);
    virtual bool initWithFrame(cocos2d::CCSize tableFrame, cocos2d::CCSize cellFrame, int row, int column);
    
    void setDelegate(SpinTableDelegate * delegate);
    
    void reload();
    
    void spin();
};

#endif /* defined(__slots__SpinTable__) */
