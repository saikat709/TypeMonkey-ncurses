#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define NUM_PARTICLES 50
#define TRAIL_LENGTH 4
#define NUM_COLORS 8


const char TITLE[] = " LEARN TYPING ";
const char INSTRUCTION[] = " * Press ENTER to start or 'q'/ESC to Exit";

int TITLE_LENGTH = (int) strlen(TITLE);
int INSTRUCTION_LENGTH = (int) strlen(INSTRUCTION);


float CORD_X[NUM_PARTICLES], CORD_Y[NUM_PARTICLES], CORD_Dx[NUM_PARTICLES], CORD_Dy[NUM_PARTICLES];
int COLOR[NUM_PARTICLES];
char CURRENT_CHAR[NUM_PARTICLES];

void init_particles(int max_x, int max_y) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        CORD_X[i]       = rand()  % ( max_x - 2 ) + 1;
        CORD_Y[i]       = rand()  % ( max_y - 2 ) + 1;
        CORD_Dx[i]      = (rand() % 200 - 100) / 100.0;
        CORD_Dy[i]      = (rand() % 200 - 100) / 100.0;
        COLOR[i]        = rand()  % COLORS + 1;
        CURRENT_CHAR[i] = 'A' + (i % 26);
    }
}


int showAnimationScreen( bool isIntroAnim ) {
    srand(time(NULL));

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    start_color();

    int ch = 0;
    size_t startTime = time(NULL), endTime = -1;

    for (int i = 1; i <= COLORS; i++) {
        init_pair(i, i % 8, COLOR_BLACK);
    }

    int MAX_X, MAX_Y;
    getmaxyx(stdscr, MAX_Y, MAX_X);

    init_particles(MAX_X, MAX_Y);

    while (TRUE) {

        ch = getch();
        if ( ch == 'q') break;
        if ( ch == 27 ){
            refresh();
            endwin();
            exit(0);
        }

        clear();

        box(stdscr, 0, 0);
        attron(A_BOLD);
        mvprintw(0, (MAX_X - TITLE_LENGTH)/2, "%s", TITLE);
        attroff(A_BOLD);


        if ( isIntroAnim ){
            attron(A_BOLD);
            mvprintw(MAX_Y/2-2, (MAX_X - TITLE_LENGTH)/2, "%s", TITLE);
            attroff(A_BOLD);

            attron(A_ITALIC);
            mvprintw(MAX_Y/2-1, (MAX_X - INSTRUCTION_LENGTH)/2, "%s", INSTRUCTION);
            attroff(A_BOLD);
        }

        if ( isIntroAnim && ( ch == 10 || ch == KEY_ENTER )  ) break; // break
        if ( !isIntroAnim && endTime - startTime == 1 )        break;

        int new_max_x, new_max_y;
        getmaxyx(stdscr, new_max_y, new_max_x);
        if (new_max_x != MAX_X || new_max_y != MAX_Y) {
            MAX_X = new_max_x;
            MAX_Y = new_max_y;
            init_particles( MAX_X, MAX_Y);
        }

        for (int i = 0; i < NUM_PARTICLES; i++) {
            CORD_X[i] += CORD_Dx[i];
            CORD_Y[i] += CORD_Dy[i];

            if (CORD_X[i] <= 1 || CORD_X[i] >= MAX_X - 2) {
                CORD_Dx[i] *= -1;
                COLOR[i]   = (COLOR[i] % NUM_COLORS) + 1;
                CURRENT_CHAR[i] = 'A' + (CURRENT_CHAR[i] - 'A' + 1) % 26;
            }
            if (CORD_Y[i] <= 1 || CORD_Y[i] >= MAX_Y - 2) {
                CORD_Dy[i] *= -1;
                COLOR[i]   = (COLOR[i] % NUM_COLORS) + 1;
                CURRENT_CHAR[i] = 'A' + (CURRENT_CHAR[i] - 'A' + 1) % 26;
            }

            attron(COLOR_PAIR(COLOR[i]) | A_BOLD);
            for (int t = 0; t < TRAIL_LENGTH; t++) {
                int trail_x =  CORD_X[i] -  CORD_Dx[i] * t;
                int trail_y =  CORD_Y[i] -  CORD_Dy[i] * t;
                if (trail_x > 0 && trail_x < MAX_X - 1 &&
                    trail_y > 0 && trail_y < MAX_Y - 1) {
                    int char_offset = (CURRENT_CHAR[i] - 'A' - t + 26) % 26;
                    char trail_char = 'A' + char_offset;
                    mvaddch(trail_y, trail_x, trail_char);
                }
            }
            attroff(COLOR_PAIR(COLOR[i]) | A_BOLD);
        }

        endTime = time(NULL);
        refresh();
        napms(20);
    }

    // endwin();
    return 0;
}
