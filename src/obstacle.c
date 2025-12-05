#include "../include/handlers.h"
int main(int argc, char *argv[]) {
    check_args_for_help(argc, argv, "Generates random obstacles periodically.");
    setup_child_signals();

    if (argc < 2) { fprintf(stderr, "Obstacle: Missing FD\n"); exit(1); }
    
    int fd = atoi(argv[1]);
    srand(time(NULL)^getpid());
    Obstacle obs;
    time_t last_gen_time = 0;
    while(1) {
        time_t current_time = time(NULL);
        if (current_time - last_gen_time >= 20) {
            for(int i=0; i<MAX_OBSTACLES; i++) {
                obs.x[i] = rand() % (COLS_WIN-6)+3;
                obs.y[i] = rand() % (LINES_WIN-6)+3;
            }
            write(fd, &obs, sizeof(Obstacle));
            last_gen_time = current_time;
        }
        // No log execution
        sleep(1); 
    }
    return 0;
}
