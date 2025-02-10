#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <menu.h>
#include <unistd.h>

#define NUM_PARTICLES 50
#define TRAIL_LENGTH  4
#define NUM_COLORS    8


const int LEVEL_WORDS[4] = { 0, 30, 40, 60 };
const int LEVEL_TIME[4]  = { 0, 60, 50, 40 };
const int LEVEL_LINE[4]  = { 0, 3, 5, 9 };

int  WORD_LIST_SIZES[4];
char WORDS_LIST[4][150][100];

const char TITLE[] = " LEARN TYPING ";
const char INSTRUCTION[] = " * Press ENTER to start or 'q'/ESC to Exit";

int TITLE_LENGTH = (int) strlen(TITLE);
int INSTRUCTION_LENGTH = (int) strlen(INSTRUCTION);


float CORD_X[NUM_PARTICLES], CORD_Y[NUM_PARTICLES], CORD_Dx[NUM_PARTICLES], CORD_Dy[NUM_PARTICLES];
int COLOR[NUM_PARTICLES];
char CURRENT_CHAR[NUM_PARTICLES];


int showAnimationScreen( bool isIntroAnim );
int showHomeScreen();
int showLevelChoiceScreen();
int showTypingScreen( int level );
int showTypingCompleteScreen();
int showScoresScreen();
int showBestScoreScreen();

void getWordsFromWordPool();
char ** getRandomWords(int level, int word_count);

int main(){
    initscr();
    curs_set(0);
    noecho();
    clear();

    mvprintw(0, 0, "GAME START..");

    getWordsFromWordPool();
    freopen("/dev/tty", "r", stdin);

    showAnimationScreen(true);
    while ( true ){
        showHomeScreen();
    }

    endwin();
    return 0;
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
        }
    }
}

void init_particles(int max_x, int max_y) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        CORD_X[i]       =  rand() % ( max_x - 2 ) + 1;
        CORD_Y[i]       =  rand() % ( max_y - 2 ) + 1;
        CORD_Dx[i]      = (rand() % 200 - 100) / 100.0;
        CORD_Dy[i]      = (rand() % 200 - 100) / 100.0;
        COLOR[i]        =  rand() % COLORS + 1;
        CURRENT_CHAR[i] = 'A' + (i % 26);
    }
}

int showAnimationScreen( bool isIntroAnim ) {
    srand(time(NULL));

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    start_color();

    int ch = 0;
    size_t startTime = time(NULL), endTime = -1;

    for (int i = 1; i <= COLORS; i++) {
        init_pair(i, i % 8, COLOR_BLACK);
    }

    int MAX_X, MAX_Y;
    getmaxyx(stdscr, MAX_Y, MAX_X);

    init_particles(MAX_X, MAX_Y);

    while (TRUE) {

        ch = getch();
        if ( ch == 'q') break;
        if ( ch == 27 ){
            refresh();
            endwin();
            exit(0);
        }

        clear();

        box(stdscr, 0, 0);
        attron(A_BOLD);
        mvprintw(0, (MAX_X - TITLE_LENGTH)/2, "%s", TITLE);
        attroff(A_BOLD);


        if ( isIntroAnim ){
            attron(A_BOLD);
            mvprintw(MAX_Y/2-2, (MAX_X - TITLE_LENGTH)/2, "%s", TITLE);
            attroff(A_BOLD);

            attron(A_ITALIC);
            mvprintw(MAX_Y/2-1, (MAX_X - INSTRUCTION_LENGTH)/2, "%s", INSTRUCTION);
            attroff(A_BOLD);
        }

        if ( isIntroAnim && ( ch == 10 || ch == KEY_ENTER )  ) break; // break
        if ( !isIntroAnim && endTime - startTime == 1 )        break;

        int new_max_x, new_max_y;
        getmaxyx(stdscr, new_max_y, new_max_x);
        if (new_max_x != MAX_X || new_max_y != MAX_Y) {
            MAX_X = new_max_x;
            MAX_Y = new_max_y;
            init_particles( MAX_X, MAX_Y);
        }

        for (int i = 0; i < NUM_PARTICLES; i++) {
            CORD_X[i] += CORD_Dx[i];
            CORD_Y[i] += CORD_Dy[i];

            if (CORD_X[i] <= 1 || CORD_X[i] >= MAX_X - 2) {
                CORD_Dx[i] *= -1;
                COLOR[i]   = (COLOR[i] % NUM_COLORS) + 1;
                CURRENT_CHAR[i] = 'A' + (CURRENT_CHAR[i] - 'A' + 1) % 26;
            }
            if (CORD_Y[i] <= 1 || CORD_Y[i] >= MAX_Y - 2) {
                CORD_Dy[i] *= -1;
                COLOR[i]   = (COLOR[i] % NUM_COLORS) + 1;
                CURRENT_CHAR[i] = 'A' + (CURRENT_CHAR[i] - 'A' + 1) % 26;
            }

            attron(COLOR_PAIR(COLOR[i]) | A_BOLD);
            for (int t = 0; t < TRAIL_LENGTH; t++) {
                int trail_x =  CORD_X[i] -  CORD_Dx[i] * t;
                int trail_y =  CORD_Y[i] -  CORD_Dy[i] * t;
                if (trail_x > 0 && trail_x < MAX_X - 1 &&
                    trail_y > 0 && trail_y < MAX_Y - 1) {
                    int char_offset = (CURRENT_CHAR[i] - 'A' - t + 26) % 26;
                    char trail_char = 'A' + char_offset;
                    mvaddch(trail_y, trail_x, trail_char);
                }
            }
            attroff(COLOR_PAIR(COLOR[i]) | A_BOLD);
        }
        endTime = time(NULL);
        refresh();
        napms(20);
    }
    return 0;
}


