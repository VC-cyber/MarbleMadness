#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
//add all objects here along with the actor class read spec for details

Actor::Actor(int imageID, int startX, int startY, StudentWorld* wrld):GraphObject(imageID, startX, startY), m_wrld(wrld)
{
    //setting isVis = true for the isVisible function we made
    isVis = true; 
}

AliveObj::AliveObj(int imageID, int startX, int startY, StudentWorld* wrld):
    Actor(imageID, startX, startY, wrld)
{
}

Shooters::Shooters(int imageID, int startX, int startY, StudentWorld* wrld):
AliveObj(imageID, startX, startY, wrld)
{
    //initiating m_peas
    m_peas = 0;
}

void Shooters::shoot(){
    //checking if can shoot and shooting if can
    if(isBot() || m_peas > 0){
        m_peas = m_peas -1;
        getWorld() -> shotsFired(getX(), getY(), getDirection());
    }
}

Robot::Robot(int imageID, int startX, int startY, StudentWorld* wrld):
Shooters(imageID, startX, startY, wrld)
{num_ticks = 0;}//initiating num_ticks

Ragebot::Ragebot(int startX, int startY, StudentWorld* wrld, bool isVertical):
Robot(IID_RAGEBOT, startX, startY, wrld)
{
    //setting hp and direction of ragebot
    setHP(10);
    if(isVertical){
        setDirection(down);
    }
    else{
        setDirection(right);
    }
}
bool Robot::doStuff(){
    //checking if allowed to shoot and shooting at player if you can and playing sound
    //getting player's location
    int playerX = getWorld() -> getPlayer() -> getX();
    int playerY = getWorld() -> getPlayer() -> getY();
    if((getDirection() == left) && (playerX) < getX() && playerY == getY() && !(getWorld() -> anythingInWay(playerX, getX(), playerY, true))){
        shoot();
        getWorld() -> playSound(SOUND_ENEMY_FIRE);
        return true;
    }
    if((getDirection() == right) && (playerX) > getX() && playerY == getY() && !(getWorld() -> anythingInWay(getX(), playerX, playerY, true))){
        shoot();
        getWorld() -> playSound(SOUND_ENEMY_FIRE);
        return true;
    }
    if((getDirection() == up) && (playerY) > getY() && playerX == getX() && !(getWorld() -> anythingInWay(getY(), playerY, playerX, false))){
        shoot();
        getWorld() -> playSound(SOUND_ENEMY_FIRE);
        return true;
    }
    if((getDirection() == down) && (getWorld() -> getPlayer() -> getY()) < getY() && getWorld() -> getPlayer() -> getX() == getX() && !(getWorld() -> anythingInWay(playerY, getY(), playerX, false))){
        getWorld() -> playSound(SOUND_ENEMY_FIRE);
        shoot();
        return true;
    }
    return false;
}

void Ragebot::doSomething(){
    //checking if the ragebot ticks is on a tick that is allowed to move
    if(getRTicks() % getWorld() -> getTicks()==0){
        if(!isAlive()){
            return;
        }
        //carrying out the if possible to shoot then shoot
        if(doStuff()){
            incrementTicks();
            return;
        }
        //moving in the given direction, if not possible reversing direction
        else{
            if(getWorld() -> isValidPos(getX(), getY(), false, false, false, false, true, getDirection())){
                int moveX = getX();
                int moveY = getY();
                updateMove(moveX, moveY, getDirection());
                moveTo(moveX, moveY);
            }
            else{
                setDirection(getDirection() + 180);
            }
            
        }
    }
    //going to next tick
    incrementTicks();
}

ThiefBot::ThiefBot(int imageID, int startX, int startY, StudentWorld* wrld):
Robot(imageID, startX, startY, wrld){
    //setting direction, HP, posX, posY, distance travelled and initial distance before turning
    setDirection(right);
    setHP(5);
    distanceBeforeTurning = randInt(1,6);
    posX = -1;
    posY = -1;
    distTraveled = 0;
}

