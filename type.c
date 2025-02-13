#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <menu.h>

void animate_forward_full(int max_y, int max_x);
void animate_backward_half(int max_y, int max_x);
void animate_forward_half(int max_y, int max_x);
void animate_forward(int max_y, int max_x);
void animate_backward(int max_y, int max_x);
void show_game_title_small();
void print_type_monkey();

// int showHomeScreen();

int main() {
    initscr();
    start_color();
    curs_set(0);
    noecho();
    
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_CYAN,  COLOR_CYAN);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
    init_pair(4, COLOR_WHITE, COLOR_CYAN);
    init_pair(5, COLOR_BLACK, COLOR_CYAN);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    animate_forward_full(max_y, max_x);
    
    attron(COLOR_PAIR(3));
    // mvprintw(max_y/2, (max_x-10)/2, "ABSHS SJH");
    print_type_monkey();
    refresh();
    // napms(1000);

    animate_backward_half(max_y, max_x);

    animate_forward_half(max_y, max_x);

    napms(1*300);

    show_game_title_small();
    refresh();
    
    // animate_backward(max_y, max_x);

    // showHomeScreen();

    getch();

    endwin();
    return 0;
}



void animate_forward_full(int max_y, int max_x) {
    for(int col = 0; col < max_x; col++) {
        for(int y = 0; y < max_y; y++) {
            float ratio = (float)col / max_x;
            int color = 2;
            attron(COLOR_PAIR(color));
            mvaddch(y, col, ' ');
        }
        refresh();
        napms(10);  // Adjust speed here
    }
}

void animate_forward_half(int max_y, int max_x) {
    for(int col = 0; col < max_x/2 - 15; col++) {
        for(int y = 0; y < max_y; y++) {
            float ratio = (float)col / max_x;
            int color = 1;
            attron(COLOR_PAIR(color));
            mvaddch(y, col, ' ');
        }
        refresh();
        napms(10);  // Adjust speed here
    }
}

void animate_forward(int max_y, int max_x) {
    for(int col = 0; col < max_x; col++) {
        for(int y = 0; y < max_y; y++) {
            float ratio = (float)col / max_x;
            int color = ratio < 0.5 ? 1 : 2;
            attron(COLOR_PAIR(color));
            mvaddch(y, col, ' ');
        }
        refresh();
        napms(10);  // Adjust speed here
    }
}

void animate_backward(int max_y, int max_x) {
    for(int col = max_x-1; col >= 0; col--) {
        for(int y = 0; y < max_y; y++) {
            float ratio = (float)col / max_x;
            int color = ratio > 0.5 ? 1 : 2;
            attron(COLOR_PAIR(color));
            mvaddch(y, col, ' ');
        }
        refresh();
        napms(10);  // Adjust speed here
    }
}

void animate_backward_half(int max_y, int max_x) {
    for(int col = max_x-1; col >= max_x/2 - 15; col--) {
        for(int y = 0; y < max_y; y++) {
            int color = 2;
            attron(COLOR_PAIR(color));
            mvaddch(y, col, ' ');
        }
        refresh();
        napms(20);  // Adjust speed here
    }
}



