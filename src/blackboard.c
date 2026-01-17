#include "../include/handlers.h"

int pipes[10][2]; // [EDIT]: Increased pipes for network routing
int pids[10]; 

void cleanup(int sig) {
    for(int i=0; i<10; i++) if(pids[i] > 0) kill(pids[i], SIGTERM);
    exit(0);
}

int main() {
    signal(SIGINT, cleanup);
    
    // Clear PID log
    FILE *f = fopen("log/processes_pids.txt", "w"); if(f) fclose(f);
    log_process_pid("Blackboard");

    // 1. Create Pipes
    for(int i=0; i<10; i++) { 
        if(pipe(pipes[i]) == -1) { perror("Pipe failed"); exit(1); }
    }

    // Convert FDs to strings for execv
    char fd_strs[20][10];
    for(int i=0; i<10; i++) {
        sprintf(fd_strs[i*2], "%d", pipes[i][0]);   // Read End
        sprintf(fd_strs[i*2+1], "%d", pipes[i][1]); // Write End
    }

    // 2. Mode Selection
    int mode;
    printf("Select Mode:\n1. Standalone\n2. Server (Network)\n3. Client (Network)\n> ");
    scanf("%d", &mode);

    char str_mode[5], str_port[10], str_ip[20];
    sprintf(str_mode, "%d", mode);
    
    if (mode == 2 || mode == 3) {
        printf("Enter Port (e.g. 5555): ");
        scanf("%s", str_port);
        if (mode == 3) {
            printf("Enter Server IP: ");
            scanf("%s", str_ip);
        }
    }

    // 3. Process Spawning
    // SERVER: Obs Input comes from Pipe[5] (Standalone) or Pipe[9] (Network)
    // SERVER: Net Output goes to Pipe[8] (Network) or -1 (Standalone)
    
    char *obs_rx_fd = (mode == 1) ? fd_strs[10] : fd_strs[18]; // Pipe 5 or 9
    char *net_tx_fd = (mode == 1) ? "-1"        : fd_strs[17]; // Pipe 8

    // Launch Server
    char *args_server[] = {"./server", fd_strs[0], obs_rx_fd, fd_strs[6], fd_strs[12], fd_strs[3], fd_strs[15], net_tx_fd, NULL};
    if((pids[0]=fork()) == 0) execv("./server", args_server);

    // Launch Input Window
    char *args_input[] = {"konsole", "-e", "./input_window", fd_strs[1], fd_strs[2], NULL};
    if((pids[1]=fork()) == 0) execvp("konsole", args_input);

    // Launch Output Window
    char *args_output[] = {"konsole", "-e", "./output_window", fd_strs[14], fd_strs[13], NULL};
    if((pids[2]=fork()) == 0) execvp("konsole", args_output);

    // Launch Components based on Mode
    if (mode == 1) {
        // [STANDALONE]: Run Generators & Watchdog
        char *args_obs[] = {"./obstacle", fd_strs[11], NULL}; // Pipe 5 Write
        if((pids[3]=fork()) == 0) execv("./obstacle", args_obs);

        char *args_tar[] = {"./target", fd_strs[7], NULL};
        if((pids[4]=fork()) == 0) execv("./target", args_tar);

        char *args_wd[] = {"konsole", "-e", "./watchdog", NULL};
        if((pids[5]=fork()) == 0) execvp("konsole", args_wd);
    } else {
        // [NETWORK]: Run Network Bridge
        // Network reads Drone from Pipe 8 (Read), writes Obstacle to Pipe 9 (Write)
        char *args_net[] = {"./network", str_mode, fd_strs[16], fd_strs[19], str_port, (mode==3?str_ip:"127.0.0.1"), NULL};
        if((pids[3]=fork()) == 0) execv("./network", args_net);
    }

    while(1) sleep(10);
    return 0;
}
