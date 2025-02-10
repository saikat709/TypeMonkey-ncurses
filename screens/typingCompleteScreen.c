#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

int showTypingCompleteScreen() {
    keypad(stdscr, true);
    clear();
    noecho();
    int exitScreen = false;

    start_color();
    init_pair( 1, COLOR_BLACK, COLOR_GREEN );
    init_pair( 2, COLOR_RED, COLOR_WHITE );

    char playerName[100] = {'\0'};
    int index = 0;

    FILE* lastTypeFile  = fopen("./data_files/last_type.txt",  "r" );
    FILE* scoresFile    = fopen("./data_files/scores.txt",     "a" );
    FILE* bestScoreFile = fopen("./data_files/best_score.txt", "r" );

    const char* INS_SAVE = " * Press ENTER to save the score.";
    const char* INS      = " * Press TAB return to home screen without saving.";
    const char* INSExit  = " * Press BACKSPACE to exit the game.";
    const char* NameErrorMessage = "Please type a name withing 10 characters";

    const int INS_SAVE_LENGTH = strlen(INS_SAVE);
    const int INS_LENGTH = strlen(INS);
    const int INSExitLength = strlen(INSExit);
    const int NameErrorLength = strlen(NameErrorMessage);

    const int MAX_NAME_LENGTH = 10;
    bool hasErrorInName       = false;

    char  BEST_SCORE_NAME[50];
    float BEST_SCORE_WPM     = 0.0;
    float BEST_SCORE_CPM     = 0.0;
    int   BEST_SCORE_SECONDS = 0.0;

    float lastWPM      = 0;
    float lastCPS      = 0;
    float lastAccuracy = 0;
    int   lastSeconds  = 0;

     // Reading best score
    fscanf(bestScoreFile, "%[^\n]%*c", BEST_SCORE_NAME); // skip name
    fscanf(bestScoreFile, "%f", &BEST_SCORE_WPM);
    fscanf(bestScoreFile, "%f", &BEST_SCORE_CPM);
    fscanf(bestScoreFile, "%d", &BEST_SCORE_SECONDS); // skip accuracy
    fscanf(bestScoreFile, "%d", &BEST_SCORE_SECONDS);

    // reading last typing information
    fscanf(lastTypeFile, "%f", &lastWPM);
    fscanf(lastTypeFile, "%f", &lastCPS);
    fscanf(lastTypeFile, "%f", &lastAccuracy); // skip accuracy
    fscanf(lastTypeFile, "%d", &lastSeconds);


    while( !exitScreen ){
        clear();

        start_color();
        init_pair(0, COLOR_RED, COLOR_WHITE );

        attron(A_UNDERLINE | A_BOLD);
        mvprintw(LINES/2 - 5, COLS/2 - 10, "Typing Complete");
        attroff(A_UNDERLINE | A_BOLD);

        mvprintw(LINES/2 - 3, COLS/2 - 15, "WPM     : %f", lastWPM);
        mvprintw(LINES/2 - 2, COLS/2 - 15, "CPS     : %f", lastCPS);
        mvprintw(LINES/2 - 1, COLS/2 - 15, "Accuracy: %f", lastAccuracy);
        mvprintw(LINES/2 - 0, COLS/2 - 15, "Time (S): %d", lastSeconds);

        attron(COLOR_PAIR(1));
        mvprintw(LINES/2 + 2, COLS/2 - 15, "Write you name:  %s  ", playerName);
        attroff(COLOR_PAIR(1));

        mvprintw(LINES/2 + 4, (COLS - INS_SAVE_LENGTH) / 2, "%s", INS_SAVE);
        mvprintw(LINES/2 + 5, (COLS - INS_LENGTH) / 2, "%s", INS);
        mvprintw(LINES/2 + 6, (COLS - INSExitLength) / 2, "%s", INSExit);

        if ( hasErrorInName ){
            attron(COLOR_PAIR(2));
            mvprintw(LINES - 2, 2, "%s", NameErrorMessage);
            attroff(COLOR_PAIR(2));
        }

        int ch = getch();
        if (ch == 27) {  // ESC key
            endwin();
            exit(0);
        } else if ( ch == 127 || ch == KEY_BACKSPACE ){  // backspace, KEY_BACKSPACE not working
            playerName[index-1] = '\0';
            index -= 1;
            if ( index < MAX_NAME_LENGTH ) hasErrorInName = false;
        }
        else if ( ch == 10 || ch == KEY_ENTER ){ // enter

            if ( index == 0 ){
                hasErrorInName = true;
            }
            if ( index > 0 && !hasErrorInName ){
                fprintf(scoresFile, "%s\n", playerName);
                fprintf(scoresFile, "%f\n", lastWPM);
                fprintf(scoresFile, "%f\n", lastCPS);
                fprintf(scoresFile, "%f\n", lastAccuracy);
                fprintf(scoresFile, "%d\n", lastSeconds);

                if ( lastWPM > BEST_SCORE_WPM  ){
                    FILE* bestScoreFileWrite = fopen("./data_files/best_score.txt", "w");
                    fprintf(bestScoreFileWrite, "%s\n", playerName);
                    fprintf(bestScoreFileWrite, "%f\n", lastWPM);
                    fprintf(bestScoreFileWrite, "%f\n", lastCPS);
                    fprintf(bestScoreFileWrite, "%f\n", lastAccuracy);
                    fprintf(bestScoreFileWrite, "%d\n", lastSeconds);
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
        } else if ( ch == 121 || ch == KEY_BACKSPACE ) {
            endwin();
            exitScreen = true;
            break;
        }
        refresh();
        napms(60);
    }


    fclose(bestScoreFile);
    fclose(lastTypeFile);
    fclose(scoresFile);
    // endwin();

    return 0;  // Return to home screen
}