void ThiefBot::thiefBotActivities(){
    //getting the random integer to see if allowed to pick up a goodie
    int x = randInt(1, 10);
    //if allowed to do anything on this tick
    if(getRTicks() % getWorld() -> getTicks() ==0){
        //thief check checks if goodies are able to be picked up and carries out that process
        if(x == 1 && getWorld() -> thiefCheck(getX(), getY(), posX, posY)){
            //the goodie being held's position is saved here
            posX = getX();
            posY = getY();
        }
        //if the distance traveled in the direction is not at the distance before turning
        //continue going in direction
        else if(distTraveled != distanceBeforeTurning && getWorld() -> isValidPos(getX(), getY(), false, false, false, false, true, getDirection())){
            int x = getX();
            int y = getY();
            updateMove(x, y, getDirection());
            moveTo(x, y);
        }
        //if the distance traveled in the direction is at distance before turning
        //or its not possible to go more in this direction change direction randomly
        //change distance before turning too
        else if(distTraveled == distanceBeforeTurning || !getWorld() -> isValidPos(getX(), getY(), false, false, false, false, true, getDirection())){
            //changing distanceBeforeTurning
            distanceBeforeTurning = randInt(1, 6);
            //array for the four directions
            int arr[] = {1,2,3,4};
            //holds the initial direction just in case all directions trapped
            int initialDir = -1;
            bool failedAllFour = true;
            //until all directions are failures
            while(!everythingInArrayNeg(arr, 4)){
                //check a random direction
                int direction = randInt(0,3);
                if(initialDir == -1){
                    initialDir = direction;
                }
                //if direction already found to be a failure retry with another
                if(arr[direction]== -1){
                    continue;
                }
                //if it is possible to go in this direction, move in this direction
                if(getWorld() -> isValidPos(getX(), getY(), false, false, false, false, true, direction * 90)){
                    setDirection(direction * 90);
                    int x = getX();
                    int y = getY();
                    updateMove(x, y, getDirection());
                    moveTo(x, y);
                    failedAllFour = false;
                    break;
                }
                //otherwise make direction -1
                arr[direction] = -1;
            }
            //if all four have failed, set the direction to the first checked direction, initialdir
            if(failedAllFour){
                setDirection(initialDir * 90);
            }
        }
    }
    //continue ticks
    incrementTicks();
}

void ThiefBot::doSomething(){
    //check if alive
    if(!isAlive()){
        return;
    }
    //complete either movement, shooting, or changing direction
    thiefBotActivities();
}

RegThiefBot::RegThiefBot(int startX, int startY, StudentWorld* wrld):ThiefBot(IID_THIEFBOT, startX, startY, wrld){
}

MeanThiefBot::MeanThiefBot(int startX, int startY, StudentWorld* wrld):ThiefBot(IID_MEAN_THIEFBOT, startX, startY, wrld){
    //setting up mean theif bot health as 8 instead of 5
    setHP(8);
}

void MeanThiefBot::doSomething(){
    if(!isAlive()){
        return;
    }
    //check if can shoot
    if(getRTicks() % getWorld() -> getTicks() ==0 && doStuff()){
        incrementTicks();
        return;
    }
    //otherwise carry out regular thiefbot activities
    thiefBotActivities();
    
}

Objects::Objects(int imageID, int startX, int startY, StudentWorld* wrld):
Actor(imageID, startX, startY, wrld){
    setDirection(none);
}

Removables::Removables(int imageID, int startX, int startY, StudentWorld* wrld):
Objects(imageID, startX, startY, wrld)
{
    //initiating the boolean of being alive as true
    m_alive = true;
}

Goodies::Goodies(int imageID, int startX, int startY, StudentWorld* wrld):
Removables(imageID, startX, startY, wrld){
    //setting visibility to be true
    setVis(true);
}

bool Goodies::doStuff(int amount){
    if(!isAlive()){
        return false;
    }
    //if it is visible and the player is on the goodie, increase score play sound
    // and set object to not alive
    if(isVisib() && getWorld() -> isValidPos(getX(), getY(), true, false, false, false, false, getDirection())){
        getWorld() -> increaseScore(amount);
        getWorld() -> playSound(SOUND_GOT_GOODIE);
        eliminate();
        return true;
    }
    return false; 
}

Avatar::Avatar(int startX, int startY, StudentWorld* wrld):Shooters(IID_PLAYER, startX, startY, wrld){
    //set hp and set peas
    setHP(20);
    setPeas(20);
}

void Avatar::doSomething(){
    //check if alive
    if(!isAlive()){
        return;
    }
    //check different cases of keypad clickings
    else{
        int ch;
        if(getWorld() -> getKey(ch)){
            if(ch == KEY_PRESS_ESCAPE){
                //if escape should die
                setHP(0);
            }
            if(ch == KEY_PRESS_SPACE){
                //if space shoot
                shoot();
                if(getPeas() > 0){
                    getWorld() -> playSound(SOUND_PLAYER_FIRE);
                }
            }
            //movement keys
            if(ch == KEY_PRESS_LEFT){
                setDirection(left);
                if(getWorld() -> isValidPos(getX()-1, getY(), false, false, false, false, false, left)){
                    moveTo(getX()-1, getY());
                }
            }
            else if(ch == KEY_PRESS_RIGHT){
                setDirection(right);
                if(getWorld() -> isValidPos(getX()+1, getY(), false, false, false,false, false, right)){
                    moveTo(getX()+1, getY());
                }
            }
            else if(ch == KEY_PRESS_UP){
                setDirection(up);
                if(getWorld() -> isValidPos(getX(), getY()+1, false, false, false, false, false,up)){
                    moveTo(getX(), getY()+1);
                }
            }
            else if(ch == KEY_PRESS_DOWN){
                setDirection(down);
                if(getWorld() -> isValidPos(getX(), getY()-1, false, false, false,false, false, down)){
                    moveTo(getX(), getY()-1);
                }
            }
        }
    }
}

