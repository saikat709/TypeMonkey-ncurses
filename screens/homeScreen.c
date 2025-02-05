#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>
#include <string.h>

int showHomeScreen() {
    int MAX_X, MAX_Y;
    bool exitHome = false;

    clear();
    getmaxyx(stdscr, MAX_Y, MAX_X);

    int ch = 0;
    int n_choices = 4;
    int menu_height = n_choices + 2;
    int menu_width = 0;

    const char title[] = "Type Master Game";
    const char info[] = "* Use 'U' or 'D' to move and Enter to select.";
    const char infoQuit[] = "* Use 'q' to quit.";

    char choices[][100] = {
        "Start Game",
        "Best Score",
        "Check Scores",
        "Exit"
    };

    for (int i = 0; i < n_choices; ++i) {
        int len = (int) strlen(choices[i]) + 8;
        if (len > menu_width) menu_width = len;
    }

    ITEM **items = (ITEM **) malloc(sizeof(ITEM*) * (n_choices + 1));
    items[n_choices] = NULL;
    for (int i = 0; i < n_choices; ++i) {
        items[i] = new_item(choices[i], "");
    }

    WINDOW* menu_win = newwin(menu_height, menu_width, LINES/2 - menu_height/2, COLS/2 - menu_width/2);
    MENU* menu = new_menu((ITEM**) items);
    keypad(menu_win, TRUE);

    set_menu_win(menu, menu_win);
    set_menu_sub(menu, derwin(menu_win, n_choices, menu_width - 2, 1, 1));
    set_menu_mark(menu, " -$ ");

    if (!menu_win) {
        printw("Could not get window.");
        return 0;
    }

    box(menu_win, 0, 0 );
    attron(A_UNDERLINE);
    mvprintw(LINES/2 - menu_height/2 - 2, COLS/2 - strlen(title)/2, "%s", title);
    attroff(A_UNDERLINE | COLOR_GREEN);

    mvprintw(LINES/2 + menu_height/2 + 1, COLS/2 - strlen(info)/2, "%s", info);
    mvprintw(LINES/2 + menu_height/2 + 2, COLS/2 - strlen(info)/2, "%s", infoQuit);

    move(LINES-1, COLS-1);

    post_menu(menu);
    refresh();
    wrefresh(menu_win);

    // nodelay(menu_win, true);
    while( !exitHome ){
        ch = wgetch(menu_win);
        printf("C: %c ", ch);
        if ( ch == 'q' ) exitHome = true;

        mvprintw( LINES - 2, 1, "CLICKED: %c |", (char) ch );

        switch (ch) {
            case 'u':
            case 'U':
            case  KEY_UP: // arrow up
                mvprintw(LINES - 1, 0, "                    ");
                menu_driver(menu, REQ_UP_ITEM);
                break;
            case 'D':
            case 'd':
            case  KEY_DOWN:  // arrow down
                mvprintw(LINES - 1, 0, "                    ");
                menu_driver(menu, REQ_DOWN_ITEM);
                break;
            case 10:    // Enter key
                mvprintw( LINES - 2, 1, "TEXT: %s\n", item_name(current_item(menu)));
                mvprintw( LINES - 1, 1, "Str Comp ( with EXIT ): %d\n", strcmp(item_name(current_item(menu)), "Exit"));

                if ( strcmp( item_name( current_item( menu ) ), choices[0] ) == 0 ){
                    showAnimationScreen(false);
                    showLevelChoiceScreen();
                    exitHome = true;
                } else if ( strcmp( item_name( current_item( menu ) ), choices[1] ) == 0 ){
                    
                } else if ( strcmp( item_name( current_item( menu ) ), choices[2] ) == 0 ){
                    
                } else if ( strcmp( item_name( current_item( menu ) ), choices[3] ) == 0 ){
                    exitHome = true;
                    endwin();
                    exit(0);
                }
                break;

            default:
                break;
        }
        move(LINES-1, COLS-1);
        wrefresh(menu_win);
        refresh();
    }


    // Cleanup
    unpost_menu(menu);
    free_menu(menu);
    for (int i = 0; i < n_choices; ++i) free_item(items[i]);
    free(items);
    wclear(menu_win);
    delwin(menu_win);
    endwin();

    return 0;
}