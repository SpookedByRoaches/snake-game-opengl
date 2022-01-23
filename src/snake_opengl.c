#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <ncurses.h>
#include <locale.h>
#include <pthread.h>
#include <list.h>
#include <snake_opengl.h>


pthread_mutex_t screen_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pause_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t input_flag_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pause_cond = PTHREAD_COND_INITIALIZER;
int received_input, is_paused;
char *pause_menu_options[] = {
	"Resume",
	"Quit",
	NULL
};

struct IO_handler *input_output;
int main()
{
	pthread_t main_thread, timer_thread;
	if (pthread_mutex_init(&screen_lock, NULL) != 0)
		throw_error("Cannot create pthread_mutex", errno);
	
	if (pthread_mutex_init(&screen_lock, NULL) != 0)
		throw_error("Cannot create pthread_mutex", errno);
	pthread_create(&main_thread, NULL, main_thread_routine, (void *) NULL);
	//pthread_create(&timer_thread, NULL, timer_thread_routine, (void *)NULL);

	pthread_join(main_thread, NULL);
	//pthread_join(timer_thread, NULL);
}

int snake_get_size(struct snake_segment *player)
{
	struct list_head *i;
	int score = 0;
	list_for_each(i, &player->list){
		score++;
	}
	return score;
}

void draw_frame()
{
	pthread_mutex_lock(&screen_lock);
	move(INFO_LINES, 0);
	char horz_blanks[COLS];
	memset(horz_blanks, ' ', COLS - 1);
	horz_blanks[COLS - 1] = '\0';
	attron(COLOR_PAIR(FRAME_COLOR_PAIR));
	printw("%s", horz_blanks);
	move(LINES - 1, 0);
	printw("%s", horz_blanks);
	for (int i = INFO_LINES; i < LINES; i++){
		move(i, 0);
		printw(" ");
		move(i, COLS - 1);
		printw(" ");
	}
	attroff(COLOR_PAIR(FRAME_COLOR_PAIR));
	pthread_mutex_unlock(&screen_lock);
}

void *timer_thread_routine()
{
	struct timeval t_before, t_after;
	long int usec_before, usec_after, usec_diff;
	while(1){
		for (int i = 0; i < T_DIV; i++){
			gettimeofday(&t_before, NULL);
			usec_before = t_before.tv_sec*1000000 + t_before.tv_usec;
			if (received_input || is_paused)
				break;
			timer_draw_time(i);
			gettimeofday(&t_after, NULL);
			usec_after = t_after.tv_sec*1000000 + t_after.tv_usec;
			usec_diff = usec_after - usec_before;
			usleep(TICK_PERIOD_US/T_DIV - usec_diff);
		}
		timer_erase_time();
		if (is_paused)
			timer_pause_time();
	}
	return NULL;
}

void timer_pause_time()
{
	pthread_mutex_lock(&pause_lock);
	pthread_cond_wait(&pause_cond, &pause_lock);
	pthread_mutex_unlock(&pause_lock);
}

void timer_erase_time()
{
	char erase[T_DIV + 1];
	memset(erase, ' ', T_DIV);
	erase[T_DIV] = '\0';
	pthread_mutex_lock(&input_flag_lock);
	received_input = 0;
	pthread_mutex_unlock(&input_flag_lock);
	pthread_mutex_lock(&screen_lock);
	move(1, 0);
	printw("%s", erase);
	pthread_mutex_unlock(&screen_lock);
}

void timer_draw_time(int cur_time)
{
	pthread_mutex_lock(&screen_lock);
	move(1, cur_time);
	printw("X");
	refresh();
	pthread_mutex_unlock(&screen_lock);
}

void *main_thread_routine()
{
	snake_initialize_game();
	return NULL;
}

