#ifndef GAME_H
#define GAME_H

#include "bst.h"

#define UP_DIR 0
#define DOWN_DIR 1
#define LEFT_DIR 2
#define RIGHT_DIR 3

typedef enum { ARMOR, SWORD } ItemType;
typedef enum { PHANTOM, SPIDER, DEMON, GOLEM, COBRA } MonsterType;

typedef struct Item {
    char* name;
    ItemType type;
    int value;
} Item;

typedef struct Monster {
    char* name;
    MonsterType type;
    int hp;
    int maxHp;
    int attack;
} Monster;

typedef struct Room {
    int id;
    int x, y;
    int visited;
    Monster* monster;
    Item* item;
    struct Room* next;
} Room;
 
typedef struct Player {
    int hp;
    int maxHp;
    int baseAttack;
    BST* bag;
    BST* defeatedMonsters;
    Room* currentRoom;
} Player;

typedef struct {
    Room* rooms;
    Player* player;
    int roomCount;
    int configMaxHp;
    int configBaseAttack;
} GameState;

// Monster Management Functions
void freeMonster(void* data);
int compareMonsters(void* a, void* b);
void printMonster(void* data);

// Item Management Functions
void freeItem(void* data);
int compareItems(void* a, void* b);
void printItem(void* data);

// Room Management Functions
void addRoom(GameState* g);

// Player Functions
void initPlayer(GameState* g);
void moveRoom(GameState* g, Room* currentRoom);
void pickupItem(Room* currentRoom, Player* player);
void printBag(BST* bag);

// Combat Functions
void fightMonster(GameState* g, Room* currentRoom, Player* player);
void printDefeatedMonster(BST* monsterTree);

// Game Flow Functions
int checkVictory(GameState* g);
void playGame(GameState* g);
void freeGame(GameState* g);

#endif
