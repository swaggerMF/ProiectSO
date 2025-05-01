#include "header.h"
#include "func_impl.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int monitor_running = 0;
int monitor_pid = -1;

void write_cmd(const char *cmd, const char *arg){
    if( unlink("cmds.txt") == -1 && errno != ENOENT ){
        perror("file delete failed");
        exit(EXIT_FAILURE);
    }
    FILE *f = fopen("cmds.txt", "w");
    if( f == 0 ){
        perror("failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(f,"%s", cmd);
    if( strcmp(arg, "") != 0 ){
        fprintf(f," %s", arg);
    }
    fclose(f);
}

void send_sig(){
    if(monitor_pid != -1)
        kill(monitor_pid,SIGUSR1);
}

void handle_sigusr1(){
    FILE *f = fopen("cmds.txt", "r");
    if(!f){
        perror("error opening file");
        exit(EXIT_FAILURE);
    }
    char args[31];
    fgets(args,sizeof(args),f);

    if(strcmp(args,"stop_monitor") == 0){
        printf("Stopping monitor\n");
        fflush(stdout);
        usleep(500000);
        exit(0);
    }
    
//trebe implementat lista de argumente pt exec


    fclose(f);
    int pid = fork();
    if(pid == 0 ){
        char cwd[1024];
        getcwd(cwd,sizeof(cwd));
        char exec_path[1024];
        int written = snprintf(exec_path, sizeof(exec_path), "%s/treasure_manager",cwd);
        if (written < 0 || written >= sizeof(exec_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }
        // printf("%s %s",hunt,treas_id);
        // fflush(stdout);
        // if(strcmp (treas_id,"") == 0 ){
            // execl(exec_path,"treasure_manager" ,args,NULL);
        // }
        // else{
            execl(exec_path,"treasure_manager",,NULL);
        // }
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

    sa_sigusr1.sa_handler = handle_sigusr1; //seteaza campul din structul sigaction cu un handler
    sigemptyset(&sa_sigusr1.sa_mask); //seteaza masca pe 0 pt a nu bloca semnale neintentionat
    sa_sigusr1.sa_flags = 0; 

    sigaction(SIGUSR1, &sa_sigusr1, NULL); //sigaction in sine, ii spuen procesului ce sa faca daca primeste semnalul SIGUSR1 prin variabila sa_sigusr1 si campurile ei

    while(1){
        pause();
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

    write_cmd("stop_monitor", "");
    send_sig();

    int status;
    waitpid(monitor_pid, &status, 0);  // Wait for monitor to exit

    if (WIFEXITED(status)) {
        printf("Monitor exited with status %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Monitor terminated by signal %d\n", WTERMSIG(status));
    } else {
        printf("Monitor exited abnormally\n");
    }

    monitor_running = 0;
    monitor_pid = -1;
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
            start_monitor();
        }
        else if(strcmp(cmd, "list_hunts") == 0){
            write_cmd("--list_hunts","");
            send_sig();
            // printf("list hunts\n");
        }
        else if(strcmp(cmd,"list_treasures") == 0){
            printf("Please enter the hunt you would like to be displayed?\n");
            char hunt[20];
            // fgets(stdin,sizeof(hunt), hunt);
            scanf("%19s",hunt);
            getchar();
            write_cmd("--list", hunt);
            send_sig();
        }
        else if(strcmp(cmd, "view_treasure") == 0){
            char arg[31];
            printf("Please enter the hunt and the id of the treasure you want to view with a space in between(hunt_id treasure_id)\n");
            // scanf("%s", arg);
            fgets(arg, sizeof(arg), stdin);
            arg[strcspn(arg, "\n")] = '\0';
            getchar();
            write_cmd("--view",arg);
            send_sig();
        }
        else if(strcmp(cmd, "stop_monitor") == 0){
            // write_cmd("stop_monitor","");
            // send_sig();
            stop_monitor();
            // printf("stopmnt\n");
        }
        else if(strcmp(cmd, "exit") == 0){
            // printf("se exituie\n");
            if(monitor_running == 1){
                printf("ERR: Monitor still running\n");
                fflush(stdout);
            }else break;
        }
        else{
            printf("Unknown command\n");
        }
    }
    return 0;
}