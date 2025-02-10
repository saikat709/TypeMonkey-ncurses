#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

int showBestScoreScreen(){

    keypad(stdscr, true);
    clear();
    int exitScreen = false;

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

    attron(A_BOLD | A_UNDERLINE);
    mvprintw(LINES/2 - 6, COLS/2 - 10, "Best Score");
    attroff(A_BOLD | A_UNDERLINE);

    mvprintw( LINES/2 - 3, COLS/2-16, "Name         : %s ", NAME );
    mvprintw( LINES/2 - 2, COLS/2-16, "WPM          : %f ", WPM );
    mvprintw( LINES/2 - 1, COLS/2-16, "CPS          : %f ", CPS );
    mvprintw( LINES/2 - 0, COLS/2-16, "Accuracy     : %f ", ACCURACY );
    mvprintw( LINES/2 + 1, COLS/2-16, "Time (second): %d ", TIME_SECONDS );



    mvprintw(LINES/2 + 3, (COLS - INS_LENGTH) / 2, "%s", INS);
    mvprintw(LINES/2 + 4, (COLS - INSExitLength) / 2, "%s", INSExit);
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