void snake_initialize_game()
{
	struct snake_segment *player;
	struct food *mouse;
	short init_x, init_y;
	struct timeval cur_t;
	struct Shader *shader_program;
	input_output = IO_handler_construct();
	gettimeofday(&cur_t, NULL);
	player = (struct snake_segment*)malloc(sizeof(player));
	mouse = (struct food*)malloc(sizeof(mouse));
	srandom(cur_t.tv_sec);
	init_x = N_COLS/2;
	init_y = N_LINES/2;
	snake_construct(player, init_x, init_y, up);
	snake_place_food(player, mouse);
	snake_draw(player, mouse, input_output);

	snake_place_food(player, mouse);
	for (;;){
		snake_tick(player, mouse, input_output);
		snake_draw(player, mouse, input_output);
	}
}

void snake_construct(struct snake_segment *player, int x, int y, enum direction heading)
{
	player->x = x;
	player->y = y;
	player->heading = heading;
	INIT_LIST_HEAD(&player->list);
}

void snake_tick(struct snake_segment *player, struct food *mouse, struct IO_handler *input_output)
{
	static struct snake_segment temp_tail = {x: 0, y: 0, heading: up};
	double t = glfwGetTime();
	snake_handle_input(player, mouse, input_output);
	if (is_head_colliding(player)){
		//snake_alert_collision(1);
		return;
	}
	//snake_alert_collision(0);
	snake_copy_tail(player, &temp_tail);	
	snake_move_head(player);
	snake_move_body(player);
	if ((player->x == mouse->x) && (player->y == mouse->y)){
		snake_grow(player, &temp_tail);
		snake_place_food(player, mouse);
	}
}

void snake_alert_collision(int is_colliding)
{
	char *message = "COLLIDING!!";
	short offset = sizeof(message) + 3;
	pthread_mutex_lock(&screen_lock);
	if (is_colliding){
		move(0, COLS - offset);
		attron(COLOR_PAIR(ALERT_COLOR_PAIR));
		printw(message);
		attroff(COLOR_PAIR(ALERT_COLOR_PAIR));
	} else {
		move(0, COLS - offset);
		clrtoeol();
	}
	refresh();
	pthread_mutex_unlock(&screen_lock);
}

void snake_copy_tail(struct snake_segment *player, struct snake_segment *copy)
{
	struct snake_segment *tail;
	if (&player->list == player->list.prev)
		tail = player;
	else
		tail = list_last_entry(&player->list, struct snake_segment, list);
	copy->x = tail->x;
	copy->y = tail->y;
	copy->heading = tail->heading;
}

void snake_move_head(struct snake_segment *player)
{
	short nextx = snake_next_x_position(player);
	short nexty = snake_next_y_position(player);
	player->x = nextx;
	player->y = nexty;
}

void snake_grow(struct snake_segment *player, struct snake_segment *tail_copy)
{
	struct snake_segment *new_segment;
	new_segment = (struct snake_segment *)malloc(sizeof(struct snake_segment));
	new_segment->x = tail_copy->x;
	new_segment->y = tail_copy->y;
	new_segment->heading = tail_copy->heading;
	list_add_tail(&new_segment->list, &player->list);
}

void snake_move_body(struct snake_segment *player)
{
	struct snake_segment *cur_segment, *prev_segment;
	prev_segment = NULL;
	list_for_each_entry_reverse(cur_segment, &player->list, list){
		snake_move_segment(cur_segment);
		if (prev_segment != NULL)
			prev_segment->heading = cur_segment->heading;
		prev_segment = cur_segment;
	}
	if (prev_segment != NULL)
		prev_segment->heading = player->heading;
}

void snake_move_segment(struct snake_segment *segment)
{
	segment->x = snake_next_x_position(segment);
	segment->y = snake_next_y_position(segment);
}

int is_head_colliding(struct snake_segment *player)
{
	short player_next_x, player_next_y, cur_segment_next_x, cur_segment_next_y;
	struct snake_segment *cur_segment;
	player_next_x = snake_next_x_position(player);
	if ((player_next_x >= N_COLS - 1) || (player_next_x <= 0))
		return 1;
	player_next_y = snake_next_y_position(player);
	if ((player_next_y >= N_LINES - 1) || (player_next_y <= INFO_LINES))
		return 1;
	list_for_each_entry(cur_segment, &player->list, list){
		cur_segment_next_x = snake_next_x_position(cur_segment);
		cur_segment_next_y = snake_next_y_position(cur_segment);
		if ((cur_segment_next_x == player_next_x) && (cur_segment_next_y == player_next_y))
			return 1;
	}

	return 0;
}