Marble::Marble(int startX, int startY, StudentWorld* wrld):AliveObj(IID_MARBLE, startX, startY, wrld){
    //set hp of marble
    setHP(10);
}

bool Marble::pushed(int direction){
    //if marble is being pushed in a direction and checks if possible or do nothing
    if(direction == left && getWorld() -> isValidPos(getX()-1, getY(), false, true, false,false, false, left)){
        moveTo(getX()-1, getY());
        return true;
    }
    if(direction == right && getWorld() -> isValidPos(getX()+1, getY(), false, true, false,false, false, right)){
        moveTo(getX()+1, getY());
        return true;
    }
    if(direction == up && getWorld() -> isValidPos(getX(), getY()+1, false, true, false,false, false, up)){
        moveTo(getX(), getY()+1);
        return true;
    }
    if(direction == down && getWorld() -> isValidPos(getX(), getY()-1, false, true, false,false, false, down)){
        moveTo(getX(), getY()-1);
        return true;
    }
    return false; 
}

Wall::Wall(int startX, int startY, StudentWorld* wrld):Objects(IID_WALL, startX, startY, wrld){
}

Pea::Pea(int startX, int startY, int dir, StudentWorld* wrld):Removables(IID_PEA, startX, startY, wrld){
    //set direction of the pea
    setDirection(dir);
}

void Pea::doSomething(){
    if(!isAlive()){
        return; 
    }
    //keep moving if possible
    if(getWorld() -> isValidPos(getX(), getY(),false, false, true, false, false, getDirection())){
        int movex = getX();
        int movey = getY();
        updateMove(movex, movey, getDirection());
        moveTo(movex, movey);
    }
    //otherwise die
    else{
        eliminate();
    }
}

Pit::Pit(int startX, int startY, StudentWorld* wrld):Removables(IID_PIT, startX, startY, wrld){
}

void Pit::doSomething(){
    //check if alive
    if(!isAlive()){
        return;
    }
    //check if someone on the pit, if true, set object to not alive
    if(!getWorld() -> isValidPos(getX(), getY(), false, false, false, true, false, getDirection())){
        eliminate();
    }
}

Crystal::Crystal(int startX, int startY, StudentWorld* wrld):Goodies(IID_CRYSTAL, startX, startY, wrld){
}

void Crystal::doSomething(){
    //do the goodie function of checking if anything on the crystal, giving 50
    doStuff(50);
    //check if all crystals are gone and there is an open exit
    if(openExit() && !isAlive()){
        getWorld() -> playSound(SOUND_REVEAL_EXIT);
    }
    //add the sound stuff afta
}

Exit::Exit(int startX, int startY, StudentWorld* wrld):Objects(IID_EXIT, startX, startY, wrld){
    //set visible to false
    setVisible(false);
    setVis(false);
}

void Exit::doSomething(){
    //check if all crystals are gone and if so make it visible
    //if player lands on exit then increase score and go next level
    if(isVisib() && getWorld() -> isValidPos(getX(), getY(), true, false, false,false, false, getDirection())){
        getWorld() -> increaseScore(2000);
        getWorld() -> setPlayerWin();
        return;
    }
    else if(getWorld() -> allCrystalsGone()==0){
        setVisible(true);
        setVis(true);
    }
}

ExtraLifeGoodie::ExtraLifeGoodie(int startX, int startY, StudentWorld* wrld):Goodies(IID_EXTRA_LIFE, startX, startY, wrld){
}

//do stuff as a goodie giving 1000 if landed on and a increase in life
void ExtraLifeGoodie::doSomething(){
    bool x = doStuff(1000);
    
    
    if(x) getWorld() -> incLives();
}

RestoreHealthGoodie::RestoreHealthGoodie(int startX, int startY, StudentWorld* wrld):Goodies(IID_RESTORE_HEALTH, startX, startY, wrld){
}
//do stuff as a goodie giving 500 if landed on and a increase in hp
void RestoreHealthGoodie::doSomething(){
    bool x = doStuff(500);
    //add sound stuff;
    if(x) getWorld() -> getPlayer() -> setHP(20);
}

AmmoGoodie::AmmoGoodie(int startX, int startY, StudentWorld* wrld):Goodies(IID_AMMO, startX, startY, wrld){
}
//do stuff as a goodie giving 100 if landed on and a increase in ammo
void AmmoGoodie::doSomething(){
    bool x = doStuff(100);
    //add sound stuff;
    if(x) getWorld() -> getPlayer() -> setPeas(getWorld() -> getPlayer() -> getPeas() + 20);
}

ThiefBotFactory::ThiefBotFactory(int startX, int startY, bool isMean, StudentWorld* wrld):Objects(IID_ROBOT_FACTORY, startX, startY, wrld){
    m_isMean = isMean; 
}
//check if thief bots in area and if less than max then create new ones
void ThiefBotFactory::doSomething(){
    getWorld() -> countThiefBots(getX(), getY(), m_isMean);
}




