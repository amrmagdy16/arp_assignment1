#include "../include/handlers.h"
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    log_process_pid("Watchdog"); // COMPLIANCE
    initscr(); curs_set(0); 
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); 
    init_pair(2, COLOR_RED, COLOR_BLACK);   

    const char *logs[] = {"log/server_log.txt", "log/input_window_log.txt", "log/output_window_log.txt", "log/obstacle_log.txt", "log/target_log.txt"};
    const char *names[] = {"Server", "Input", "Output", "Obstacle", "Target"};
    int num_logs = 5;

    while(1) {
        clear();
        box(stdscr, 0, 0);
        mvprintw(1, 2, "WATCHDOG MONITOR");
        mvhline(2, 1, ACS_HLINE, 30);

        time_t now = time(NULL);
        
        for(int i=0; i<num_logs; i++) {
            struct stat st;
            if (stat(logs[i], &st) == 0) {
                double diff = difftime(now, st.st_mtime);
                mvprintw(4+i, 2, "%-10s: ", names[i]);
                if (diff <= 2.0) {
                    attron(COLOR_PAIR(1));
                    printw("[OK]");
                    attroff(COLOR_PAIR(1));
                } else {
                    attron(COLOR_PAIR(2));
                    printw("[ALERT] %.0fs", diff);
                    attroff(COLOR_PAIR(2));
                }
            } else {
                mvprintw(4+i, 2, "%-10s: [NO LOG]", names[i]);
            }
        }
        refresh();
        sleep(1);
    }
    endwin();
    return 0;
}
