#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// Animation styles
#define ANIM_STYLE_TYPING      0
#define ANIM_STYLE_FADE_IN     1
#define ANIM_STYLE_SLIDE_RIGHT 2
#define ANIM_STYLE_BLINK       3

#define BG_BLACK          1
#define BG_CYAN           2
#define BG_MEGENDA        3
#define BG_WHITE          7
#define TEXT_ON_BLACK     4
#define TEXT_ON_CYAN      5
#define TEXT_MENU_SUCCESS 6
#define TEXT_WHITE        8
#define TEXT_BLUE         9
#define TEXT_BLUE_INV     10
#define TEXT_RIGHT        11
#define TEXT_WRONG        12

#define COLOR_BG_BLACK           COLOR_PAIR( BG_BLACK          )
#define COLOR_BG_CYAN            COLOR_PAIR( BG_CYAN           )
#define COLOR_BG_WHITE           COLOR_PAIR( BG_WHITE          )
#define COLOR_TEXT_ON_BLACK      COLOR_PAIR( TEXT_ON_BLACK     )
#define COLOR_TEXT_ON_CYAN       COLOR_PAIR( TEXT_ON_CYAN      )
#define COLOR_TEXT_MENU_SUCCESS  COLOR_PAIR( TEXT_MENU_SUCCESS )
#define COLOR_TEXT_WHITE         COLOR_PAIR( TEXT_WHITE        )
#define COLOR_TEXT_BLUE          COLOR_PAIR( TEXT_BLUE         )
#define COLOR_TEXT_BLUE_INV      COLOR_PAIR( TEXT_BLUE_INV     )
#define COLOR_TEXT_RIGHT         COLOR_PAIR( TEXT_RIGHT        )
#define COLOR_TEXT_WRONG         COLOR_PAIR( TEXT_WRONG        )


// global variables
int  max_y, max_x;
char WORDS[4][200][50], selecetdWords[60][50], typedWords[60][50], currentWord[50];
int  numWords        = 0, correctWordsTyped = 0, correctCharsTyped = 0, 
     totalCharsTyped = 0, currentWordIndex  = 0, currentLeterIndex = 0,
     timePassed      = 0, LEVEL = 0;
float WPM = 0, CPS = 0, ACCURACY = 0;

const int LEVEL_WORDS[4] = { 0, 30, 45, 60 };
const int LEVEL_TIME[4]  = { 0, 60, 50, 40 };
const int LEVEL_LINE[4]  = { 0, 3, 5, 9 };
int  WORD_LIST_SIZES[4]  = { };

// methods for printing line or block of texts
void animate_forward_full(int max_y,  int max_x);
void animate_backward_half(int max_y, int max_x);
void animate_forward_half(int max_y,  int max_x);
void animate_forward(int max_y,       int max_x);
void animate_backward(int max_y,      int max_x);
void show_game_title_small();
void print_type_monkey();
void print_in_middle();
void animate_print_title(int starty, int startx, const char *title);

// utility functions
void loadWords();
void getRandomWords(int level );

// screens
void homeScreen();
void levelChoiceScreen();
void typingScreen(int level);
void typingEndScreen();
void bestScoreScreen();
void allScoresScreen();


int main(){
    loadWords();
    freopen("/dev/tty", "r", stdin);
    getRandomWords(1);
    initscr();
    start_color();
    curs_set(0);
    noecho();
    cbreak();

    init_pair( BG_BLACK,          COLOR_CYAN,  COLOR_BLACK );
    init_pair( BG_CYAN,           COLOR_WHITE, COLOR_CYAN  );
    init_pair( BG_WHITE,          COLOR_CYAN,  COLOR_WHITE );
    init_pair( TEXT_ON_BLACK,     COLOR_WHITE, COLOR_CYAN  );
    init_pair( TEXT_ON_CYAN,      COLOR_WHITE, COLOR_CYAN  );
    init_pair( TEXT_MENU_SUCCESS, COLOR_CYAN,  COLOR_WHITE );
    init_pair( TEXT_WHITE,        COLOR_WHITE, COLOR_CYAN  );
    init_pair( TEXT_BLUE,         COLOR_WHITE, COLOR_BLUE  );
    init_pair( TEXT_BLUE_INV,     COLOR_BLUE,  COLOR_WHITE );
    init_pair( TEXT_RIGHT,        COLOR_BLACK, COLOR_GREEN );
    init_pair( TEXT_WRONG,        COLOR_BLACK, COLOR_RED   );

    
    getRandomWords(1);

    getmaxyx(stdscr, max_y, max_x);

    animate_forward_full(max_y, max_x);
    print_type_monkey();


    while ( TRUE ) {
        homeScreen();
    }

    
    refresh();
    endwin();
}


void animate_forward_full(int max_y, int max_x) {
    attron(COLOR_BG_CYAN);
    for(int col = 0; col < max_x; col++) {
        for(int y = 0; y < max_y; y++) {
            float ratio = (float)col / max_x;
            mvaddch(y, col, ' ');
        }
        refresh();
        napms(10);
    }
    attroff(COLOR_BG_CYAN);
}

void animate_forward_half(int max_y, int max_x) {
    attron(COLOR_BG_BLACK);
    for(int col = 0; col < max_x/2 - 15; col++) {
        for(int y = 0; y < max_y; y++) {
            float ratio = (float)col / max_x;
            mvaddch(y, col, ' ');
        }
        refresh();
        napms(8);
    }
    attroff(COLOR_BG_BLACK);
}

