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
    clear();

    getWordsFromWordPool();
    freopen("/dev/tty", "r", stdin);

    showTypingScreen(1);

    showAnimationScreen(true);
    while ( true ){
        showHomeScreen();
    }

    return 0;
}
