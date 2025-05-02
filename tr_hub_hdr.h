#ifndef HEADER_H
#define HEADER_H


void start_monitor();
void write_arg(const char *arg);
void send_sig();
void stop_monitor();
void handle_sigusr1(int sig);
void monitor_proc();
void handle_sigchld();



#endif
