#include "../include/handlers.h"
#include <math.h>

// Parameters
float M = 1.0;            
float K = 1.0;            
float T = 0.05;           
float ETA = 10.0;         
float RHO = 5.0;          

// Unit Vectors for Virtual Key Projection
float UX[8] = {1.0,  0.707,  0.0, -0.707, -1.0, -0.707,  0.0,  0.707};
float UY[8] = {0.0,  0.707,  1.0,  0.707,  0.0, -0.707, -1.0, -0.707};

void load_config() {
    FILE *f = fopen("config/config.txt", "r");
    if (!f) return;
    char key[50]; float val;
    while (fscanf(f, "%s %f", key, &val) != EOF) {
        if (!strcmp(key, "DRONE_MASS")) M = val;
        else if (!strcmp(key, "DRONE_FRICTION")) K = val;
        else if (!strcmp(key, "REPULSIVE_CONSTANT")) ETA = val;
        else if (!strcmp(key, "PERCEPTION_RADIUS")) RHO = val;
        else if (!strcmp(key, "TIME_STEP")) T = val;
    }
    fclose(f);
}

// --- FIX [Resize]: Enhanced resize handler to prevent drone artifacts ---
void handle_resize(Drone *d) {
    endwin();  
    refresh(); 
    clear();   
    // Force immediate coordinate clamping to new boundaries
    if (d->x >= COLS - 2) d->x = COLS - 2;
    if (d->y >= LINES - 2) d->y = LINES - 2;
    refresh(); 
    log_debug_system("Window Resized. New Boundaries: %d x %d", COLS, LINES);
}

void calc_raw_repulsion(float dist, float dir_x, float dir_y, float *Px, float *Py) {
    if (dist < RHO) {
        // --- FIX [Physics]: Prevent singularity by increasing min distance ---
        if (dist <= 1.0) dist = 1.0; 
        
        float magnitude = (ETA * 5.0) * (1.0/dist - 1.0/RHO) * (1.0/(dist*dist));
        
        // --- FIX [Physics]: Prevent teleporting by clamping max repulsive force ---
        if (magnitude > 20.0) magnitude = 20.0; 
        
        *Px += magnitude * dir_x;
        *Py += magnitude * dir_y;
    }
}

void draw(Drone *d, Game *g) {
    werase(stdscr); 
    box(stdscr, 0, 0); 
    mvprintw(0, 2, " Score: %.2f Target: %d ", d->actual_score, g->next_target_id + 1);
    
    attron(COLOR_PAIR(2)); 
    for(int i=0; i<MAX_OBSTACLES; i++) 
        if(g->Obstacle_x[i]>0) mvaddch(g->Obstacle_y[i], g->Obstacle_x[i], 'O');
    attroff(COLOR_PAIR(2));
    
    attron(COLOR_PAIR(3)); 
    for(int i=0; i<MAX_TARGETS; i++) {
        if(g->Target_x[i] > 0) {
            if (i == g->next_target_id) attron(A_BOLD | A_BLINK);
            mvprintw(g->Target_y[i], g->Target_x[i], "%d", i+1);
            if (i == g->next_target_id) attroff(A_BOLD | A_BLINK);
        }
    }
    attroff(COLOR_PAIR(3));
    
    attron(COLOR_PAIR(1)); 
    int dx = (int)round(d->x);
    int dy = (int)round(d->y);
    if(dx > 0 && dx < COLS && dy > 0 && dy < LINES) mvaddch(dy, dx, '+' | A_BOLD);
    attroff(COLOR_PAIR(1));
    
    refresh();
}

