#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

const int LEVEL_WORDS[4] = { 0, 30, 40, 60 };
const int LEVEL_TIME[4]  = { 0, 60, 50, 40 };
const int LEVEL_LINE[4]  = { 0, 3, 5, 9 };

int  WORD_LIST_SIZES[4];
char WORDS_LIST[4][150][100];

char ** getRandomWords(int level, int words_size){
    srand(time(NULL));
    // printf("LEVEL: %d\n", level);

    char** result = malloc(words_size * sizeof(char*));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    for (int i = 0; i < words_size; ++i) {
        int random_index = rand() % WORD_LIST_SIZES[level];

        result[i] = malloc((strlen(WORDS_LIST[level][random_index]) + 1) * sizeof(char));
        if (result[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            for (int j = 0; j < i; ++j) {
                free(result[j]);
            }
            free(result);
            return NULL;
        }
        strcpy(result[i], WORDS_LIST[level][random_index]);
    }
    return result;
}

void getWordsFromWordPool(){

    int words_size = 0;
    for(int i = 1; i <= 3; ++i){
        scanf("%d", &WORD_LIST_SIZES[i]);
        for( int j = 0; j < WORD_LIST_SIZES[i]; ++j ){
            fgets(WORDS_LIST[i][j], sizeof(char)*100, stdin);

            size_t len = strlen(WORDS_LIST[i][j]);
            if (len > 0 && WORDS_LIST[i][j][len - 1] == '\n') {
                WORDS_LIST[i][j][len - 1] = '\0';
            }
            // printf("%d %d %s ||  ", i+1, j+1, WORDS_LIST[i][j]);
        }
    }
}

int showTypingScreen( int level ){ // level 1 to 3

    // printf("LEVEL: %d", level);

    char ** chosenWords = getRandomWords(level, LEVEL_WORDS[level]);

    if ( chosenWords == NULL || chosenWords[0] == NULL ){
        printf("WORDS is NULL.");
    }
    // printf("SELECTED words: %s\n", chosenWords[0]);

    clear();
    curs_set(0);

    const int exitGame  = -1, startGame =  1, bestScore =  2, allScores =  3, playAgain =  4;
    const int COLOR_CORRECT = 3, COLOR_WRONG = 4, COLOR_NORMAL = 5;

    start_color();
    // syntax: init_color(COLOR_CUSTOM_CODE, 100, 0,9); // rgb
    init_pair( COLOR_CORRECT, COLOR_GREEN, COLOR_BLACK );
    init_pair( COLOR_WRONG,   COLOR_RED,   COLOR_BLACK );
    init_pair( COLOR_NORMAL,  COLOR_WHITE, COLOR_BLACK );

    char text[1000] = { '\0' };
    const int lineOffset = LEVEL_WORDS[level] / LEVEL_LINE[level];
    for( int i = 0; i < LEVEL_WORDS[level]; ++i ){
        strcat( text, chosenWords[i] );

        // printf("%d | %d | %d\n", LEVEL_WORDS[level], LEVEL_LINE[level], LEVEL_WORDS[level] / LEVEL_LINE[level]);

        if ( i%lineOffset == 0 && i != 0 ){
            strcat( text, "\n");
        } else {
            strcat( text, " ");
        }
    }
    int l = strlen(text);
    text[l-1] = '\n';

    // printf("TEXT: %s\n", text);

    // char content[1000];

    nodelay(stdscr, TRUE);
    timeout(0);
    int ch = 0;
    int lastTyped = '-';
    size_t SECONDS = LEVEL_TIME[level];
    char written[1000] = { '\0' };
    int charTyped = 0;

    time_t START_TIME = time(NULL), CUR_TIME = time(NULL);

    box(stdscr, 0, 0);
    int line = 0;
    for( int i = 0; text[i] != '\0'; ++i ){
        int length = 0;
        while( text[length+i] != '\n' && text[length+1] != '\0' ) length++;
        int left = COLS/2 - length/2;
        // printf("LENgth: %d\n", length);
        for(int j = 0; j  < length; ++j ) {
            attron(COLOR_PAIR(COLOR_NORMAL) | A_BOLD);
            mvprintw(LINES/2-9 + line, left >= 0 ? left + j : 0 + j , "%c", text[i+j]);
            attroff(COLOR_PAIR(COLOR_NORMAL) | A_BOLD);
        }
        i += length;
        line++;
    }

    mvprintw( LINES-8, 2, "* Press ESC to go back.");
    mvprintw( LINES-7, 2, "Seconds       : %d", LEVEL_TIME[level] );
    mvprintw( LINES-6, 2, "Total Letters : %d", (int) strlen(text));
    mvprintw( LINES-5, 2, "Letters Typed : 0" );
    mvprintw( LINES-4, 2, "Correct       : 0" );
    mvprintw( LINES-3, 2, "Incorrect     : 0" );
    mvprintw( LINES-2, 2, "Seconds Left  : %d", (int) SECONDS );
    move(     LINES-1, COLS-2 );
    refresh();

    while ( TRUE ){
        CUR_TIME = time(NULL);
        if ( CUR_TIME - START_TIME >= 1 ){
            SECONDS--;
            START_TIME = time(NULL);
        }

        if ( SECONDS <= 0 ){
            // Finish
            FILE* file = fopen("./data_files/last_type.txt", "w");
            fprintf(file, "WRM: %d", 24);
            fclose(file);
            showAnimationScreen(false);
            showTypingCompleteScreen();
        }

        mvprintw( LINES - 8, 2, "Total Letters : %d   ", (int) strlen(text));
        mvprintw( LINES - 7, 2, "Seconds       : %d", LEVEL_TIME[level] );
        mvprintw( LINES - 6, 2, "Letters Typed : %d   ", charTyped );
        mvprintw( LINES - 5, 2, "Correct       : %c   ", '*' );
        mvprintw( LINES - 4, 2, "Incorrect     : %c   ", '*' );
        attron( COLOR_PAIR(3));
        mvprintw( LINES - 3, 2, "Seconds Left  : %d   ", (int) SECONDS );
        attroff( COLOR_PAIR(3));
        ch = getch();
        mvprintw( 0, 0, "ESC: %d", ch);

        if (ch != ERR ){
            if ( ( ch >= 'a' && ch <= 'z') || ( ch >= 'A' && ch <= 'Z') ){
                lastTyped = ch;
            }
            if ( ch == 127 || ch == KEY_BACKSPACE ){  // backspace, KEY_BACKSPACE not working
                written[charTyped-1] = '\0';
                charTyped -= 1;
            }
            else written[charTyped++] = ch;
            mvprintw( LINES-2, 2, "Last Key      : %c  %d", lastTyped, lastTyped );

            if ( ch == 10 ){   // enter
                FILE* file = fopen("./data_files/last_type.txt", "w");
                fprintf(file, "WRM: %d", 24);
                fclose(file);
                showAnimationScreen(false);
                showTypingCompleteScreen();
                break;
            }
        }

        int line = 0;
        for( int i = 0; text[i] != '\0'; ++i ){
            int length = 0;
            while( text[length+i] != '\n' ) length++;
            int left = COLS/2 - length/2;
            for(int j = 0; j  < length; ++j ) {
                if ( written[i+j] == 0 ) attron(COLOR_PAIR(COLOR_NORMAL) | A_BOLD);
                else if (text[i+j] == written[i+j])  attron(COLOR_PAIR(COLOR_CORRECT) | A_BOLD);
                else  attron(COLOR_PAIR(COLOR_WRONG) | A_BOLD);
                mvprintw( LINES/2-9 + line, left >= 0 ? left + j : 0 + j , "%c", text[i+j]);
                attroff( COLOR_PAIR(COLOR_NORMAL) | COLOR_PAIR(COLOR_WRONG) | COLOR_PAIR(COLOR_CORRECT) | A_BOLD);
            }
            i += length;
            line++;
        }

        move( LINES-2, COLS-2);
        refresh();
    }

    endwin();

    return 0;
}