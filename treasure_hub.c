#include "header.h"
#include "func_impl.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int monitor_status = 0;
int monitor_pid = -1;

void monitor_proc(){
    
}

void start_monitor(){
    if( monitor_pid == -1 ){
        printf("ERR:Monitor is already running\n");
        return;
    }

    monitor_pid = fork();
    if( monitor_pid == 0 ){
        monitor_proc();
        exit(0);
    }
    else if(monitor_pid > 0){
        printf("Monitor started with PID: %d", monitor_pid);
    }
    else{
        perror("Failed to start monitor");
    }
}


int main(void){
    
    char cmd[101];

    while(1){
        printf(">> ");
        fflush(stdout);
        
        if(fgets(cmd, sizeof(cmd), stdin) == NULL ){
            break;
        }

        cmd[strcspn(cmd, "\n")] = 0;

        if(strcmp(cmd, "start_monitor") == 0){
            // printf("start monitor proc\n");

            int pid = fork();

            if(pid == 0 ){
                
            }

        }
        else if(strcmp(cmd, "list_hunts") == 0){
            printf("list hunts\n");
        }
        else if(strcmp(cmd,"list_treasures") == 0){
            printf("trsr\n");
        }
        else if(strcmp(cmd, "view_treasure") == 0){
            printf("viewtrs\n");
        }
        else if(strcmp(cmd, "stop_monitor") == 0){
            printf("stopmnt\n");
        }
        else if(strcmp(cmd, "exit") == 0){
            printf("se exituie\n");
            break;
        }
        else{
            printf("Unknown command\n");
        }
    }
    return 0;
}