int main(int argc, char *argv[]) {
    check_args_for_help(argc, argv, "Display and Physics Engine.");
    setup_child_signals();

    int fd_rx = atoi(argv[1]);
    int fd_tx = atoi(argv[2]);
    fcntl(fd_rx, F_SETFL, O_NONBLOCK);

    setlocale(LC_ALL, ""); 
    initscr(); cbreak(); noecho(); curs_set(0); keypad(stdscr, TRUE); timeout(0); 

    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);   
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); 
    init_pair(3, COLOR_GREEN, COLOR_BLACK);  

    Drone d = {COLS/2.0f, LINES/2.0f, 0, 0, 0, 0, 0.0f, 0};
    Game g = {0};
    Game tmp = {0};
    time_t start_time = time(NULL);

    // --- FIX [Debug]: Systematic log on startup ---
    log_debug_system("Physics Engine Initialized. Drone at (%.2f, %.2f)", d.x, d.y);

    while(1) {
        load_config();
        
        int ch = getch();
        if (ch == KEY_RESIZE) handle_resize(&d);

        while(read(fd_rx, &tmp, sizeof(Game)) > 0) {
            if(tmp.game_update == 1) { memcpy(g.Obstacle_x, tmp.Obstacle_x, sizeof(g.Obstacle_x)); memcpy(g.Obstacle_y, tmp.Obstacle_y, sizeof(g.Obstacle_y)); }
            if(tmp.game_update == 2) { memcpy(g.Target_x, tmp.Target_x, sizeof(g.Target_x)); memcpy(g.Target_y, tmp.Target_y, sizeof(g.Target_y)); g.next_target_id = 0;}
            if(tmp.game_update == 3) { g.next_target_id = tmp.next_target_id; memcpy(g.Target_x, tmp.Target_x, sizeof(g.Target_x)); }

            char c = tmp.command;
            if(c) {
                float step = 1.0; float diag = step * 0.707;
                if(c=='w'){d.cmd_fx-=diag; d.cmd_fy-=diag;} else if(c=='e')d.cmd_fy-=step; else if(c=='r'){d.cmd_fx+=diag; d.cmd_fy-=diag;}
                else if(c=='s')d.cmd_fx-=step; else if(c=='d'){d.cmd_fx=0; d.cmd_fy=0; d.vx=0; d.vy=0;} else if(c=='f')d.cmd_fx+=step;
                else if(c=='x'){d.cmd_fx-=diag; d.cmd_fy+=diag;} else if(c=='c')d.cmd_fy+=step; else if(c=='v'){d.cmd_fx+=diag; d.cmd_fy+=diag;}
                
                if(d.cmd_fx > 15) d.cmd_fx = 15; else if(d.cmd_fx < -15) d.cmd_fx = -15;
                if(d.cmd_fy > 15) d.cmd_fy = 15; else if(d.cmd_fy < -15) d.cmd_fy = -15;
                
                // --- FIX [Debug]: Log inputs ---
                log_debug_system("Input: %c | Applied Forces: (%.2f, %.2f)", c, d.cmd_fx, d.cmd_fy);
            }
        }

        float Px = 0, Py = 0;
        calc_raw_repulsion(d.x, 1.0, 0.0, &Px, &Py);           
        calc_raw_repulsion((COLS-1) - d.x, -1.0, 0.0, &Px, &Py); 
        calc_raw_repulsion(d.y, 0.0, 1.0, &Px, &Py);           
        calc_raw_repulsion((LINES-1) - d.y, 0.0, -1.0, &Px, &Py); 

        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (g.Obstacle_x[i] > 0) {
                float dx = d.x - g.Obstacle_x[i];
                float dy = d.y - g.Obstacle_y[i];
                float dist = sqrt(dx * dx + dy * dy);
                calc_raw_repulsion(dist, dx/dist, dy/dist, &Px, &Py);
            }
        }

        float vfx = 0, vfy = 0, max_dot = -99999.0;
        int best_k = -1;
        if (Px != 0 || Py != 0) {
            for (int k = 0; k < 8; k++) {
                float dot = Px * UX[k] + Py * UY[k];
                if (dot > max_dot) { max_dot = dot; best_k = k; }
            }
            if (best_k != -1 && max_dot > 0) {
                vfx = max_dot * UX[best_k]; vfy = max_dot * UY[best_k];
            }
        }

        float SumF_x = d.cmd_fx + vfx;
        float SumF_y = d.cmd_fy + vfy;
        float ax = (SumF_x - K*d.vx)/M;
        float ay = (SumF_y - K*d.vy)/M;

        d.vx += ax * T; d.vy += ay * T;
        d.x += d.vx * T; d.y += d.vy * T;

        if(d.x < 1) d.x = 1.0; if(d.x > COLS-2) d.x = COLS-2.0;
        if(d.y < 1) d.y = 1.0; if(d.y > LINES-2) d.y = LINES-2.0;

        d.hit_target_index = -1;
        for(int i=0; i<MAX_TARGETS; i++) {
            if(g.Target_x[i] > 0 && abs((int)d.x - g.Target_x[i]) < 2 && abs((int)d.y - g.Target_y[i]) < 2) {
                if (i == g.next_target_id) {
                    d.hit_target_index = i;
                    g.Target_x[i] = -100;
                    // --- FIX [Debug]: Log Event ---
                    log_debug_system("Target %d hit! Score: %.2f", i+1, d.actual_score);
                }
            }
        }
        
        double elapsed = difftime(time(NULL), start_time);
        d.actual_score = (g.next_target_id * 100.0) - (elapsed * 0.5);
        if (d.actual_score < 0) d.actual_score = 0;

        write(fd_tx, &d, sizeof(Drone));
        draw(&d, &g);
        usleep(T*1000000);
    }
    endwin();
    return 0;
}
