#include "../include/handlers.h"
int main(int argc, char *argv[]) {
    log_process_pid("Target Gen"); // COMPLIANCE
    int fd = atoi(argv[1]);
    srand(time(NULL)+getpid());
    Target tar;
    time_t last_gen_time = 0;
    while(1) {
        time_t current_time = time(NULL);
        if (current_time - last_gen_time >= 30) {
            for(int i=0; i<MAX_TARGETS; i++) {
                tar.x[i] = rand() % (COLS_WIN-6)+3;
                tar.y[i] = rand() % (LINES_WIN-6)+3;
            }
            write(fd, &tar, sizeof(Target));
            last_gen_time = current_time;
        }
        log_execution("log/target_log.txt");
        sleep(1);
    }
    return 0;
}
