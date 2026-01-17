#include "../include/handlers.h"
#include <stdarg.h> // For variadic debug logging

// --- FIX [Debug]: Systematic Debug Logger Implementation ---
void log_debug_system(const char *format, ...) {
    FILE *dbg_fp = fopen("log/debug_log.txt", "a");
    if (!dbg_fp) return;

    int fd = fileno(dbg_fp);
    flock(fd, LOCK_EX); // Proper file locking as per mask

    va_list args;
    va_start(args, format);
    
    time_t now = time(NULL);
    fprintf(dbg_fp, "[DEBUG] PID: %d | Time: %ld | ", getpid(), now);
    vfprintf(dbg_fp, format, args);
    fprintf(dbg_fp, "\n");
    
    va_end(args);
    fflush(dbg_fp);
    flock(fd, LOCK_UN);
    fclose(dbg_fp);
}

void setup_child_signals() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perror("sigprocmask failed");
    }
}

void check_args_for_help(int argc, char *argv[], const char *usage_msg) {
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printf("Usage: %s\n", argv[0]);
            printf("%s\n", usage_msg);
            exit(0);
        }
    }
}

void close_all_pipes_except(int pipes[][2], int num_pipes, int *keep_fds, int keep_count) {
    for (int i = 0; i < num_pipes; i++) {
        for (int j = 0; j < 2; j++) {
            int fd = pipes[i][j];
            int needed = 0;
            for (int k = 0; k < keep_count; k++) {
                if (fd == keep_fds[k]) { needed = 1; break; }
            }
            if (!needed) close(fd);
        }
    }
}

void log_execution(const char *log_file) {
    FILE *log_fp = fopen(log_file, "a");
    if (log_fp == NULL) return;
    int fd = fileno(log_fp);
    flock(fd, LOCK_EX); 
    time_t now = time(NULL);
    fprintf(log_fp, "PID: %d, Time: %ld\n", getpid(), now);
    fflush(log_fp);
    flock(fd, LOCK_UN);
    fclose(log_fp);
}

void log_process_pid(const char *process_name) {
    FILE *pid_fp = fopen("log/processes_pids.txt", "a");
    if (pid_fp == NULL) return;
    int fd = fileno(pid_fp);
    flock(fd, LOCK_EX);
    fprintf(pid_fp, "Process: %-15s | PID: %d\n", process_name, getpid());
    fflush(pid_fp);
    flock(fd, LOCK_UN);
    fclose(pid_fp);
}

void log_error_custom(const char *msg) {
    perror(msg); 
    FILE *err_fp = fopen("log/error_log.txt", "a");
    if (err_fp) {
        int fd = fileno(err_fp);
        flock(fd, LOCK_EX);
        time_t now = time(NULL);
        fprintf(err_fp, "[ERROR] PID: %d | Time: %ld | Msg: %s | Errno: %s\n", 
                getpid(), now, msg, strerror(errno));
        fflush(err_fp);
        flock(fd, LOCK_UN);
        fclose(err_fp);
    }
    exit(EXIT_FAILURE);
}
