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

    char wpm[100], playerName[100] = {'\0'};
    int index = 0;
    FILE* file = fopen("./data_files/last_type.txt", "r");
    fscanf(file, "%[^\n].*c", &wpm);
    fclose(file);

    const char* INS_SAVE = " * Press ENTER to save the score.";
    const char* INS      = " * Press TAB return to home screen without saving.";
    const char* INSExit  = " * Press ESC to exit the game.";

    const int INS_SAVE_LENGTH = strlen(INS_SAVE);
    const int INS_LENGTH = strlen(INS);
    const int INSExitLength = strlen(INSExit);


    while( !exitScreen ){
        clear();

        attron(A_UNDERLINE | A_BOLD);
        mvprintw(LINES/2 - 5, COLS/2 - 10, "Typing Complete");
        attroff(A_UNDERLINE | A_BOLD);

        mvprintw(LINES/2 - 2, COLS/2 - 15, "%s", wpm);
        mvprintw(LINES/2 - 1, COLS/2 - 15, "%s", wpm);

        attron(COLOR_PAIR(1));
        mvprintw(LINES/2 - 0, COLS/2 - 15, "Write you name:  %s  ", playerName);
        attroff(COLOR_PAIR(1));

        mvprintw(LINES/2 + 3, (COLS - INS_SAVE_LENGTH) / 2, "%s", INS_SAVE);
        mvprintw(LINES/2 + 4, (COLS - INS_LENGTH) / 2, "%s", INS);
        mvprintw(LINES/2 + 5, (COLS - INSExitLength) / 2, "%s", INSExit);


        int ch = getch();
        if (ch == 27) {  // ESC key
            endwin();
            exit(0);
        } else if ( ch == 127 || ch == KEY_BACKSPACE ){  // backspace, KEY_BACKSPACE not working
            playerName[index-1] = '\0';
            index -= 1;
        }
        else if ( ch == 10 ){ // enter
            // TODO: svae
            endwin();
            exitScreen = true;
        } else if ( ( ch >= 'a' && ch <= 'z') || ( ch >= 'A' && ch <= 'Z')  ) {
            playerName[index++] = ch;
            playerName[index] = '\0';
        } else if ( ch != ERR ) {
            endwin();
            exitScreen = true;
        }
        refresh();

        napms(60);
    }

    return 0;  // Return to home screen
}
