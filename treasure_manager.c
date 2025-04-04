#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


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
}

int main(int argc, char **argv){
	if( strcmp(argv[1], "--add") == 0){
		struct stat st;
		if( !stat(argv[2], &st) ){
			if( S_ISDIR(st.st_mode) ){
				
			}
		}
		printf("add\n");
		if( )
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