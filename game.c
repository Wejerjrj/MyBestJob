/******************
Name: Yarone Djebali
ID: 330242769
Assignment: ex6
*******************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "utils.h"

// Arrays for displaying type names
static const char *itemTypeNames[2] = {"ARMOR", "SWORD"};
static const char *monsterTypeNames[5] = {"Phantom","Spider","Demon","Golem","Cobra"};

// Map display functions
static void displayMap(GameState* g) {
    if (!g->rooms) return;
    
    // Find bounds
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (Room* r = g->rooms; r; r = r->next) {
        if (r->x < minX) minX = r->x;
        if (r->x > maxX) maxX = r->x;
        if (r->y < minY) minY = r->y;
        if (r->y > maxY) maxY = r->y;
    }
    
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;
    
    // Create grid
    int** grid = malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        grid[i] = malloc(width * sizeof(int));
        for (int j = 0; j < width; j++) grid[i][j] = -1;
    }
    
    for (Room* r = g->rooms; r; r = r->next)
        grid[r->y - minY][r->x - minX] = r->id;
    
    printf("=== SPATIAL MAP ===\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] != -1) printf("[%2d]", grid[i][j]);
            else printf("\t");
        }
        printf("\n");
    }
    
    for (int i = 0; i < height; i++) free(grid[i]);
    free(grid);
}

// Display Legend showing monster and item presence, for each room in descending ID order
static void displayLegend(GameState* g) {
    int id;
    Room* curr;
    printf("=== ROOM LEGEND ===\n");
    for (id = g->roomCount - 1; id >= 0; id--) {
        curr = g->rooms;
        while (curr && curr->id != id)
            curr = curr->next;

        if (curr) {
            printf("ID %d: [M:%c] [I:%c]\n", curr->id,
                   curr->monster ? 'V':'X',
                   curr->item    ? 'V':'X');
        }
    }
    printf("===================\n");
}

// Monster functions
void freeMonster(void* data){
    Monster *monster = data;
    free(monster->name);
    free(monster);
}

int compareMonsters(void* a, void* b){
    Monster* m1 = a;
    Monster* m2 = b;

    // Compare by name first
    int res = strcmp(m1->name, m2->name);
    if(res != 0)
        return res;

    // Compare by attack value
    if(m1->attack != m2->attack)
        return m1->attack - m2->attack;

    // Compare by HP
    if(m1->maxHp != m2->maxHp)
        return m1->maxHp - m2->maxHp;

    // Compare by type
    if(m1->type != m2->type)
        return m1->type - m2->type;

    // If monsters are identical
    return 0;
}

void printMonster(void* data){
    Monster *monster = data;
    printf("[%s] Type: %s, Attack: %d, HP: %d\n",
        monster->name, monsterTypeNames[monster->type],
        monster->attack, monster->maxHp);
}


// Item functions
void freeItem(void* data){
    Item* item = data;
    free(item->name);
    free(item);
}

int compareItems(void* a, void* b){
    Item* i1 = a;
    Item* i2 = b;

    // Compare by name first
    int res = strcmp(i1->name, i2->name);
    if(res != 0)
        return res;

    // Compare by value
    if(i1->value != i2->value)
        return i1->value - i2->value;

    // Compare by type
    if(i1->type != i2->type)
        return i1->type - i2->type;

    // If items are identical
    return 0;
}

void printItem(void* data){
    Item *item = data;
    printf("\t\t[%s] %s - Value: %d\n", itemTypeNames[item->type] ,item->name, item->value);
}

// We search for a room by its ID and returns pointer to room or NULL if not found.
static Room* findRoomById(Room* rooms, int id) {
    while (rooms) {
        if (rooms->id == id)
            return rooms;
        rooms = rooms->next;
    }
    return NULL;
}

//  We searches for a room by its grid coordinates and returns pointer to room or NULL if not found.
static Room* findRoomByIndex(Room* rooms, int x, int y) {
    while (rooms) {
        if (rooms->x == x && rooms->y == y)
            return rooms;
        rooms = rooms->next;
    }
    return NULL;
}

// Creates a new empty room with given ID and coordinates.
static Room* createEmptyRoom(int id, int x, int y) {
    Room* r = malloc(sizeof(Room));
    r->id = id;
    r->x = x;
    r->y = y;
    // We initialize all fields to default values.
    r->visited = 0;
    r->monster = NULL;
    r->item = NULL;
    r->next = NULL;
    return r;
}

static void printRoom(Room *room, Player *player) {
    if (!room) return;

    printf("\n--- Room %d ---\n", room->id);

    // Display monster if present
    if (room->monster) {
        printf("Monster: %s (HP:%d)\n", room->monster->name, room->monster->hp);
    }

    // Display item if present
    if (room->item) {
        printf("Item: %s\n", room->item->name);
    }

    // Display player HP if in this room.
    if (player && player->currentRoom == room) {
        printf("HP: %d/%d\n", player->hp, player->maxHp);
    }
}

// Game Functions

// Adds a new room to the game. First room goes at origin, subsequent rooms attach to existing rooms
void addRoom(GameState* g) {

    // Default coordinates for first room
    int xIndex = 0, yIndex = 0;
    if (g->rooms != NULL) {
        // Show map to help user choose attachment point.
        displayMap(g);
        displayLegend(g);

        // Get id room to attach to, and find him.
        int attachId = getInt("Attach to room ID: ");

        Room* baseRoom = findRoomById(g->rooms, attachId);
        if (!baseRoom){
            printf("Invalid ID\n");
            return;
        }

        // Start from base room coordinates
        xIndex = baseRoom->x;
        yIndex = baseRoom->y;

        // Get direction and calculate new coordinates
        int direction = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");
        if (direction == UP_DIR) yIndex--;
        else if (direction == DOWN_DIR) yIndex++;
        else if (direction == LEFT_DIR) xIndex--;
        else if (direction == RIGHT_DIR) xIndex++;

        // Check if room already exists at target location
        if (findRoomByIndex(g->rooms, xIndex, yIndex)) {
            printf("Room exists there\n");
            return;
        }
    }

    // Create new empty room
    Room* newRoom = createEmptyRoom(g->roomCount, xIndex, yIndex);
    int choice = getInt("Add monster? (1=Yes, 0=No): ");
    
    // Optionally add monster
    if (choice == 1) {
        newRoom->monster = malloc(sizeof(Monster));
        newRoom->monster->name = getString("Monster name: ");
        newRoom->monster->type = getInt("Type (0-4): ");
        newRoom->monster->hp = getInt("HP: ");
        newRoom->monster->maxHp = newRoom->monster->hp;
        newRoom->monster->attack = getInt("Attack: ");
    }

    choice = getInt("Add item? (1=Yes, 0=No): ");
    // Optionally add item
    if (choice == 1) {
        newRoom->item = malloc(sizeof(Item));
        newRoom->item->name = getString("Item name: ");
        newRoom->item->type = getInt("Type (0=Armor, 1=Sword): ");
        newRoom->item->value = getInt("Value: ");
    }

    newRoom->visited = 0;
    newRoom->x = xIndex;
    newRoom->y = yIndex;

    // Add room to linked list
    newRoom->next = g->rooms;
    g->rooms = newRoom;
    g->roomCount++;

    printf("Created room %d at (%d,%d)\n", newRoom->id, newRoom->x, newRoom->y);
}

// We initializes the player with default stats, and places him in room 0, and finally marks it as visited room.
void initPlayer(GameState* g) {
    if (!g->rooms) {
        printf("Create rooms first\n");
        return;
    }
    if(g->player){
        printf("Player exists\n");
        return;
    }

    g->player = (Player*)malloc(sizeof(Player));

    // Set initial stats from config.
    g->player->maxHp = g->configMaxHp;
    g->player->hp = g->configMaxHp;
    g->player->baseAttack = g->configBaseAttack;

    // Create BST for inventory and defeated monsters.
    g->player->bag = createBst(compareItems, printItem, freeItem);
    g->player->defeatedMonsters = createBst(compareMonsters, printMonster, freeMonster);

    // Place player in starting room (ID 0)
    Room* currentRoom = findRoomById(g->rooms, 0);
    g->player->currentRoom = currentRoom;
    currentRoom->visited = 1;
}

// Moves player to an adjacent room. We block the player if current room has a monster.
void moveRoom(GameState* g, Room *currentRoom){
    if(!currentRoom) return;
    // Check if monster blocks movement.
    if(currentRoom->monster){
        printf("Kill monster first\n");
        return;
    }

    // Calculate target coordinates
    int xIndex = currentRoom->x;
    int yIndex = currentRoom->y;
    int direction = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");
    if (direction == UP_DIR) yIndex--;
    else if (direction == DOWN_DIR) yIndex++;
    else if (direction == LEFT_DIR) xIndex--;
    else if (direction == RIGHT_DIR) xIndex++;

    
    // Check if room exists at target location.
    Room *searchRoom = findRoomByIndex(g->rooms, xIndex, yIndex);
    if(!searchRoom){
        printf("No room there\n");
        return;
    }

    // Move player and mark room as visited.
    g->player->currentRoom = searchRoom;
    searchRoom->visited = 1;
    return;
}

// Handle combat between player and monster. At the end, or the monster dies or the player dies (gameover).
void fightMonster(GameState *g, Room *currentRoom, Player *player){
    if (!player) return;
    if (!currentRoom) return;
    // If the room doesn't contains a monster.
    if (!currentRoom->monster) {
        printf("No monster\n");
        return;
    }

    Monster *currentMonster = currentRoom->monster;
    // We open a combat loop.
    while(1){
        // The player attacks first.
        currentMonster->hp -= player->baseAttack;
        if(currentMonster->hp < 0) currentMonster->hp = 0;
        printf("You deal %d damage. Monster HP: %d\n",player->baseAttack,currentMonster->hp);
        
        // Check if monster defeated
        if(currentMonster->hp == 0){
            printf("Monster defeated!\n");
            // We add the monster in the bst of defeated monsters.
            bstInsert(player->defeatedMonsters,currentMonster);
            currentRoom->monster = NULL;
            return;
        }

        // The monster attacks player
        player->hp -= currentMonster->attack;
        if(player->hp < 0) player->hp = 0;
        printf("Monster deals %d damage. Your HP: %d\n",currentMonster->attack,player->hp); 
        
        // If player died, It's game over so we free the allocations and exit the game.
        if(player->hp == 0){
            printf("--- YOU DIED ---\n");
            freeGame(g);
            exit(0);
        }          
    }
}

// We picks up item from current room and adds to inventory
void pickupItem(Room *currentRoom, Player *player){
    if(!currentRoom || !player) return;
    // Check if monster blocks pickup
    if(currentRoom->monster){
        printf("Kill monster first\n");
        return;
    }
    // Check if item exists
    if(!currentRoom->item){
        printf("No item here\n");
        return;
    }

    // Check for duplicate items
    Item* isInBag = bstSearch(player->bag,currentRoom->item);
    if(isInBag){
        printf("Duplicate item.\n");
        return;
    }

    // Add item to inventory
    bstInsert(player->bag,currentRoom->item);
    printf("Picked up %s\n",currentRoom->item->name);
    currentRoom->item = NULL;
}

// Displays inventory using selected traversal method. The user chooses if preorder, inorder, or postorder method.
void printBag(BST *bag){
    if(!bag) return;
    printf("=== INVENTORY ===\n");
    if(!bag->root){
        printf("\tEmpty\n");
        return;
    }
    int choice = getInt("1.Preorder 2.Inorder 3.Postorder\n");
    if(choice == 1){
        bstPreorder(bag->root,bag->print);
    } else if(choice == 2){
        bstInorder(bag->root,bag->print);
    } else {
        bstPostorder(bag->root,bag->print);        
    }
}

// Displays defeated monsters using selected traversal method. The user chooses preorder, inorder, or postorder method.
void printDefeatedMonster(BST *monsterTree){
    if(!monsterTree) return;
    printf("=== DEFEATED MONSTERS ===\n");
    if(!monsterTree->root){
        printf("\tNone\n");
        return;
    }
    int choice = getInt("1.Preorder 2.Inorder 3.Postorder\n");
    if(choice == 1){
        bstPreorder(monsterTree->root,monsterTree->print);
    } else if(choice == 2){
        bstInorder(monsterTree->root,monsterTree->print);
    } else {
        bstPostorder(monsterTree->root,monsterTree->print);        
    }
}

/*
Checks if victory conditions are met.
Victory occurs when all rooms were visited and all monsters were defeated.
*/
int checkVictory(GameState *g){
    if(!g || !g->rooms) return 0;

    // Assume victory until proven otherwise
    int isVictory = 1; 
    Room *currentRoom = g->rooms;
    // Check all rooms
    while(currentRoom){
        if(!currentRoom->visited || currentRoom->monster){
            isVictory = 0;
        }
        currentRoom = currentRoom->next;
    }
    // If there are victory, we display victory message, free allocations and exit.
    if(isVictory){
        printf("***************************************\n"
               "VICTORY!\n"
               "All rooms explored. All monsters defeated.\n"
               "***************************************\n");
        freeGame(g);
        exit(0);
    }
    return 0;
}

