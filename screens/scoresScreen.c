#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

int showScoresScreen(){

    keypad(stdscr, true);
    clear();
    int exitScreen = false;

    const char* INS     = " * Press any key to return to home screen.";
    const char* INSExit = " * Press ESC to exit the game.";

    const int INS_LENGTH = strlen(INS);
    const int INSExitLength = strlen(INSExit);

    mvprintw(LINES/2 - 3, COLS/2 - 10, "All Scores....");
    mvprintw(LINES/2 - 1, (COLS - INS_LENGTH) / 2, "%s", INS);
    mvprintw(LINES/2 - 0, (COLS - INSExitLength) / 2, "%s", INSExit);
    refresh();

    while( !exitScreen ){
        int ch = getch();
        if (ch == 27) {  // ESC key
            endwin();
            exit(0);
        }
        else if ( ch == 10 ){ // enter
            endwin();
            exitScreen = true;
        }
    }

    return 0;  // Return to home screen
}
