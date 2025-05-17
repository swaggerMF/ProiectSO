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

void handle_sigchld(int sig)
{
    if(monitor_stopping)
    {
        monitor_pid=-1;
        monitor_stopping=0;
        printf("Monitor stopped\n");
    }
}


void stop_monitor() {
    struct sigaction sigchld;
    sigchld.sa_handler = handle_sigchld;
    sigchld.sa_flags=0;
    sigemptyset(&sigchld.sa_mask);
    if((sigaction(SIGCHLD, &sigchld, NULL))==-1)
    {
        perror("Error sigaction");
        exit(EXIT_FAILURE);   
    }

    if (monitor_running == 0) {
        printf("ERR: Monitor is not running\n");
        return;
    }
    monitor_stopping = 1;
    printf("Monitor will be stopped...\n");
    int pid = fork();
    if( pid == 0 ){
        usleep(5000000);
        kill(monitor_pid, SIGTERM);
        exit(0);
    }
    else if(pid > 0){
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
    
}

void handle_sigusr1(int sig){
    FILE *f = fopen("cmds.txt", "r");
    if(!f){
        perror("error opening file");
        exit(EXIT_FAILURE);
    }
    int calc_score = 0;
    char args[31];
    char *v_arg[6];
    for (int idx = 0; idx < 6; idx++) {
    v_arg[idx] = NULL;
}
    int i = 0;
    v_arg[i] = malloc(strlen("treasure_manager") + 1);
    if( v_arg[i] == NULL ){
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    strcpy(v_arg[i++], "treasure_manager");
    while(fgets(args,sizeof(args),f) != NULL){
        args[strcspn(args, "\n")] = 0;
        if(strcmp(args,"calc") == 0){
            calc_score =1;
        }
        else{
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
    }   
    v_arg[i] = NULL;
    fclose(f); 

    

    if(!calc_score){
        int pfd[2];
        if(pipe(pfd) < 0 ){
            perror("Pipe creation failed\n");
            exit(EXIT_FAILURE);
        }
        int pid = fork();
        if(pid == 0 ){

            close(pfd[0]);
            if(dup2(pfd[1], 1) < 0){
                perror("dup2 error\n");
                exit(EXIT_FAILURE);
            }

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
            FILE *stream;
            stream = fdopen(pfd[0], "r");
            close(pfd[1]);
            if( stream == NULL ){
                perror("fdopen error\n");
                exit(EXIT_FAILURE);
            }
            char string[1024];
            printf("Output din proces parinte cu pipe:\n");
            while( fgets(string,sizeof(string), stream) != NULL){
                printf("%s", string);
            }
            close(pfd[0]);
            int status;
            waitpid(pid,&status, 0);  
        }
        else{
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; v_arg[i] != NULL; i++) {
            free(v_arg[i]);
        }
    }
    else{
        char exec_path[1024];
        int written = snprintf(exec_path, sizeof(exec_path), "%s/calc_score",v_arg[1]);
        if (written < 0 || written >= sizeof(exec_path)) {
            perror("Path too long\n");
            exit(EXIT_FAILURE);
        }
        strcpy(v_arg[0], "calc_score");
        DIR *d;
        struct dirent *dir;
        if((d = opendir(v_arg[2])) == NULL ){
            perror("Error opening directory\n");
            exit(EXIT_FAILURE);
        }
        
        while((dir = readdir(d)) != NULL ){
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;

            struct stat st;
            char hunt_path[1024];
            int written = snprintf(hunt_path, sizeof(hunt_path), "%s%s",v_arg[2] , dir->d_name); //path catre un hunt specific
            if (written < 0 || written >= sizeof(hunt_path)) {
                perror("Path too long\n");
                exit(EXIT_FAILURE);
            }
            if(stat(hunt_path,&st) == -1){
                perror("stat failed\n");
                exit(EXIT_FAILURE);
            }
            if(S_ISDIR(st.st_mode)){
                 int pfd[2];
                if(pipe(pfd) < 0 ){
                    perror("Pipe creation failed\n");
                    exit(EXIT_FAILURE);
                }
                int pid = fork();
                if( pid == 0 ){
                    close(pfd[0]);
                    if(dup2(pfd[1], 1) < 0){
                        perror("dup2 error\n");
                        exit(EXIT_FAILURE);
                    }
                    printf("%s: \n", dir->d_name);
                    char treasure_path[1024];
                    written = snprintf(treasure_path, sizeof(treasure_path), "%s/treasure_%s.dat", hunt_path,dir->d_name); //path catre un lista cu treasureuri
                    if (written < 0 || written >= sizeof(hunt_path)) {
                        perror("Path too long\n");
                        exit(EXIT_FAILURE);
                    }
                    v_arg[3] = malloc(strlen(treasure_path) + 1);
                    if (!v_arg[3]) {
                        perror("malloc failed");
                        exit(EXIT_FAILURE);
                    }
                    strcpy(v_arg[3],treasure_path);
                    execvp(exec_path, v_arg);
                    perror("eroare la exec");
                    exit(EXIT_FAILURE);
                }
                else if(pid > 0){
                    FILE *stream;
                    stream = fdopen(pfd[0], "r");
                    close(pfd[1]);
                    if( stream == NULL ){
                        perror("fdopen error\n");
                        exit(EXIT_FAILURE);
                    }
                    char string[1024];
                    printf("\n\nOutput din proces parinte cu pipe pt calc:\n");
                    while( fgets(string,sizeof(string), stream) != NULL){
                        printf("%s", string);
                    }
                    close(pfd[0]);
                    int status;
                    waitpid(pid,&status, 0);
                }
            } 
        }
        closedir(d);
    } 
}

void monitor_proc(){
    struct sigaction sa_sigusr1;
    sa_sigusr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_sigusr1.sa_mask); 
    sa_sigusr1.sa_flags = 0; 

    sigaction(SIGUSR1, &sa_sigusr1, NULL);

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


