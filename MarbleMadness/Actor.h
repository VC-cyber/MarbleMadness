#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cstdlib>
#include "StudentWorld.h"
using namespace std; 
//Students: Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
//add declarations of stuff here need an actor class, avatar and wall for p1
//has all declarations of objects here 
class Actor: public GraphObject{
    public:
        Actor(int imageID, int startX, int startY, StudentWorld* wrld);
        //booleans to check what object is being looked at
        //pure virtual do something, along with some virtual functions needed
        virtual void doSomething() = 0;
        virtual bool pushed(int direction){return false;};
        virtual bool isMarble(){return false;}
        virtual bool isPit(){return false;}
        virtual bool isCrystal(){return false;}
        virtual void getHurt(int amount){};
        virtual int getPosX(){return 0;}
        virtual int getPosY(){return 0;}
        virtual bool isRemovable(){return false;}
        virtual bool isRemovableExceptPit(){return false;}
        virtual bool isGoodieNotCrystal(){return false;}
        virtual bool isMean(){return false;}
        virtual bool isExit(){return false;}
        virtual bool isBot(){return false;}
        virtual bool isThiefBot(){return false;}
        //check if there is one crystal left which will be used to later play the sound
        virtual bool openExit(){return getWorld() -> allCrystalsGone() == 1;}
        virtual bool isAlive(){return true;}
        virtual void setVis(bool x){isVis = x;}
        virtual bool isVisib(){return isVis;}
        //use direction to update the coordinates for an easier move to
        virtual void updateMove(int& x, int& y, int direction){
            if(direction == 0){
                x++;
            }
            if(direction == 180){
                x--;
            }
            if(direction == 90){
                y++;
            }
            if(direction == 270){
                y--;
            }
        }
        //return the world
        StudentWorld* getWorld(){
            return m_wrld;
        }
    private:
        StudentWorld* m_wrld;
        bool isVis;
};
//All objects with hp
class AliveObj: public Actor{
    public:
        AliveObj(int imageID, int startX, int startY, StudentWorld* wrld);
        virtual void getHurt(int amount){
            m_hp-=amount;
        }
        virtual void doSomething() = 0;
        int setHP(int hp){
            m_hp = hp;
            return m_hp; 
        }
        int getHP(){return m_hp;}
        virtual bool isAlive(){
            if(m_hp > 0){
                return true;
            }
            return false;
        }
    private:
        int m_hp;
};
//all objects with shooting capabilities
class Shooters: public AliveObj{
    public:
        Shooters(int imageID, int startX, int startY, StudentWorld* wrld);
        virtual void shoot();
        void setPeas(int amount){m_peas = amount;}
        int getPeas(){return m_peas;}
    private:
        int m_peas;
};
//all robots
class Robot: public Shooters{
    public:
        Robot(int imageID, int startX, int startY, StudentWorld* wrld);
        virtual bool isBot(){return true;}
        bool doStuff();
        int getRTicks(){return num_ticks;}
        void incrementTicks(){num_ticks++;}
    private:
        int num_ticks;
};
//ragebot
class Ragebot: public Robot{
    public:
        Ragebot(int startX, int startY, StudentWorld* wrld, bool isVertical);
        virtual void doSomething();
};
//all thiefbots
class ThiefBot: public Robot{
    public:
        ThiefBot(int imageID, int startX, int startY, StudentWorld* wrld);
        virtual void doSomething();
        void thiefBotActivities();
        int getPosX(){return posX;}
        int getPosY(){return posY;}
        virtual bool isThiefBot(){return true;}
    private:
        int distanceBeforeTurning = 0;
        int posX;
        int posY;
        int distTraveled;
        bool everythingInArrayNeg(int arr[], int size){
            for(int i = 0; i < size; i++){
                if(arr[i] != -1){
                    return false;
                }
            }
            return true;
        }
};
//regthiefbot
class RegThiefBot:public ThiefBot{
    public:
        RegThiefBot(int startX, int startY, StudentWorld* wrld);
};
//mean
class MeanThiefBot: public ThiefBot{
    public:
        MeanThiefBot(int startX, int startY, StudentWorld* wrld);
        virtual void doSomething();
    virtual bool isMean(){return true;}
};
//all objects
class Objects: public Actor{
    public:
        Objects(int imageID, int startX, int startY, StudentWorld* wrld);
};
//all objects that can be picked up
class Removables: public Objects{
    public:
        Removables(int imageID, int startX, int startY, StudentWorld* wrld);
        virtual bool isRemovable(){return true;}
        virtual bool isRemovableExceptPit(){return true;}
        virtual bool isAlive(){
            return m_alive;
        }
        void eliminate(){
            m_alive = false;
        }
    private:
        bool m_alive;
};
//all goodies
class Goodies: public Removables{
    public:
        Goodies(int imageID, int startX, int startY, StudentWorld* wrld);
        virtual bool isGoodieNotCrystal(){return true;}
        bool doStuff(int amount);
};
//player
class Avatar: public Shooters{
    public:
        Avatar(int startX, int startY, StudentWorld* wrld);
        virtual void doSomething();
};
//marble
class Marble: public AliveObj{
    public:
    Marble(int startX, int startY, StudentWorld* wrld);
    virtual void doSomething(){}
    virtual bool isMarble(){return true;}
    virtual bool pushed(int direction);
};
//wall
class Wall: public Objects{
    public:
        Wall(int startX, int startY, StudentWorld* wrld);
        virtual void doSomething(){}
};
//pea
class Pea: public Removables{
    public:
        Pea(int startX, int startY, int dir, StudentWorld* wrld);
        virtual void doSomething();
};
//pit
class Pit: public Removables{
    public:
        Pit(int startX, int startY, StudentWorld* wrld);
        virtual void doSomething();
        virtual bool isPit(){return true;}
        virtual bool isRemovableExceptPit(){return false;}
};
//crystal
class Crystal: public Goodies{
    public:
        Crystal(int startX, int startY, StudentWorld* wrld);
        virtual void doSomething();
        virtual bool isGoodieNotCrystal(){return false;}
        virtual bool isCrystal(){return true;}
};
//exit
class Exit: public Objects{
    public:
        Exit(int startX, int startY, StudentWorld* wrld);
        virtual void doSomething();
        virtual bool isExit(){return true;}
};
//extralifegoodie
class ExtraLifeGoodie: public Goodies{
    public:
        ExtraLifeGoodie(int startX, int startY, StudentWorld* wrld);
        virtual void doSomething();
};
//restorehealthgoodie
class RestoreHealthGoodie: public Goodies{
    public:
        RestoreHealthGoodie(int startX, int startY, StudentWorld* wrld);
        virtual void doSomething();
};
//ammogoodie
class AmmoGoodie: public Goodies{
    public:
        AmmoGoodie(int startX, int startY, StudentWorld* wrld);
        virtual void doSomething();
};
//thiefbotfactor
class ThiefBotFactory: public Objects{
    public:
        ThiefBotFactory(int startX, int startY, bool isMean, StudentWorld* wrld);
        virtual void doSomething();
    private:
        bool m_isMean;
};

#endif // ACTOR_H_
