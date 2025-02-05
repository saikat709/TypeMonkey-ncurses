#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int exitGame  = -1, startGame =  1, bestScore =  2, allScores =  3, playAgain =  4;
const int COLOR_CORRECT = 3, COLOR_WRONG = 4, COLOR_NORMAL = 5;

const char text[1000]    = "Coin Change (the special case), Load Balancing, and Interval Covering.\nFor these classical problems, it is helpful to memorize their solutions. \nWe have also discussed an important problem solving strategy usually applicable to greedy problems: \nSorting the input data to elucidate hidden greedy strategies\n";
char content[1000];

// utility functions
char * readFile(char filename[]){
    FILE* f = fopen(filename, "r");
    if ( !f ){
        printf("File not found\n");
    }
    int length = fread(content, 1, 1000 - 1, f);
    content[length] = '\0';

    return content;
}

// screens
int showSelectionScreen(){
    int ch = 0;
    int n_choices = 4;
    int menu_height = n_choices + 2;
    int menu_width = -1;

    const char title[] = "Type Master Game";
    const char info[] = "* Use 'U' or 'D' to move and Enter to select.";
    const char infoQuit[] = "* Use 'q' to quit.";

    char choices[][100] = {
        "Start typing",
        "Best Score",
        "Check Scores",
        "Exit"
    };


    for( int i = 0; i < 4; ++i ){
        if ( (int) strlen(choices[i]) > menu_width )
            menu_width = (int) strlen(choices[i]) + 8;
    }

    ITEM ** items = (ITEM **) malloc(sizeof(ITEM*)*(n_choices+1));
    items[n_choices] = NULL;
    for( int i = 0; i < n_choices; ++i ){
        items[i] = new_item(choices[i], "");
    }

    WINDOW* win = newwin(LINES, COLS, 0, 0);
    WINDOW* menu_win = newwin(menu_height, menu_width, LINES/2 - menu_height/2, COLS/2 - menu_width/2);
    MENU*   menu = new_menu( (ITEM**) items );
    keypad(win, TRUE);
    keypad(menu_win, TRUE);  // to ditect arrow keys as KEY_UP

    set_menu_win(menu, menu_win);
    set_menu_sub(menu, derwin(menu_win, n_choices, menu_width - 2, 1, 1) );
    set_menu_mark(menu, " -$ ");

    if ( !win ){
        printw("Could not get window.");
        return exitGame;
    }

    if ( !menu_win ){
        printw("Could not get window.");
        return exitGame;
    }

    box(menu_win, 0, 0 );
    wattron(win, A_UNDERLINE);
    mvwprintw(win, LINES/2 - menu_height/2 - 2, COLS/2 - strlen(title)/2, "%s", title);
    wattroff(win, A_UNDERLINE | COLOR_GREEN);

    mvwprintw(win, LINES/2 + menu_height/2 + 1, COLS/2 - strlen(info)/2, "%s", info);
    mvwprintw(win, LINES/2 + menu_height/2 + 2, COLS/2 - strlen(info)/2, "%s", infoQuit);

    wmove(win, LINES-1, COLS-1);
    wrefresh(win);

    post_menu(menu);
    wrefresh(menu_win);

    while ((ch = wgetch(menu_win)) != 'q') {
        int selected_choice = -999;

        switch (ch) {
            case 'u':
            case 'U':
            case  KEY_UP: // arrow up
                mvwprintw(win, LINES - 1, 0, "                    ");
                menu_driver(menu, REQ_UP_ITEM);
                break;
            case 'D':
            case 'd':
            case  KEY_DOWN:  // arrow down
                mvwprintw(win, LINES - 1, 0, "                    ");
                menu_driver(menu, REQ_DOWN_ITEM);
                break;
            case 10:    // Enter key
                mvwprintw(win, LINES - 2, 1, "TEXT: %s\n", item_name(current_item(menu)));
                mvwprintw(win, LINES - 1, 1, "Str Comp ( with EXIT ): %d\n", strcmp(item_name(current_item(menu)), "Exit"));

                if ( strcmp( item_name( current_item( menu ) ), choices[0] ) == 0 ) selected_choice =  startGame;
                if ( strcmp( item_name( current_item( menu ) ), choices[1] ) == 0 ) selected_choice =  bestScore;
                if ( strcmp( item_name( current_item( menu ) ), choices[2] ) == 0 ) selected_choice =  allScores;
                if ( strcmp( item_name( current_item( menu ) ), choices[3] ) == 0 ){
                    wclear(win);
                    printf("##########\n");
                    exit(0);
                }

                if ( selected_choice != -999){
                    unpost_menu(menu);
                    free_menu(menu);
                    for( int i = 0; i < n_choices; ++i ) free_item(items[i]);
                    free(items);
                    wclear(menu_win);
                    wclear(win);
                    delwin(win);
                    delwin(menu_win);

                    return selected_choice;
                }
                break;

            default:
                mvwprintw(win, LINES - 2, 1, " ");
                mvwprintw(win, LINES - 1, 1, " ");
                mvwprintw(win, LINES - 1, 1, "Not a Valid Key." );
                break;
        }
        wmove(win, LINES-1, COLS-1);
        wrefresh(menu_win);
        wrefresh(win);
        // wrefresh(titleWin);
    }

    unpost_menu(menu);
    free_menu(menu);
    for( int i = 0; i < n_choices; ++i ) free_item(items[i]);
    free(items);
    wclear(menu_win);
    wclear(win);
    delwin(win);
    delwin(menu_win);

    return exitGame;
}



