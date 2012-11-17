//
//  SpinTable.cpp
//  slots
//
//  Created by YANGSHAOQUN on 12-11-16.
//
//

#include "SpinTable.h"

USING_NS_CC;

//SpinTableCell
SpinTableCell * SpinTableCell::create()
{
    SpinTableCell * cell = new SpinTableCell();
    if (cell && cell->init()) {
        cell->autorelease();
        return cell;
    }
    CC_SAFE_DELETE(cell);
    return NULL;
}

bool SpinTableCell::init()
{
    return true;
}




//SpinTableDelegate
float SpinTableDelegate::cellsSpaceWidthWithSpinTable(SpinTable * table)
{
    return 0;
}

float SpinTableDelegate::cellsSpaceHeightWithSpinTable(SpinTable * table)
{
    return 0;
}

SpinTableCell * SpinTableDelegate::getCell(SpinTable * table, int row, int column)
{
    SpinTableCell * cell = SpinTableCell::create();
    return cell;
}

void SpinTableDelegate::cellWillSpinInTable(SpinTable * table, SpinTableCell * cell)
{
    
}

void SpinTableDelegate::cellWillSpinInTableAndShowResult(SpinTable * table, SpinTableCell * cell, int rowIndex, int columnIndex)
{
    
}

float SpinTableDelegate::spinVelocity(int column)
{
    return 0;
}

float SpinTableDelegate::spinVelocityShowResult(int column)
{
    return 0;
}

float SpinTableDelegate::spinDuration(int column)
{
    return 0;
}




//SpinTable
SpinTable::SpinTable()
:m_fSpinTime(0),
m_bStartSpin(false)
{
    
}

SpinTable::~SpinTable()
{
    CC_SAFE_RELEASE(m_arrTableColumns);
    CC_SAFE_DELETE(m_iarrRowIndex);
    CC_SAFE_DELETE(m_barrShowResult);
}

SpinTable * SpinTable::create(cocos2d::CCSize tableFrame, cocos2d::CCSize cellFrame, int row, int column)
{
    SpinTable * spinTable = new SpinTable();
    if (spinTable && spinTable->initWithFrame(tableFrame,cellFrame,row,column)) {
        spinTable->autorelease();
        return spinTable;
    }
    CC_SAFE_DELETE(spinTable);
    return NULL;
}

bool SpinTable::initWithFrame(cocos2d::CCSize tableFrame, cocos2d::CCSize cellFrame, int row, int column)
{
    m_szTableFrame = tableFrame;
    setContentSize(m_szTableFrame);
    
    m_szCellFrame = cellFrame;
    m_iRowNum = row;
    m_iColumnNum = column;
    
    m_iarrRowIndex = (int *)calloc(m_iColumnNum,sizeof(int));
    m_barrShowResult = (bool *)calloc(m_iColumnNum, sizeof(bool));
    
    m_arrTableColumns = CCArray::create();
    CC_SAFE_RETAIN(m_arrTableColumns);
    
    return true;
}

void SpinTable::setDelegate(SpinTableDelegate *delegate)
{
    m_oDelegate = delegate;
}

void SpinTable::onEnter()
{
    CCNode::onEnter();
    CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, 0, false);
    reload();
}

void SpinTable::onExit()
{
    CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(this);
    CCNode::onExit();
}

bool SpinTable::cellOutOfBottomBound(SpinTableCell *cell)
{
    float posY = cell->getPosition().y;
    if (posY < -(m_szCellFrame.height*1.5+m_fSpaceHeight)) {
        return true;
    }
    return false;
}

