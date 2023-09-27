#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

#include <cmath>
#include <iostream>
#include <string>
#include "GameConstants.h"

using namespace std;

//PUBLIC MEMBER FUNCTIONS
Actor::Actor(StudentWorld* w, int id, int xc, int yc, int dir, int depth, int size): GraphObject(id, xc * SPRITE_WIDTH, yc * SPRITE_HEIGHT, dir, depth, size), m_world(w){
    m_alive = true;
}

Moveable::Moveable(StudentWorld* world, int id, int x, int y, int dir):
Actor(world, id, x, y, dir, 0, 1){
    m_walking = false;
    m_sprite_direction = dir;
    m_ticks_to_move = 0;
    m_squares_to_move = 0;
    m_teleported = false;
}

Baddie::Baddie(StudentWorld* w, int id, int xc, int yc, int travelDist): Moveable(w, id, xc, yc, right){
    
    m_travel = travelDist;
    paused = true;
    m_hit = false;
}

Player::Player(StudentWorld* w, int id, int xc, int yc, int player): Moveable(w, id, xc, yc, right){
    
    playerNum = player;
    newplr = true;
    m_totalCoins = 0;
    m_numRolls = 0;
    m_holds_Vortex = false;
    startNew = true;
    
    getWorld()->playSound(SOUND_GIVE_COIN);
}

void Player::doSomething(){
    
    if (!walking()){
        
        if (getDir() == 450){
            
            pickRandomSquare();
            setTeleport();
            Choose(getX(), getY());
            setNew();
            return;

        }
            
        int action = getWorld()->getAction(playerNum);
        switch (action){
            
            case ACTION_ROLL:{
                this->setNew();
                m_numRolls++;
                int die_roll = randInt(1, 10);
                if (die_roll > 0)
                    updateTicks(die_roll * 8);
                else{
                    die_roll = randInt(1,10);
                    return;
                }
                
                this->setWalk(true);
                break;
            }
                
            case ACTION_FIRE:{
                
                if (holdsVortex()){
                    int j, h;
                    
                    getPositionInThisDirection(getDir(), 16, j, h);
                    getWorld()->addVortex(j, h, getDir());
                    
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    
                    this->m_holds_Vortex = false;
                    return;
                    break;
                }
                
                return;
            }
                
            default:{
                this->setOld();
                return;
                break;
            }
        }
            
    }
        
    if (walking()){
        
        if (numP(getX(),getY()) > 1 && !getWorld()->DirectionalatSquare(getX(), getY())){
            
            int action1 = getWorld()->getAction(playerNum);
            
            if (action1 == ACTION_RIGHT && isValidDirection(right) && getDir()!= left){
                setDir(right);
                setDirection(right);
            }
            
            else if (action1 == ACTION_LEFT && isValidDirection(left) && getDir() != right){
                setDir(left);
                setDirection(left);
            }
            
            else if (action1 == ACTION_UP && isValidDirection(up) && getDir() != down){
                setDir(up);
                setDirection(right);
            }
            
            else if (action1 == ACTION_DOWN && isValidDirection(down) && getDir() != up){
                setDir(down);
                setDirection(right);
            }
            
            else
                return;
            
        }
        
        int x, y = 0;
        
        getPositionInThisDirection(getDir(), 16, x, y);
        
        //Check if we cannot move in our current direction:
        if (x % 16 == 0 && y % 16 == 0 && !(getWorld()->isValidPosition(x, y))){
            pickNewDir();
            
        }
    }
        
            
    moveAtAngle(getDir(), 2);
            
    updateTicks(-1);
            
    if (getTicks() == 0){
        setWalk(false);
    }
}

Square::Square(StudentWorld* w, int id, int xc, int yc, int depth, int dir, int size): Actor(w, id, xc, yc, dir, depth, 1)
{}

Coin_Square::Coin_Square(StudentWorld* w, int id, int xc, int yc, int num) : Square(w, id, xc, yc, 1, right, 1){
    
    if (num == 1)
        changeCoins = 3;
    if (num == 2)
        changeCoins = -3;
}



void Coin_Square::doSomething(){
    
    Player* p = getWorld()->Lands(getX(), getY());
    
    if (!isAlive())
        return;
    
    
    if (getWorld()->getYoshi()->returnStart()){
        getWorld()->getYoshi()->setCoins(3);
        getWorld()->getPeach()->setCoins(3);
        getWorld()->getYoshi()->setPlaying();
        getWorld()->getPeach()->setPlaying();
        getWorld()->playSound(SOUND_GIVE_COIN);
        return;
    }
    
    else{
            
        if ( p!= nullptr && p->isNew()){
            if (!p->walking() && isAlive()){
                p->updateCoins(changeCoins);
                if (changeCoins == 3)
                    getWorld()->playSound(SOUND_GIVE_COIN);
                if (changeCoins == -3)
                    getWorld()->playSound(SOUND_TAKE_COIN);
                return;
            }
        }
    }
}

