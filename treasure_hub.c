#include "func_impl_tr_hub.c"

int main(void){
    char cmd[101];

    struct sigaction sigchld;
    sigchld.sa_handler=handle_sigchld;
    sigchld.sa_flags=0;
    sigemptyset(&sigchld.sa_mask);

    if((sigaction(SIGCHLD, &sigchld, NULL))==-1)
    {
        perror("Error sigaction");
        exit(-1);   
    }

    while(1){
        usleep(10000);
        printf(">> ");
        fflush(stdout);
        
        if(fgets(cmd, sizeof(cmd), stdin) == NULL ){
            break;
        }

        cmd[strcspn(cmd, "\n")] = 0;
        if(strcmp(cmd, "start_monitor") == 0){
            start_monitor();
        }
        else if(strcmp(cmd, "list_hunts") == 0){
            if( monitor_running == 0){
                printf("Monitor not running. Please use start_monitor first\n");
                continue;
            }
            if( unlink("cmds.txt") == -1 && errno != ENOENT ){
                perror("file delete failed");
                exit(EXIT_FAILURE);
            }
            write_arg("--list_hunts");
            send_sig();
            // printf("list hunts\n");
        }
        else if(strcmp(cmd,"list_treasures") == 0){
            if( monitor_running == 0){
                printf("Monitor not running. Please use start_monitor first\n");
                continue;
            }
            if( unlink("cmds.txt") == -1 && errno != ENOENT ){
                perror("file delete failed");
                exit(EXIT_FAILURE);
            }
            write_arg("--list");
            printf("Please enter the hunt you would like to be displayed: ");
            char hunt[20];
            // fgets(stdin,sizeof(hunt), hunt);
            scanf("%19s",hunt);
            getchar();
            printf("\n");
            write_arg(hunt);
            send_sig();
        }
        else if(strcmp(cmd, "view_treasure") == 0){
            if( monitor_running == 0){
                printf("Monitor not running. Please use start_monitor first\n");
                continue;
            }
            if( unlink("cmds.txt") == -1 && errno != ENOENT ){
                perror("file delete failed");
                exit(EXIT_FAILURE);
            }
            write_arg("--view");
            char arg[31];
            printf("Please enter the hunt id: ");
            scanf("%30s", arg);
            write_arg(arg);
            getchar();
            printf("\n");
            printf("Please enter the treasure_id: ");
            scanf("%30s", arg);
            printf("\n");
            getchar();
            write_arg(arg);
            send_sig();
            
        }
        else if(strcmp(cmd, "stop_monitor") == 0){
            if( unlink("cmds.txt") == -1 && errno != ENOENT ){
                perror("file delete failed");
                 exit(EXIT_FAILURE);
            }
            stop_monitor();
            monitor_running = 0;
        }
        else if(strcmp(cmd, "exit") == 0){
            if(monitor_running == 1){
                printf("ERR: Monitor still running\n");
                fflush(stdout);
            }else break;
        }
        else if(strcmp(cmd, "clear") == 0){
            system("clear");
        }
        else if(strcmp(cmd, "calculate_score") == 0){
            
        }
        else{
            printf("Unknown command\n");
        }
    }
    return 0;
}