short snake_next_x_position(struct snake_segment *segment)
{
	switch (segment->heading){
	case right:
		return segment->x + 1;
	case left:
		return segment->x - 1;
	default:
		return segment->x;
	}
}

short snake_previous_x_position(struct snake_segment *segment)
{
	switch (segment->heading){
	case right:
		return segment->x - 2;
	case left:
		return segment->x + 2;
	default:
		return segment->x;
	}
}

short snake_next_y_position(struct snake_segment *segment)
{
	switch (segment->heading){
	case down:
		return segment->y - 1;
	case up:
		return segment->y + 1;
	default:
		return segment->y;
	}
}

short snake_previous_y_position(struct snake_segment *segment)
{
	switch (segment->heading){
	case down:
		return segment->y - 1;
	case up:
		return segment->y + 1;
	default:
		return segment->y;
	}
}

void snake_draw_food(struct food *mouse, struct IO_handler *input_output)
{
	float size[2] = {1.0f, 1.0f};
	float pos[2] = {0.0, 0.0};
	float col[3] = {1.0f, 1.0f, 1.0f};

	pos[0] = mouse->x * SNAKE_PART_LENGTH;
	pos[1] = mouse->y * SNAKE_PART_LENGTH;

	float rot = 0;

	graphics_draw_square(input_output, size, pos, &rot, col, food);
}

void snake_draw(struct snake_segment *player, struct food *mouse, struct IO_handler *input_output)
{
	struct timeval before, after;
	struct snake_segment *cur_segment;
	struct snake_segment *tail_segment = NULL;
	struct list_head *i;
	gettimeofday(&before, NULL);
	pthread_mutex_lock(&screen_lock);
	graphics_clear_game_screen(input_output);
	snake_draw_background(input_output);
	snake_draw_head(player, input_output);
	list_for_each(i, &player->list){
		if (list_is_last(i, &player->list)){
			tail_segment = list_entry(i, struct snake_segment, list);
			break;
		}
		cur_segment = list_entry(i, struct snake_segment, list);
		snake_draw_segment(cur_segment, input_output);
	}
	if (tail_segment)
		snake_draw_tail(tail_segment, input_output);
	snake_draw_food(mouse, input_output);
	graphics_show(input_output);
	//snake_draw_info(player, mouse);
	gettimeofday(&after, NULL);
	pthread_mutex_unlock(&screen_lock);
}

void snake_draw_info(struct snake_segment *player, struct food *mouse)
{
	int cur_score = snake_get_size(player);

	move(0, N_COLS/2);
	printw("%d/%d", cur_score, (N_COLS/2 - 1)*(N_LINES - 4));
}

void snake_draw_head(struct snake_segment *player, struct IO_handler *input_output)
{
	float size[2] = {1.0f, 1.0f};
	float pos[2] = {0.0, 0.0};
	float col[3] = {1.0f, 1.0f, 1.0f};

	pos[0] = player->x * SNAKE_PART_LENGTH;
	pos[1] = player->y * SNAKE_PART_LENGTH;

	float rot;

	switch(player->heading){
		case left:
			rot = GLM_PI_2f;
			break;
		case up:
			rot = 0.0f;
			break;
		case down:
			rot = GLM_PI;
			break;
		case right:
			rot = GLM_PI_2f*3;
			break;
	}
	graphics_draw_square(input_output, size, pos, &rot, col, head);
}

