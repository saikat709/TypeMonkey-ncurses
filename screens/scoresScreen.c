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

    const int TABLE_COLUMN = 5; // name, wpm, cps, accu, time
    const int TABLE_COL_OFFSET = COLS/5;

    attron(A_BOLD);
    mvprintw(1, COLS/2 - 10, "..... All Scores ....");
    attroff(A_BOLD);

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
    mvprintw(LINES - 2, ( COLS - INS_LENGTH ) / 2, "%s", INS);
    mvprintw(LINES - 1, ( COLS - INSExitLength ) / 2, "%s", INSExit);
    refresh();

    while( !exitScreen ){
        int ch = getch();
        if (ch == 27) {  // ESC key
            endwin();
            exit(0);
        }
        else if ( ch == KEY_ENTER || ch == 10 ){ // enter
            endwin();
            exitScreen = true;
        }
    }

    return 0;  // Return to home screen
}
