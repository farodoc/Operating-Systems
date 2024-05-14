#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

const int grid_size = 30;

int main(int argc, char **argv)
{
	if(argc < 2){
		printf("Usage: %s <number of threads>\n", argv[0]);
		return -1;
	}

	int no_threads = atoi(argv[1]);

	if(no_threads > grid_size * grid_size){
		printf("Number of threads must be less than or equal to the number of cells in the grid\n");
		return -1;
	}

	if(no_threads < 1){
		printf("Number of threads must be greater than 0\n");
		return -1;
	}

	pthread_t threads[no_threads];
	ThreadData thread_data[no_threads];

	int cells_per_thread = grid_size / no_threads;

	for(int i = 0; i < no_threads; i++){
		thread_data[i].start = i * cells_per_thread;
		thread_data[i].end = (i == no_threads - 1) ? grid_size * grid_size : (i + 1) * cells_per_thread;
	}

	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		for(int i = 0; i < no_threads; i++){
			thread_data[i].foreground = foreground;
			thread_data[i].background = background;
			pthread_create(&threads[i], NULL, update_grid_part, &thread_data[i]);
		}

		for(int i = 0; i < no_threads; i++){
			pthread_join(threads[i], NULL);
		}

		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