void snake_draw_tail(struct snake_segment *tail_segment, struct IO_handler *input_output)
{
	float size[2] = {1.0f, 1.0f};
	float pos[2] = {0.0, 0.0};
	float col[3] = {1.0f, 1.0f, 1.0f};

	pos[0] = tail_segment->x * SNAKE_PART_LENGTH;
	pos[1] = tail_segment->y * SNAKE_PART_LENGTH;

	float rot;

	switch(tail_segment->heading){
		case left:
			rot = GLM_PI_2f;
			break;
		case up:
			rot = 0.0f;
			break;
		case down:
			rot = GLM_PIf;
			break;
		case right:
			rot = GLM_PI_2f*3;
			break;
	}
	graphics_draw_square(input_output, size, pos, &rot, col, tail);
}

void snake_draw_segment(struct snake_segment *segment, struct IO_handler *input_output)
{
	float size[2] = {1.0f, 1.0f};
	float pos[2] = {0.0, 0.0};
	float col[3] = {1.0f, 1.0f, 1.0f};

	pos[0] = segment->x * SNAKE_PART_LENGTH;
	pos[1] = segment->y * SNAKE_PART_LENGTH;

	float rot = segment->heading * GLM_PI_2f;

	graphics_draw_square(input_output, size, pos, &rot, col, body);
}

void clear_game_screen()
{
	char message[COLS - 1];
	memset(message, ' ', COLS - 2);
	message[COLS - 2] = '\0';
	for (int i = INFO_LINES + 1; i < LINES - 1; i++){
		move(i, 1);
		printw("%s", message);
	}
}

void snake_set_up_terminal_settings()
{
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	curs_set(0);
	timeout(TICK_PERIOD_MS);
	start_color();
	init_pair(SNAKE_COLOR_PAIR, COLOR_DARK_GREEN, COLOR_BLACK);
	init_pair(ALERT_COLOR_PAIR, COLOR_BLACK, COLOR_BRIGHT_YELLOW);
	init_pair(FOOD_COLOR_PAIR, COLOR_BRIGHT_RED, COLOR_BLACK);
	init_pair(FRAME_COLOR_PAIR, COLOR_BLACK, COLOR_BEIGE);
	init_pair(INVERSE_COLOR_PAIR, COLOR_BLACK, COLOR_BEIGE);
	clear();
}
void snake_restore_terminal_settings()
{
	endwin();
}

void snake_draw_background(struct IO_handler *input_output)
{
	float size[2] = {WINDOW_SCALE/SNAKE_PART_LENGTH, WINDOW_SCALE/SNAKE_PART_LENGTH};
	float pos[2] = {0, 0};
	float rot = 0;
	float col[3] = {1.0f, 1.0f, 1.0f};
	graphics_draw_square(input_output, size, pos, &rot, col, background);
}

void snake_handle_input(struct snake_segment *player, struct food *mouse, struct IO_handler *input_output)
{
	int input;
	static long unsigned int last_in_time;
	long unsigned int call_time = glfwGetTime();
	const float delay = 0.1;
	float t_elapsed = (call_time - last_in_time); 
	if (t_elapsed < delay)
		usleep((delay - t_elapsed) * 1000000);
	while(1){
		glfwPollEvents();
		if (input_output->cur_button == GLFW_KEY_Q){
			glfwSetWindowShouldClose(input_output->win, true);
			kill_everything(player, mouse, input_output);
		} else if (input_output->cur_button == GLFW_KEY_UP){
			snake_change_direction(player, KEY_UP);
		} else if (input_output->cur_button == GLFW_KEY_DOWN){
			snake_change_direction(player, KEY_DOWN);
		} else if (input_output->cur_button == GLFW_KEY_RIGHT){
			snake_change_direction(player, KEY_RIGHT);
		} else if (input_output->cur_button == GLFW_KEY_LEFT){
			snake_change_direction(player, KEY_LEFT);
		} else if (input_output->cur_button == GLFW_KEY_P){
			//snake_pause_game(player, mouse);
		} else {
			if ((glfwGetTime() - call_time) > 0.2)
				break;
			else
				continue;
		}
		break;
	}

	last_in_time = glfwGetTime();

	pthread_mutex_lock(&input_flag_lock);
	received_input = 1;
	pthread_mutex_unlock(&input_flag_lock);
}

