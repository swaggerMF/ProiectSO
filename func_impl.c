#define _GNU_SOURCE 
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>

treasure add_info() {
    treasure t;
    printf("ID: ");
    scanf("%d", &t.id);
    printf("Username: ");
    scanf("%99s", t.username);
    printf("Longitude: ");
    scanf("%f", &t.longi);
    printf("Latitude: ");
    scanf("%f", &t.lat);
    printf("Clue: ");
    getchar();
    fgets(t.clue, 199, stdin);
    t.clue[strcspn(t.clue, "\n")] = 0;
    printf("Value: ");
    scanf("%d", &t.value);
    return t;
}

void log_action(const char *action, char *path) { 
    //functia pt log, care marcheaza o actiune impreuna cu un timestamp
    //action reprezinta mesajul care va fi transmis in log, iar path ul reprezinta path ul catre log

    int log_file = open(path, O_WRONLY | O_APPEND | O_CREAT, 0644); //deschidem fisierul log sau il facem in cazul in care nu exista
    if (log_file == -1) {
        perror("Unable to open log file");
        return;
    }

    time_t raw_time;
    struct tm *time_info;
    char time_string[100];

    time(&raw_time);
    time_info = localtime(&raw_time);

    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);

    //avem in time_string un string cu timestamp ul la momentul la care s a facut actiunea

    char log_message[256];
    snprintf(log_message, sizeof(log_message), "Performed action: '%s' @ %s\n", action, time_string);
    //se concateneaza mesajul cu actiuena facuta si timestamp ul

    ssize_t bytes_written = write(log_file, log_message, strlen(log_message)); //se verifica cati biti se scriu in log_file
    if (bytes_written == -1) {
        perror("Unable to write to log file");
    }

    close(log_file);
}

void add_treasure(char *cwd,char *path, char *arg, char* log_path){
    // functie pentru adaugarea unui treasure la un hunt
    // cwd- current working directory, arg(argv[2])-id-ul hunt ului, log_path- path ul catre log ul din acest hunt
    char treasure_path[1024];
    int written = snprintf(treasure_path, sizeof(treasure_path), "%s/treasure_%s.dat", path,arg); //se creaza path ul catre file ul care da store la treasure
    if (written < 0 || written >= sizeof(treasure_path)) { //se verifica daca nu cumva path ul e prea lung
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }

    int f = open(treasure_path, O_WRONLY | O_CREAT | O_APPEND, 0777); // se deschide file ul in care se vor scrie datele despre treasure
    if( f == -1){
        perror("failed to open file");
        exit(EXIT_FAILURE);
    }

    treasure t = add_info(); //se adauga informatiile despre treasure in variabila t dupa care se scriu in fisier

    if( write(f, &t, sizeof(treasure)) == -1){
        perror("Unable to add treasure");
        close(f);
        exit(EXIT_FAILURE);
    }
    else{
        printf("Treasure added succesfully\n");
    }

    char symlink_path[1024];
    written = snprintf(symlink_path, sizeof(symlink_path), "%s/logged_hunt-%s", cwd, arg); //se creaza path ul unde va fi localizat symlinkul
    if (written < 0 || written >= sizeof(symlink_path)) {
        perror("Path too long\n");
        exit(EXIT_FAILURE);
    }
    struct stat st;

    if( lstat(symlink_path,&st) == -1 ){ //se verifica daca exista deja un symlink creat
        if (errno == ENOENT) {  // Daca nu exista atunci se creaza unul
        if (symlink(log_path, symlink_path) == -1) {
            perror("Failed to create symbolic link");
            exit(EXIT_FAILURE);
        } else {
            printf("Symbolic link created\n");
        }
    } else {
        perror("Error checking symbolic link");
        exit(EXIT_FAILURE);
    }
    }

    log_action("Add Treasure",log_path); //se noteaza in log faptul ca am adaugat o comoara
    close(f);
}

void print_treasure(char* treasure_path){ //functie pentru afisarea detaliilor despre un treasure
    int f = open(treasure_path, O_RDONLY);
    if( f == -1 ){
        perror("failed to open file\n");
        close(f);
        exit(EXIT_FAILURE);
    }

    treasure t;
    int nr = 1;
    while( read(f,&t,sizeof(treasure))){
        printf("Treasure no.%d\n", nr++);
        printf("ID: %d\n", t.id);
        printf("Username: %s\n", t.username);
        printf("GPS Longitude: %f\n", t.longi);
        printf("GPS Latitude: %f\n", t.lat);
        printf("Clue: %s\n", t.clue);
        printf("Value: %d\n\n\n",t.value);
    }
}

