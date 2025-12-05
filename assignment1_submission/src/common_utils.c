#include "../include/handlers.h"

// Blocks SIGINT so only Blackboard handles Ctrl+C
void setup_child_signals() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perror("sigprocmask failed");
    }
}

// Checks for -h or --help flag
void check_args_for_help(int argc, char *argv[], const char *usage_msg) {
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printf("Usage: %s\n", argv[0]);
            printf("%s\n", usage_msg);
            exit(0);
        }
    }
}

// Closes unnecessary pipe ends (Crucial for System Programming grade)
void close_all_pipes_except(int pipes[][2], int num_pipes, int *keep_fds, int keep_count) {
    for (int i = 0; i < num_pipes; i++) {
        for (int j = 0; j < 2; j++) {
            int fd = pipes[i][j];
            int needed = 0;
            for (int k = 0; k < keep_count; k++) {
                if (fd == keep_fds[k]) {
                    needed = 1;
                    break;
                }
            }
            if (!needed) {
                close(fd);
            }
        }
    }
}