int showGameScreen(){
    WINDOW* win = newwin(LINES, COLS, 0, 0);
    nodelay(win, TRUE);
    wtimeout(win, 0);
    int ch = 0;
    int lastTyped = '-';
    size_t seconds = 0;
    char written[1000] = {'\0'};
    int charTyped = 0;

    time_t start = time(NULL);

    if ( !win ){
        printw("Could not get window.");
        return exitGame;
    }
 
    box(win, 0, 0);
    int line = 0;
    for( int i = 0; text[i] != '\0'; ++i ){
        int length = 0;
        while( text[length+i] != '\n' ) length++;
        int left = COLS/2 - length/2;
        for(int j = 0; j  < length; ++j ) {
            wattron(win, COLOR_PAIR(COLOR_NORMAL) | A_BOLD);
            mvwprintw( win, LINES/2-4 + line, left >= 0 ? left + j : 0 + j , "%c", text[i+j]);
            wattroff(win, COLOR_PAIR(COLOR_NORMAL) | A_BOLD);
        }
        i += length;
        line++;
    }

    mvwprintw( win, LINES-7, 2, "* Press ESC to go back.");
    mvwprintw( win, LINES-6, 2, "Total Letters : %d", (int) strlen(text));
    mvwprintw( win, LINES-5, 2, "Letters Typed : 0" );
    mvwprintw( win, LINES-4, 2, "Correct       : 0" );
    mvwprintw( win, LINES-3, 2, "Incorrect     : 0" );
    mvwprintw( win, LINES-2, 2, "Seconds       : 0" );
    wmove(     win, LINES-1, COLS-2 );
    wrefresh(win);
    
    while ( TRUE ){
        seconds = time(NULL) - start;

        mvwprintw( win, LINES - 7, 2, "Total Letters : %d   ", (int) strlen(text));
        mvwprintw( win, LINES - 6, 2, "Letters Typed : %d   ", charTyped );
        
        wattron(win, COLOR_PAIR(3));
        mvwprintw( win, LINES - 5, 2, "Correct       : %c   ", '*' );
        wattroff(win, COLOR_PAIR(3));
        
        mvwprintw( win, LINES - 4, 2, "Incorrect     : %c   ", '*' );
        mvwprintw( win, LINES - 3, 2, "Seconds       : %d   ", (int) seconds );
        // wprintw(win, "TRUE %c, %c", text[i], written[i]);
        // wprintw(win, "## %d\n", length);
        ch = wgetch(win);
        mvwprintw(win, 0, 0, "ESC: %d", ch);

        if( ch == 'q' ) return playAgain; 
        if (ch != ERR ){
            if ( ( ch >= 'a' && ch <= 'z') || ( ch >= 'A' && ch <= 'Z') ){
                lastTyped = ch;
            }
            // else {
            //     // wrong sound
            //     written[charTyped] = 1;
            // }
            if ( ch == 127 ){  // backspace, KEY_BACKSPACE not working
                written[charTyped-1] = 0;
                charTyped -= 1;
            }
            else written[charTyped++] = ch;
            mvwprintw(win, LINES-2, 2, "Last Key      : %c,  ", lastTyped );
        }

        int line = 0;
        for( int i = 0; text[i] != '\0'; ++i ){
            int length = 0;
            while( text[length+i] != '\n' ) length++;
            int left = COLS/2 - length/2;
            for(int j = 0; j  < length; ++j ) {
                
                if ( written[i+j] == 0 ) wattron(win, COLOR_PAIR(COLOR_NORMAL) | A_BOLD);
                else if (text[i+j] == written[i+j])  wattron(win, COLOR_PAIR(COLOR_CORRECT) | A_BOLD);
                else  wattron(win, COLOR_PAIR(COLOR_WRONG) | A_BOLD);
                
                mvwprintw( win, LINES/2-4 + line, left >= 0 ? left + j : 0 + j , "%c", text[i+j]);
                wattroff(win, COLOR_PAIR(COLOR_NORMAL) | COLOR_PAIR(COLOR_WRONG) | COLOR_PAIR(COLOR_CORRECT) | A_BOLD);
            }
            i += length;
            line++;
        }

        wmove(     win, LINES-2, COLS-2);
        wrefresh(win);
    }

    wclear(win);
    delwin(win);
    return exitGame;
}


int typingCompleteScreen(){

    // rate
    // best rate
    // Complete ascii img

    return 0;
}


int ABC(){

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    start_color();
    // init_color(COLOR_CUSTOM, 100, 0,9); // rgb
    init_pair(COLOR_CORRECT, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_WRONG, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_NORMAL, COLOR_WHITE, COLOR_BLACK);


    int playGame = 1;

    // showGameScreen();
    while(playGame){
        int res = showSelectionScreen();

        if ( res == startGame ){
            res = showGameScreen();
            if ( res != playAgain ) break;
        } else if ( res == exitGame ){
            return 0;
        }
    }
    endwin();
    return 0;
}