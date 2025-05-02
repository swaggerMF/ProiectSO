#ifndef HEADER_H
#define HEADER_H

typedef struct {
    int id;
    char username[100];
    float longi;
    float lat;
    char clue[200];
    int value;
} treasure;


// Function declarations
treasure add_info();
void log_action(const char *action, char *path);
void add_treasure(char *cwd, char *path, char *arg, char *log_path);
void print_treasure(char *treasure_path);
void list(char *path, char *arg, char *log_path);
int find_treasure(char *treasure_path, int treasure_id);
void view(char *path, char *hunt_id, int treasure_id, char *log_path);
void remove_treasure(char *treasure_path, int treasure_id, char *hunt, char *log_path);
void remove_hunt(char *cwd, char *hunt_path, char *treasure_path, char *log_path, char *hunt_id);
void list_hunts(char *hunts_path);
int getTreasureNo(char *hunt_path);
char* getHuntsPath();

#endif