void draw_pause_menu(enum menu_commands selected)
{
	pthread_mutex_lock(&screen_lock);
	clear_game_screen();
	short x, y, num_options, option_len;
	for (num_options = 0; pause_menu_options[num_options] != NULL; num_options++);
	
	num_options++;
	y = N_LINES/2 - num_options/2;
	
	for (int i = 0; pause_menu_options[i] != NULL; i++){
		if (i == selected)
			attron(COLOR_PAIR(INVERSE_COLOR_PAIR));

		option_len = strlen(pause_menu_options[i]);
		x = N_COLS/2 - option_len/2;
		move(y, x);
		printw("%s", pause_menu_options[i]);
		y++;
		if (i == selected)
			attroff(COLOR_PAIR(INVERSE_COLOR_PAIR));

	}
	refresh();
	pthread_mutex_unlock(&screen_lock);
}


void snake_pause_game(struct snake_segment *player, struct food *mouse)
{
	is_paused = 1;
	int input, num_options;
	static enum menu_commands selected = 0;
	for (num_options = 0; pause_menu_options[num_options] != NULL; num_options++);
	num_options--;
	draw_pause_menu(0);
	input = getch();
	while(1){
		input = getch();
		if ((input == 'p') || (input =='P')){
			goto unpause;
		} else if (input == KEY_UP && selected > 0){
			selected--;
			draw_pause_menu(selected);
		} else if (input == KEY_DOWN && selected < num_options){
			selected++;
			draw_pause_menu(selected);
		} else if (input == '\n'){
			switch (selected){
			case Resume:
				goto unpause;
				break;
			case Quit:
				//kill_everything(player, mouse);
			}
		}
	}
unpause:
	is_paused = 0;
	pthread_cond_signal(&pause_cond);
	return;
}

int snake_input_is_acceptable(int input)
{
	int acceptable_inputs[] = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, 'q', 'Q', 'p', 'P'};
	int numel = sizeof(acceptable_inputs)/sizeof(int);
	for (int i = 0; i < numel; i++)
		if (input == acceptable_inputs[i])
			return 1;
	return 0;
}

void snake_change_direction(struct snake_segment *player, int input)
{
	switch (input){
	case KEY_DOWN:
		player->heading = down;
		break;
	case KEY_UP:
		player->heading = up;
		break;
	case KEY_RIGHT:
		player->heading = right;
		break;
	case KEY_LEFT:
		player->heading = left;
		break;
	default:
		player->heading = 6;
	}
}

void kill_everything(struct snake_segment *player, struct food *mouse, struct IO_handler *input_output)
{
	struct list_head *i, *tmp;
	struct snake_segment *cur_segment;
	snake_restore_terminal_settings();
	glfwDestroyWindow(input_output->win);
	glfwMakeContextCurrent(NULL);
	glfwTerminate();
	list_for_each_safe(i, tmp, &player->list){
		cur_segment = list_entry(i, struct snake_segment, list);
		list_del(i);
		free(cur_segment);
	}
	free(player);
	free(mouse);

	pthread_mutex_destroy(&screen_lock);
	pthread_mutex_destroy(&pause_lock);
	pthread_mutex_destroy(&input_flag_lock);
	printf("THANKS FOR PLAYING\n");
	exit(0);
}

void snake_place_food(struct snake_segment *player, struct food *mouse)
{
	short x, y, is_ok;
	struct snake_segment *cur_segment;
	is_ok = 0;
	while (!is_ok){
		x = random()%(HIGHER_X_LIMIT - LOWER_X_LIMIT) + LOWER_X_LIMIT;
		y = (random()%(HIGHER_Y_LIMIT - LOWER_Y_LIMIT) + LOWER_Y_LIMIT);
		is_ok = 1;
		if (player->x == x && player->y == y){
			is_ok= 0;
			continue;
		}

		list_for_each_entry(cur_segment, &player->list, list){
			if ((cur_segment->x == x) && (cur_segment->y == y)){
				is_ok = 0;
				break;
			}
		}
	}
	mouse->x = x;
	mouse->y = y;
}