// Main game loop
void playGame(GameState* g){

    if(!g->player){
        printf("Init player first\n");
        return;
    }

    while(1){
        // Check victory at start of each turn
        if(checkVictory(g)) return;

        displayMap(g);
        displayLegend(g);
        printRoom(g->player->currentRoom,g->player);
        // Get player action
        int choice = getInt("1.Move 2.Fight 3.Pickup 4.Bag 5.Defeated 6.Quit\n");
        switch (choice) {
            case 1:
                moveRoom(g,g->player->currentRoom);
                if(checkVictory(g)) return;
                break;
            case 2:
                fightMonster(g,g->player->currentRoom,g->player);
                if(checkVictory(g)) return;
                break;
            case 3:
                pickupItem(g->player->currentRoom,g->player);
                break;
            case 4:
                printBag(g->player->bag);
                break;
            case 5:
                printDefeatedMonster(g->player->defeatedMonsters);
                break;
            case 6:
                return;
            default:
                printf("Invalid choice, please try again.\n");
                break;
        }
    }
}

// Frees all memory allocated for the game: cleans up rooms, items, monsters, and player data.
void freeGame(GameState* g){
    if(!g) return;

    // Free all rooms in linked list
    Room* currRoom = g->rooms;
    while(currRoom){
        Room* nextRoom = currRoom->next;
        // Free room's item if present
        if(currRoom->item)
            freeItem(currRoom->item);

        // Free room's monster if present
        if(currRoom->monster)
            freeMonster(currRoom->monster);

        free(currRoom);
        currRoom = nextRoom;
    }

    // Free player data
    if(g->player){
        // Free BSTs
        if(g->player->bag)
            bstFree(g->player->bag);
        if(g->player->defeatedMonsters)
            bstFree(g->player->defeatedMonsters);

        g->player->currentRoom = NULL;
        free(g->player);
    }
}
