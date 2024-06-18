#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include "Level.h"
#include <string>
using namespace std;
#include <list>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
/*
 Add any private data members to this class required to keep track of
 walls as well as the Avatar object. You may ignore all other items in
 the maze such as RageBots, the exit, etc. for part #1.
 ii. Implement a constructor for this class that initializes your data
 members.
 iii. Implement a destructor for this class that frees any remaining
 dynamically allocated data that has not yet been freed at the time the
 StudentWorld object is destroyed.
 iv. Implement the init() method in this class. It must create the player and
 insert it into the maze at the right starting location (see the Level class
 section of this document for details on the starting location). It must
 also create all of the Walls and add them to the maze as specified in
 the current Level’s data file.
 v. Implement the move() method in your StudentWorld class. During
 each tick, it must ask your Avatar and walls to do something. Your
 move() method need not check to see if the player has died or not; you
 may assume at this point that the player cannot die. Your move()
 method does not have to deal with any actors other than the player and
 the Walls.
 vi. Implement a cleanup() method that frees any dynamically allocated
 data that was allocated during calls to the init() method or the move()
 method (e.g., it should delete all your allocated Walls and the player).
 Note: Your StudentWorld class must have both a destructor and the
 cleanUp() method even though they likely do the same thing (in which
 case the destructor could just call cleanup()).
 */
class Avatar;
class Actor;
class Pea; 
class StudentWorld : public GameWorld
{
public:
    //constructor destructor, with three essential functions: init, move, cleanup,
  StudentWorld(std::string assetPath);
  ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();
    //removing all the dead objects every tick
  void removeDeadObj();
    //checking if it is a valid position for that perspective given by the boolean
  bool isValidPos(int x, int y, bool wantPlayer, bool isMarble, bool isP, bool isAPit, bool isBot, int direction);
    //create new pea object
  void shotsFired(int x, int y, int direction);
    //check and return number of crystals
  int allCrystalsGone();
    //return player
  Avatar* getPlayer(){return m_player;}
    //set players win
  void setPlayerWin(){goNextLevel = true;}
    //get the number of ticks
  int getTicks(){return m_ticks;}
    //check if robot can shoot player
  bool anythingInWay(int startVal, int endVal, int otherDim, bool isX);
    //check if thiefbot can snatch a goodie and if it can, snatch it
  bool thiefCheck(int x, int y, int posX, int posY);
    //count the thief bots in area
  bool countThiefBots(int x, int y, bool isMean);
    //set the display
  void setDisplayText();
  string formatString(int score, int level, int health, int lives, int ammo, int bonus);
    //make the goodie visible
  bool makeGoodieVisible(int x, int y, int posX, int posY);
private:
    Avatar* m_player;
    std::list<Actor*> m_objects;
    bool goNextLevel = false;
    //check position with the coordinates
    bool checkPos(int x, int y, int x2, int y2){return x == x2 && y == y2;}
    int bonus;
    //update to the next one based on the direction
    void aheadCheck(int &newX, int &newY, int direction){
        if(direction == 0){
            newX++;
        }
        if(direction == 180){
            newX--;
        }
        if(direction == 90){
            newY++;
        }
        if(direction == 270){
            newY--;
        }
    }
    int m_ticks;
};

#endif // STUDENTWORLD_H_
