#define _GNU_SOURCE
#include "tr_manager_hdr.h"
#include "func_impl_tr_manager.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int monitor_stopping = 0;
int monitor_running = 0;
int monitor_pid = -1;

void write_arg(const char *arg){
    FILE *f = fopen("cmds.txt", "a");
    if( f == 0 ){
        perror("failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(f,"%s\n", arg);
    fclose(f);
}

void send_sig(){
    if(monitor_pid != -1)
        kill(monitor_pid,SIGUSR1);
}

void handle_sigusr1(int sig){
    FILE *f = fopen("cmds.txt", "r");
    if(!f){
        perror("error opening file");
        exit(EXIT_FAILURE);
    }
    char args[31];
    char *v_arg[5];
    int i = 0;
    v_arg[i] = malloc(strlen("treasure_manager") + 1);
    if( v_arg[i] == NULL ){
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    strcpy(v_arg[i++], "treasure_manager");
    while(fgets(args,sizeof(args),f) != NULL){
        args[strcspn(args, "\n")] = 0;
        v_arg[i] = malloc(strlen(args) + 1);
        if( v_arg[i] == NULL ){
            perror("malloc failed");
            for( int j = i - 1; j >=0 ; j--){
                free(v_arg[j]);
            }
            exit(EXIT_FAILURE);
        }
        strcpy(v_arg[i++], args);
    }   
    v_arg[i] = NULL;
    if(strcmp(args,"stop_monitor") == 0){
        printf("Stopping monitor\n");
        fflush(stdout);
        usleep(10000000);
        exit(0);
    }

    fclose(f);

    int pid = vfork();
    if(pid == 0 ){
        char cwd[1024];
        getcwd(cwd,sizeof(cwd));
        char exec_path[1024];
        int written = snprintf(exec_path, sizeof(exec_path), "%s/treasure_manager",cwd);
        if (written < 0 || written >= sizeof(exec_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }
        execvp(exec_path,v_arg);
        perror("exec failed");
        exit(EXIT_FAILURE);
    }
    else if(pid > 0 ){
        int status;
        waitpid(pid,&status, 0);
    }
    else{
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

}

void monitor_proc(){
    struct sigaction sa_sigusr1;
    sigset_t blocked_signals, wait;

    sa_sigusr1.sa_handler = handle_sigusr1; //seteaza campul din structul sigaction cu un handler
    sigemptyset(&sa_sigusr1.sa_mask); //seteaza masca pe 0 pt a nu bloca semnale neintentionat
    sa_sigusr1.sa_flags = 0; 

    sigaction(SIGUSR1, &sa_sigusr1, NULL); //sigaction in sine, ii spuen procesului ce sa faca daca primeste semnalul SIGUSR1 prin variabila sa_sigusr1 si campurile ei

    
    sigemptyset(&blocked_signals); 
    sigaddset(&blocked_signals, SIGUSR1); //blocam SIGUSR1 pt a nu interfera cu procesul de setup
    sigprocmask(SIG_BLOCK, &blocked_signals, NULL);
    
    sigemptyset(&wait); //deblocam sigusr1 
    sigaddset(&wait, SIGUSR1);

    sigprocmask(SIG_UNBLOCK, &wait, NULL);


    while(1){
        sigsuspend(&wait);
        // pause();
    }
}

void start_monitor(){
    if( monitor_running != 0){
        printf("ERR:Monitor is already running\n");
        return;
    }
    monitor_running = 1;
    monitor_pid = fork();
    if( monitor_pid == 0 ){
        monitor_proc();
        exit(0);
    }
    else if(monitor_pid > 0){
        printf("Monitor started with PID: %d\n", monitor_pid);
    }
    else{
        perror("Failed to start monitor");
    }
}

void stop_monitor() {
    if (monitor_running == 0) {
        printf("ERR: Monitor is not running\n");
        return;
    }

    monitor_stopping=1;

    write_arg("stop_monitor");
    send_sig();

    int status;
    waitpid(monitor_pid, &status, 0);

    if (WIFEXITED(status)) {
        printf("Monitor exited with status %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Monitor terminated by signal %d\n", WTERMSIG(status));
    } else {
        printf("Monitor exited abnormally\n");
    }

    monitor_running = 0;
    monitor_pid = -1;
    monitor_stopping = 0;
}




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
            if( monitor_stopping){
                printf("ERR: Monitor is stopping. No user inputs are allowed\n");
                continue;
            }
            // printf("start monitor proc\n");
            start_monitor();
        }
        else if(strcmp(cmd, "list_hunts") == 0){
            if( monitor_stopping){
                printf("ERR: Monitor is stopping. No user inputs are allowed\n");
                continue;
            }
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
            if( monitor_stopping){
                printf("ERR: Monitor is stopping. No user inputs are allowed\n");
                continue;
            }
            if( monitor_running == 0){
                printf("Monitor not running. Please use start_monitor first\n");
                continue;
            }
            if( unlink("cmds.txt") == -1 && errno != ENOENT ){
                perror("file delete failed");
                exit(EXIT_FAILURE);
            }
            write_arg("--list");
            printf("Please enter the hunt you would like to be displayed?\n");
            char hunt[20];
            // fgets(stdin,sizeof(hunt), hunt);
            scanf("%19s",hunt);
            getchar();
            write_arg(hunt);
            send_sig();
        }
        else if(strcmp(cmd, "view_treasure") == 0){
            if( monitor_stopping){
                printf("ERR: Monitor is stopping. No user inputs are allowed\n");
                continue;
            }
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
            if( monitor_stopping){
                printf("ERR: Monitor is stopping. No user inputs are allowed\n");
                continue;
            }
            if( unlink("cmds.txt") == -1 && errno != ENOENT ){
                perror("file delete failed");
                 exit(EXIT_FAILURE);
            }
            // write_arg("stop_monitor");
            // send_sig();
            stop_monitor();
            // printf("stopmnt\n");
        }
        else if(strcmp(cmd, "exit") == 0){
            if( monitor_stopping){
                printf("ERR: Monitor is stopping. No user inputs are allowed\n");
                continue;
            }
            // printf("se exituie\n");
            if(monitor_running == 1){
                printf("ERR: Monitor still running\n");
                fflush(stdout);
            }else break;
        }
        else if(strcmp(cmd, "clear") == 0){
            system("clear");
        }
        else{
            printf("Unknown command\n");
        }
    }
    return 0;
}