void print_type_monkey() {

    char *art[] = {
        " .--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--.   ",
        " / .. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\ ",
        " \\ \\/\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ \\/ / ",
        "  \\/ /`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'\\/ /   ",
        " / /\\                                                                        / /\\   ",
        " / /\\ \\  _____                      __  __                _                  / /\\ \\ ",
        " \\ \\/ / |_   _|_   _  _ __    ___  |  \\/  |  ___   _ __  | | __ ___  _   _   \\ \\/ / ",
        "  \\/ /    | | | | | || '_ \\  / _ \\ | |\\/| | / _ \\ | '_ \\ | |/ // _ \\| | | |   \\/ /   ",
        "  / /\\    | | | |_| || |_) ||  __/ | |  | || (_) || | | ||   <|  __/| |_| |   / /\\   ",
        " / /\\ \\   |_|  \\__, || .__/  \\___| |_|  |_| \\___/ |_| |_||_|\\_\\___| \\__, |  / /\\ \\  ",
        " \\ \\/ /        |___/ |_|                                             |___/   \\ \\/ / ",
        "  \\/ /                                                                        \\/ /   ",
        " / /\\.--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--..--./ /\\   ",
        " / /\\ \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\.. \\/\\ \\ ",
        " \\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `'\\ `' / ",
        " `--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'`--'   ",
    };
    int art_lines = sizeof(art)/sizeof(art[0]);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int start_y = max_y/2 - art_lines/2;
    int start_x = max_x/2 - 35;

    int directions[] = {0, 1, 2, 3, 0, 1}; // 0=left,1=right,2=top,3=bottom
    const char animation_chars[] = "/|\\-";

    int timeOffSet = 1;

    srand(time(NULL));

    for(int i = 0; i < art_lines; i++) {
        int dir = directions[i % art_lines];
        int delay = 50;
        int pos = 0;
        int max_iter = 50;

        attron(COLOR_PAIR(5) | A_BOLD);

        for(int iter = 0; iter < max_iter; iter++) {
            //clear();

            for(int j = 0; j < i; j++) {
                mvprintw(start_y + j, start_x, "%s", art[j]);
            }

            float progress = (float)iter / max_iter;
            int offset = progress * strlen(art[i]);

            switch(dir) {
                case 0:
                case 2:
                    mvprintw(start_y + i, start_x, "%.*s", offset, art[i]);
                    break;
                case 1:
                case 3:
                    mvprintw(start_y + i, start_x + strlen(art[i]) - offset, "%s", art[i] + strlen(art[i]) - offset);
                    break;
                // case 2:
                //     for(int c = 0; c < offset; c++) {
                //         mvaddch(start_y + i, start_x + c, art[i][c]);
                //     }
                //     break;
                // case 3:
                //     mvprintw(start_y + i - (max_iter - iter), start_x, "%s", art[i]);
                //     break;
            }

            if(iter < max_iter - 1) {
                mvaddch(start_y + i, start_x + offset,
                        animation_chars[iter % sizeof(animation_chars)]);
            }

            refresh();
            usleep(delay * 100 - timeOffSet );

            if ( timeOffSet < 400 ) timeOffSet += 100;
        }

        mvprintw(start_y + i, start_x, "%s", art[i]);
        attroff(COLOR_PAIR(5) | A_BOLD);
        refresh();
    }

    // Wait for key press
    while(getch() == ERR) {
        usleep(10000);
    }

}



