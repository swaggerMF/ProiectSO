#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>  
#include <time.h>


typedef struct{
	int id;
	char username[100];
	float longi;
    float lat;
	char clue[200];
	int value;
}treasure;

treasure add_info(){
	treasure t;
    printf("ID: ");
    scanf("%d", &t.id);
	
    printf("Username: ");
    scanf("%99s", t.username);

    printf("Longitudine: ");
    scanf("%f", &t.longi);

    printf("Latitudine: ");
    scanf("%f", &t.lat);

    printf("Clue: ");
    getchar();
    fgets(t.clue, 199,stdin);
    t.clue[strcspn(t.clue, "\n")] = 0;

    printf("Value: ");
    // getchar();
    scanf("%d", &t.value);

    return t;
}

void add_treasure(char *path, char *arg){
    char treasure_path[1024];
    snprintf(treasure_path, sizeof(treasure_path), "%s/treasure_%s.dat", path,arg);

    int f = open(treasure_path, O_WRONLY | O_CREAT | O_APPEND, 0777);
    if( f == -1){
        perror("failed to open file");
        exit(EXIT_FAILURE);
    }

    treasure trs = add_info();

    if( write(f, &trs, sizeof(trs)) == -1){
        perror("Unable to add treasure");
        close(f);
        exit(EXIT_FAILURE);
    }
    else{
        printf("Treasure added succesfully\n");
    }
    close(f);
}


void print_treasure(char* hunt_path){
    int f = open(hunt_path, O_RDONLY);
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

void list(char* path, char* arg){
    char hunt_path[1024];
    snprintf(hunt_path, sizeof(hunt_path), "%s/%s/treasure_%s.dat", path,arg,arg);

    printf("Hunt name:%s\n", arg);
    struct stat st;
    if(stat(hunt_path, &st)){
        perror("stat failed,hunt probably doesn't exist");
        exit(EXIT_FAILURE);
    }
    else{
        printf("File size: %ld\n", st.st_size);
        printf("Last modified: %s\n", ctime(&st.st_mtime));
    }
    print_treasure(hunt_path);
}

void view(char *path, char *hunt_id, int treasure_id){
    char hunt_path[1024];
    snprintf(hunt_path, sizeof(hunt_path), "%s/%s/treasure_%s.dat", path,hunt_id,hunt_id);

    int f = open(hunt_path, O_RDONLY);
    if( f == -1 ){
        perror("failed to open file\n");
        close(f);
        exit(EXIT_FAILURE);
    }

    treasure t;
    int nr = 1;
    while( read(f,&t,sizeof(treasure))){
        nr++;
        if( t.id == treasure_id ){
            printf("Treasure no.%d\n", nr);
            printf("ID: %d\n", t.id);
            printf("Username: %s\n", t.username);
            printf("GPS Longitude: %f\n", t.longi);
            printf("GPS Latitude: %f\n", t.lat);
            printf("Clue: %s\n", t.clue);
            printf("Value: %d\n\n\n",t.value);
        }
    }
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
    if(getcwd(cwd,sizeof(cwd)) == NULL ){ //Obtinem current working directory
        perror("cwd failed");
        exit(EXIT_FAILURE);
    }

    char cwd_N[1024];
    snprintf(cwd_N, sizeof(cwd_N), "%s%s", cwd, "/Hunts/"); //Concatenam Hunts la cwd 
    // printf("%s\n",cwd_N);

	if( strcmp(argv[1], "--add") == 0){
        char dir_path[1024];
		snprintf(dir_path, sizeof(dir_path), "%s%s", cwd_N, argv[2]); //Concatenam directorul dat ca argument la path

		if( !stat(dir_path, &st) ){ //verificam daca exista hunt ul specificat in argument
			if( S_ISDIR(st.st_mode) ){ //daca exista adaugam date despre treasure
				add_treasure(dir_path,argv[2]);
			}
		}
        else{
            if( mkdir(dir_path, 0777) == -1){ //daca nu exista se creaza un nou director cu numele hunt
                perror("mkdir failed");
                exit(EXIT_FAILURE);
            }
            add_treasure(dir_path,argv[2]);
        }
	}

    
	else if( strcmp(argv[1], "--list") == 0){
		list(cwd_N, argv[2]);
	}
	else if( strcmp(argv[1], "--view") == 0){
        int t_id = atoi(argv[3]);
		view(cwd_N, argv[2],t_id);
	}
	else if( strcmp(argv[1], "--remove_treasure") == 0){
		printf("list\n");
	}
	else if( strcmp(argv[1], "--remove_hunt") == 0){
		printf("list\n");
	}
	
	return 0;
}