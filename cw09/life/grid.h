#pragma once
#include <stdbool.h>

typedef struct {
    char *foreground;
    char *background;
    int start;
    int end;
} ThreadData;

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void *update_grid_part(void *arg);