void animate_forward(int max_y, int max_x) {
    attron(COLOR_BG_BLACK);
    for(int col = 0; col < max_x; col++) {
        for(int y = 0; y < max_y; y++) {
            float ratio = (float)col / max_x;
            int color = ratio < 0.5 ? 1 : 2;
            mvaddch(y, col, ' ');
        }
        refresh();
        napms(10);
    }
    attroff(COLOR_BG_BLACK);
}

void animate_backward(int max_y, int max_x) {
    attron(COLOR_BG_CYAN);
    for(int col = max_x-1; col >= 0; col--) {
        for(int y = 0; y < max_y; y++) {
            float ratio = (float)col / max_x;
            int color = ratio > 0.5 ? 1 : 2;
            mvaddch(y, col, ' ');
        }
        refresh();
        napms(10);
    }
    attroff(COLOR_BG_CYAN);
}

void animate_backward_half(int max_y, int max_x) {
    attron(COLOR_BG_CYAN);
    for(int col = max_x-1; col >= max_x/2 - 15; col--) {
        for(int y = 0; y < max_y; y++) {
            mvaddch(y, col, ' ');
        }
        refresh();
        napms(12);
    }
    attroff(COLOR_BG_CYAN);
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

        attron(COLOR_TEXT_ON_CYAN | A_BOLD);

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
        attroff(COLOR_TEXT_ON_CYAN | A_BOLD);
        refresh();
    }

    // Wait for key press
    // while(getch() == ERR) {
    //     usleep(10000);
    // }

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
    int start_y = max_y/2 - art_lines/2 - 10;
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
        attron(COLOR_TEXT_ON_BLACK  | A_BOLD);
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
        attroff(COLOR_TEXT_ON_BLACK | A_BOLD);
        refresh();
    }

    // Wait for key press
    // while(getch() == ERR) {
    //     usleep(500);
    // }
}

void animate_print_title(int starty, int startx, const char *title) {
    int delay_ms = 100;
    int length = strlen(title);
    int x, i;
    int style = ANIM_STYLE_TYPING;
    
    int old_y, old_x;
    getyx(stdscr, old_y, old_x);
    
    switch(style) {
        case ANIM_STYLE_TYPING:
            for (x = 0; x < length; x++) {
                mvaddch(starty, startx + x, title[x]);
                refresh();
                usleep(delay_ms * 1000);
            }
            break;
            
        case ANIM_STYLE_FADE_IN:
            for (i = 0; i < 4; i++) {
                mvprintw(starty, startx, "%s", title);
                switch(i) {
                    case 0: attrset(A_DIM); break;
                    case 1: attrset(A_NORMAL); break;
                    case 2: attrset(A_BOLD); break;
                    case 3: attrset(A_BOLD | A_STANDOUT); break;
                }
                refresh();
                usleep(delay_ms * 2000);
            }
            attrset(A_NORMAL);
            break;
            
        case ANIM_STYLE_SLIDE_RIGHT:
            for (x = 0; x <= startx; x++) {
                mvprintw(starty, x, "%s", title);
                refresh();
                if (x < startx) {
                    mvprintw(starty, x, "%*s", length, "");  // Clear previous position
                }
                usleep(delay_ms * 500);
            }
            break;
            
        case ANIM_STYLE_BLINK:
            // Blink effect
            mvprintw(starty, startx, "%s", title);
            for (i = 0; i < 3; i++) {  // Blink 3 times
                attrset(A_NORMAL);
                refresh();
                usleep(delay_ms * 2000);
                attrset(A_REVERSE);
                refresh();
                usleep(delay_ms * 2000);
            }
            attrset(A_NORMAL);
            break;
    }
    
    move(old_y, old_x);
    refresh();
}