void list(char* path, char* arg,char* log_path){ //functie pentru afisarea detaliata a unui hunt, path->cwd/Hunts, arg->argv[2](id ul huntului)
    char treasure_path[1024];
    int written = snprintf(treasure_path, sizeof(treasure_path), "%s/%s/treasure_%s.dat", path,arg,arg); // se creaza path ul catre treasure
    if (written < 0 || written >= sizeof(treasure_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
    }

    printf("Hunt name:%s\n", arg);
    struct stat st;
    if(stat(treasure_path, &st)){ //se verifica daca exista hunt ul, in cazul in care nu, se afiseaza o eroare, altfel se afiseaza dim. si timpul ultimei modificari
        perror("stat failed,hunt probably doesn't exist");
        exit(EXIT_FAILURE);
    }
    else{
        printf("File size: %ld\n", st.st_size);
        printf("Last modified: %s\n", ctime(&st.st_mtime));
    }

    print_treasure(treasure_path); //se afiseaza toate treasure urile cu detaliile lor
    char msg[100];
    written = snprintf(msg,sizeof(msg),"List %s",arg);
    if (written < 0 || written >= sizeof(msg)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
    }
    log_action(msg,log_path); // se marcheaza in log ca s a listat huntul cu id ul arg
}

int find_treasure(char *treasure_path, int treasure_id){// functie pentru a gasi un treasure dupa id ul sau 
    int f = open(treasure_path, O_RDONLY);
    if( f == -1 ){
        perror("failed to open file");
        close(f);
        exit(EXIT_FAILURE);
    }
    int i = 0;
    treasure t;
    while( read(f, &t, sizeof(treasure))){
        if( t.id == treasure_id){
            close(f);
            return i;
        }
        i++;
    }
    close(f);
    return -1;
}

void view(char *path, char *hunt_id, int treasure_id, char* log_path){ //functie pt afisarea unui treasure specific; path->cwd/Hunts
    char treasure_path[1024];
    int written = snprintf(treasure_path, sizeof(treasure_path), "%s/%s/treasure_%s.dat", path,hunt_id,hunt_id);
    if (written < 0 || written >= sizeof(treasure_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
    }
    
    int index = find_treasure(treasure_path,treasure_id); //index ul unde se afla treasure ul cautat
    if( index == -1) printf("Nu exista treasure ul cu id ul %d", treasure_id);
    else{
        int fd = open( treasure_path,O_RDONLY); // se deschide fisierul cu treasure uri
        if( fd == -1){
            perror("failed to open file\n");
            exit(EXIT_FAILURE);
        }
        if( lseek(fd , index*sizeof(treasure),SEEK_SET) == -1){ // se muta cursorul la treasure ul cautat si se printeaza valorile
            perror("lseek failed\n");
            exit(EXIT_FAILURE);
        }
        treasure t;
        read(fd,&t,sizeof(treasure));
        printf("ID: %d\n", t.id);
        printf("Username: %s\n", t.username);
        printf("GPS Longitude: %f\n", t.longi);
        printf("GPS Latitude: %f\n", t.lat);
        printf("Clue: %s\n", t.clue);
        printf("Value: %d\n\n\n",t.value);
    }
    
    char msg[100];
    written = snprintf(msg,sizeof(msg),"View Treasure %d from %s",treasure_id,hunt_id);
    if (written < 0 || written >= sizeof(msg)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
    }
    log_action(msg,log_path); // vezi functiile anterioare
}


void remove_treasure(char* treasure_path, int treasure_id, char* hunt,char* log_path){
    int index = -1;
    while( (index = find_treasure(treasure_path,treasure_id)) != -1) {//index ul unde se afla treasure ul cautat, repetam asta pana se elimina toate treasure urile cu acest index
        off_t src_offset = (index + 1) * sizeof(treasure); //offset catre urmatorul traesure de dupa treasure ul cautat de noi 
        off_t dst_offset = index * sizeof(treasure); //offset direct la treasure ul cautat

        int fd = open(treasure_path, O_RDWR); // se deschide file ul cu treasure uri
        if( fd == -1){
            perror("file failed to open\n");
            close(fd);
            exit(EXIT_FAILURE);
        }
        treasure t;
        while(1){
            if(lseek(fd, src_offset, SEEK_SET) == -1 ){ //se muta cursorul catre treasure ul urmator
                perror("lseek failed\n");
                exit(EXIT_FAILURE); 
            }
            ssize_t bytes = read(fd, &t, sizeof(treasure)); // se incearca sa se citeasca treasure ul de dupa treasure ul cautat
            if( bytes == 0 ) break; // daca treasure ul cautat e ultimul, sau loop ul ajunge la final, nu se mai citesc bytes si se iese din loop
            if( bytes != sizeof(treasure)){ // daca se citesc alt numar de bytes se da o eroare
                perror("read failed\n");
                exit(EXIT_FAILURE);
            }
            //pana aici s a citit treasure ul urmator, scopul de fapt este de a shifta toate treasure urile cu o pozitie la stanga peste cel pe care vrem sa 
            //il stergem
            if( lseek(fd, dst_offset, SEEK_SET) == -1){ // se muta cursorul la treasure ul care trebuie sters
                perror("lseek failed\n");
                exit(EXIT_FAILURE);
            }

            if(write(fd, &t, sizeof(treasure)) != sizeof(treasure)){ // se scrie peste el
                perror("write failed\n");
                exit(EXIT_FAILURE);
            }
            src_offset = src_offset + sizeof(treasure); // se incrementeaza offset urile pentru a putea shifta toate treasure urile din continuare
            dst_offset = dst_offset + sizeof(treasure);
        }
        // la final, daca treasure ul pe care vrem sa il stergem este la mijlocul listei vom avea doua treasure uri identice, datorita shiftarii
        //trebuie truncat sau stearsa ultima inregistrare din treasure.dat

        if( ftruncate(fd, dst_offset) == -1){ 
            perror("truncation failed\n");
            exit(EXIT_FAILURE);
        }
    }
    char msg[100];
    int written = snprintf(msg,sizeof(msg),"Remove Treasure %d from %s",treasure_id,hunt);
    if (written < 0 || written >= sizeof(msg)) {
        perror("Path too long\n");
        exit(EXIT_FAILURE);
    }
    log_action(msg,log_path);
}

void remove_hunt(char *cwd,char *hunt_path, char* treasure_path, char* log_path, char* hunt_id){
    char symlink_path[1024];
    int written = snprintf(symlink_path, sizeof(symlink_path), "%s/logged_hunt-%s",cwd , hunt_id); //path catre symlink
    if (written < 0 || written >= sizeof(symlink_path)) {
        perror("Path too long\n");
        exit(EXIT_FAILURE);
    }
    if (unlink(symlink_path) == -1) { // se sterge intai symlink ul
        perror("Failed to remove symbolic link");
        exit(EXIT_FAILURE);
    }
    
    if( unlink(log_path) == -1){ // se sterge log ul
        perror("unlink failed\n");
        exit(EXIT_FAILURE);
    }
    if( unlink(treasure_path) == -1){ // se sterg datele despre treasure uri
        perror("unlink failed\n");
        exit(EXIT_FAILURE);
    }
    if( rmdir(hunt_path) == -1){ // se sterge directorul pentru hunt
        perror("rmdir failed\n");
        exit(EXIT_FAILURE);
    }
}

int getTreasureNo(char *hunt_path){
    int f = open(hunt_path, O_RDONLY);
    if( f == -1){
        perror("error opening file\n");
        exit(EXIT_FAILURE);
    }

    treasure t;
    int ct = 0;
    while( read(f,&t,sizeof(treasure))) ct++;
    close(f);
    return ct;
}

void list_hunts(char *hunts_path){
    DIR *d;
    struct dirent *dir;
    if((d = opendir(hunts_path)) == NULL ){
        perror("Error opening directory\n");
        exit(EXIT_FAILURE);
    }
    // printf("%s", hunts_path);
    if(d){
        while((dir = readdir(d)) != NULL ){
            if( dir->d_type == DT_DIR ){
                printf("Hunt ul %s cu ", dir->d_name);
                char h_path[1024];
                int written = snprintf(h_path, sizeof(h_path), "%s/%s",hunts_path , dir->d_name); //path catre un hunt specific
                if (written < 0 || written >= sizeof(h_path)) {
                    perror("Path too long\n");
                    exit(EXIT_FAILURE);
                }
                printf("%d treasure uri\n",getTreasureNo(h_path));
            }
        }
    }
}