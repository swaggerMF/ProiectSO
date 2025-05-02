#include "tr_manager_hdr.h"
#include "func_impl_tr_manager.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv){
    if( argc < 2 ){
        printf("Usage: ./treasure_manager arg1 arg2...(--add / --list / --view / --remove_treasure / --remove_hunt)\n");
        exit(EXIT_FAILURE);
    }
    int written = 0;
    struct stat st;
    if (stat("Hunts", &st) == -1) {
        //Daca directorul Hunts nu exista il creaza
        if (mkdir("Hunts", 0777) == -1) {
            perror("mkdir Hunts failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Daca Hunts nu e director, eroare
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "Hunts is not a directory\n");
            exit(EXIT_FAILURE);
        }
    }


    char cwd[1024];
    if(getcwd(cwd,sizeof(cwd)) == NULL ){ //Obtinem current working directory, current working directory este path ul de unde se executa programul
        perror("cwd failed");
        exit(EXIT_FAILURE);
    }

    char cwd_N[1024];
    written = snprintf(cwd_N, sizeof(cwd_N), "%s%s", cwd, "/Hunts/"); //Concatenam Hunts la cwd si vom avea cwd_N
    if (written < 0 || written >= sizeof(cwd_N)) {
        perror("Path too long\n");
        exit(EXIT_FAILURE);
    }  
    // printf("%s\n",cwd_N);

    char log_path[1024];
    written = snprintf(log_path, sizeof(log_path), "%s/%s/logged_hunt.txt",cwd_N, argv[2] ); // se creaza path ul catre log 
    if (written < 0 || written >= sizeof(log_path)) {
        perror("Path too long\n");
        exit(EXIT_FAILURE);
    }

	if( strcmp(argv[1], "--add") == 0){ // se verifica primul argument pt a sti ce fel de actiune trebuie sa facem
        if( argv[2] == NULL ){
            printf("Usage: ./treasure_manager hunt_name\n");
            exit(EXIT_FAILURE);
        }
        char dir_path[1024];
		written = snprintf(dir_path, sizeof(dir_path), "%s%s", cwd_N, argv[2]); //Concatenam current working directory si hunt id pt a obtiine dir path
        if (written < 0 || written >= sizeof(dir_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }

		if( !stat(dir_path, &st) ){ //verificam daca exista hunt ul specificat in dir_path(cwd/Hunts/argv[2])
			if( S_ISDIR(st.st_mode) ){ //daca exista adaugam date despre treasure
				add_treasure(cwd,dir_path,argv[2],log_path);
			}
		}
        else{
            if( mkdir(dir_path, 0777) == -1){ //daca nu exista se creaza un nou director cu id ul huntului
                perror("mkdir failed");
                exit(EXIT_FAILURE);
            }
            
            add_treasure(cwd,dir_path,argv[2],log_path);
        }
	}

    
	else if( strcmp(argv[1], "--list") == 0){
        if( argv[2] == NULL ){
            printf("Usage: ./treasure_manager hunt_name\n");
            exit(EXIT_FAILURE);
        }
		list(cwd_N, argv[2],log_path);
	}

	else if( strcmp(argv[1], "--view") == 0){
        if( argv[2] == NULL || argv[3] == NULL){
            printf("Usage: --view hunt_name treasure_id\n");
            exit(EXIT_FAILURE);
        }
        int t_id = atoi(argv[3]);
		view(cwd_N, argv[2],t_id,log_path);
	}

	else if( strcmp(argv[1], "--remove_treasure") == 0){
        if( argv[2] == NULL || argv[3] == NULL){
            printf("Usage: ./treasure_manager --remove_treasure Hunt_name treasure_id\n");
            exit(EXIT_FAILURE);
        }
        char treasure_path[1024];
        written = snprintf(treasure_path, sizeof(treasure_path), "%s/%s/treasure_%s.dat",cwd_N, argv[2], argv[2]);
        if (written < 0 || written >= sizeof(treasure_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }
        int t_id = atoi(argv[3]);

        remove_treasure(treasure_path, t_id,argv[2],log_path);
	}

	else if( strcmp(argv[1], "--remove_hunt") == 0){
        if( argv[2] == NULL ){
            printf("Usage: ./treasure_manager hunt_name\n");
            exit(EXIT_FAILURE);
        }
        char treasure_path[1024];
        written = snprintf(treasure_path, sizeof(treasure_path), "%s/%s/treasure_%s.dat",cwd_N, argv[2], argv[2]);
        if (written < 0 || written >= sizeof(treasure_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }
        char hunt_path[1024];
        written = snprintf(hunt_path, sizeof(hunt_path), "%s/%s",cwd_N, argv[2]);
        if (written < 0 || written >= sizeof(hunt_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }

        char log_path[1024];
        written = snprintf(log_path, sizeof(log_path), "%s/%s/logged_hunt.txt",cwd_N, argv[2]);
        if (written < 0 || written >= sizeof(log_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }
		remove_hunt(cwd,hunt_path,treasure_path,log_path,argv[2]);
	}
    else if (strcmp(argv[1], "--list_hunts") == 0 ){
        list_hunts(cwd_N);
    }
    else{
        printf("Unknown command\n");
    }
	
	return 0;
}