void print_in_middle_animate(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{
    int length, x, y;
    float temp;
    int delay_us = 50000;
    
    if(win == NULL)
        win = stdscr;
    getyx(win, y, x);
    
    if(startx != 0)
        x = startx;
    if(starty != 0)
        y = starty;
    if(width == 0)
        width = 80;
    
    length = strlen(string);
    temp = (width - length) / 2;
    x = startx + (int)temp;
    
    wattron(win, color | A_BOLD);
    for(int i = 0; i < length; i++) {
        mvwaddch(win, y, x + i, string[i]);
        wrefresh(win);
        usleep(delay_us);
    }
    wattroff(win, color | A_BOLD);
    wrefresh(win);
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{
    int length, x, y;
    float temp;
    if(win == NULL)
        win = stdscr;
    getyx(win, y, x);
    if(startx != 0)
    x = startx;
    if(starty != 0)
        y = starty;
    if(width == 0)
        width = 80;
    length = strlen(string);
        temp = (width - length) / 2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
} 

void animate_borders(WINDOW *win, int delay_ms) {
    int height, width;
    getmaxyx(win, height, width);

    for(int x = 0; x < width; x++) {
        mvwaddch(win, 0, x, ACS_HLINE);
        wrefresh(win);
        usleep(delay_ms * 200);
    }
    
    for(int y = 0; y < height; y++) {
        mvwaddch(win, y, width-1, ACS_VLINE);
        wrefresh(win);
        usleep(delay_ms * 200);
    }
    
    for(int x = width-1; x >= 0; x--) {
        mvwaddch(win, height-1, x, ACS_HLINE);
        wrefresh(win);
        usleep(delay_ms * 200);
    }
    
    for(int y = height-1; y >= 0; y--) {
        mvwaddch(win, y, 0, ACS_VLINE);
        wrefresh(win);
        usleep(delay_ms * 200);
    }
    
    mvwaddch(win, 0, 0, ACS_ULCORNER);
    mvwaddch(win, 0, width-1, ACS_URCORNER);
    mvwaddch(win, height-1, 0, ACS_LLCORNER);
    mvwaddch(win, height-1, width-1, ACS_LRCORNER);
    wrefresh(win);
}

// UTILITY

void loadWords(){
    int words_size = 0;
    for(int i = 1; i <= 3; ++i){
        scanf("%d", &WORD_LIST_SIZES[i]);
        for( int j = 0; j < WORD_LIST_SIZES[i]; ++j ){
            fgets(WORDS[i][j], sizeof(char)*100, stdin);

            size_t len = strlen(WORDS[i][j]);
            if (len > 0 && WORDS[i][j][len - 1] == '\n') {
                if ( WORDS[i][j][len - 2] != ' ' ) WORDS[i][j][len - 1] = ' ';
                else WORDS[i][j][len - 1] = '\0';
            }
        }
    }
}

void getRandomWords(int level){
    srand(time(NULL));
    int words_size = LEVEL_WORDS[level];

    for (int i = 0; i < words_size; ++i) {
        int random_index = rand() % WORD_LIST_SIZES[level];
        strcpy(selecetdWords[i], WORDS[level][random_index]);
    }
}


// SCREENS

void homeScreen(){

    int c;
    WINDOW *my_menu_win, *my_win;
    int n_choices, i;
    int selectedIndex = 0;

    bool isScreen = true;

    animate_backward_half(max_y, max_x);
    animate_forward_half(max_y, max_x);

    napms(1*20);
    show_game_title_small();

    char *choices[] = {
        "Start Game",
        "Best Score",
        "Check Scores",
        "Exit",
        (char *)NULL,
    };
    
    const char info[] = "* Use 'U' or 'D' to move and Enter to select.";
    const char infoQuit[] = "* Use 'q' to quit.";

    const char *title = "Select a option";
    const char *titleUnderline = "---------------";
    const int length = strlen(title);
    
    
    attron(COLOR_TEXT_WHITE | A_BOLD);
    animate_print_title(LINES/3 - 5, COLS*3/4 - length/2, title);
    animate_print_title(LINES/3 - 4, COLS*3/4 - length/2, titleUnderline);
    attroff(COLOR_TEXT_WHITE | A_BOLD);


    n_choices = ARRAY_SIZE(choices) - 1;

    my_menu_win = newwin(4 * n_choices + 1, 40, LINES/3 - 1, COLS*3/4 - 20);
    keypad(my_menu_win, TRUE);
    wrefresh(my_menu_win);
    nodelay(my_menu_win, TRUE);

    wbkgd(my_menu_win, COLOR_BG_CYAN);


    box(my_menu_win, 0, 0);
        
    mvwaddch(my_menu_win, 4, 0,  ACS_LTEE );
    mvwhline(my_menu_win, 4, 1,  ACS_HLINE, 38 );
    mvwaddch(my_menu_win, 4, 39, ACS_RTEE );

    for(i = 0; i < n_choices; ++i) {
        if (i == selectedIndex) {
            wattron(my_menu_win, COLOR_TEXT_WHITE);
            mvwprintw(my_menu_win, 2 + i*4, 3, ">");
            wattroff(my_menu_win, COLOR_TEXT_WHITE);
        } else {
            mvwprintw(my_menu_win, 2 + i*4, 3, " ");
        }

        print_in_middle_animate(my_menu_win, 2 + i*4, 1, 40, choices[i], 
                    selectedIndex == i ? COLOR_TEXT_MENU_SUCCESS | A_BOLD : COLOR_TEXT_WHITE );

        if (i < n_choices - 1) {  // Don't draw line after last item
            mvwhline(my_menu_win, 4 + i*4, 1, ACS_HLINE, 38);
            mvwaddch(my_menu_win, 4 + i*4, 0, ACS_LTEE);
            mvwaddch(my_menu_win, 4 + i*4, 39, ACS_RTEE);
        }
    }

    while((c = wgetch(my_menu_win)) != KEY_F(1) && isScreen)  // F1 to exit
    {

        for(i = 0; i < n_choices; ++i) {
            if (i == selectedIndex) {
                wattron(my_menu_win, COLOR_TEXT_WHITE);
                mvwprintw(my_menu_win, 2 + i*4, 3, ">");
                wattroff(my_menu_win, COLOR_TEXT_WHITE);
            } else {
                mvwprintw(my_menu_win, 2 + i*4, 3, " ");
            }

            print_in_middle(my_menu_win, 2 + i*4, 1, 40, choices[i], 
                        selectedIndex == i ? COLOR_TEXT_MENU_SUCCESS | A_BOLD : COLOR_TEXT_WHITE );

            if (i < n_choices - 1) {  // Don't draw line after last item
                mvwhline(my_menu_win, 4 + i*4, 1, ACS_HLINE, 38);
                mvwaddch(my_menu_win, 4 + i*4, 0, ACS_LTEE);
                mvwaddch(my_menu_win, 4 + i*4, 39, ACS_RTEE);
            }
        }

        attron(COLOR_TEXT_WHITE);
        mvprintw(LINES - 5, COLS/2 + 10, " * Use PageUp and PageDown to scroll to first/last item");
        mvprintw(LINES - 4, COLS/2 + 10, " * Arrow Keys to navigate (F1 to Exit)");
        attroff(COLOR_TEXT_WHITE);


        switch(c)
        {
            case KEY_DOWN:
                selectedIndex = (selectedIndex + 1) % n_choices;  // Wrap around
                break;
            case KEY_UP:
                selectedIndex = (selectedIndex - 1 + n_choices) % n_choices;  // Wrap around
                break;
            case KEY_NPAGE:  // Page down - jump to last item
                selectedIndex = n_choices - 1;
                break;
            case KEY_PPAGE:  // Page up - jump to first item
                selectedIndex = 0;
                break;
            case 10: // catching ENTER press 
                if( selectedIndex == 0 ){
                    levelChoiceScreen();
                    isScreen = false;
                } else if ( selectedIndex == 1 ){
                    bestScoreScreen();
                    isScreen = false;
                } else if ( selectedIndex == 2 ) {
                    allScoresScreen();
                    isScreen = false;
                } else if ( selectedIndex == 3 ) {
                    endwin();
                    exit(0);
                }
                break;
        }
        wrefresh(my_menu_win);
        refresh();

        if ( !isScreen ) break;
    }
}

void levelChoiceScreen(){
    int c;
    WINDOW *my_menu_win, *my_win;
    int n_choices, i;
    int selectedIndex = 0;
    bool isScreen = true;

    animate_backward_half(max_y, max_x);
    animate_forward_half(max_y, max_x);

    napms(1*20);
    show_game_title_small();

    char *choices[] = {
        "Level 1 ( Easy   )",
        "Level 2 ( Medium )",
        "Level 3 ( Hard   )",
        "Go Back",
        "Exit",
        (char *)NULL,
    };
    
    const char info[] = "* Use 'U' or 'D' to move and Enter to select.";
    const char infoQuit[] = "* Use 'q' to quit.";

    const char *title = "Select a level";
    const char *titleUnderline = "---------------";
    const int length = strlen(title);
    
    
    attron(COLOR_TEXT_WHITE | A_BOLD);
    animate_print_title(LINES/3 - 5, COLS*3/4 - length/2, title);
    animate_print_title(LINES/3 - 4, COLS*3/4 - length/2, titleUnderline);
    attroff(COLOR_TEXT_WHITE | A_BOLD);


    n_choices = ARRAY_SIZE(choices) - 1;

    my_menu_win = newwin(4 * n_choices + 1, 40, LINES/3 - 1, COLS*3/4 - 20);
    keypad(my_menu_win, TRUE);
    wrefresh(my_menu_win);
    nodelay(my_menu_win, TRUE);

    wbkgd(my_menu_win, COLOR_BG_CYAN);


    box(my_menu_win, 0, 0);
        
    mvwaddch(my_menu_win, 4, 0,  ACS_LTEE );
    mvwhline(my_menu_win, 4, 1,  ACS_HLINE, 38 );
    mvwaddch(my_menu_win, 4, 39, ACS_RTEE );

    for(i = 0; i < n_choices; ++i) {
        if (i == selectedIndex) {
            wattron(my_menu_win, COLOR_TEXT_WHITE);
            mvwprintw(my_menu_win, 2 + i*4, 3, ">");
            wattroff(my_menu_win, COLOR_TEXT_WHITE);
        } else {
            mvwprintw(my_menu_win, 2 + i*4, 3, " ");
        }

        print_in_middle_animate(my_menu_win, 2 + i*4, 1, 40, choices[i], 
                    selectedIndex == i ? COLOR_TEXT_MENU_SUCCESS | A_BOLD : COLOR_TEXT_WHITE );

        if (i < n_choices - 1) {  // Don't draw line after last item
            mvwhline(my_menu_win, 4 + i*4, 1, ACS_HLINE, 38);
            mvwaddch(my_menu_win, 4 + i*4, 0, ACS_LTEE);
            mvwaddch(my_menu_win, 4 + i*4, 39, ACS_RTEE);
        }
    }

    while((c = wgetch(my_menu_win)) != KEY_F(1) && isScreen)  // F1 to exit
    {

        for(i = 0; i < n_choices; ++i) {
            if (i == selectedIndex) {
                wattron(my_menu_win, COLOR_TEXT_WHITE);
                mvwprintw(my_menu_win, 2 + i*4, 3, ">");
                wattroff(my_menu_win, COLOR_TEXT_WHITE);
            } else {
                mvwprintw(my_menu_win, 2 + i*4, 3, " ");
            }

            print_in_middle(my_menu_win, 2 + i*4, 1, 40, choices[i], 
                        selectedIndex == i ? COLOR_TEXT_MENU_SUCCESS | A_BOLD : COLOR_TEXT_WHITE );

            if (i < n_choices - 1) {  // Don't draw line after last item
                mvwhline(my_menu_win, 4 + i*4, 1, ACS_HLINE, 38);
                mvwaddch(my_menu_win, 4 + i*4, 0, ACS_LTEE);
                mvwaddch(my_menu_win, 4 + i*4, 39, ACS_RTEE);
            }
        }

        attron(COLOR_TEXT_WHITE);
        mvprintw(LINES - 5, COLS/2 + 10, " * Use PageUp and PageDown to scroll to first/last item");
        mvprintw(LINES - 4, COLS/2 + 10, " * Arrow Keys to navigate (F1 to Exit)");
        attroff(COLOR_TEXT_WHITE);

        switch(c){
            case KEY_DOWN:
                selectedIndex = (selectedIndex + 1) % n_choices;  // Wrap around
                break;
            case KEY_UP:
                selectedIndex = (selectedIndex - 1 + n_choices) % n_choices;  // Wrap around
                break;
            case KEY_NPAGE:  // Page down - jump to last item
                selectedIndex = n_choices - 1;
                break;
            case KEY_PPAGE:  // Page up - jump to first item
                selectedIndex = 0;
                break;
            case 10: // catching ENTER press 
                if( selectedIndex == 0 ){
                    typingScreen(1);
                    isScreen = false;
                } else if ( selectedIndex == 1 ){
                    typingScreen(2);
                    isScreen = false;
                } else if ( selectedIndex == 2 ) {
                    typingScreen(3);
                    isScreen = false;
                } else if ( selectedIndex == 3 ) {
                    homeScreen();
                    isScreen = false;
                } else if ( selectedIndex == 4 ) {
                    endwin();
                    exit(0);
                }
                break;
        }

        // mvprintw(0, 0, "CLICK: %d\n", selectedIndex);
        wrefresh(my_menu_win);
        refresh();
    }
}

void typingScreen(int level) {

    LEVEL = level;

    numWords = LEVEL_WORDS[level];

    getRandomWords(level);

    // printf("%s\n", selecetdWords[0]);

    const char RULE1[] = " * Press Ctrl+C or TAB to exit the game.";
    const char RULE2[] = " * Press ESC to exit to home screen.    ";
    const char RULE3[] = " * Press END button to restart the game.";


    int c;
    WINDOW *textWin, *statsWin;
    bool isScreen = true;
    int delay_ms = 20;
    
    int win_height = LINES * 3/4;
    int text_width = COLS/2 - 4;
    int stats_width = COLS/2 - 4;

    animate_backward_half(max_y, max_x);
    animate_forward_half(max_y, max_x);
    show_game_title_small();

    statsWin = newwin(10, COLS/3 - 2, LINES/2 + 6, 6 );

    int startTime = time(NULL);
    int endTime   = time(NULL);

    nodelay( stdscr, TRUE );
    keypad(  stdscr, TRUE );

    wattron  (statsWin, A_BOLD);
    mvwprintw(statsWin, 2, 2, "Level     :  %d ", level);
    mvwprintw(statsWin, 3, 2, "Word count:  %d ", LEVEL_WORDS[level]);
    mvwprintw(statsWin, 4, 2, "Time      :  %d ", LEVEL_TIME[level] - ( endTime - startTime ) );
    mvwprintw(statsWin, 5, 2, "WPM       : --- ");
    mvwprintw(statsWin, 6, 2, "CPS       : --- ");
    mvwprintw(statsWin, 7, 2, "Accuracy  : ---%%");
    wattron  (statsWin, A_BOLD);

    
    while(isScreen && (c = getch()) != KEY_F(1)) {
    
        endTime = time(NULL);
        timePassed = endTime - startTime;

        box(statsWin, 0, 0);

        wattron(statsWin, COLOR_PAIR(2));
        mvwprintw(statsWin, 0, 0, "Statistics");
        wattroff(statsWin, COLOR_PAIR(2));
        
        wattron  (statsWin, A_BOLD);
        mvwprintw(statsWin, 2, 2, "Level     :  %d  ", level);
        mvwprintw(statsWin, 3, 2, "Word count:  %d  ", LEVEL_WORDS[level]);
        mvwprintw(statsWin, 4, 2, "Time      :  %d  ", LEVEL_TIME[level] - ( endTime - startTime ) );

        if ( timePassed > 0 ){
            mvwprintw(statsWin, 5, 2, "WPM       :  %f  ", WPM);
            mvwprintw(statsWin, 6, 2, "CPS       :  %f  ", CPS);
        }
        if ( totalCharsTyped > 0 ) mvwprintw(statsWin, 7, 2, "Accuracy  :  %f%%  ", ACCURACY );
        wattron  (statsWin, A_BOLD);

        int row = 2;

        attron(COLOR_TEXT_WHITE | A_BOLD);
        mvprintw(row++, COLS/3 + 20, "TEXT TO TYPE: ");
        mvprintw(row++, COLS/3 + 20, "----------------------------------------------------");
        attroff(COLOR_TEXT_WHITE | A_BOLD);
        
        
        // printing the text assigned to write
        int column = 0;
        bool isCorrectWord = true;
        correctCharsTyped = 0;
        totalCharsTyped   = 0;
        correctWordsTyped = 0;

        
        // printing character by character and keep track of correct characters and words
        for( int i = 0; i < numWords; ++i ){
            isCorrectWord = true;

            int len = strlen(selecetdWords[i]);
            for( int j = 0; j < len; ++j ){
                if ( i < currentWordIndex || (i == currentWordIndex && j < currentLeterIndex) ){
                    if ( typedWords[i][j] == selecetdWords[i][j] ){
                        attron(COLOR_TEXT_BLUE | A_BOLD);
                        correctCharsTyped++;
                    } else {
                        attron(COLOR_TEXT_WRONG | A_BOLD );
                        isCorrectWord = false;
                    }
                    totalCharsTyped++;
                } else {
                    attron(COLOR_TEXT_WHITE | A_BOLD);
                    isCorrectWord = false;
                }
                
                mvprintw(row, COLS/3 + 20 + column, "%c", selecetdWords[i][j] );
                attroff(COLOR_TEXT_RIGHT | COLOR_TEXT_WRONG | COLOR_TEXT_WHITE | A_BOLD);

                column++;
                if ( COLS/3 + 20 + column >= COLS - 1 ){
                    column = 0;
                    row++;
                }
            }

            if ( isCorrectWord ) correctWordsTyped++;
        }

        // calculation
        WPM      = correctWordsTyped*60.0 / timePassed;
        CPS      = correctCharsTyped*1.0  / timePassed;
        ACCURACY =  correctCharsTyped*100.0/totalCharsTyped;


        // right side part here
        attron(COLOR_TEXT_WHITE | A_BOLD);

        mvprintw(row + 2,  COLS/3 + 20, "---------------------------------------------------- %d", row );
        mvprintw(row + 4,  COLS/3 + 20, "                                                    " );
        mvprintw(row + 4,  COLS/3 + 20, "Current word: " );
        attron(A_UNDERLINE | A_BLINK | TEXT_MENU_SUCCESS );
        printw("%s", typedWords[currentWordIndex]);
        attroff(A_UNDERLINE | A_BLINK | TEXT_MENU_SUCCESS);

        mvprintw(row + 5,  COLS/3 + 20, "TIME LEFT  :  %d", LEVEL_TIME[level] - (endTime - startTime) );

        row += 8;

        mvprintw(row++, COLS/3 + 20, "Your text : " );
        mvprintw(row++, COLS/3 + 20, "----------------------------------------------------");
        
        column = 0;
        // print new letters
        for( int i = 0; i <= numWords; ++i){
            int len = strlen(selecetdWords[i]);
            for( int j = 0; j < len; ++j ){

                if ( i < currentWordIndex ){
                    mvprintw( row, COLS/3 + 20 + column, "%c", typedWords[i][j] );
                } else if ( i == currentWordIndex ){
                    if (j < currentLeterIndex ) mvprintw( row, COLS/3 + 20 + column, "%c", typedWords[i][j] );
                    else  mvprintw( row, COLS/3 + 20 + column, " " );
                } else {
                    mvprintw( row, COLS/3 + 20 + column, " " );
                }

                column++;
                if ( COLS/3 + 20 + column >= COLS - 1 ){
                    column = 0;
                    row++;
                }
            }
        }

        attroff(COLOR_TEXT_WHITE | A_BOLD);


        attron(COLOR_TEXT_WHITE);
        mvprintw( LINES - 4, COLS/3 + 20, "%s", RULE3 );
        mvprintw( LINES - 3, COLS/3 + 20, "%s", RULE1 );
        mvprintw( LINES - 2, COLS/3 + 20, "%s", RULE2 );
        attroff(COLOR_TEXT_WHITE);
        

        if (c != ERR) {
            if (c == KEY_ENTER || c == 10)  // ENTER
                continue;
            
            if (c == 27) {  // EScape
                isScreen = false;
                break;
            }
            else if (c == KEY_END) {
                typingScreen(level);
                isScreen = false;
                break;
            }
            else if (c == KEY_BACKSPACE || c == 127) {
                if ( currentLeterIndex == 1 && currentWordIndex == 0 ){
                    typedWords[currentWordIndex][--currentLeterIndex]  = '\0';
                }
                else if (currentLeterIndex > 1) {
                    currentLeterIndex--;
                    typedWords[currentWordIndex][currentLeterIndex] = '\0';
                    
                    // mvprintw(LINES/2, COLS/2, "%-50s", " ");  // using this for clearing previous message
                } 
                else if ( currentWordIndex > 0) {
                    currentWordIndex--;
                    currentLeterIndex = strlen(typedWords[currentWordIndex]);

                    currentLeterIndex--;
                    typedWords[currentWordIndex][currentLeterIndex] = '\0';
                    
                    mvprintw(LINES/2, COLS/2, "%-50s", " ");
                }

                if ( currentWordIndex == numWords && currentLeterIndex == strlen(selecetdWords[numWords-1]) ){
                    typingEndScreen();
                    isScreen = false;
                    break;
                }
            }
            else if ( c != ' ' && typedWords[currentWordIndex][currentLeterIndex-1] == ' ' ) {
                currentWordIndex++;
                currentLeterIndex = 0;
                typedWords[currentWordIndex][currentLeterIndex++] = c;
                typedWords[currentWordIndex][currentLeterIndex] = '\0';
            }
            else {
                typedWords[currentWordIndex][currentLeterIndex++] = c;
                typedWords[currentWordIndex][currentLeterIndex] = '\0';
            }
        }


        if ( timePassed > LEVEL_TIME[level] ){
            typingEndScreen();
            isScreen = false;
            break;
        }
        
        wrefresh(statsWin);
        refresh();
    }
    
    delwin(statsWin);
}

void typingEndScreen(){

    currentLeterIndex = 0;
    currentLeterIndex = 0;
    numWords          = 0;

    keypad(stdscr, TRUE);

    char playerName[100] = {'\0'};
    int index            = 0;
    bool exitScreen      = false;

    FILE* scoresFile    = fopen("./data_files/scores.txt",     "a" );
    FILE* bestScoreFile = fopen("./data_files/best_score.txt", "r" );

    const char* INS_SAVE         = " * Press ENTER to save the score.";
    const char* INS              = " * Press ESC return to home screen without saving.";
    const char* INSExit          = " * Press BACKSPACE to exit the game.";
    const char* INSRestart       = " * Press TAB to restart typing.";
    const char* NameErrorMessage = "Please type a name withing 10 characters";

    const int INS_SAVE_LENGTH = strlen(INS_SAVE);
    const int INS_LENGTH      = strlen(INS);
    const int INSExitLength   = strlen(INSExit);
    const int INSRestartLength = strlen(INSExit);
    const int NameErrorLength = strlen(NameErrorMessage);

    const int MAX_NAME_LENGTH = 10;
    bool hasErrorInName       = false;

    char  BEST_SCORE_NAME[50];
    float BEST_SCORE_WPM     = 0.0;
    float BEST_SCORE_CPM     = 0.0;
    int   BEST_SCORE_SECONDS = 0.0;

     // Reading best score
    fscanf(bestScoreFile, "%[^\n]%*c", BEST_SCORE_NAME); // skip name
    fscanf(bestScoreFile, "%f", &BEST_SCORE_WPM);
    fscanf(bestScoreFile, "%f", &BEST_SCORE_CPM);
    fscanf(bestScoreFile, "%d", &BEST_SCORE_SECONDS); // skip accuracy
    fscanf(bestScoreFile, "%d", &BEST_SCORE_SECONDS);

    while( !exitScreen ){
        clear();

        start_color();
        init_pair(0, COLOR_RED, COLOR_WHITE );

        attron(A_UNDERLINE | A_BOLD);
        mvprintw(LINES/2 - 5, COLS/2 - 10, "Typing Complete");
        attroff(A_UNDERLINE | A_BOLD);

        mvprintw(LINES/2 - 3, COLS/2 - 15, "WPM     : %f", WPM);
        mvprintw(LINES/2 - 2, COLS/2 - 15, "CPS     : %f", CPS);
        mvprintw(LINES/2 - 1, COLS/2 - 15, "Accuracy: %f", ACCURACY);
        mvprintw(LINES/2 - 0, COLS/2 - 15, "Time (S): %d", timePassed);

        attron(COLOR_PAIR(1));
        mvprintw(LINES/2 + 2, COLS/2 - 15, "Write you name:  %s  ", playerName);
        attroff(COLOR_PAIR(1));

        mvprintw(LINES/2 + 4, (COLS - INS_SAVE_LENGTH ) / 2, "%s", INS_SAVE   );
        mvprintw(LINES/2 + 5, (COLS - INS_LENGTH      ) / 2, "%s", INS        );
        mvprintw(LINES/2 + 6, (COLS - INSRestartLength) / 2, "%s", INSRestart );
        mvprintw(LINES/2 + 7, (COLS - INSExitLength   ) / 2, "%s", INSExit    );

        if ( hasErrorInName ){
            attron(COLOR_PAIR(2));
            mvprintw(LINES - 2, 2, "%s", NameErrorMessage);
            attroff(COLOR_PAIR(2));
        }

        int ch = getch();
        if (ch == 27) {  // ESC key
            endwin();
            exit(0);
        } else if ( ch == 127 || ch == 121 || ch == KEY_BACKSPACE ){  // backspace, KEY_BACKSPACE not working 
                playerName[index-1] = '\0';
                index -= 1;
                if ( index < MAX_NAME_LENGTH ) hasErrorInName = false;
                if ( playerName[0] == '\0' ) {
                    endwin();
                    exitScreen = true;
                    break;
                }
        }
        else if ( ch == 10 || ch == KEY_ENTER ){ // enter
            if ( index == 0 ){
                hasErrorInName = true;
            }
            if ( index > 0 && !hasErrorInName ){
                fprintf(scoresFile, "%s\n", playerName);
                fprintf(scoresFile, "%f\n", WPM);
                fprintf(scoresFile, "%f\n", CPS);
                fprintf(scoresFile, "%f\n", ACCURACY);
                fprintf(scoresFile, "%d\n", timePassed);

                if ( WPM > BEST_SCORE_WPM  ){
                    FILE* bestScoreFileWrite = fopen("./data_files/best_score.txt", "w");
                    fprintf(bestScoreFileWrite, "%s\n", playerName);
                    fprintf(bestScoreFileWrite, "%f\n", WPM);
                    fprintf(bestScoreFileWrite, "%f\n", CPS);
                    fprintf(bestScoreFileWrite, "%f\n", ACCURACY);
                    fprintf(bestScoreFileWrite, "%d\n", timePassed);
                    fclose(bestScoreFileWrite);
                }
                exitScreen = true;
                break;
            }
        } else if ( ( ch >= 'a' && ch <= 'z') || ( ch >= 'A' && ch <= 'Z')  ) {
            if ( index < MAX_NAME_LENGTH ){
                playerName[index++] = ch;
                playerName[index] = '\0';
                hasErrorInName = false;
            }else{
                hasErrorInName = true;
                continue;
            }
        } else if ( ch == KEY_STAB ){
            levelChoiceScreen();
            exitScreen = true;
            break;
        }
        refresh();
        napms(60);
    }


    fclose(bestScoreFile);
    fclose(scoresFile);
    // endwin()
}

void bestScoreScreen(){
    keypad(stdscr, true);
    int exitScreen = false;

    animate_forward_full(max_y, max_x);

    const char* INS     = " * Press ENTER key to return to home screen.";
    const char* INSExit = " * Press ESC to exit the game.";

    const int INS_LENGTH = strlen(INS);
    const int INSExitLength = strlen(INSExit);

    char NAME[50];
    float WPM, CPS, ACCURACY;
    int TIME_SECONDS;

    FILE* bestScoreFile = fopen("./data_files/best_score.txt", "r");
    fscanf(bestScoreFile, "%s", NAME);
    fscanf(bestScoreFile, "%f", &WPM);
    fscanf(bestScoreFile, "%f", &CPS);
    fscanf(bestScoreFile, "%f", &ACCURACY);
    fscanf(bestScoreFile, "%d", &TIME_SECONDS);
    fclose(bestScoreFile);

    attron(COLOR_TEXT_BLUE | A_BOLD | A_UNDERLINE);
    mvprintw(LINES/2 - 6, COLS/2 - 10, "Best Score");
    attroff(COLOR_TEXT_BLUE | A_BOLD | A_UNDERLINE);

    attron(COLOR_TEXT_WHITE | A_BOLD );
    mvprintw( LINES/2 - 3, COLS/2-16, "Name         : %s ", NAME );
    mvprintw( LINES/2 - 2, COLS/2-16, "WPM          : %f ", WPM );
    mvprintw( LINES/2 - 1, COLS/2-16, "CPS          : %f ", CPS );
    mvprintw( LINES/2 - 0, COLS/2-16, "Accuracy     : %f ", ACCURACY );
    mvprintw( LINES/2 + 1, COLS/2-16, "Time (second): %d ", TIME_SECONDS );
    attroff(COLOR_TEXT_WHITE | A_BOLD );

    attron(COLOR_TEXT_WHITE | A_BLINK );
    mvprintw(LINES/2 + 3, (COLS - INS_LENGTH) / 2, "%s", INS);
    mvprintw(LINES/2 + 4, (COLS - INSExitLength) / 2, "%s", INSExit);
    attroff(COLOR_TEXT_WHITE | A_BLINK );

    refresh();

    while( !exitScreen ){
        int ch = getch();
        if (ch == 27) {  // ESC key
            endwin();
            exit(0);
        }
        else if ( ch == 10 ){ // enter
            homeScreen();
            exitScreen = true;
        }
    }
}

void allScoresScreen(){
    keypad(stdscr, true);
    int exitScreen = false;

    animate_forward_full(max_y, max_x);

    const char* INS     = " * Press any key to return to home screen.";
    const char* INSExit = " * Press ESC to exit the game.";

    const int INS_LENGTH = strlen(INS);
    const int INSExitLength = strlen(INSExit);

    const int TABLE_COLUMN = 5; // name, wpm, cps, accu, time
    const int TABLE_COL_OFFSET = COLS/5;

    attron( COLOR_TEXT_BLUE | A_BOLD);
    
    mvprintw(1, COLS/2 - 10, "########## All Scores #############");
    attroff( COLOR_TEXT_BLUE | A_BOLD);

    attron( COLOR_TEXT_WHITE | A_BOLD);
    mvprintw(3, TABLE_COL_OFFSET*0 + 2, "NAME");
    mvprintw(3, TABLE_COL_OFFSET*1 + 2, "WPM");
    mvprintw(3, TABLE_COL_OFFSET*2 + 2, "CPS");
    mvprintw(3, TABLE_COL_OFFSET*3 + 2, "ACCURACY");
    mvprintw(3, TABLE_COL_OFFSET*4 + 2, "TIME ( seconds )");

    for( int i = 0; i < COLS; ++i ){
        mvprintw(4, i, "-");
    }

    int currentRowNumber = 0;
    char name[50];
    float wpm, cps, accuracy;
    int time;
    currentRowNumber = 6;

    FILE* scoresFile = fopen("./data_files/scores.txt", "r");

    while( fgets(name, sizeof(name), scoresFile) ){
        int len = strlen(name);
        name[len-1] = '\0';

        fscanf(scoresFile, "%f\n", &wpm);
        fscanf(scoresFile, "%f\n", &cps);
        fscanf(scoresFile, "%f\n", &accuracy);
        fscanf(scoresFile, "%d\n", &time);

        mvprintw(currentRowNumber, TABLE_COL_OFFSET*0 + 2, "%s", name );
        mvprintw(currentRowNumber, TABLE_COL_OFFSET*1 + 2, "%f", wpm );
        mvprintw(currentRowNumber, TABLE_COL_OFFSET*2 + 2, "%f", cps );
        mvprintw(currentRowNumber, TABLE_COL_OFFSET*3 + 2, "%f", accuracy );
        mvprintw(currentRowNumber, TABLE_COL_OFFSET*4 + 2, "%d", time );

        currentRowNumber++;
    }

    fclose(scoresFile);

    for( int i = 0; i < COLS; ++i ){
        mvprintw(LINES-3, i, "-");
    }

    attroff( COLOR_TEXT_WHITE| A_BOLD);

    attron( COLOR_TEXT_ON_CYAN);
    mvprintw(LINES - 2, ( COLS - INS_LENGTH ) / 2, "%s", INS);
    mvprintw(LINES - 1, ( COLS - INSExitLength ) / 2, "%s", INSExit);
    attroff( COLOR_TEXT_ON_CYAN );
    refresh();

    while( !exitScreen ){
        int ch = getch();
        if (ch == 27) {  // ESC key
            endwin();
            exit(0);
        }
        else if ( ch == KEY_ENTER || ch == 10 ){ // enter
            homeScreen();
            exitScreen = true;
        }
    }
}