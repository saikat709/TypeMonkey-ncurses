#include <curses.h>
#include <string.h>
#include <stdlib.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4

char *choices[] = {
    "Choice 1",
    "Choice 2", 
    "Choice 3",
    "Choice 4",
    "Exit",
    (char *) NULL,
};

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

#include <curses.h>
#include <string.h>

// Draw a rounded box with a title centered inside
void print_rounded_title(WINDOW *win, int starty, int startx, const char *title, chtype color) {
    int width = strlen(title) + 4;  // Add padding for sides
    int height = 3;  // Fixed height for title box
    
    // Enable drawing with specified color
    wattron(win, color);
    
    // Draw top line with rounded corners
    mvwaddch(win, starty, startx, ACS_ULCORNER);
    whline(win, ACS_HLINE, width - 2);
    mvwaddch(win, starty, startx + width - 1, ACS_URCORNER);
    
    // Draw sides
    mvwvline(win, starty + 1, startx, ACS_VLINE, 1);
    mvwvline(win, starty + 1, startx + width - 1, ACS_VLINE, 1);
    
    // Draw bottom line with rounded corners
    mvwaddch(win, starty + height - 1, startx, ACS_LLCORNER);
    whline(win, ACS_HLINE, width - 2);
    mvwaddch(win, starty + height - 1, startx + width - 1, ACS_LRCORNER);
    
    // Print centered title
    mvwprintw(win, starty + 1, startx + 2, "%s", title);
    
    // Disable color attributes
    wattroff(win, color);
    
    // Refresh the window
    wrefresh(win);
}

// Alternative version with double-line borders
void print_rounded_title_double(WINDOW *win, int starty, int startx, const char *title, chtype color) {
    int width = strlen(title) + 4;
    int height = 3;
    
    wattron(win, color);
    
    // Draw top line with double-line rounded corners
    mvwaddch(win, starty, startx, ACS_ULCORNER);
    for (int i = 1; i < width - 1; i++) {
        mvwaddch(win, starty, startx + i, ACS_HLINE);
    }
    mvwaddch(win, starty, startx + width - 1, ACS_URCORNER);
    
    // Draw sides with double lines
    mvwvline(win, starty + 1, startx, ACS_VLINE, 1);
    mvwvline(win, starty + 1, startx + width - 1, ACS_VLINE, 1);
    
    // Draw bottom line with double-line rounded corners
    mvwaddch(win, starty + height - 1, startx, ACS_LLCORNER);
    for (int i = 1; i < width - 1; i++) {
        mvwaddch(win, starty + height - 1, startx + i, ACS_HLINE);
    }
    mvwaddch(win, starty + height - 1, startx + width - 1, ACS_LRCORNER);
    
    // Print centered title
    mvwprintw(win, starty + 1, startx + 2, "%s", title);
    
    wattroff(win, color);
    wrefresh(win);
}

// Example usage
void example_usage() {
    // Initialize ncurses
    initscr();
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    
    // Create a window
    WINDOW *win = newwin(24, 80, 0, 0);
    box(win, 0, 0);
    
    // Print single-line rounded title
    print_rounded_title(win, 2, 10, "Main Menu", COLOR_PAIR(1));
    
    // Print double-line rounded title
    print_rounded_title_double(win, 6, 10, "Settings", COLOR_PAIR(1));
    
    // Refresh and wait for key
    wrefresh(win);
    wgetch(win);
    
    // Clean up
    delwin(win);
    endwin();
}

int main()
{
    int c;
    WINDOW *my_menu_win, *my_win;
    int n_choices, i;
    int selectedIndex = 0;

    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);

    n_choices = ARRAY_SIZE(choices) - 1;  // Subtract 1 to exclude NULL terminator

    my_menu_win = newwin(4 * n_choices + 1, 40, 4, 4);
    keypad(my_menu_win, TRUE);
    wrefresh(my_menu_win);
    nodelay(my_menu_win, TRUE);


    example_usage();
    while((c = wgetch(my_menu_win)) != KEY_F(1))  // F1 to exit
    {
        switch(c)
        {
            case KEY_DOWN:
                selectedIndex = (selectedIndex + 1) % n_choices;  // Wrap around
                break;
            case KEY_UP:
                selectedIndex = (selectedIndex - 1 + n_choices) % n_choices;  // Wrap around
                break;
            case KEY_NPAGE:  // Page down - jump to last item
                selectedIndex = n_choices - 1;
                break;
            case KEY_PPAGE:  // Page up - jump to first item
                selectedIndex = 0;
                break;
        }

        box(my_menu_win, 0, 0);
        
        print_in_middle(my_menu_win, 2, 0, 40, "My Menu", COLOR_PAIR(3));
        
        mvwaddch(my_menu_win, 4, 0, ACS_LTEE);
        mvwhline(my_menu_win, 4, 1, ACS_HLINE, 38);
        mvwaddch(my_menu_win, 4, 39, ACS_RTEE);

        for(i = 0; i < n_choices; ++i) {
            if (i == selectedIndex) {
                wattron(my_menu_win, COLOR_PAIR(2));
                mvwprintw(my_menu_win, 2 + i*4, 3, "*");
                wattroff(my_menu_win, COLOR_PAIR(2));
            } else {
                mvwprintw(my_menu_win, 2 + i*4, 3, " ");
            }

            print_in_middle(my_menu_win, 2 + i*4, 1, 40, choices[i], 
                          COLOR_PAIR(selectedIndex == i ? 2 : 3));

            if (i < n_choices - 1) {  // Don't draw line after last item
                mvwhline(my_menu_win, 4 + i*4, 1, ACS_HLINE, 38);
                mvwaddch(my_menu_win, 4 + i*4, 0, ACS_LTEE);
                mvwaddch(my_menu_win, 4 + i*4, 39, ACS_RTEE);
            }
        }

        attron(COLOR_PAIR(2));
        mvprintw(LINES - 2, 0, "Use PageUp and PageDown to scroll to first/last item");
        mvprintw(LINES - 1, 0, "Arrow Keys to navigate (F1 to Exit)");
        attroff(COLOR_PAIR(2));

        wrefresh(my_menu_win);
        refresh();
    }

    endwin();
    return 0;
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{
    int length, x, y;
    float temp;

    if(win == NULL)
        win = stdscr;
    getyx(win, y, x);
    if(startx != 0)
        x = startx;
    if(starty != 0)
        y = starty;
    if(width == 0)
        width = 80;

    length = strlen(string);
    temp = (width - length) / 2;
    x = startx + (int)temp;

    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
}