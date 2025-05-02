#define _GNU_SOURCE
#include "tr_manager_hdr.h"
#include "func_impl_tr_manager.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "tr_hub_hdr.h"

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


void stop_monitor() {
    if (monitor_running == 0) {
        printf("ERR: Monitor is not running\n");
        return;
    }
    printf("Monitor will be stopped...\n");
    usleep(5000000);
    kill(monitor_pid, SIGTERM);
    monitor_stopping = 1;
    
    while( monitor_pid != -1 ){
        char cmd[31];
        if(scanf("%s", cmd) == 1){
            printf("Command <%s> couldn't be processed while monitor is stopping\n", cmd);
        }
    }

    int status;
    waitpid(monitor_pid,&status, 0);

    if (WIFEXITED(status)) {
        printf("Monitor exited with status %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Monitor terminated by signal %d\n", WTERMSIG(status));
    } else {
        printf("Monitor exited abnormally\n");
    }
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


void handle_sigchld(int sig)
{
    if(monitor_stopping)
    {
        monitor_pid=-1;
        // printf("Monitor stopped\n");

        monitor_stopping=0;
    }
}