#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>
using namespace std;
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    //constructor of studentworld initializing player and initial bonus
    m_player = nullptr;
    bonus = 1000;
}

int StudentWorld::init()
{
    //load level
    bonus = 1000;
    string curLevel;
    goNextLevel = false; 
    if(getLevel() > 9){
        curLevel = "level" + std::to_string(getLevel()) + ".txt";
    }
    else{
        curLevel = "level0" + std::to_string(getLevel()) + ".txt";
    }
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found ||
        result == Level:: load_fail_bad_format)
        return -1; // something bad happened!
    // otherwise the load was successful and you can access the
    // contents of the level – here’s an example
    //get contents of level into their respective classes
    for(int i = 0; i < VIEW_HEIGHT; i++){
        for(int j = 0; j < VIEW_WIDTH; j++){
            Level::MazeEntry item = lev.getContentsOf(i, j);
            if (item == Level::player){
                m_player = new Avatar(i,j, this);
            }
            if(item == Level::wall){
                m_objects.push_front(new Wall(i, j, this));
            }
            if(item == Level::marble){
                m_objects.push_front(new Marble(i, j, this));
            }
            if(item == Level::pit){
                m_objects.push_front(new Pit(i, j, this));
            }
            if(item == Level::crystal){
                m_objects.push_front(new Crystal(i, j, this));
            }
            if(item == Level::exit){
                m_objects.push_front(new Exit(i, j, this));
            }
            if(item == Level::extra_life){
                m_objects.push_front(new ExtraLifeGoodie(i, j, this));
            }
            if(item == Level::restore_health){
                m_objects.push_front(new RestoreHealthGoodie(i, j, this));
            }
            if(item == Level::ammo){
                m_objects.push_front(new AmmoGoodie(i, j, this));
            }
            if(item == Level::vert_ragebot){
                m_objects.push_front(new Ragebot(i, j, this, true));
            }
            if(item == Level::horiz_ragebot){
                m_objects.push_front(new Ragebot(i, j, this, false));
            }
            if(item == Level::thiefbot_factory){
                m_objects.push_front(new ThiefBotFactory(i, j, false, this));
            }
            if(item == Level::mean_thiefbot_factory){
                m_objects.push_front(new ThiefBotFactory(i, j, true, this));
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you type q
    //check ticks for robots to move every so often
    m_ticks = (28 - getLevel())/4;
    if(m_ticks < 3){
        m_ticks = 3;
    }
    //player do something
    m_player -> doSomething();
    if(!(m_player -> isAlive())){
        decLives();
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    //iterate through objects and call do something function for each of them
    for(list<Actor*>::iterator p = m_objects.begin(); p != m_objects.end(); p++){
        //when theres stuff to do
        //if player is dead
        if(!(m_player -> isAlive())){
            decLives();
            playSound(SOUND_PLAYER_DIE);
            return GWSTATUS_PLAYER_DIED;
        }
        //if ready to go next level
        else if(goNextLevel){
            increaseScore(bonus);
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
        //do something if object alive
        else if((*p) -> isAlive()){
            (*p) -> doSomething();
        }
    }
    //remove dead objects update stat line and bonus
    removeDeadObj();
    setGameStatText("Game will end when you type q");
    setDisplayText();
    if(bonus > 0)
        bonus--;
    if(goNextLevel){
        increaseScore(bonus);
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::removeDeadObj(){
    //removing dead objects
    for(list<Actor*>::iterator p = m_objects.begin(); p != m_objects.end();){
        if(!((*p) -> isAlive())){
            //if its a bot, play the sound, and increase the score based on meanness
            if((*p) -> isBot()){
                playSound(SOUND_ROBOT_DIE);
                if((*p) -> isThiefBot()){
                    if((*p) -> isMean()){
                        increaseScore(20);
                    }
                    else{
                        increaseScore(10);
                    }
                    //if thiefbot make the goodie visible
                    makeGoodieVisible((*p) -> getX(), (*p) -> getY(), (*p) -> getPosX(), (*p) -> getPosY());
                }
                else{
                    increaseScore(100);
                }
            }
            //delete objects
            delete (*p);
            p = m_objects.erase(p);
        }
        else{
            p++;
        }
    }
}

bool StudentWorld::isValidPos(int x, int y, bool wantPlayer, bool isMarb, bool isP, bool isAPit, bool isBot, int direction){
    //checking is valid position in the perspective of different objects
    if(wantPlayer){
        return checkPos(m_player -> getX(), m_player -> getY(), x, y);
    }
    int newX = x;
    int newY = y;
    //iterate through objects
    for(list<Actor*>::iterator p = m_objects.begin(); p != m_objects.end(); p++){
        //marble using isValidPos
        if(isMarb){
            if(!((*p) -> isPit()) && checkPos((*p) -> getX(), (*p) -> getY(), x, y)){
                return false; 
            }
        }
        //pea using isValidPos
        else if(isP){
            newX = x;
            newY = y;
            aheadCheck(newX, newY, direction);
            if(!((*p) -> isRemovable() || (*p) -> isExit()) && checkPos((*p) -> getX(), (*p) -> getY(), newX, newY)){
                if((*p) -> isBot()){
                    playSound(SOUND_ROBOT_IMPACT);
                }
                (*p) -> getHurt(2);
                return false;
            }
        }
        //pit using isValidPos
        else if(isAPit){
            if((*p) -> isMarble() && checkPos((*p) -> getX(), (*p) -> getY(), x, y)){
                (*p) -> getHurt(10);
                 return false;
             }
        }
        //bot using isValidPos
        else if(isBot){
            newX = x;
            newY = y;
            aheadCheck(newX, newY, direction);
            if(!((*p) -> isRemovableExceptPit()) && checkPos((*p) -> getX(), (*p) -> getY(), newX, newY)){
                return false;
            }
        }
        //player and others
        else if(!((*p) -> isRemovableExceptPit()) && !((*p) -> isExit()) && checkPos((*p) -> getX(), (*p) -> getY(), x, y)){
            if((*p) -> isMarble() && (*p) -> pushed(direction)){
                return true;
            }
            return false;
        }
    }
    //checking again
    if(isP){
        newX = x;
        newY = y;
        aheadCheck(newX, newY, direction);
        if(checkPos((m_player) -> getX(), (m_player) -> getY(), newX, newY)){
            (m_player) -> getHurt(2);
            playSound(SOUND_PLAYER_IMPACT);
            return false;
        }
    }
    //check the next
    newX = x;
    newY = y;
    aheadCheck(newX, newY, direction);
    if(isBot && m_player -> getX() == newX && m_player -> getY() == newY) {
        return false; 
    }
    return true; 
}

void StudentWorld::shotsFired(int x, int y, int direction){
    //creating new pea object
    m_objects.push_front(new Pea(x, y, direction, this));
}

int StudentWorld::allCrystalsGone(){
    //checking if all crystals are gone and returning number of crystals
    int count = 0;
    for(list<Actor*>::iterator q = m_objects.begin(); q != m_objects.end(); q++){
        if((*q) -> isCrystal()){
            count++;
        }
    }
    return count; 
}

bool StudentWorld::anythingInWay(int startVal, int endVal, int otherDim, bool isX){
    //checking if anything in the way horizontally
    if(isX){
        for(list<Actor*>::iterator p = m_objects.begin(); p != m_objects.end(); p++){
            //if its not a pickupable item
            if(!((*p) -> isRemovable()||(*p) -> isExit()) && (*p) -> getX() > startVal && (*p) -> getX() < endVal && (*p) -> getY() == otherDim){
                return true;
            }
        }
        return false;
    }
    //checking if anything in the way vertically
    else{
        for(list<Actor*>::iterator p = m_objects.begin(); p != m_objects.end(); p++){
            //if its not a pickupable item or an exit
            if(!((*p) -> isRemovable()||(*p) -> isExit()) && (*p) -> getY() > startVal && (*p) -> getY() < endVal && (*p) -> getX() == otherDim){
                return true;
            }
        }
        return false;
    }
}

bool StudentWorld::thiefCheck(int x, int y, int posX, int posY){
    //if there is a thiefbot on a goodie make the goodie not visible and play munch sound
    if(posX != -1 && posY != -1){
        return false;
    }
    for(list<Actor*>::iterator p = m_objects.begin(); p != m_objects.end(); p++){
        if((*p) -> isGoodieNotCrystal() && (*p) -> isVisib() && checkPos((*p) -> getX(), (*p) -> getY(), x, y)){
            playSound(SOUND_ROBOT_MUNCH);
            (*p) -> setVis(false);
            (*p) -> setVisible(false);
            return true;
        }
    }
    return false;
}

bool StudentWorld::countThiefBots(int x, int y, bool isMean){
    //count thiefbots near thief factory and if allowed to spawn, spawn the respective bots
    int count = 0;
    //check 6 * 6 box around the thief factory
    for(int i = x-3; i < x + 3; i++){
        for(int j = y-3; j < y + 3; j++){
            //if thief bot is in the box
            for(list<Actor*>::iterator p = m_objects.begin(); p != m_objects.end(); p++){
                if((*p) -> isThiefBot() && checkPos(i, j, (*p) -> getX(), (*p) -> getY())){
                    if(checkPos(x, y, (*p) -> getX(), (*p) -> getY())){
                        return false;
                    }
                    count++;
                }
            }
        }
    }
    int chance = randInt(1, 50);
    if(count<3 && chance ==1){
        playSound(SOUND_ROBOT_BORN);
        if(isMean){
            m_objects.push_front(new MeanThiefBot(x, y, this));
        }
        else{
            m_objects.push_front(new RegThiefBot(x, y, this));
        }
        return true;
    }
    return false; 
}

bool StudentWorld::makeGoodieVisible(int x, int y, int posX, int posY){
    //after thief bot dies, make the goodie visible at the position of the death
    for(list<Actor*>::iterator p = m_objects.begin(); p != m_objects.end(); p++){
        //when theres stuff to do
        if((*p) -> isGoodieNotCrystal() && checkPos(posX, posY, (*p) -> getX(),  (*p) -> getY())){
            (*p) -> setVis(true);
            (*p) -> setVisible(true);
            (*p) -> moveTo(x, y);
            return true;
        }
    }
    return false; 
}

void StudentWorld::cleanUp()
{
   //cleanup
    delete m_player;
    m_player = nullptr;
    list<Actor*>::iterator p = m_objects.begin();
   while(p != m_objects.end()){
        //when theres stuff to do
        delete (*p);
        p = m_objects.erase(p);
    }
}

void StudentWorld::setDisplayText()
{
    //scoreboard
 int score = getScore();
 int level = getLevel();
 int livesLeft = getLives();
 // Next, create a string from your statistics, of the form:
 // Score: 0000100 Level: 03 Lives: 3 Health: 70% Ammo: 216 Bonus: 34
 string s = formatString(score, level, livesLeft,
 ((m_player -> getHP())* 5), m_player -> getPeas(), bonus);
// Finally, update the display text at the top of the screen with your
// newly created stats
 setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

string StudentWorld::formatString(int score, int level, int lives, int health, int ammo, int bonus){
    //formatting string
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(7) << score << "  ";
    oss.fill('0');
    oss << "Level: " << setw(2) << level << "  ";
    oss.fill(' ');
    oss << "Lives: " << setw(2) << lives << "  ";
    oss.fill(' ');
    oss << "Health: " << setw(3) << health << "%  ";
    oss.fill(' ');
    oss << "Ammo: " << setw(3) << ammo << "  ";
    oss.fill(' ');
    oss << "Bonus: " << setw(4) << bonus;
    return oss.str();
}

StudentWorld::~StudentWorld(){
    cleanUp();
}

