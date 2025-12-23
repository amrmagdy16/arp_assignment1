#ifndef COMMON_H
#define COMMON_H

#define MAX_OBSTACLES 10
#define MAX_TARGETS 5

// Data sent from Output Window -> Server
typedef struct {
    float x, y;
    float vx, vy;
    float cmd_fx, cmd_fy;
    float score;
    float actual_score;
    int hit_target_index; // NEW: Tells Server which target was hit (-1 if none)
} Drone;

typedef struct {
    int x[MAX_OBSTACLES], y[MAX_OBSTACLES];
} Obstacle;

typedef struct {
    int x[MAX_TARGETS], y[MAX_TARGETS];
    int id[MAX_TARGETS];
} Target;

typedef struct {
    int command;
    int Obstacle_x[MAX_OBSTACLES], Obstacle_y[MAX_OBSTACLES];
    int Target_x[MAX_TARGETS], Target_y[MAX_TARGETS];
    int game_update; // 1=Obs, 2=Tar, 3=ScoreUpdate
    int next_target_id; // NEW: Enforces sequence (1, then 2...)
} Game;

#endif
