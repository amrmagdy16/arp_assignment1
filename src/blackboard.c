#include "../include/handlers.h"

// Master Process (Launcher)
// Spawns: Server, Input, Output, Obstacle, Target
// Excluded: Watchdog

int pipes[5][2]; // Reduced to 5 pipes (Removed Watchdog pipes)
int pids[5];     // Reduced to 5 processes

void cleanup(int sig) {
    (void)sig;
    for(int i=0; i<5; i++) {
        if(pids[i] > 0) kill(pids[i], SIGTERM);
    }
    while(wait(NULL) > 0);
    exit(0);
}

int main(int argc, char *argv[]) {
    check_args_for_help(argc, argv, "Master process. Spawns Server, GUI, and Logic components.");
    signal(SIGINT, cleanup);
    
    // Create Pipes
    for(int i=0; i<5; i++) { 
        if(pipe(pipes[i]) == -1) { perror("Pipe failed"); exit(1); }
    }

    char fd_strs[10][10];
    for(int i=0; i<5; i++) {
        sprintf(fd_strs[i*2], "%d", pipes[i][0]);   // Read
        sprintf(fd_strs[i*2+1], "%d", pipes[i][1]); // Write
    }

    // PIPE MAP:
    // 0: Server Read  <-- Input Write
    // 1: Input Read   <-- Server Write (Feedback)
    // 2: Server Read  <-- Obstacle Write
    // 3: Server Read  <-- Target Write
    // 4: Output Read  <-- Server Write (Game State)
    // Note: Output -> Server is handled via Pipe 0 logic usually, but let's check original.
    // Original had 6 pipes. We need to ensure we have enough.
    // Let's stick to the original mapping logic but remove WD.
    // P0: Input -> Server
    // P1: Server -> Input
    // P2: Obstacle -> Server
    // P3: Target -> Server
    // P4: Server -> Output
    // P5: Output -> Server (Needed for Drone Position!) -> Wait, original used 6.
    
    // RE-MAPPING for clarity:
    // Pipe 0: Input -> Server
    // Pipe 1: Server -> Input
    // Pipe 2: Obstacle -> Server
    // Pipe 3: Target -> Server
    // Pipe 4: Server -> Output (Game State)
    // Pipe 5: Output -> Server (Drone State) -> WE NEED 6 PIPES for full duplex Output.
    
    // Correction: We need 6 pipes.
    // pipes[5] was WD previously? No, let's look at original server args:
    // server args: fd_in_rx, fd_obs_rx, fd_tar_rx, fd_out_rx, fd_in_tx, fd_out_tx
    // We need:
    // 1. Input -> Server
    // 2. Obs -> Server
    // 3. Tar -> Server
    // 4. Output -> Server
    // 5. Server -> Input
    // 6. Server -> Output
    
    // Re-allocating to 6 pipes to be safe, even if WD is gone.
    int real_pipes[6][2];
    for(int i=0; i<6; i++) pipe(real_pipes[i]);
    
    char fds[12][10];
    for(int i=0; i<6; i++) {
        sprintf(fds[i*2], "%d", real_pipes[i][0]);
        sprintf(fds[i*2+1], "%d", real_pipes[i][1]);
    }

    // 1. Server
    // Rx: Input(P0), Obs(P2), Tar(P3), Out(P4)
    // Tx: Input(P1), Out(P5)
    char *args_server[] = {"./server", fds[0], fds[4], fds[6], fds[8], fds[3], fds[11], NULL};
    if((pids[0]=fork()) == 0) {
        int keep[] = {real_pipes[0][0], real_pipes[2][0], real_pipes[3][0], real_pipes[4][0], 
                      real_pipes[1][1], real_pipes[5][1]};
        close_all_pipes_except(real_pipes, 6, keep, 6);
        execv("./server", args_server);
    }

    // 2. Input Window
    // Tx: Server(P0), Rx: Server(P1)
    char *args_input[] = {"konsole", "-e", "./input_window", fds[1], fds[2], NULL};
    if((pids[1]=fork()) == 0) {
        int keep[] = {real_pipes[0][1], real_pipes[1][0]};
        close_all_pipes_except(real_pipes, 6, keep, 2);
        execvp("konsole", args_input);
    }

    // 3. Output Window
    // Rx: Server(P5), Tx: Server(P4 is ReadEnd, so Output writes to P4-Write? No, Server writes to Output)
    // Server TX is P5-Write (fds[11]). Output Rx is P5-Read (fds[10]).
    // Output TX is P4-Write (fds[9]). Server Rx is P4-Read (fds[8]).
    char *args_output[] = {"konsole", "-e", "./output_window", fds[10], fds[9], NULL};
    if((pids[2]=fork()) == 0) {
        int keep[] = {real_pipes[5][0], real_pipes[4][1]};
        close_all_pipes_except(real_pipes, 6, keep, 2);
        execvp("konsole", args_output);
    }

    // 4. Obstacle
    // Tx: Server(P2)
    char *args_obs[] = {"./obstacle", fds[5], NULL};
    if((pids[3]=fork()) == 0) {
        int keep[] = {real_pipes[2][1]};
        close_all_pipes_except(real_pipes, 6, keep, 1);
        execv("./obstacle", args_obs);
    }

    // 5. Target
    // Tx: Server(P3)
    char *args_tar[] = {"./target", fds[7], NULL};
    if((pids[4]=fork()) == 0) {
        int keep[] = {real_pipes[3][1]};
        close_all_pipes_except(real_pipes, 6, keep, 1);
        execv("./target", args_tar);
    }

    close_all_pipes_except(real_pipes, 6, NULL, 0);
    
    while(1) sleep(10);
    return 0;
}
