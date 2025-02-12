#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


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

    // showScoresScreen();

    showAnimationScreen(true);
    while ( true ){
        showHomeScreen();
    }

    endwin();
    return 0;
}