int showHomeScreen() {
    int MAX_X, MAX_Y;
    bool exitHome = false;

    clear();
    getmaxyx(stdscr, MAX_Y, MAX_X);

    int ch = 0;
    int n_choices = 4;
    int menu_height = n_choices + 2;
    int menu_width = 0;

    const char title[] = "Type Monkey Game";
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
                    showAnimationScreen(false);
                    showBestScoreScreen();
                    exitHome = true;
                } else if ( strcmp( item_name( current_item( menu ) ), choices[2] ) == 0 ){
                    showAnimationScreen(false);
                    showScoresScreen();
                    exitHome = true;
                } else if ( strcmp( item_name( current_item( menu ) ), choices[3] ) == 0 ){
                    exitHome = true;
                    endwin();
                    exit(0);
                }
                break;

            case 'q':
            case 'Q':
                endwin();
                exitHome = true;
                exit(0);
                break;
            default:
                break;
        }

        move(LINES-1, COLS-1);
        wrefresh(menu_win);
        refresh();
    }

    unpost_menu(menu);
    free_menu(menu);
    for (int i = 0; i < n_choices; ++i) free_item(items[i]);
    free(items);
    wclear(menu_win);
    delwin(menu_win);

    return 0;
}


int showLevelChoiceScreen() {
    int MAX_X, MAX_Y;
    bool exitChoiceScreen = false;
    int level = 0;

    clear();
    getmaxyx(stdscr, MAX_Y, MAX_X);

    int ch = 0;
    int n_choices = 5;
    int menu_height = n_choices + 2;
    int menu_width = 0;

    const char title[] = "Choose level";
    const char info[] = "* Use 'U' or 'D' to move and Enter to select.";
    const char infoQuit[] = "* Use 'q' to quit.";

    char choices[][100] = {
        "Basic  ( Level 1 )",
        "Middle ( Level 2 )",
        "Hard   ( Level 3 )",
        "Go Back",
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
        printf("Could not get window.");
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
    while( !exitChoiceScreen ){
        ch = wgetch(menu_win);
        printf("C: %c ", ch);
        if ( ch == 'q' ) exitChoiceScreen = true;

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

                if      ( strcmp( item_name( current_item( menu ) ), choices[0] ) == 0 )  level = 1;
                else if ( strcmp( item_name( current_item( menu ) ), choices[1] ) == 0 )  level = 2;
                else if ( strcmp( item_name( current_item( menu ) ), choices[2] ) == 0 )  level = 3;
                else if ( strcmp( item_name( current_item( menu ) ), choices[3] ) == 0 )  exitChoiceScreen = true;
                else if ( strcmp( item_name( current_item( menu ) ), choices[4] ) == 0 ) {
                    endwin();
                    exit(0);
                }
                break;
            default:
                break;
        }
        if ( level != 0 ){
            showAnimationScreen(false);
            showTypingScreen(level);
            exitChoiceScreen = true;
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

    return 0;
}


char ** getRandomWords(int level, int words_size){
    srand(time(NULL));

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

const char RULES_TEXT[] = "* Press Ctrl + C to exit the and  ESC to restart writing. Use END to end writing anytime. ";

int showTypingScreen( int level ){ // level 1 to 3

    clear();
    curs_set(0);

    const int exitGame  = -1, startGame =  1, bestScore =  2, allScores =  3, playAgain =  4;
    const int COLOR_CORRECT = 3, COLOR_WRONG = 4, COLOR_NORMAL = 5, COLOR_WARNING = 6, COLOR_WRONG_INVERSE = 7, COLOR_CORRECT_INVERSE = 8;

    start_color();
    // syntax: init_color(COLOR_CUSTOM_CODE, 100, 0,9); // rgb
    init_pair( COLOR_CORRECT,          COLOR_GREEN,  COLOR_BLACK );
    init_pair( COLOR_CORRECT_INVERSE,  COLOR_BLACK,  COLOR_GREEN );
    init_pair( COLOR_WRONG,            COLOR_RED,    COLOR_BLACK );
    init_pair( COLOR_WRONG_INVERSE,    COLOR_BLACK,  COLOR_RED   );
    init_pair( COLOR_NORMAL,           COLOR_WHITE,  COLOR_BLACK );
    init_pair( COLOR_WARNING,          COLOR_YELLOW, COLOR_BLACK );

    char ** chosenWords = getRandomWords(level, LEVEL_WORDS[level]);
    if ( chosenWords == NULL || chosenWords[0] == NULL ){
        printf("WORDS is NULL.");
    }

    char text[1000]          = { '\0' };
    int  CORRECT_KEYSTROKE   = 0;
    int  INCORRECT_KEYSTROKE = 0;
    int  CORRECT_WORD_COUNT  = 0;
    char currentWord[100]    = { '\0' };
    int  currentLeterIndex   = 0;
    int  currentWordStart    = 0;
    int  wordsWrittenCount   = 0;
    char written[1000]       = {'\0'};
    int  charTyped           = 0;

    float wpm = 0.0;
    float cps = 0.0;
    float accuracy = 0.0;

    int ch = 0;
    int lastTyped = '-';
    size_t SECONDS = LEVEL_TIME[level];
    bool writingFinished = false;

    // Organize the words in lines ( separated by newline character )
    const int lineOffset = LEVEL_WORDS[level] / LEVEL_LINE[level];
    for( int i = 0; i < LEVEL_WORDS[level]; ++i ){
        strcat( text, chosenWords[i] );
        if ( i%lineOffset == 0 && i != 0 ){
            strcat( text, "\n");
        } else {
            strcat( text, " ");
        }
    }
    int l = strlen(text);
    text[l-1] = '\n';

    nodelay(stdscr, TRUE);
    timeout(0);

    time_t START_TIME = time(NULL), CUR_TIME = time(NULL);

    box(stdscr, 0, 0);
    int line = 0;
    for( int i = 0; text[i] != '\0'; ++i ){
        int length = 0;
        while( text[length+i] != '\n' && text[length+1] != '\0' ) length++;
        int left = COLS/2 - length/2;
        for(int j = 0; j  < length; ++j ) {
            attron(COLOR_PAIR(COLOR_NORMAL) | A_BOLD);
            mvprintw(LINES/2-9 + line, left >= 0 ? left + j : 0 + j , "%c", text[i+j]);
            attroff(COLOR_PAIR(COLOR_NORMAL) | A_BOLD);
        }
        i += length;
        line++;
    }

    refresh();

    while ( TRUE ){

        attron(A_BOLD);
        mvprintw( LINES - 12, 2, "%s", RULES_TEXT);
        attroff(A_BOLD);
        mvprintw( LINES - 10, 2,"Total Words            : %d   ", LEVEL_WORDS[level]);
        mvprintw( LINES - 9, 2, "Total Time ( seconds ) : %d", LEVEL_TIME[level] );
        mvprintw( LINES - 8, 2, "Total Key strokes      : %d   ", CORRECT_KEYSTROKE + INCORRECT_KEYSTROKE);
        attron( COLOR_PAIR(COLOR_CORRECT));
        mvprintw( LINES - 7, 2, "Correct Key Stroke     : %d   ", CORRECT_KEYSTROKE );
        mvprintw( LINES - 6, 2, "Your typing statistics : WPM %f, CPM %f, ACCURACY %f", wpm, cps, accuracy );
        attroff( COLOR_PAIR(COLOR_CORRECT));
        attron( COLOR_PAIR(COLOR_WRONG));
        mvprintw( LINES - 5, 2, "Incorrect KEYSTROKE    : %d  ", INCORRECT_KEYSTROKE );
        attroff( COLOR_PAIR(COLOR_WRONG));
        attron( COLOR_PAIR(COLOR_WARNING));
        mvprintw( LINES - 4, 2, "Time Left ( seconds )  : %d  ", (int) SECONDS );
        attroff( COLOR_PAIR(COLOR_WARNING));
        // mvprintw( 0, 0, "ESC: %d", ch);

        if ( written[0] != '\0' )
            mvprintw( LINES - 3, 2, "Your Text         : %s   ", written);

        ch = getch();
        if (ch != ERR ){
            if ( ( ch >= 'a' && ch <= 'z') || ( ch >= 'A' && ch <= 'Z') ) lastTyped = ch;
            // if ( ch == 111 ){ // escape
            //     showTypingScreen(level);
            //     break;
            // }
            if ( ch == KEY_END ){
                break;
            } else if ( ch == 127 || ch == KEY_BACKSPACE ){  // backspace, KEY_BACKSPACE not working
                written[charTyped-1] = '\0';
                charTyped -= 1;
                if ( currentLeterIndex > 0 ){
                    currentWord[--currentLeterIndex] = '\0';
                }
            } else {
                if ( text[charTyped] == ch ){
                    CORRECT_KEYSTROKE++;
                } else {
                    INCORRECT_KEYSTROKE++;
                }
                written[charTyped++] = ch; // text[charTyped] == '\n' && ch == ' ' ? '\n' : ch;
                cps = CORRECT_KEYSTROKE * 1.0 / ( LEVEL_TIME[level] - SECONDS);
                accuracy = CORRECT_KEYSTROKE * 100.0 /( CORRECT_KEYSTROKE + INCORRECT_KEYSTROKE );
            }

            if ( currentLeterIndex > 0 ){
                bool isCorrectWord = true;
                for( int i = 0; i < currentLeterIndex; ++i ){
                    if ( text[currentWordStart+i] != currentWord[i] ) isCorrectWord = false;
                }
                if ( text[charTyped+1] == ' ' || text[charTyped+1] == '\0'  ){
                    wordsWrittenCount++;
                    if ( isCorrectWord ) CORRECT_WORD_COUNT++;
                }

                wpm = CORRECT_WORD_COUNT * 60.0 / ( LEVEL_TIME[level] - SECONDS );

            } else {
                currentWord[currentLeterIndex++] = ch;
            }
            if ( ch == 10 || ch == KEY_ENTER || ch == ' ' || text[charTyped] == '\0' ){
                currentLeterIndex = 0;
                currentWord[0] = '\0';
                currentWordStart = charTyped;
            }
        }

        int line = 0;
        for( int i = 0; text[i] != '\0'; ++i ){
            int length = 0;
            while( text[length+i] != '\n' ) length++;
            int left = COLS/2 - length/2;
            for(int j = 0; j  < length; ++j ) {
                if ( written[i+j] == 0 ) attron(COLOR_PAIR(COLOR_NORMAL) | A_BOLD);
                else if ( text[i+j] == ' '  ) attron(COLOR_PAIR( (written[i+j] == ' '   ? COLOR_CORRECT_INVERSE : COLOR_WRONG_INVERSE )) | A_BOLD);
                else if ( text[i+j] == written[i+j])  attron(COLOR_PAIR(COLOR_CORRECT) | A_BOLD);
                else attron(COLOR_PAIR( (text[i+j] == ' '   ? COLOR_WRONG_INVERSE : COLOR_WRONG )) | A_BOLD);
                mvprintw( LINES/2-9 + line, left >= 0 ? left + j : 0 + j , "%c", text[i+j]);
                attroff( COLOR_PAIR(COLOR_NORMAL) | COLOR_PAIR(COLOR_WRONG) | COLOR_PAIR(COLOR_CORRECT) | COLOR_PAIR(COLOR_WRONG_INVERSE) | COLOR_PAIR(COLOR_CORRECT_INVERSE) | A_BOLD);
            }
            // at the end.. for \n
            if ( charTyped > i+length  ){
                attron( written[i+length] == '\n' ? COLOR_PAIR(COLOR_CORRECT_INVERSE) : COLOR_PAIR(COLOR_WRONG_INVERSE) );
                mvprintw( LINES/2-9 + line, left >= 0 ? left + length : 0 + length , " ");
                attroff(COLOR_PAIR(COLOR_CORRECT_INVERSE) | COLOR_PAIR(COLOR_WRONG_INVERSE));
            }
            i += length;
            line++;
        }

        move( LINES-2, COLS-2);
        refresh();

        CUR_TIME = time(NULL);
        if ( CUR_TIME - START_TIME >= 1 ){
            SECONDS--;
            START_TIME = time(NULL);
        }

        if ( SECONDS <= 0 || text[charTyped+1] == '\0' || writingFinished ){
            // Finish
            FILE* file = fopen("./data_files/last_type.txt", "w"); // wpm, cps, accuracy, time
            fprintf(file, "%f\n", wpm);
            fprintf(file, "%f\n", cps);
            fprintf(file, "%f\n", accuracy);
            fprintf(file, "%ld\n", LEVEL_TIME[level] - SECONDS);
            fclose(file);
            showAnimationScreen(false);
            showTypingCompleteScreen();
            break;
        }
    }
    endwin();
    return 0;
}

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

    return 0;  // Return to home screen
}


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

    return 0;
}


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

    attron(  A_BOLD | A_UNDERLINE );
    mvprintw( LINES/2 - 6, COLS/2 - 10, "Best Score");
    attroff( A_BOLD | A_UNDERLINE );

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

    return 0;
}