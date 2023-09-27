#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include <sstream>
#include <string>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    board = new Board();
    m_yoshi = nullptr;
    m_peach = nullptr;

    
}

int StudentWorld::init()
{
    
    int x = getBoardNumber();
    string path;
    Board::LoadResult result;
    
    switch (x){
        case 1:
            path = assetPath() + "board01.txt";
            result = board->loadBoard(path);
            break;
        case 2:
            path = assetPath() + "board02.txt";
            result = board->loadBoard(path);
            break;
        case 3:
            path = assetPath() + "board03.txt";
            result = board->loadBoard(path);
            break;
        case 4:
            path = assetPath() + "board04.txt";
            result = board->loadBoard(path);
            break;
        case 5:
            path = assetPath() + "board05.txt";
            result = board->loadBoard(path);
            break;
        case 6:
            path = assetPath() + "board06.txt";
            result = board->loadBoard(path);
            break;
        case 7:
            path = assetPath() + "board07.txt";
            result = board->loadBoard(path);
            break;
        case 8:
            path = assetPath() + "board08.txt";
            result = board->loadBoard(path);
            break;
        default:
            path = assetPath() + "board09.txt";
            result = board->loadBoard(path);
            break;
            
    }
    
    Board::GridEntry ge;
    for (int GridCol = 0; GridCol < BOARD_WIDTH; GridCol++){
        for (int GridRow = 0; GridRow < BOARD_HEIGHT; GridRow++){
            ge = board->getContentsOf(GridCol,GridRow);
            
            switch (ge){
                case Board::empty:
                    break;
                case Board::player:
                    m_peach = new Player(this, IID_PEACH, GridCol, GridRow, 1);
                    m_yoshi = new Player(this, IID_YOSHI, GridCol, GridRow, 2);
                    m_actors.push_back(new Coin_Square(this, IID_BLUE_COIN_SQUARE, GridCol, GridRow, 1));
                    break;
                case Board::blue_coin_square:
                    m_actors.push_back(new Coin_Square(this, IID_BLUE_COIN_SQUARE, GridCol, GridRow, 1));
                    break;
                case Board::red_coin_square:
                    m_actors.push_back(new Coin_Square(this, IID_RED_COIN_SQUARE, GridCol, GridRow, 2));
                    break;
                case Board::star_square:
                    m_actors.push_back(new Star_Square(this, GridCol, GridRow));
                    break;
                case Board::right_dir_square:
                    m_actors.push_back(new Direction_Square(this, GridCol, GridRow, 0));
                    break;
                case Board::left_dir_square:
                    m_actors.push_back(new Direction_Square(this, GridCol, GridRow, 180));
                    break;
                case Board::up_dir_square:
                    m_actors.push_back(new Direction_Square(this, GridCol, GridRow, 90));
                    break;
                case Board::down_dir_square:
                    m_actors.push_back(new Direction_Square(this, GridCol, GridRow, 270));
                    break;
                case Board::event_square:
                    m_actors.push_back(new Event_Square(this, GridCol, GridRow));
                    break;
                case Board::bank_square:
                    m_actors.push_back(new Bank_Square(this, GridCol, GridRow));
                    break;
                case Board::bowser:
                    m_actors.push_back(new Bowser(this, GridCol, GridRow));
                    m_actors.push_back(new Coin_Square(this, IID_BLUE_COIN_SQUARE, GridCol, GridRow, 1));
                    break;
                case Board::boo:
                    m_actors.push_back(new Boo(this, GridCol, GridRow));
                    m_actors.push_back(new Coin_Square(this, IID_BLUE_COIN_SQUARE, GridCol, GridRow, 1));
                    break;
                default:
                    break;
                    
            }
        }
    }
    
    
    startCountdownTimer(99);
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_peach->doSomething();
    m_yoshi->doSomething();
    for (vector<Actor*> ::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        
        if ((*it)->isAlive()) {
            (*it)->doSomething();
        }
    }

    for (auto it = m_actors.begin(); it != m_actors.end(); it++) {
            if (!(*it)->isAlive()) {
                delete *it;
                m_actors.erase(it);
                it = m_actors.begin();
            }
    }
    
    string k = "";
    string q = "";
    
    if (getPeach()->holdsVortex()){
        k = " VOR ";
    }
    
    if (getYoshi()->holdsVortex()){
        q = " VOR ";
    }
    
        
    ostringstream ss;
    ss  << "P1 Roll: " << getPeach()->getNumTicks() / 8
        << " Stars: " << getPeach()->numStars()
        << " $$: " << getPeach()->numCoins()
        << k
        << " | Time: " << timeRemaining()
        << " | Bank: " << getBank()
        << " | P2 Roll: " << getYoshi()->getNumTicks() / 8
        << " Stars: " << getYoshi()->numStars()
        << " $$: " << getYoshi()->numCoins()
        << q;
    
    setGameStatText(ss.str());
    
    if (timeRemaining() == 0){
        
        int winner;
        if (getYoshi()->numStars() == getPeach()->numStars()){
            if (getYoshi()->numCoins() == getPeach()->numCoins())
                winner = randInt(1,2);
            
            else if (getYoshi()->numCoins() > getPeach()->numCoins())
                winner = 1;
            else
                winner = 2;
        }
        
        else{
            if (getYoshi()->numStars() > getPeach()->numStars())
                winner = 1;
            else
                winner = 2;
        }
                
        playSound(SOUND_GAME_FINISHED);
        
        if (winner == 1){
            setFinalScore(getYoshi()->numStars(), getYoshi()->numCoins());
            return GWSTATUS_YOSHI_WON;
        }
        if (winner == 2){
            setFinalScore(getPeach()->numStars(), getPeach()->numCoins());
            return GWSTATUS_PEACH_WON;
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (auto a: m_actors)
        delete a;
    delete m_peach;
    delete m_yoshi;
    m_actors.clear();
}

StudentWorld::~StudentWorld(){}

bool StudentWorld::isValidPosition(int x, int y) {
    
    int xCoord = x;
    int yCoord = y;
    if (x % 16 == 0 && y % 16 == 0){
        
        if (xCoord < 0 || xCoord >= VIEW_WIDTH || yCoord < 0 || yCoord >= VIEW_HEIGHT)
            return false;
        
        if (board->getContentsOf(xCoord/16, yCoord/16) == Board::empty)
            return false;
        
        
        return true;
    }
    return false;
}


Player* StudentWorld::Lands(int x, int y){
    
    
    if (getPeach()->getX() == x && getPeach()->getY() == y && getPeach()->isNew()){
        return getPeach();
    }
    if (getYoshi()->getX() == x && getYoshi()->getY() == y && getYoshi()->isNew()){
        return getYoshi();
    }
    return nullptr;
}


bool StudentWorld::DirectionalatSquare(int x, int y) const{
    Board::GridEntry ge;
    ge = board->getContentsOf(x / 16, y / 16);
    
    if (ge == Board::up_dir_square || ge == Board::down_dir_square || ge == Board::right_dir_square || ge == Board::left_dir_square)
        return true;
    return false;
}

void StudentWorld::dropSquare(int x, int y){
    
    vector<Actor*>::iterator v;
    v = m_actors.begin();
    
    while (v != m_actors.end()){
        if(isSquare(*v) && (*v)->getX() == x && (*v)->getY() == y && !(*v)->isMoveable()){
            (*v)->setDead();
            delete *v;
            v = m_actors.erase(v);
            break;
            
        }
        
        else
            v++;
    }
}

void StudentWorld::addDropSquare(int x, int y){
    m_actors.push_back(new Dropping_Square(this, x / 16, y / 16));
}

bool StudentWorld::isSquare(Actor *p){
    if (p->getX() % 16 == 0 && p->getY() % 16 == 0){
        Board::GridEntry ge;
        ge = board->getContentsOf(p->getX() / 16, p->getY() / 16);
        
        if (ge == Board::up_dir_square || ge == Board::down_dir_square || ge == Board::left_dir_square || ge == Board::right_dir_square || ge == Board::event_square || ge == Board::blue_coin_square || ge == Board::red_coin_square || ge == Board::star_square || ge == Board::bank_square){
            return true;
        }
    }
    
    
    return false;
}


void StudentWorld::addVortex(int x, int y, int dir){
    
    m_actors.push_back(new Vortex(this, x / 16, y / 16, dir));
}


bool StudentWorld::overlaps(Actor* p){
    
    for (vector<Actor*> :: iterator it = m_actors.begin(); it != m_actors.end(); it++ ){
        if ((*it)->canBeHitByAVortex())
            if (overlap((*it)->getX(), (*it)->getY(), p->getX(), p->getY())){
                (*it)->hit();
                return true;
            }
    }
    
    return false;
}

bool StudentWorld::overlap(int x1, int y1, int x2, int y2){
    if (x1 == x2 && y1 == y2)
        return true;
        
    if ((x1 == x2 && (abs(y1 - y2) < 16)) || (y1 == y2 && (abs(x1 - x2) < 16)))
        return true;
    
    return false;
}