void show_game_title_small(){
    char *art[] = {
        "  _____                   __  __             _               ",
        " |_   _|   _ _ __   ___  |  \\/  | ___  _ __ | | _____ _   _  ",
        "   | || | | | '_ \\ / _ \\ | |\\/| |/ _ \\| '_ \\| |/ / _ \\ | | | ",
        "   | || |_| | |_) |  __/ | |  | | (_) | | | |   <  __/ |_| | ",
        "   |_| \\__, | .__/ \\___| |_|  |_|\\___/|_| |_|_|\\_\\___|\\__, | ",
        "       |___/|_|                                       |___/  ",
    };
    int art_lines = sizeof(art)/sizeof(art[0]);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int start_y = max_y/2 - art_lines/2;
    int start_x = max_x/2 - max_x/2 + 5;

    int directions[] = {0, 1, 2, 3, 0, 1}; // 0=left,1=right,2=top,3=bottom
    const char animation_chars[] = "/|\\-";

    int timeOffSet = 1;

    srand(time(NULL));

    for(int i = 0; i < art_lines; i++) {
        int dir = directions[i % art_lines];
        int delay = 50;
        int pos = 0;
        int max_iter = 50;

        attron(COLOR_PAIR(4) | A_BOLD);

        for(int iter = 0; iter < max_iter; iter++) {
            //clear();

            for(int j = 0; j < i; j++) {
                mvprintw(start_y + j, start_x, "%s", art[j]);
            }

            float progress = (float)iter / max_iter;
            int offset = progress * strlen(art[i]);

            switch(dir) {
                case 0:
                case 2:
                    mvprintw(start_y + i, start_x, "%.*s", offset, art[i]);
                    break;
                case 1:
                case 3:
                    mvprintw(start_y + i, start_x + strlen(art[i]) - offset, "%s", art[i] + strlen(art[i]) - offset);
                    break;
                    // case 2:
                    //     for(int c = 0; c < offset; c++) {
                    //         mvaddch(start_y + i, start_x + c, art[i][c]);
                    //     }
                    //     break;
                    // case 3:
                    //     mvprintw(start_y + i - (max_iter - iter), start_x, "%s", art[i]);
                    //     break;
            }

            if(iter < max_iter - 1) {
                mvaddch(start_y + i, start_x + offset,
                        animation_chars[iter % sizeof(animation_chars)]);
            }

            refresh();
            usleep(delay * 100 - timeOffSet );

            if ( timeOffSet < 400 ) timeOffSet += 100;
        }

        mvprintw(start_y + i, start_x, "%s", art[i]);
        attroff(COLOR_PAIR(4) | A_BOLD);
        refresh();
    }

    // Wait for key press
    while(getch() == ERR) {
        usleep(500);
    }
}



// void show_game_title_small(){
//     int max_x = getmaxx(stdscr);
//     const char *title = "TYPE MONKEY";
//     int title_len = strlen(title);
//     int box_width = title_len + 4; // Padding for box
//     int box_height = 4;
//
//     // Define color pairs
//     init_pair(7, COLOR_YELLOW, COLOR_WHITE);  // Box color
//     init_pair(8, COLOR_WHITE, COLOR_CYAN);    // Text color
//
//     int box_start = (max_x - box_width) / 4 - 15;
//     int box_up    = LINES/2 - 4;
//     int text_start = box_start + 2;
//     attron(COLOR_PAIR(7));
//
//     for(int i = 0; i < box_width; i++) {
//         if(i == 0) mvaddch(box_up, box_start + i, ACS_ULCORNER);
//         else if(i == box_width - 1) mvaddch(box_up, box_start + i, ACS_URCORNER);
//         // else mvaddch(box_up, box_start + i, ACS_HLINE);
//         // mvaddch(box_up + 4, box_start + i, ACS_HLINE);
//
//         refresh();
//         napms(10);
//     }
//
//
//     // Close bottom corners
//     mvaddch(box_up  + 4, box_start + box_width - 1, ACS_LRCORNER);
//     mvaddch(box_up  + 4, box_start, ACS_LLCORNER);
//
//     attroff(COLOR_PAIR(7));
//
//     // Animate text typing
//     attron(COLOR_PAIR(8) | A_BOLD);
//     for(int i = 0; i < title_len; i++) {
//         mvaddch(box_up + 2, text_start + i, title[i]);
//         refresh();
//         napms(100); // Typing speed
//         beep();     // Add typewriter sound effect
//     }
//     attroff(COLOR_PAIR(8) | A_BOLD);
//
//     refresh();
// }



// int showHomeScreen() {
//     int MAX_X, MAX_Y;
//     bool exitHome = false;

//     getmaxyx(stdscr, MAX_Y, MAX_X);

//     int ch = 0;
//     int n_choices = 4;
//     int menu_height = n_choices + 2;
//     int menu_width = 0;

//     const char title[] = "Type Monkey Game";
//     const char info[] = "* Use 'U' or 'D' to move and Enter to select.";
//     const char infoQuit[] = "* Use 'q' to quit.";

//     char choices[][100] = {
//         "Start Game",
//         "Best Score",
//         "Check Scores",
//         "Exit"
//     };