void SpinTable::update(float dt)
{
    if (m_bStartSpin) {
        m_fSpinTime += dt;
        for (int i=0; i<m_iColumnNum; i++) {
            CCArray * columnNodes = (CCArray *)m_arrTableColumns->objectAtIndex(i);
            if (m_iarrRowIndex[i] < m_iRowNum+1) {
                for (int j=0; j<columnNodes->count(); j++) {
                    SpinTableCell * cell = (SpinTableCell *)columnNodes->objectAtIndex(j);
                    float spinDuration = m_oDelegate->spinDuration(i);
                    if (m_fSpinTime >= spinDuration) {
                        m_barrShowResult[i] = true;
                    }
                    cell->setPosition(ccpSub(cell->getPosition(), ccp(0, realSpinVelocity(i)*dt)));
                }
            }else if(m_iarrRowIndex[i] < m_iRowNum+2) {
                for (int j=0; j<columnNodes->count(); j++) {
                    SpinTableCell * cell = (SpinTableCell *)columnNodes->objectAtIndex(j);
                    float distance = ((j-1)*(m_szCellFrame.height+m_fSpaceHeight))-cell->getPositionY();
                    float duration = 0.5;//distance/realSpinVelocity(i);
                    cell->runAction(CCEaseBackOut::create(CCMoveBy::create(duration, ccp(0, distance))));
                }
                m_iarrRowIndex[i]++;
                if (i == m_iColumnNum-1) {
                    spinEnded();
                }
            }
            SpinTableCell * headCell = (SpinTableCell *)columnNodes->objectAtIndex(0);
            if (cellOutOfBottomBound(headCell)) {
                SpinTableCell * tailCell = (SpinTableCell *)columnNodes->objectAtIndex(columnNodes->count()-1);
                headCell->setPosition(ccpAdd(tailCell->getPosition(), ccp(0, m_szCellFrame.height+m_fSpaceHeight)));
                if (m_barrShowResult[i]) {
                    if (m_iarrRowIndex[i]<m_iRowNum) {
                        m_oDelegate->cellWillSpinInTableAndShowResult(this, headCell, m_iarrRowIndex[i], i);
                    }else {
                        m_oDelegate->cellWillSpinInTable(this, headCell);
                    }
                    m_iarrRowIndex[i]++;
                }else {
                    m_oDelegate->cellWillSpinInTable(this, headCell);
                }
                columnNodes->removeObjectAtIndex(0);
                columnNodes->addObject(headCell);
            }
        }
    }
}

void SpinTable::reload()
{
    CCAssert(m_oDelegate != NULL, "SpinTable: delegate could not be NULL !");
    m_fSpaceWidth = m_oDelegate->cellsSpaceWidthWithSpinTable(this);
    m_fSpaceHeight = m_oDelegate->cellsSpaceHeightWithSpinTable(this);
    
    for (int i=0; i<m_iColumnNum; i++) {
        CCArray * columnNodes = CCArray::create();
        for (int j=-1; j<m_iRowNum+1; j++) {
            SpinTableCell * cell = m_oDelegate->getCell(this, j, i);
            cell->setContentSize(m_szCellFrame);
            cell->setPosition(ccp(i*m_szCellFrame.width+i*m_fSpaceWidth, j*m_szCellFrame.height+j*m_fSpaceHeight));
            addChild(cell);
            columnNodes->addObject(cell);
        }
        m_arrTableColumns->addObject(columnNodes);
    }
}

void SpinTable::spin()
{
    m_fSpinTime = 0;
    for (int i=0; i<m_iColumnNum; i++) {
        m_iarrRowIndex[i] = 0;
        m_barrShowResult[i] = false;
    }
    for (int i=0; i<m_iColumnNum; i++) {
        CCArray * columnNodes = (CCArray *)m_arrTableColumns->objectAtIndex(i);
        for (int j=0; j<m_iRowNum+2; j++) {
            SpinTableCell * cell = (SpinTableCell *)columnNodes->objectAtIndex(j);
            cell->runAction(CCSequence::create(CCEaseBackIn::create(CCMoveBy::create(0.75, ccp(0, m_szCellFrame.height/2))), CCCallFunc::create(this, callfunc_selector(SpinTable::startSpin)), NULL));
        }
    }
}

void SpinTable::startSpin()
{
    m_bStartSpin = true;
}

float SpinTable::realSpinVelocity(int column)
{
    float distance = m_iRowNum*m_szCellFrame.height+(m_iRowNum-1)*m_fSpaceHeight;
    if (m_barrShowResult[column]) {
        return distance*m_oDelegate->spinVelocityShowResult(column);
    }else {
        return distance*m_oDelegate->spinVelocity(column);
    }
}

void SpinTable::spinEnded()
{
    m_bStartSpin = false;
}

void SpinTable::visit()
{
    glEnable(GL_SCISSOR_TEST);
    CCRect currentRect = currentTableFrameRect();
    glScissor(currentRect.origin.x, currentRect.origin.y, currentRect.size.width, currentRect.size.height);
    //CCAffineTransform transform = nodeToWorldTransform();
    //glScissor(getPositionX(), getPositionY(), m_szTableFrame.width*transform.a, m_szTableFrame.height*transform.d);
    CCNode::visit();
    glDisable(GL_SCISSOR_TEST);
}

CCRect SpinTable::currentTableFrameRect()
{
    //CCAffineTransform transform = nodeToWorldTransform();
    CCPoint pos = convertToWorldSpace(CCPointZero);
    CCPoint realSize = ccpSub(convertToWorldSpace(ccp(m_szTableFrame.width, m_szTableFrame.height)), pos);
    return CCRectMake(pos.x, pos.y, realSize.x, realSize.y);
}



