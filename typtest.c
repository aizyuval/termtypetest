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
WINDOW *create_testwin();
struct _test_result spawn_test(WINDOW * testwin);

WINDOW *create_statswin();
//void spawn_stats(WINDOW * statswin);

void destroy_win(WINDOW *local_win);

void wrap_insch(WINDOW * win, int existing_space[], int line_number, int text_line_length, unsigned int * awaiting_words, char recvChar, int in_x, int in_y, int nextl_location);
int main()
{
	initscr();			/* Start curses mode 		*/
	keypad(stdscr, TRUE);
noecho();			/* Don't echo() while we do getch */
	use_default_colors();           // use default background and foregrounds 
	start_color();			//Start color
	refresh();

	WINDOW * testwin = create_testwin();
	spawn_test(testwin);	
	wrefresh(testwin);

	destroy_win(testwin);	

	create_statswin();
	//WINDOW * statswin = 
	//spawn_stats(statswin);

	getch();
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

WINDOW *create_statswin(){
	int height = LINES/1.5;
	int width = COLS/1.2;
	int textstarty = LINES/12; 
	int textstartx =  COLS/12;

	WINDOW *statswin = newwin(height, width, textstarty, textstartx);
	box(statswin, 0, 0);
	wprintw(statswin, "this is the stats win");

	wrefresh(statswin);
	return statswin;
}
// also wrap_delch! 

void wrap_insch(WINDOW * win, int existing_space[], int line_number, int text_line_length, unsigned int * awaiting_words, char recvChar, int in_y, int in_x, int nextl_location){
	int text_line_index = text_line_length-1; 
	int y,x;	
	bool done = false;
	while (!done) {
		if(existing_space[line_number]>0){ // no space means that there is no space beyond the last ' '. BECAUSE, ' ' is included in the last word.
			winsch(win, recvChar);//normal insch.
			existing_space[line_number]--;
			break;
		}
		else if(line_number==2){
			// got to last line with not enough space. 

			wmove(win,line_number,text_line_index); 

			do{
				getyx(win, y, x);
				wdelch(win);//can cause problems
				wmove(win, line_number, x-1);
				printw("|ln2, dlt: %c;|", winch(win));
				existing_space[line_number]++;
			}while((winch(win))!=KEY_SPACE);

			(*awaiting_words)++;


			mvwinsch(win, in_y, in_x, recvChar);
			// do not move after it. im doing it later.
			existing_space[line_number]--;
			break;
		}else{

			char charholder_0;

			bool space_occ =false;
			while(true){	
				wmove(win, line_number, text_line_index); 

				charholder_0 = winch(win);// first time, it will be space
				if(space_occ==true && charholder_0 == ' '){ //we encountered a space before, and encountered one now (new hold), stop. loop is completed.
					mvwinsch(win, in_y, in_x, recvChar);
					existing_space[line_number]--;
					done = true;
					break;
				}
				wdelch(win);
				text_line_index--;
				existing_space[line_number]++; 
				wmove(win, line_number+1, 0); 
				wrap_insch(win, existing_space, line_number+1, text_line_length, awaiting_words, charholder_0, line_number+1, nextl_location, 0);

				//nextl_location++; // no need. always go to 0, and push everything to the right except last stroke where moving ' '
				if(charholder_0 == ' '){ 
					space_occ=true;
				}

			}
		}
	}

}
struct _test_result spawn_test(WINDOW *testwin){
	// destroy other wins.
	int height = LINES/2;
	int width = COLS/1.5;
	int textstarty = height/3;
	int textstartx = width/8;


	wmove(testwin, textstarty, textstartx);// 0,0 of textwin
	int text_line_length = width*0.75;

	WINDOW *textWin = derwin(testwin,12, text_line_length, textstarty-1, textstartx-1);// only 3 lines


	touchwin(testwin);
	wrefresh(textWin);

	wmove(textWin, 0, 0);

	char * strings[] = {"word", "school", "thought", "man", "mongol", "horse", "paper", "china", "bottle", "child", "directly", "start", "mode", "setting", "headphone","thought", "man", "mongol", "horse", "paper", "china", "bottle", "child", "directly", "start", "mode", "setting", "headphone","thought", "man", "mongol", "horse", "paper", "china", "bottle", "child", "directly", "start", "mode"}; //TEMP

	size_t wordAmount = sizeof(strings)/sizeof(const char *);// temp, will be in data file
	unsigned char line_number = 0;
	int existing_space[3] = {text_line_length,text_line_length,text_line_length}; 

	unsigned int val=0;
	unsigned int * awaiting_words = &val;

	// 3 linked lists that point to the same place, but are used in different place.
	struct _word *initializer_ll, *dynamic_ll, *printed_ll;

	initializer_ll = malloc(_word_size);
	dynamic_ll = initializer_ll;
	printed_ll = initializer_ll;

	for(size_t i=0; i<wordAmount; initializer_ll=initializer_ll->next_word){
		// initialization of linked list

		int length = strlen(strings[i]);
		initializer_ll->length = (char)length;

		initializer_ll->string = malloc(length);
		strcpy(initializer_ll->string, strings[i]);

		if(existing_space[line_number]<=length&&line_number!=2){
			line_number+=1;
			wmove(textWin, line_number, 0);
		}
		if(existing_space[line_number]>length){// only > because if equal i can't fill it. need the user to space to move a line
			wprintw(textWin,"%s ", initializer_ll->string);
			existing_space[line_number] -= length+1;

			(*awaiting_words)++;// next word that is waiting to be printed.
		}
		//if no space, and line number ==2. don't print nor move line.

		initializer_ll->ver_arr = malloc(strlen(strings[i]));
		memset(initializer_ll->ver_arr, NONTYPED_CHAR, strlen(strings[i]));

		initializer_ll->overword = 0;
		initializer_ll->next_word = malloc(_word_size);
		initializer_ll->next_word->prev_word = initializer_ll;
		i++;
	}

	line_number = 0; // start with line 0
	wmove(textWin, 0, 0);
	touchwin(testwin);
	wrefresh(textWin);

	init_pair(RED,COLOR_RED, -1); 
	init_pair(GREEN,COLOR_GREEN, -1);
	//init_pair(NORMAL,COLOR_GREEN, -1);


	float spaces = 0;
	int middleRun = 0, row = 0, col = 0;
	unsigned char relCursor = 0;
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
		getyx(textWin, row, col);
		// if he is in the end of the line:
		if(middleRun){ 
			recvChar = getch();
			if(recvChar == ERR){
				break;
			}
		}else { // we already got the first key stroke.
			middleRun++;

		}


		if (recvChar == KEY_BACKSPACE){ 

			getyx(textWin, row, col);
			if(row == textstarty && col == textstartx){
				continue;
			}
			if(relCursor==0){// backspaces a space
				dynamic_ll = dynamic_ll->prev_word;
				relCursor = dynamic_ll->length; 
				col -=1;
				wmove(textWin,row,col);	
				spaces--;

				// check if the word is overworded, or underworded
				if(dynamic_ll->overword){ 
					relCursor += dynamic_ll->overword;
				}else
				{	
					for (unsigned char i=0; i<dynamic_ll->length; i++) {
						if (dynamic_ll->ver_arr[i]==NONTYPED_CHAR) {
							relCursor=i;
							wmove(textWin,row, col-(dynamic_ll->length-relCursor));
							break;
						}
					}
				}
			}else if(relCursor<=dynamic_ll->length){// he is in the word
				relCursor--;	
				dynamic_ll->ver_arr[relCursor] = NONTYPED_CHAR;

				//move to the left, correct it to be the original, then stay there;
				mvwaddch(textWin,row, col-1,dynamic_ll->string[relCursor] );
				wmove(textWin,row, col-1);
			}else{//backspace when on overword
				dynamic_ll->overword -= 1;
				mvwdelch(textWin,row, col-1);
				relCursor--;
			}

		}

		else if(recvChar == ' '){
			// if next_word->dynamic_ll->string is null, he is a god and finished everthing(what about mistakes?)


			if (relCursor<dynamic_ll->length){//he spaces in the middle of the word
				memset((dynamic_ll->ver_arr + relCursor), NONTYPED_CHAR, (dynamic_ll->length - relCursor)); // tab to the next word, non-type next chars.
				getyx(textWin, row, col);
				wmove(textWin,row, col+(int)(dynamic_ll->length - relCursor));// what if it drops a line? ERROR
			}


			getyx(textWin, row, col);
			if(col== (text_line_length-existing_space[line_number])){// if he is on the last space, move to next line
				wmove(textWin,row+1,0);
				line_number++;
			}else{
				waddch(textWin, recvChar);
			}
			dynamic_ll=dynamic_ll->next_word;
			spaces++;
			relCursor=0;
		}

		else if (relCursor >= dynamic_ll->length){//overwording 
			getyx(textWin, row, col);
			if(dynamic_ll->overword ==8){
				// if he is overwording by 8, stop insch'ing
				continue;
			}
			wrap_insch(textWin, existing_space, line_number, text_line_length, awaiting_words, recvChar, row, col, 0);
			wchgat(textWin,1, COLOR_PAIR(RED), RED,NULL);

			wmove(textWin,row,col+1);	
			dynamic_ll->overword += 1; 
			relCursor++;
		}

		else  { //recvChar = normal key
			if(recvChar == dynamic_ll->string[relCursor]){ 
				waddch(textWin,recvChar | COLOR_PAIR(GREEN));
				dynamic_ll->ver_arr[relCursor] = CORRECT_CHAR;
			}else {
				waddch(textWin,dynamic_ll->string[relCursor] | COLOR_PAIR(RED));
				dynamic_ll->ver_arr[relCursor] = MISTAKEN_CHAR;
			}
			relCursor++;

		}
		touchwin(testwin);
		wrefresh(textWin);
	}


	timeout(-1); // cancel timeout for getch()
	getyx(testwin, row, col);
	//wmove(testwin, row+1, col);
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

	struct _test_result a;
	return a;
}
void destroy_win(WINDOW *local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	*/
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}
//
//UNSOLVED: OVERWORDING IN LAST WORD
//decrease ints to shorts