Star_Square::Star_Square(StudentWorld* world, int xc, int yc): Square(world, IID_STAR_SQUARE, xc, yc, 1, right, 1){}

void Star_Square::doSomething(){
    Player *p = getWorld()->Lands(getX(), getY());
    
    if (p != nullptr && p->isNew()){
        if (p->numCoins() < 20)
            return;
        else{
            p->updateCoins(-20);
            p->updateStars(1);
            getWorld()->playSound(SOUND_GIVE_STAR);
            
        }
    }
        
}

Direction_Square::Direction_Square(StudentWorld* w, int xc, int yc, int dir) : Square(w, IID_DIR_SQUARE, xc, yc, 1, dir, 1){
    m_forceDirection = dir;
}

void Direction_Square::doSomething(){
    Player* p = getWorld()->Lands(getX(), getY());
    
    if (p != nullptr){
        p->setDir(m_forceDirection);
        if (m_forceDirection == left)
            p->setDirection(left);
        else{
            p->setDirection(right);
        }
    }
}

void Moveable::pickRandomSquare(){
    
    int i = randInt(0,255);
    int j = randInt(0,255);
    
    while (i % 16 != 0 || j % 16 != 0 || !getWorld()->isValidPosition(i, j)){
        i = randInt(0,255);
        j = randInt(0,255);
        
    }
    
    if (i % 16 == 0 && j % 16 == 0 && getWorld()->isValidPosition(i, j))
        moveTo(i,j);
    
}

void Moveable::teleport(){
    
    m_teleported = true;
    setDir(450);
    
}


Event_Square::Event_Square(StudentWorld* w, int xc, int yc): Square(w, IID_EVENT_SQUARE, xc, yc, 1, right,1){}

void Event_Square::doSomething(){
    
    Player* p = getWorld()->Lands(getX(), getY());
    
    
    if (p != nullptr && p->isNew()){
        int action = randInt(0,2);
        if (!p->walking() && isAlive()){
            if (action == 0){
                p->teleport();
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                return;
            }
            
            else if (action == 1){
                p->swapLocations();
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                return;
            }
            
            else if (action == 2){
                p->setHoldVortex();
                getWorld()->playSound(SOUND_GIVE_VORTEX);
                return;
            }
            
        }
    }
}


Bank_Square::Bank_Square(StudentWorld* w, int xc, int yc): Square(w, IID_BANK_SQUARE, xc, yc, 1, right, 1){
    individualBalance = 0;
}

void Bank_Square::doSomething(){
    Player* p = getWorld()->Lands(getX(), getY());
    
    
    if (p != nullptr && p->isNew() && !p->walking()){
        p->updateCoins(getWorld()->getBank());
        getWorld()->withdraw();
        getWorld()->playSound(SOUND_WITHDRAW_BANK);
        return;
    }
    
    else if (p != nullptr && p->isNew() && p->walking()){
        if (p->numCoins() < 5){
            p->setCoins(0);
            getWorld()->deposit(p->numCoins());
            getWorld()->playSound(SOUND_DEPOSIT_BANK);
            return;
            
        }
        else{
            p->updateCoins(-5);
            getWorld()->deposit(5);
            getWorld()->playSound(SOUND_DEPOSIT_BANK);
            return;
        }
        
        
    }
    
}

Vortex::Vortex(StudentWorld* w, int xc, int yc, int dir): Moveable(w, IID_VORTEX, xc, yc, dir){
}

void Vortex::doSomething(){
    
    if (!isAlive())
        return;
    
    moveAtAngle(getDir(), 2);
    
    
    if (getX() >= VIEW_WIDTH || getX() < 0 || getY() >= VIEW_HEIGHT || getY() < 0)
        setDead();
    
   
    if (getWorld()->overlaps(this)){
        setDead();
        getWorld()->playSound(SOUND_HIT_BY_VORTEX);
        return;
    }
        
        
}

