#include <ncurses.h>
#include <sys/random.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    WINDOW * scr;
    int height;
    int width;
    int x;
    int y;
} MAIN_WIN;

int main(int argc, char *argv[]) {
    int TESTED_WORDS_TOTAL = atoi(argv[1]);
    if (TESTED_WORDS_TOTAL > 250) {
        TESTED_WORDS_TOTAL = 250;
    }
    int ch;

    FILE * fp = fopen("words.txt", "r");
    char * buf = NULL; // will be alloced by getline
    size_t n = 0; // length of buf
    size_t total_words = 0;
    char ** words = NULL;
    time_t start_time = 0;

    size_t random_data = 0;
    int tested_words = TESTED_WORDS_TOTAL;
    char ** random_words = malloc(sizeof(*words) * tested_words);
    size_t total_random_words;

    int word_col = 2;
    int word_row = 2;
    time_t end_time = 0;
    time_t time_taken; 
    int total_mistakes = 0;
    char ** final_words = malloc(sizeof(*final_words) * TESTED_WORDS_TOTAL);
    char test_ch;
    size_t total_final_words = 0;
    int start_pos;

    int word_index_mid_typing;
    char word_buf[100];
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    // init the screen
    MAIN_WIN * typing_window = malloc(sizeof(typing_window));
    typing_window->x = 1;
    typing_window->y = 1;
    typing_window->height = LINES - 8;
    typing_window->width = COLS - 2;
    typing_window->scr = newwin(typing_window->height, typing_window->width, typing_window->y, typing_window->x);
    box(typing_window->scr, 0, 0);
    wrefresh(typing_window->scr);

    keypad(typing_window->scr, TRUE);

    // init stats screen
    MAIN_WIN * stats_window = malloc(sizeof(stats_window));
    stats_window->x = 1;
    stats_window->y = LINES - 7;
    stats_window->height = 7;
    stats_window->width = COLS - 2;
    stats_window->scr = newwin(stats_window->height, stats_window->width, stats_window->y, stats_window->x);
    box(stats_window->scr, 0, 0);
    wrefresh(stats_window->scr);

    mvwaddstr(stats_window->scr, 1,1, "welcome to renee's typing tutor!");
    mvwaddstr(stats_window->scr, 2,1, "backspaces aren't allowed (just dont mess up lol its a feature)");
    mvwaddstr(stats_window->scr, 3,1, "timer will begin when typing begins");
    mvwaddstr(stats_window->scr, 4,1, "progam will exit after pressing space on last word");
    mvwaddstr(stats_window->scr, 5,1, "GLHF :)");
    wrefresh(stats_window->scr);
    // read the words 
    // reads the words into arr
    while(getline(&buf, &n, fp) > 0)
    {
        // add space for next word
        words = realloc(words, sizeof(*words) * (total_words + 1));
        // delete the \n
        buf[strlen(buf) - 1] = 0;
        words[total_words] = strdup(buf);
        total_words++;
    }


    // takes the data and makes random words to be tested on
    for(int i = 0; i < tested_words; i++) {
        getrandom(&random_data, sizeof(random_data), GRND_RANDOM);
        random_words[i] = words[random_data % 1000];
    }

    // output the words for initial testing
    for(int i = 0; i < tested_words; i++) {
        // add the current word
        mvwaddstr(typing_window->scr, word_row, word_col, random_words[i]);
        word_col += strlen(random_words[i]) + 1;
        // makes newline
        if (i < tested_words - 1) {
            if (word_col + strlen(random_words[i + 1]) >= typing_window->width - 2){
                word_row += 2;
                word_col = 2;
            }
        }
    }

    wrefresh(typing_window->scr);
    
    word_col = 2;
    word_row = 2;
    // start to type

    wmove(typing_window->scr, word_row, word_col);
    // iterate through every word in the test
    for(int i = 0; i < TESTED_WORDS_TOTAL; i++) {
        getyx(typing_window->scr, word_row, start_pos);
        word_index_mid_typing = 0;
        // until space is pressed
        while( (ch = wgetch(typing_window->scr)) != 32) {
            if(start_time == 0) {
                time(&start_time);
            }
            // update cursor position
            word_col++;
            // get cursor hovering on
            test_ch = winch(typing_window->scr);
            // capitalize the letter to indicate that it has been typed
            // only do this before the next space
            if ( word_col - start_pos  - 1< strlen(random_words[i]) ) {
                // this is purely visual
                waddch(typing_window->scr, test_ch - 32);
                // add the char typed to the buffer
                word_buf[word_index_mid_typing] = ch;
                word_index_mid_typing++;
            } else {
                // if your at the end of the word you dont want it to keep going 
                // until spacebar is pressed
                word_col--;
            }
        }
        word_buf[word_index_mid_typing] = 0;
        // if space is pressed to early update to where it should be
        if (word_col - start_pos < strlen(random_words[i]) ) {
            word_col = start_pos + strlen(random_words[i]);
            wmove(typing_window->scr, word_row, word_col);
        }
        // skip past the spacebar
        word_col++;
        waddch(typing_window->scr, 32);
        // update the imputted word into the array
        final_words[i] = strdup(word_buf);
        if( i < tested_words - 1 ) {
            if (word_col + strlen(random_words[i + 1]) >= typing_window->width - 2){
                word_row += 2;
                word_col = 2;
                wmove(typing_window->scr, word_row, word_col);
            }
        }
    }
    // close ncurses stuff
    endwin();
    time(&end_time);
    time_taken = end_time - start_time;
    // final formatting
    printf("Time Taken: %d seconds\n", time_taken);
    printf("Seconds taken per word: %.2f\n", (float) time_taken / TESTED_WORDS_TOTAL);
    printf("WPM: %f\n", (TESTED_WORDS_TOTAL / ( (float) time_taken / 60)));
    // calculate words that are wrong
    printf("Mistakes:\n");
    for(int i = 0; i < TESTED_WORDS_TOTAL; i ++) {
        if (strcmp(final_words[i], random_words[i]) != 0) {
            printf("%s : %s\n", random_words[i], final_words[i]);
            total_mistakes++;
        }
    }
    printf("\n");
    printf("Total mistakes: %d\n", total_mistakes);
    printf("Accuracy: %.2f%%\n", (1 - ((float) total_mistakes / TESTED_WORDS_TOTAL)) * 100);

    fclose(fp);
    free(buf);
	return 0;
}

