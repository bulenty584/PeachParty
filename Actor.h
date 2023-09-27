#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <vector>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

class Actor : public GraphObject
{
    
    public:
        Actor(StudentWorld* w, int id, int xc, int yc, int dir, int depth, int size);
        virtual void doSomething() = 0;
        virtual bool isAlive() const {return m_alive;}
        virtual void setDead(){m_alive = false;}
        virtual bool isMoveable() const = 0;
        virtual bool canBeHitByAVortex() const = 0;
        virtual void hit() = 0;
    protected:
        virtual StudentWorld* getWorld() const {return m_world;};
    private:
        bool m_alive;
        int x;
        int y;
        StudentWorld* m_world;
    
};

class Moveable : public Actor{
    public:
        Moveable(StudentWorld* w, int id, int xc, int yc, int dir);
        virtual int getDir() const{return m_sprite_direction;}
        virtual bool walking() const {return m_walking;}
        virtual void setWalk(bool m) {m_walking = m;}
        virtual void setDir(int dir){m_sprite_direction = dir;}
        virtual void pickRandomSquare();
        virtual int numP(int x, int y);
        virtual bool isValidDirection(int dir);
        virtual void pickNewDir();
        virtual void updateDir(int dir);
        virtual void teleport();
        virtual int getNumTicks() const {return m_ticks_to_move;}
        virtual void setNumTicks(int x) {m_ticks_to_move = x;}
        virtual bool isMoveable() const {return true;}
        virtual bool canBeHitByAVortex() const {return false;}
        virtual int retSquaresToMove() const {return m_squares_to_move;}
        virtual void updateSquaresToMove(int x) {m_squares_to_move+=x;}
        virtual void setSquaresToMove(int x) {m_squares_to_move = x;}
        virtual bool teleported() const {return m_teleported;}
        virtual void setTeleport() {if (m_teleported == true)m_teleported = false; m_teleported = true;}
        virtual int getTicks() const {return m_ticks_to_move;}
        virtual void setTicks(int x) {m_ticks_to_move = x;}
        virtual void updateTicks(int x){m_ticks_to_move+=x;}
        virtual void hit() {return;}
        virtual bool isBaddie() const {return false;}
        virtual void Choose(int x, int y);
    
        
    private:
        int m_sprite_direction;
        bool m_walking;
        int m_squares_to_move;
        int m_ticks_to_move;
        bool m_teleported;
};



class Player : public Moveable
{
    public:
        Player(StudentWorld* w, int id, int xc, int yc, int player);
        virtual void doSomething();
        virtual int numCoins() const {return m_totalCoins;}
        virtual void setCoins(int x) {m_totalCoins = x;}
        virtual void updateCoins(int x) {
            if (m_totalCoins + x < 0){
                m_totalCoins = 0;
                return;
            }
            
            m_totalCoins += x;
            
        }
        virtual int numStars() const {return m_numStars;}
        virtual bool returnStart() const {return startNew;}
        virtual void setPlaying() {startNew = false;}
        virtual void updateStars(int x) {m_numStars += x;}
        virtual void setStars(int x) {m_numStars = x;}
        virtual int numRolls() const {return m_numRolls;}
        virtual int getTicksLeft() const {return getTicks();}
        virtual int getPlayerNum() const {return playerNum;}
        virtual bool isNew() const{return newplr;}
        virtual void setNew() {newplr = true;}
        virtual void setOld() {newplr = false;}
        virtual void swapCoins();
        virtual void swapLocations();
        virtual void swapStars();
        virtual bool holdsVortex() const {return m_holds_Vortex;}
        virtual void setHoldVortex(){m_holds_Vortex = true;}
    
    protected:
        Player* getPlayer(){return this;};
    private:
        bool newplr;
        int m_totalCoins;
        int m_numStars;
        int m_numRolls;
        int playerNum;
        bool m_holds_Vortex;
        bool startNew;
    
        
        
};

class Baddie: public Moveable{
    public:
        Baddie(StudentWorld* w, int id, int xc, int yc, int travelDist);
        virtual bool getPaused() const { return paused;}
        virtual void unPause() {paused = false;}
        virtual void pause() {paused = true;}
        virtual int getCount() const {return counter;}
        virtual void adjustCount() {counter--;}
        virtual void setCount() {counter = 180;}
        virtual bool isAlive() const {return true;}
        virtual bool canBeHitByAVortex() const {return true;}
        virtual bool isHit(){return m_hit;}
        virtual void hit(){m_hit = true;}
        virtual void setBack(){m_hit = false;}
        virtual ~Baddie(){}
    private:
        int counter;
        int m_travel;
        bool paused;
        bool m_hit;
};

class Bowser: public Baddie{
    
    public:
        Bowser(StudentWorld* w, int xc, int yc);
        virtual bool isAlive() const {return true;}
        virtual void doSomething();
};


class Square: public Actor{
    public:
        Square(StudentWorld* w, int id, int xc, int yc, int depth, int dir, int size);
        virtual bool isMoveable() const {return false;}
        virtual bool canBeHitByAVortex() const {return false;}
        virtual void doSomething() = 0;
        virtual void hit(){return;}
};


class Coin_Square: public Square
{
    
    public:
        Coin_Square(StudentWorld* w, int id, int xc, int yc, int num);
        virtual void doSomething();
    private:
        int changeCoins;
        
                
};



class Star_Square: public Square{
    public:
        Star_Square(StudentWorld* w, int xc, int yc);
        virtual void doSomething();
    
        
};

class Direction_Square: public Square{
    
    public:
        Direction_Square(StudentWorld* w, int xc, int yc, int num);
        virtual void doSomething();
        virtual int getForce() const{return m_forceDirection;}
    private:
        int spriteDirection;
        int m_forceDirection;
};

class Event_Square: public Square{
    public:
        Event_Square(StudentWorld* w, int xc, int yc);
        virtual void doSomething();
};

class Bank_Square: public Square{
    public:
        Bank_Square(StudentWorld* w, int xc, int yc);
        virtual void doSomething();
        
    private:
        int individualBalance;
    
};

class Dropping_Square: public Square{
    public:
        Dropping_Square(StudentWorld* w, int xc, int yc);
        virtual void doSomething();
        
};

class Boo: public Baddie{
    public:
        Boo(StudentWorld* w, int xc, int yc);
        virtual void doSomething();
    
};
        
class Vortex: public Moveable{
    public:
        Vortex(StudentWorld* w, int xc, int yc, int dir);
        virtual void doSomething();
};
    
    


#endif // ACTOR_H_

