#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include "Actor.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    virtual Board* getBoard() const {return board;}
    virtual bool isValidPosition(int x, int y);
    virtual Player* getPeach() const {return m_peach;}
    virtual Player* getYoshi() const {return m_yoshi;}
    virtual Player* Lands(int x, int y);
    virtual bool isSquare(Actor* p);
    virtual void deposit(int num){m_bank+=num;}
    virtual void withdraw(){m_bank = 0;}
    virtual int getBank() const {return m_bank;}
    virtual bool DirectionalatSquare(int x, int y) const;
    virtual void dropSquare(int x, int y);
    virtual void addDropSquare(int x, int y);
    virtual void addVortex (int x, int y, int dir);
    virtual Player* getOtherP(Player* p) const {if (p == m_yoshi) return m_peach; return m_yoshi;}
    virtual bool overlaps(Actor* p);
    virtual bool overlap(int x1, int y1, int x2, int y2);
private:
    
    std::vector<Actor*> m_actors;
    Player* m_peach;
    Player* m_yoshi;
    Board* board;
    int m_bank;
    
    
};

#endif // STUDENTWORLD_H_