int Moveable::numP(int x, int y){
    int count = 0;
    
    int a,b;
    int c,d;
    int e,f;
    int g,h;
    if (x % 16 == 0 && y % 16 == 0){
        if (getDir() == right){
            getPositionInThisDirection(up, 16, a, b);
            getPositionInThisDirection(down, 16, c, d);
            getPositionInThisDirection(left, 16, e, f);
            getPositionInThisDirection(right, 16, g, h);
            
            if (getWorld()->isValidPosition(g, h)) //right
                count++;
            
            if (getWorld()->isValidPosition(e,f) && getWorld()->isValidPosition(a,b)) //Check opposite and perp
                count++;
            
            if (getWorld()->isValidPosition(e, f) && getWorld()->isValidPosition(c, d)) //check opposite and perp
                count++;
            return count;
        }
        
        else if (getDir()== left){
            getPositionInThisDirection(up, 16, a, b);
            getPositionInThisDirection(down, 16, c, d);
            getPositionInThisDirection(left, 16, e, f);
            getPositionInThisDirection(right, 16, g, h);
            
            if (getWorld()->isValidPosition(e, f))
                count++;
            
            if (getWorld()->isValidPosition(g,h) && getWorld()->isValidPosition(a,b))
                count++;
            
            if (getWorld()->isValidPosition(g, h) && getWorld()->isValidPosition(c, d))
                count++;
            return count;
        }
        
        else if (getDir()== up){
            getPositionInThisDirection(up, 16, a, b);
            getPositionInThisDirection(down, 16, c, d);
            getPositionInThisDirection(left, 16, e, f);
            getPositionInThisDirection(right, 16, g, h);
            
            if (getWorld()->isValidPosition(a, b))
                count++;
            
            if (getWorld()->isValidPosition(c,d) && getWorld()->isValidPosition(e,f))
                count++;
            
            if (getWorld()->isValidPosition(c, d) && getWorld()->isValidPosition(g, h))
                count++;
            return count;
        }
        
        else{
            getPositionInThisDirection(up, 16, a, b);
            getPositionInThisDirection(down, 16, c, d);
            getPositionInThisDirection(left, 16, e, f);
            getPositionInThisDirection(right, 16, g, h);
            
            if (getWorld()->isValidPosition(c, d))
                count++;
            
            if (getWorld()->isValidPosition(a,b) && getWorld()->isValidPosition(g,h))
                count++;
            
            if (getWorld()->isValidPosition(a, b) && getWorld()->isValidPosition(e, f))
                count++;
            return count;
        }
    }
    
    return count;
}

bool Moveable::isValidDirection(int dir){
    int x, y;
    
    
    getPositionInThisDirection(dir, 16, x, y);
    
    if (getWorld()->isValidPosition(x, y))
        return true;
    return false;
}


Dropping_Square::Dropping_Square(StudentWorld* w, int x, int y): Square(w, IID_DROPPING_SQUARE, x, y, 1, right, 1){}

void Dropping_Square::doSomething(){
    Player *p = getWorld()->Lands(getX(), getY());
    int action = randInt(0,1);
    
    if (p != nullptr && p->isNew() && !p->walking()){
        if (action == 0){
            if (p->numCoins() > 10){
                p->updateCoins(-10);
                getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
                return;
            }
            else{
                p->setCoins(0);
                getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
                return;
            }
            
        }
        
        if (action == 1){
            if (p->numStars() > 1){
                p->updateStars(-1);
                getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
                return;
            }
            
            else{
                getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
                return;
            }
            
        }
    }
}

Bowser::Bowser(StudentWorld* w, int xc, int yc): Baddie(w, IID_BOWSER, xc, yc, 0){
    pause();
    setCount();
}


void Bowser::doSomething(){
    
    if (isHit()){
        pickRandomSquare();
        setBack();
        pause();
        setCount();
        setDir(right);
        setDirection(right);
    }
    
    if (getPaused()){
        Player* p = getWorld()->Lands(getX(), getY());
        
        if (p != nullptr && p->isNew() && !p->walking()){
            int chance = randInt(0,1);
            if (chance == 0){
                p->setCoins(0);
                p->setStars(0);
                getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
                return;
            }
        }
        
       adjustCount();
        
        if (getCount() == 0){
            setSquaresToMove(randInt(1,10));
            setTicks(retSquaresToMove() * 8);
            
            Choose(getX(), getY());
            updateDir(getDir());
            
            unPause();
        }
    }
        
        
    if (!getPaused()){
        
        if (numP(getX(), getY()) > 1){
            Choose(getX(), getY());
        }
        
        int x,y;
        getPositionInThisDirection(getDir(), 16, x, y);
        if (x % 16 == 0 && y % 16 == 0 && !getWorld()->isValidPosition(x, y)){
            pickNewDir();
        }
       
        moveAtAngle(getDir(), 2);
        updateTicks(-1);
        if (getTicks() == 0){
            pause();
            setCount();
            
            int p = randInt(0,3);
            
            if (p == 0){
                getWorld()->dropSquare(getX(), getY());
                getWorld()->addDropSquare(getX(), getY());
                getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
                return;
            }
        }
    }
}



