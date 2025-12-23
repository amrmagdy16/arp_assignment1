#include "../include/handlers.h"

// PROFESSOR'S RESIZE HELPER
void handle_resize() {
    resize_term(0, 0); 
    clear(); 
    refresh();
}

int main(int argc, char *argv[]) {
    log_process_pid("Input Window");

    int fd_tx = atoi(argv[1]);
    int fd_rx = atoi(argv[2]);
    fcntl(fd_rx, F_SETFL, O_NONBLOCK);

    setlocale(LC_ALL, "");
    initscr(); 
    cbreak(); 
    noecho(); 
    keypad(stdscr, TRUE); // REQUIRED for KEY_RESIZE
    timeout(100);

    Game g = {0};
    Drone d = {0};

    while(1) {
        // Redraw UI based on current COLS/LINES
        werase(stdscr);
        box(stdscr, 0, 0);
        
        int cx = COLS / 2;
        int cy = LINES / 2;

        mvprintw(1, cx - 10, "DRONE CONTROLS");
        mvprintw(cy - 2, cx - 12, "W E R   (Up-L, Up, Up-R)");
        mvprintw(cy - 1, cx - 12, "S D F   (Left, Brake, Right)");
        mvprintw(cy,     cx - 12, "X C V   (Dn-L, Down, Dn-R)");
        mvprintw(LINES - 2, cx - 4, "Q: Quit");

        mvprintw(cy + 4, cx - 15, "Pos: %.2f, %.2f", d.x, d.y);
        mvprintw(cy + 5, cx - 15, "Vel: %.2f, %.2f", d.vx, d.vy);
        mvprintw(cy + 6, cx - 15, "Score: %.0f", d.actual_score);
        
        refresh();

        int ch = getch();
        
        // --- PROFESSOR'S RESIZE CHECK ---
        if (ch == KEY_RESIZE) {
            handle_resize();
            continue; // Skip processing this key as a command
        }

        if (ch != ERR) {
            g.command = ch;
            if(ch == 'q') exit(0);
            write(fd_tx, &g, sizeof(Game));
        }

        if(read(fd_rx, &d, sizeof(Drone)) > 0) {
            // Data updated, loop will redraw
        }
        log_execution("log/input_window_log.txt");
    }
    endwin();
    return 0;
}
