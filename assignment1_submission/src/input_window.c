#include "../include/handlers.h"

// Italian resize logic retained for consistency
void handle_resize() {
    resize_term(0, 0); 
    clear(); 
    refresh();
}

// Helper to draw a key box
void draw_key_box(int y, int x, char key, const char *desc, int is_pressed) {
    if(is_pressed) attron(COLOR_PAIR(4) | A_BOLD); // Highlight if needed
    else attron(COLOR_PAIR(2));

    mvprintw(y, x,   ".---.");
    mvprintw(y+1, x, "| %c |", key);
    mvprintw(y+2, x, "'---'");
    
    if(is_pressed) attroff(COLOR_PAIR(4) | A_BOLD);
    else attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    mvprintw(y+1, x+6, "%s", desc);
    attroff(COLOR_PAIR(3));
}

int main(int argc, char *argv[]) {
    check_args_for_help(argc, argv, "Keyboard input handler.");
    setup_child_signals();

    int fd_tx = atoi(argv[1]);
    int fd_rx = atoi(argv[2]);
    fcntl(fd_rx, F_SETFL, O_NONBLOCK);

    setlocale(LC_ALL, "");
    initscr(); 
    cbreak(); 
    noecho(); 
    keypad(stdscr, TRUE); 
    timeout(100); // 100ms refresh for UI responsiveness

    // Make it look attractive!
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);   // Title
        init_pair(2, COLOR_WHITE, COLOR_BLUE);   // Key Box
        init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Descriptions
        init_pair(4, COLOR_BLACK, COLOR_GREEN);  // Pressed Effect (optional logic)
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);// Border
    }

    Game g = {0};
    Drone d = {0};
    int last_cmd = 0;

    while(1) {
        werase(stdscr);
        
        // Fancy Border
        attron(COLOR_PAIR(5));
        box(stdscr, 0, 0);
        mvprintw(0, 2, "[ DRONE CONTROLLER V1.0 ]");
        attroff(COLOR_PAIR(5));
        
        int cx = COLS / 2;
        int cy = LINES / 2;
        
        // Centering offset for a 3x3 grid (approx 20 chars wide)
        int gx = cx - 12; 
        int gy = cy - 5;

        attron(COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
        mvprintw(gy - 2, cx - 6, "COMMAND DECK");
        attroff(COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);

        // Row 1: W E R
        draw_key_box(gy,     gx, 'W', "Up-L",  last_cmd == 'w');
        draw_key_box(gy,     gx+8, 'E', "Up",    last_cmd == 'e');
        draw_key_box(gy,     gx+16, 'R', "Up-R",  last_cmd == 'r');

        // Row 2: S D F
        draw_key_box(gy+3,   gx, 'S', "Left",  last_cmd == 's');
        draw_key_box(gy+3,   gx+8, 'D', "STOP",  last_cmd == 'd');
        draw_key_box(gy+3,   gx+16, 'F', "Right", last_cmd == 'f');

        // Row 3: X C V
        draw_key_box(gy+6,   gx, 'X', "Dn-L",  last_cmd == 'x');
        draw_key_box(gy+6,   gx+8, 'C', "Down",  last_cmd == 'c');
        draw_key_box(gy+6,   gx+16, 'V', "Dn-R",  last_cmd == 'v');

        // Telemetry Data Panel
        int ty = gy + 10;
        mvhline(ty, cx - 15, ACS_HLINE, 30);
        mvprintw(ty, cx - 4, "[ TELEMETRY ]");
        
        attron(COLOR_PAIR(3));
        mvprintw(ty + 2, cx - 10, "Position : X=%6.2f  Y=%6.2f", d.x, d.y);
        mvprintw(ty + 3, cx - 10, "Velocity : Vx=%5.2f  Vy=%5.2f", d.vx, d.vy);
        mvprintw(ty + 4, cx - 10, "Score    : %-10.2f", d.actual_score);
        attroff(COLOR_PAIR(3));

        mvprintw(LINES - 2, 2, "Press 'Q' to Quit system.");
        
        refresh();

        int ch = getch();
        if (ch == KEY_RESIZE) {
            handle_resize();
            continue;
        }

        if (ch != ERR) {
            // Simple logic to highlight last pressed key for a moment
            if (ch == 'w' || ch == 'e' || ch == 'r' || 
                ch == 's' || ch == 'd' || ch == 'f' || 
                ch == 'x' || ch == 'c' || ch == 'v') {
                last_cmd = ch;
            }
            
            g.command = ch;
            if(ch == 'q') exit(0);
            write(fd_tx, &g, sizeof(Game));
        }

        if(read(fd_rx, &d, sizeof(Drone)) > 0) {
            // Data updated
        }
    }
    endwin();
    return 0;
}
