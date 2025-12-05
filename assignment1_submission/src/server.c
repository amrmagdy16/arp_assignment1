#include "../include/handlers.h"

int main(int argc, char *argv[]) {
    check_args_for_help(argc, argv, "Coordinates data between Input, Output, Obstacle, and Target.");
    setup_child_signals();

    if (argc < 7) { fprintf(stderr, "Server: Insufficient args\n"); exit(1); }

    int fd_in_rx  = atoi(argv[1]);
    int fd_obs_rx = atoi(argv[2]);
    int fd_tar_rx = atoi(argv[3]);
    int fd_out_rx = atoi(argv[4]);
    int fd_in_tx  = atoi(argv[5]);
    int fd_out_tx = atoi(argv[6]);

    fd_set read_fds;
    Game game = {0};
    game.next_target_id = 0;

    Game temp_in = {0};
    Obstacle obs = {0};
    Target tar = {0};
    Drone drone = {0};
    int max_fd = 1024;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(fd_in_rx, &read_fds);
        FD_SET(fd_obs_rx, &read_fds);
        FD_SET(fd_tar_rx, &read_fds);
        FD_SET(fd_out_rx, &read_fds);

        struct timeval tv = {0, 10000};
        int res = select(max_fd, &read_fds, NULL, NULL, &tv);
        if (res < 0) continue; 

        // 1. Read Obstacles
        if (FD_ISSET(fd_obs_rx, &read_fds)) {
            if(read(fd_obs_rx, &obs, sizeof(Obstacle)) > 0) {
                memcpy(game.Obstacle_x, obs.x, sizeof(obs.x));
                memcpy(game.Obstacle_y, obs.y, sizeof(obs.y));
                game.game_update = 1;
                write(fd_out_tx, &game, sizeof(Game));
            }
        }
        // 2. Read Targets
        if (FD_ISSET(fd_tar_rx, &read_fds)) {
            if(read(fd_tar_rx, &tar, sizeof(Target)) > 0) {
                memcpy(game.Target_x, tar.x, sizeof(tar.x));
                memcpy(game.Target_y, tar.y, sizeof(tar.y));
                game.game_update = 2;
                write(fd_out_tx, &game, sizeof(Game));
            }
        }
        // 3. Read Input Commands
        if (FD_ISSET(fd_in_rx, &read_fds)) {
            if(read(fd_in_rx, &temp_in, sizeof(Game)) > 0) {
                game.command = temp_in.command;
                write(fd_out_tx, &game, sizeof(Game));
            }
        }
        // 4. Read Drone Logic (Physics feedback)
        if (FD_ISSET(fd_out_rx, &read_fds)) {
            if(read(fd_out_rx, &drone, sizeof(Drone)) > 0) {
                if (drone.hit_target_index != -1) {
                    int idx = drone.hit_target_index;
                    game.Target_x[idx] = -100; // Hide hit target
                    game.Target_y[idx] = -100;
                    game.next_target_id++;     
                    game.game_update = 3; 
                    write(fd_out_tx, &game, sizeof(Game));
                }
                write(fd_in_tx, &drone, sizeof(Drone)); // Send to Input for telemetry
            }
        }
    }
    return 0;
}