void Moveable::pickNewDir(){
    
    int x = getX(), y = getY();
    
    if (x % 16 == 0 && y % 16 == 0){
        
        
        if (getDir() == right || getDir() == left){
            
            //Attempt to go up:
            getPositionInThisDirection(up, 16, x, y);
            
            //Check if valid:
            if (getWorld()->isValidPosition(x,y))
                setDir(up);
            else
                setDir(down);
            setDirection(right);
        }
        
        //Check if the player should move left or right:
        else{
            
            //Attempt to go right:
            getPositionInThisDirection(right, 16, x, y);
            
            //Check if valid:
            if (getWorld()->isValidPosition(x, y)){
                setDir(right);
                setDirection(right);
            }
            
            else{
                setDir(left);
                setDirection(left);
            }
        }
    }
}

Boo::Boo(StudentWorld* w, int xc, int yc): Baddie(w, IID_BOO, xc, yc, 0){
    pause();
    setCount();
    
    
}


void Boo::doSomething(){
    
    if (isHit()){
        pickRandomSquare();
        setBack();
        pause();
        setCount();
        setDir(right);
        setDirection(right);
    }
    
    
    if (getPaused()){
        
        Player *p = getWorld()->Lands(getX(), getY());
        
        if (p != nullptr && !p->walking() && p->isNew()){
            int n = randInt(0,1);
            if (n == 0){
                p->swapCoins();
                getWorld()->playSound(SOUND_BOO_ACTIVATE);
                return;
            }
            
            else if (n == 1){
                p->swapStars();
                getWorld()->playSound(SOUND_BOO_ACTIVATE);
                return;
            }
        }
        
    
        adjustCount();
    
        if (getCount() == 0){
            setSquaresToMove(randInt(1, 3));
            setTicks(retSquaresToMove() * 8);
            
            Choose(getX(), getY());
            
            updateDir(getDir());
            
            unPause();
        }
    }
    
    else if (!getPaused()){
        if (numP(getX(), getY()) > 1){
            Choose(getX(), getY());
            updateDir(getDir());
        }
        
        int x,y;
        getPositionInThisDirection(getDir(), 16, x, y);
        
        if (x % 16 == 0 && y % 16 == 0 && !getWorld()->isValidPosition(x, y)){
            pickNewDir();
            updateDir(getDir());
        }
        
        moveAtAngle(getDir(), 2);
        updateTicks(-1);
        if (getTicks() == 0){
            pause();
            setCount();
        }
    }
}


void Player::swapLocations(){
    int tempx = this->getX();
    int tempy = this->getY();
    
    Player* other = getWorld()->getOtherP(this);
    
    this->moveTo(other->getX(), other->getY());
    other->moveTo(tempx, tempy);
    
    int temp = this->getNumTicks();
    this->setNumTicks(other->getNumTicks());
    other->setNumTicks(temp);
    
    int t = this->getDir();
    
    this->setDir(other->getDir());
    other->setDir(t);
    
    int te = this->getDirection();
    
    this->setDirection(other->getDirection());
    other->setDirection(te);
    
    int m = this->walking();
    
    this->setWalk(other->walking());
    
    other->setWalk(m);
}

void Player::swapCoins(){
    
    int tempC = this->numCoins();
    
    Player* other = getWorld()->getOtherP(this);
    
    this->setCoins(other->numCoins());
    other->setCoins(tempC);
}

void Player::swapStars(){
    
    int tempS = this->numStars();
    
    Player* other = getWorld()->getOtherP(this);
    
    this->setStars(other->numStars());
    other->setStars(tempS);
}


void Moveable::Choose(int x, int y){
    int a,b,c,d,e,f,g,h;
    int pos = randInt(0,3);
    
    getPositionInThisDirection(right, 16, a, b);
    
    if (pos == 0 && getWorld()->isValidPosition(a, b)){
        this->setDir(right);
        this->setDirection(right);
    }
    
    getPositionInThisDirection(left, 16, c, d);
    
    if (pos == 1 && getWorld()->isValidPosition(c, d)){
        this->setDir(left);
        this->setDirection(left);
    }
    
    getPositionInThisDirection(up, 16, e, f);
    
    if (pos == 2 && getWorld()->isValidPosition(e, f)){
        this->setDir(up);
        this->setDirection(right);
    }
    
    getPositionInThisDirection(down, 16, g, h);
    if (pos == 3 && getWorld()->isValidPosition(g, h)){
        this->setDir(down);
        this->setDirection(right);
    }
}


void Moveable::updateDir(int dir){
    
    int direction = dir;
    
    if (direction == left)
        setDirection(left);
    else
        setDirection(right);
}
