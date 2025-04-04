#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


typedef struct{
	int id;
	char username[100];
	float lat;
	float longi;
	char clue[200];
	int value;
}treasure;

treasure add_info(treasure *t, int id, const char *username, float lat, float longi, const char *clue, int value){
	
	(*t).id = id;
	strcpy((*t).username, username);
	(*t).lat = lat;
	(*t).longi = longi;
	strcpy((*t).clue,clue);
	(*t).value = value;
    return *t;
}
/*  Structura Fisierelor

Hunts/
├── Hunt1/
│   ├── file1.txt
│   └── file2.txt
├── Hunt2/
│   └── file3.txt
└── Hunt3/
    └── file4.txt


*/


int main(int argc, char **argv){
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
    if(getcwd(cwd,sizeof(cwd)) == NULL ){
        perror("cwd failed");
        exit(EXIT_FAILURE);
    }
    char cwd_N[1024];
    snprintf(cwd_N, sizeof(cwd_N), "%s%s", cwd, "/Hunts/");
    printf("%s\n",cwd_N);
	if( strcmp(argv[1], "--add") == 0){
        char dir_path[1024];
		snprintf(dir_path, sizeof(dir_path), "%s%s", cwd_N, argv[2]);

		if( !stat(dir_path, &st) ){
			if( S_ISDIR(st.st_mode) ){
				
			}
		}
        else{
            if( mkdir(dir_path, 0777) == -1){
                perror("mkdir failed");
                exit(EXIT_FAILURE);
            }
        }
	}
	else if( strcmp(argv[1], "--list") == 0){
		printf("list\n");
	}
	else if( strcmp(argv[1], "--view") == 0){
		printf("list\n");
	}
	else if( strcmp(argv[1], "--remove_treasure") == 0){
		printf("list\n");
	}
	else if( strcmp(argv[1], "--remove_hunt") == 0){
		printf("list\n");
	}
	
	return 0;
}