#include "../include/handlers.h"

int pipes[6][2]; 
int pids[6]; 

void cleanup(int sig) {
    for(int i=0; i<6; i++) kill(pids[i], SIGTERM);
    exit(0);
}

int main() {
    signal(SIGINT, cleanup);
    
    // Reset the PID file
    FILE *f = fopen("log/processes_pids.txt", "w"); 
    if(f) fclose(f);
    
    log_process_pid("Blackboard"); // COMPLIANCE

    for(int i=0; i<6; i++) { 
        if(pipe(pipes[i]) == -1) { perror("Pipe failed"); exit(1); }
    }

    char fd_strs[12][10];
    for(int i=0; i<6; i++) {
        sprintf(fd_strs[i*2], "%d", pipes[i][0]);
        sprintf(fd_strs[i*2+1], "%d", pipes[i][1]);
    }

    // Spawn and Log
    char *args_server[] = {"./server", fd_strs[0], fd_strs[4], fd_strs[6], fd_strs[10], fd_strs[3], fd_strs[9], NULL};
    if((pids[0]=fork()) == 0) execv("./server", args_server);

    char *args_input[] = {"konsole", "-e", "./input_window", fd_strs[1], fd_strs[2], NULL};
    if((pids[1]=fork()) == 0) execvp("konsole", args_input);

    char *args_output[] = {"konsole", "-e", "./output_window", fd_strs[8], fd_strs[11], NULL};
    if((pids[2]=fork()) == 0) execvp("konsole", args_output);

    char *args_obs[] = {"./obstacle", fd_strs[5], NULL};
    if((pids[3]=fork()) == 0) execv("./obstacle", args_obs);

    char *args_tar[] = {"./target", fd_strs[7], NULL};
    if((pids[4]=fork()) == 0) execv("./target", args_tar);

    char *args_wd[] = {"konsole", "-e", "./watchdog", NULL};
    if((pids[5]=fork()) == 0) execvp("konsole", args_wd);
    
    while(1) sleep(10);
    return 0;
}
