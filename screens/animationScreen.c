#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define NUM_PARTICLES 26
#define TRAIL_LENGTH 4
#define COLORS 8


const char TITLE[] = " LEARN TYPING ";
const char INSTRUCTION[] = " * Press ENTER to start or 'q'/ESC to Exit";

int TITLE_LENGTH = (int) strlen(TITLE);
int INSTRUCTION_LENGTH = (int) strlen(INSTRUCTION);

typedef struct {
    float x, y;
    float dx, dy;
    int color;
    char current_char;
} Particle;

void init_particle(Particle *p, int max_x, int max_y, int index) {
    p->x = rand() % (max_x - 2) + 1;
    p->y = rand() % (max_y - 2) + 1;
    p->dx = (rand() % 200 - 100) / 100.0;
    p->dy = (rand() % 200 - 100) / 100.0;
    p->color = rand() % COLORS + 1;
    p->current_char = 'A' + (index % 26);
}


int showAnimationScreen( bool isIntroAnim ) {
    srand(time(NULL));
    curs_set(0);
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    start_color();

    int ch = 0;
    bool play = true;
    size_t startTime = time(NULL), endTime = -1;

    for (int i = 1; i <= COLORS; i++) {
        init_pair(i, i % 8, COLOR_BLACK);
    }

    int MAX_X, MAX_Y;
    getmaxyx(stdscr, MAX_Y, MAX_X);

    Particle particles[NUM_PARTICLES];
    for (int i = 0; i < NUM_PARTICLES; i++) {
        init_particle(&particles[i], MAX_X, MAX_Y, i);
    }

    while (play) {
        clear();

        int new_max_x, new_max_y;
        getmaxyx(stdscr, new_max_y, new_max_x);
        if (new_max_x != MAX_X || new_max_y != MAX_Y) {
            MAX_X = new_max_x;
            MAX_Y = new_max_y;
            for (int i = 0; i < NUM_PARTICLES; i++) {
                init_particle(&particles[i], MAX_X, MAX_Y, i);
            }
        }

        box(stdscr, 0, 0);

        for (int i = 0; i < NUM_PARTICLES; i++) {
            particles[i].x += particles[i].dx;
            particles[i].y += particles[i].dy;

            if (particles[i].x <= 1 || particles[i].x >= MAX_X - 2) {
                particles[i].dx *= -1;
                particles[i].color = (particles[i].color % COLORS) + 1;
                particles[i].current_char = 'A' + (particles[i].current_char - 'A' + 1) % 26;
            }
            if (particles[i].y <= 1 || particles[i].y >= MAX_Y - 2) {
                particles[i].dy *= -1;
                particles[i].color = (particles[i].color % COLORS) + 1;
                particles[i].current_char = 'A' + (particles[i].current_char - 'A' + 1) % 26;
            }

            attron(COLOR_PAIR(particles[i].color) | A_BOLD);
            for (int t = 0; t < TRAIL_LENGTH; t++) {
                int trail_x = particles[i].x - particles[i].dx * t;
                int trail_y = particles[i].y - particles[i].dy * t;
                if (trail_x > 0 && trail_x < MAX_X - 1 && 
                    trail_y > 0 && trail_y < MAX_Y - 1) {
                    int char_offset = (particles[i].current_char - 'A' - t + 26) % 26;
                    char trail_char = 'A' + char_offset;
                    mvaddch(trail_y, trail_x, trail_char);
                }
            }
            attroff(COLOR_PAIR(particles[i].color) | A_BOLD);
        }

        ch = getch();
        if ( ch == 'q') break;

        attron(A_BLINK);
        mvprintw(0, (MAX_X - TITLE_LENGTH)/2, "%s", TITLE);
        attroff(A_BOLD);


       if ( isIntroAnim ){     attron(A_BOLD);
            mvprintw(MAX_Y/2-2, (MAX_X - TITLE_LENGTH)/2, "%s", TITLE);
            attroff(A_BOLD);

            attron(A_ITALIC);
            mvprintw(MAX_Y/2-1, (MAX_X - INSTRUCTION_LENGTH)/2, "%s", INSTRUCTION);
            attroff(A_BOLD);}

        if ( isIntroAnim && ch == 10 )  break;
        if ( !isIntroAnim && endTime - startTime == 1 ) break;

        if ( ch == 27 ){
            refresh();
            endwin();
            exit(0);
        }


        endTime = time(NULL);
        refresh();
        napms(60);
    }

    endwin();
    return 0;
}