//     for (int i = 0; i < n_choices; ++i) {
//         int len = (int) strlen(choices[i]) + 8;
//         if (len > menu_width) menu_width = len;
//     }

//     ITEM **items = (ITEM **) malloc(sizeof(ITEM*) * (n_choices + 1));
//     items[n_choices] = NULL;
//     for (int i = 0; i < n_choices; ++i) {
//         items[i] = new_item(choices[i], "");
//     }

//     WINDOW* menu_win = newwin(menu_height, menu_width, LINES/2 - menu_height/2, COLS/2 - menu_width/2);
//     MENU* menu = new_menu((ITEM**) items);
//     keypad(menu_win, TRUE);

//     set_menu_win(menu, menu_win);
//     set_menu_sub(menu, derwin(menu_win, n_choices, menu_width - 2, 1, 1));
//     set_menu_mark(menu, " -$ ");

//     if (!menu_win) {
//         printw("Could not get window.");
//         return 0;
//     }

//     box(menu_win, 0, 0 );
//     attron(A_UNDERLINE);
//     mvprintw(LINES/2 - menu_height/2 - 2, COLS/2 - strlen(title)/2, "%s", title);
//     attroff(A_UNDERLINE | COLOR_GREEN);

//     mvprintw(LINES/2 + menu_height/2 + 1, COLS/2 - strlen(info)/2, "%s", info);
//     mvprintw(LINES/2 + menu_height/2 + 2, COLS/2 - strlen(info)/2, "%s", infoQuit);

//     move(LINES-1, COLS-1);

//     post_menu(menu);
//     refresh();
//     wrefresh(menu_win);

//     // nodelay(menu_win, true);
//     while( !exitHome ){
//         ch = wgetch(menu_win);
//         mvprintw( LINES - 2, 1, "CLICKED: %c |", (char) ch );

//         switch (ch) {
//             case 'u':
//             case 'U':
//             case  KEY_UP: // arrow up
//                 mvprintw(LINES - 1, 0, "                    ");
//                 menu_driver(menu, REQ_UP_ITEM);
//                 break;
//             case 'D':
//             case 'd':
//             case  KEY_DOWN:  // arrow down
//                 mvprintw(LINES - 1, 0, "                    ");
//                 menu_driver(menu, REQ_DOWN_ITEM);
//                 break;
//             case 10:    // Enter key
//                 mvprintw( LINES - 2, 1, "TEXT: %s\n", item_name(current_item(menu)));
//                 mvprintw( LINES - 1, 1, "Str Comp ( with EXIT ): %d\n", strcmp(item_name(current_item(menu)), "Exit"));

//                 if ( strcmp( item_name( current_item( menu ) ), choices[0] ) == 0 ){
//                     // showAnimationScreen(false);
//                     // showLevelChoiceScreen();
//                     exitHome = true;
//                 } else if ( strcmp( item_name( current_item( menu ) ), choices[1] ) == 0 ){
//                     // showAnimationScreen(false);
//                     // showBestScoreScreen();
//                     exitHome = true;
//                 } else if ( strcmp( item_name( current_item( menu ) ), choices[2] ) == 0 ){
//                     // showAnimationScreen(false);
//                     // showScoresScreen();
//                     exitHome = true;
//                 } else if ( strcmp( item_name( current_item( menu ) ), choices[3] ) == 0 ){
//                     exitHome = true;
//                     endwin();
//                     exit(0);
//                 }
//                 break;

//             case 'q':
//             case 'Q':
//                 endwin();
//                 exitHome = true;
//                 exit(0);
//                 break;
//             default:
//                 break;
//         }

//         move(LINES-1, COLS-1);
//         wrefresh(menu_win);
//         refresh();
//     }

//     unpost_menu(menu);
//     free_menu(menu);
//     for (int i = 0; i < n_choices; ++i) free_item(items[i]);
//     free(items);
//     wclear(menu_win);
//     delwin(menu_win);

//     return 0;
// }

