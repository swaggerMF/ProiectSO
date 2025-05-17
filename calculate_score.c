#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "tr_manager_hdr.h"
#include "func_impl_tr_manager.c"
#include <dirent.h>

int get_score(int* viz,const char *user, const char *path){
    int score = 0;
    int fd = open(path, O_RDONLY, 0644);
    if( fd == -1 ){
        perror("err at file open");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    treasure t;
    while( read(fd, &t, sizeof(treasure))){
        if( strcmp(user,t.username) == 0){
            score = score + t.value;
            viz[i] = 1;
        }
        i++;
    }
    return score;
}

int main(void){
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));
    char cwd_N[1024];
    int written = snprintf(cwd_N, sizeof(cwd_N), "%s%s", cwd, "/Hunts/"); //Concatenam Hunts la cwd si vom avea cwd_N
    if (written < 0 || written >= sizeof(cwd_N)) {
        perror("Path too long\n");
        exit(EXIT_FAILURE);
    }

    // int fd = open(cwd_N, O_RDONLY, 0644);
    // if( fd == -1 ){
    //     perror("err at file open");
    //     exit(EXIT_FAILURE);
    // }

    DIR *d;
    struct dirent *dir;
    if((d = opendir(cwd_N)) == NULL ){
        perror("Error opening directory\n");
        exit(EXIT_FAILURE);
    }
    while((dir = readdir(d)) != NULL ){
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            continue;

        struct stat st;
        char hunt_path[1024];
        int written = snprintf(hunt_path, sizeof(hunt_path), "%s%s",cwd_N , dir->d_name); //path catre un hunt specific
        if (written < 0 || written >= sizeof(hunt_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }
        if(stat(hunt_path,&st) == -1){
            perror("stat failed\n");
            exit(EXIT_FAILURE);
        }
        if(S_ISDIR(st.st_mode)){
            printf("%s: \n\n", dir->d_name);
            char treasure_path[1024];
            written = snprintf(treasure_path, sizeof(treasure_path), "%s/treasure_%s.dat", hunt_path,dir->d_name); //path catre un lista cu treasureuri
            if (written < 0 || written >= sizeof(hunt_path)) {
                perror("Path too long\n");
                exit(EXIT_FAILURE);
            }
            int tr_no = getTreasureNo(treasure_path);
            int* viz = (int*)malloc(tr_no * sizeof(int));
            if(viz == NULL ){
                perror("Err at malloc");
                exit(EXIT_FAILURE);
            }
            treasure t;
            int ct = 0;
            int fd2 = open(treasure_path, O_RDONLY, 0644);
            if( fd2 == -1 ){
                perror("err at file open");
                exit(EXIT_FAILURE);
            }
            while( read(fd2, &t, sizeof(treasure))){
                if( viz[ct] == 0){
                    printf("Username: %s cu scorul %d\n", t.username, get_score(viz,t.username,treasure_path));
                }
                viz[ct++] = 1;
            }
            printf("\n");

        }   
    }
    closedir(d);

    return 0;
}