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

int main(int argc, char** argv){
    int *viz = malloc(getTreasureNo(argv[3]) * sizeof(int));
    if( viz == NULL ){
        perror("malloc err");
        exit(EXIT_FAILURE);
    }
    treasure t;
    int ct = 0;
    int fd = open(argv[3], O_RDONLY, 0644);
    if(fd == -1 ){
        perror("failed to open file");
        exit(EXIT_FAILURE);
    }
    while( read(fd, &t, sizeof(treasure))){
        if( viz[ct] == 0){
            printf("User-ul: %s are scorul %d\n",t.username,get_score(viz,t.username,argv[3]));
            viz[ct] = 1;
        }
        ct++;
    }
    close(fd);
    return 0;
}