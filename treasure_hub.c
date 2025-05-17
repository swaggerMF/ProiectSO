#include "func_impl_tr_hub.c"

int main(void){
    char cmd[101];
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
            if( unlink("cmds.txt") == -1 && errno != ENOENT ){
                perror("file delete failed");
                exit(EXIT_FAILURE);
            }
            char cwd[1024];
            getcwd(cwd,sizeof(cwd));
            char cwd_N[1024];
            int written = snprintf(cwd_N, sizeof(cwd_N), "%s%s", cwd, "/Hunts/"); //Concatenam Hunts la cwd si vom avea cwd_N
            if (written < 0 || written >= sizeof(cwd_N)) {
                perror("Path too long\n");
                exit(EXIT_FAILURE);
            }
            write_arg("calc");
            write_arg(cwd);
            write_arg(cwd_N);
            send_sig();
        }
        else{
            printf("Unknown command\n");
        }
    }
    return 0;
}