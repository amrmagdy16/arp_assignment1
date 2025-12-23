#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/file.h> // For flock
#include <signal.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ncurses.h>
#include <locale.h>
#include <errno.h>
#include "common.h"

#define COLS_WIN 80
#define LINES_WIN 25

// Logging
void log_execution(const char *log_file);
void log_process_pid(const char *process_name);
void log_error_custom(const char *msg); 

// --- FIX [Debug]: Systematic Debug Prototype ---
void log_debug_system(const char *format, ...); 

// Signals & Setup
void setup_child_signals(); 
void check_args_for_help(int argc, char *argv[], const char *usage_msg); 

// Pipe Helpers
void close_all_pipes_except(int pipes[][2], int num_pipes, int *keep_fds, int keep_count);

#endif
