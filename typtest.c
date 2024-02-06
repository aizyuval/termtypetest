#include <curses.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define KEY_SPACE 32 
#define MISTAKEN_CHAR 0
#define CORRECT_CHAR 1
#define	NONTYPED_CHAR 2
#define GREEN 3 
#define RED 2
#define _word_size sizeof(struct _word)

struct _test_result{
	char * language;
	short wpm;
	short rwpm;
	short acc;
	short time;
};
struct _word {
	char * string;
	char length;
	char * ver_arr;
	char overword; 
	struct _word * prev_word;
	struct _word * next_word;
};
//struct _test_result 
void spawn_test(WINDOW * testwin);
WINDOW *create_testwin();
int main()
{
	initscr();			/* Start curses mode 		*/
	keypad(stdscr, TRUE);
	noecho();			/* Don't echo() while we do getch */
	use_default_colors(); // use default background and foregrounds 
	start_color();			//Start color
	refresh();

	WINDOW * testwin = create_testwin();
	spawn_test(testwin);	

	wrefresh(testwin);
	endwin();
	return 0;
}
WINDOW *create_testwin(){

	int height = LINES/2;
	int width = COLS/1.5;
	int textstarty = (LINES - height) / 2; 
	int textstartx = (COLS - width) /2;
	WINDOW *testwin = newwin(height, width, textstarty, textstartx);
	keypad(testwin, TRUE);
	box(testwin, 0, 0);

	wrefresh(testwin);

	return testwin;
}
//struct _test_result
void spawn_test(WINDOW *testwin){
	// destroy other wins.
	// LINES, COLS, 
	/* lines are relative. wmove 1,1 is inside the window
	int textstarty = (LINES - height) / 2; 
	int textstartx = (COLS - width) /2;*/

	int height = LINES/2;
	int width = COLS/1.5;
	int textstarty = height/3;
	int textstartx = width/8;
	wmove(testwin, textstarty, textstartx);
	char * strings[] = {"word", "school", "thought", "man", "mongol", "horse", "paper", "china", "bottle", "child", "directly", "start", "mode", "setting", "headphone"}; //TEMP
	size_t wordAmount = sizeof(strings)/sizeof(const char *);// temp, will be in data file

	// 3 linked lists that point to the same place, but are used in different place.
	struct _word *initializer_ll, *dynamic_ll, *printed_ll;

	initializer_ll = malloc(_word_size);
	dynamic_ll = initializer_ll;
	printed_ll = initializer_ll;

	for(size_t i=0; i<wordAmount; initializer_ll=initializer_ll->next_word){
		// initialization of linked list

		initializer_ll->string = malloc(strlen(strings[i]));
		strcpy(initializer_ll->string, strings[i]);

		wprintw(testwin,"%s ", initializer_ll->string);//TEMP
		initializer_ll->ver_arr = malloc(strlen(strings[i]));
		memset(initializer_ll->ver_arr, NONTYPED_CHAR, strlen(strings[i]));

		initializer_ll->length = (char)strlen(initializer_ll->string);
		initializer_ll->overword = 0;
		initializer_ll->next_word = malloc(_word_size);
		initializer_ll->next_word->prev_word = initializer_ll;
		i++;
	}
	wmove(testwin, textstarty, textstartx);
	wrefresh(testwin);

	init_pair(RED,COLOR_RED, -1); 
	init_pair(GREEN,COLOR_GREEN, -1);


	float spaces = 0;
	int middleRun = 0, row = 0, col = 0;
	char relCursor = 0;
	long time_out = 15;

	
	int recvChar = getch(); // getting the first keystroke, and timing from there.
	if(recvChar == KEY_BACKSPACE || recvChar == ' '){ // add: for other types.
		while (recvChar == KEY_BACKSPACE || recvChar == ' ')
		{
			recvChar = getch();
		}
	}




	long start =time(NULL);	
	while ((time(NULL)-start) < time_out) { 
		timeout(1000*(time_out - (time(NULL)-start)));	// relative timeout
		if(middleRun){ 
			recvChar = getch();
			if(recvChar == ERR){
				break;
			}
		}else { // we already got the first key stroke.
			middleRun++;

		}

		
		if (recvChar == KEY_BACKSPACE){ 

			getyx(testwin, row, col);
			if(row == textstarty && col == textstartx){
				continue;
			}
			if(relCursor==0){// backspaces a space
				dynamic_ll = dynamic_ll->prev_word;
				relCursor = dynamic_ll->length; 
				col -=1;
				wmove(testwin,row,col);	
				spaces--;

				// check if the word is overworded, or underworded
				if(dynamic_ll->overword){ 
					relCursor += dynamic_ll->overword;
				}else
				{	
					for (char i=0; i<dynamic_ll->length; i++) {
						if (dynamic_ll->ver_arr[i]==NONTYPED_CHAR) {
							relCursor=i;
							wmove(testwin,row, col-(dynamic_ll->length-relCursor));
							break;
						}
					}
				}
			}else if(relCursor<=dynamic_ll->length){// he is in the word
				relCursor--;	
				dynamic_ll->ver_arr[relCursor] = NONTYPED_CHAR;

				//move to the left, correct it to be the original, then stay there;
				mvwaddch(testwin,row, col-1,dynamic_ll->string[relCursor] );
				wmove(testwin,row, col-1);
			}else{//backspace when on overword
				dynamic_ll->overword -= 1;
				mvwdelch(testwin,row, col-1);
				relCursor--;
			}

		}

		else if(recvChar == ' '){
			// if next_word->dynamic_ll->string is null, he is a god and finished everthing(what about mistakes?)
			
			if (relCursor<dynamic_ll->length){//he spaces in the middle of the word
				memset((dynamic_ll->ver_arr + relCursor), NONTYPED_CHAR, (dynamic_ll->length - relCursor)); // tab to the next word, non-type next chars.
				getyx(testwin, row, col);
				wmove(testwin,row, col+(int)(dynamic_ll->length - relCursor));// what if it drops a line? ERROR
			}
			waddch(testwin, recvChar);
			dynamic_ll=dynamic_ll->next_word;
			spaces++;
			relCursor=0;
		}

		else if (relCursor >= dynamic_ll->length){//overwording 
			getyx(testwin, row, col);
			winsch(testwin,recvChar| COLOR_PAIR(RED)); // add red char, push right
			
			wmove(testwin,row,col+1);	
			dynamic_ll->overword += 1; 
			relCursor++;
		}

		else  { //recvChar = normal key
			if(recvChar == dynamic_ll->string[relCursor]){ 
				waddch(testwin,recvChar | COLOR_PAIR(GREEN));
				dynamic_ll->ver_arr[relCursor] = CORRECT_CHAR;
			}else {
				waddch(testwin,dynamic_ll->string[relCursor] | COLOR_PAIR(RED));
				dynamic_ll->ver_arr[relCursor] = MISTAKEN_CHAR;
			}
			relCursor++;

		}
		wrefresh(testwin);
	}


	timeout(-1); // cancel timeout for getch()
	getyx(testwin, row, col);
	wmove(testwin, row+1, col);
	wprintw(testwin,"loopended");


	// initialize with spaces:
	float corrects = spaces;
	float charAmount = spaces;
	float correctWordChars = spaces;
	bool correctWord;

	for( ;printed_ll!=dynamic_ll->next_word; printed_ll=printed_ll->next_word){// stop enumeration when getting to word after the last
		correctWord = true;	
		for (int i =0; i<printed_ll->length; i++) {
			wprintw(testwin,"%d ", printed_ll->ver_arr[i]);	
			if(printed_ll->ver_arr[i]==NONTYPED_CHAR){
				correctWord = false;
				break;
			}else if (printed_ll->ver_arr[i] ==CORRECT_CHAR){
				corrects++;
			}else{
				correctWord = false;
			}
			charAmount++;
		}
		if (printed_ll->overword) {
			correctWord=false;
			charAmount+=printed_ll->overword;
		}
		wprintw(testwin,"%s\n", printed_ll->string);
		if(correctWord){
			correctWordChars+= printed_ll->length;
		}

			wrefresh(testwin);

	}
	// wpm: all correct chars in correct words + spaces / 5 * 60/15
	// raw wpm: all chars + spaces /5 ) * 60/15
	// corrects: all correct chars + spaces 
	// acc: all corrects / all chars
	
	wprintw(testwin,"wpm: %f, raw wpm: %f, accuracy: %f", ((correctWordChars)/5.0)*(60.0/((float)time_out)), (charAmount/5)*(60/((float)time_out)), (corrects/charAmount)*100);
	wprintw(testwin,"\ncalcs-> correct chars in full words + spaces: %f, charAmount: %f,  corrects total: %f, spaces: %f", correctWordChars, charAmount, corrects, spaces);

	wrefresh(testwin);
	getch(); 
	
//	struct _test_result a;
//	return a; 
};
// function who's goals are:
// initialize typing test on screen
// listen for keys
// calculate result with timeout
//
//returns test result. language, time, wpm, raw wpm, accuracy
//
