#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define KEY_SPACE 32 
#define MISTAKEN_CHAR 0
#define CORRECT_CHAR 1
#define	NONTYPED_CHAR 2
#define GREEN 2 
#define RED 3
struct _word {
	char * string;
	char length;
	char * ver_arr; // 0=wrong,1=true,2=none
	char overword; // mistakes are here once relCursor is over length (instead of in ver_arr) IMPORTANT
	struct _word * prev_word;
	struct _word * next_word;
};
int main()
{
	initscr();			/* Start curses mode 		*/
	noecho();			/* Don't echo() while we do getch */
	keypad(stdscr, TRUE);
	use_default_colors(); // use default background and foregrounds 
	start_color();			//Start color

	init_pair(2,COLOR_RED, -1); 
	init_pair(3,COLOR_GREEN, -1);


	char * strings[] = {"word", "school", "thought", "man", "mongol", "horse", "paper", "china", "bottle", "child", "directly", "start", "mode", "setting", "headphone"}; //TEMP
	size_t wordAmount = sizeof(strings)/sizeof(const char *);// temp, will be in data file
	size_t _word_size = sizeof(struct _word);

	// 3 linked lists that point to the same place, but are used in different place.
	struct _word *initializer_ll, *dynamic_ll, *printed_ll;

	initializer_ll = malloc(_word_size);
	dynamic_ll = initializer_ll;
	printed_ll = initializer_ll;

	for(size_t i=0; i<wordAmount; initializer_ll=initializer_ll->next_word){
		// initialization of linked list

		initializer_ll->string = malloc(strlen(strings[i]));
		strcpy(initializer_ll->string, strings[i]);

		printw("%s ", initializer_ll->string);//TEMP
		initializer_ll->ver_arr = malloc(strlen(strings[i]));
		memset(initializer_ll->ver_arr, 2, strlen(strings[i]));

		initializer_ll->length = (char)strlen(initializer_ll->string);
		initializer_ll->overword = 0;
		initializer_ll->next_word = malloc(_word_size);
		initializer_ll->next_word->prev_word = initializer_ll;
		i++;
	}

	float spaces = 0;
	int middleRun = 0, row = 0, col = 0;
	char relCursor = 0;
	long time_out = 15;

	move(0, 0); // move to text start 
	
	int recvChar = getch(); // getting the first keystroke, and timing from there.
	if(recvChar == KEY_BACKSPACE || recvChar == ' '){ // add: for other types.
		while (recvChar == KEY_BACKSPACE || recvChar == ' ')
		{
			recvChar = getch();
		}
	}


	long start =time(NULL);	
	while ((time(NULL)-start) < time_out) { 
		timeout(1000*(time_out - (time(NULL)-start)));	
		if(middleRun){ 
			recvChar = getch();
			if(recvChar == ERR){
				break;
			}
		}else { // we already got the first key stroke.
			middleRun++;

		}

		
		if (recvChar == KEY_BACKSPACE){ 

			getyx(stdscr, row, col);
			if(row == 0 && col == 0){
				continue;
			}
			if(relCursor==0){// backspaces a space
				dynamic_ll = dynamic_ll->prev_word;
				relCursor = dynamic_ll->length; 
				col -=1;
				move(row,col);	
				spaces--;

				// check if the word is overworded, or underworded
				if(dynamic_ll->overword){ 
					relCursor += dynamic_ll->overword;
				}else
				{	
					for (char i=0; i<dynamic_ll->length; i++) {
						if (dynamic_ll->ver_arr[i]==2) {
							relCursor=i;
							move(row, col-(dynamic_ll->length-relCursor));
							break;
						}
					}
				}
			}else if(relCursor<=dynamic_ll->length){// he is in the word
				relCursor--;	
				dynamic_ll->ver_arr[relCursor] = 2;

				//move to the left, correct it to be the original, then stay there;
				mvwaddch(stdscr,row, col-1,dynamic_ll->string[relCursor] );
				move(row, col-1);
			}else{//backspace when on overword
				dynamic_ll->overword -= 1;
				mvdelch(row, col-1);
				relCursor--;
			}

		}

		else if(recvChar == ' '){
			// if next_word->dynamic_ll->string is null, he is a god and finished everthing(what about mistakes?)
			
			if (relCursor<dynamic_ll->length){//he spaces in the middle of the word
				memset((dynamic_ll->ver_arr + relCursor), 2, (dynamic_ll->length - relCursor)); // tab to the next word, non-type next chars.
				getyx(stdscr, row, col);
				move(row, col+(int)(dynamic_ll->length - relCursor));// what if it drops a line? ERROR
			}
			addch(recvChar);
			dynamic_ll=dynamic_ll->next_word;
			spaces++;
			relCursor=0;
		}

		else if (relCursor >= dynamic_ll->length){//overwording 
			getyx(stdscr, row, col);
			insch(recvChar| COLOR_PAIR(2)); // add red char, push right
			move(row,col+1);	
			dynamic_ll->overword += 1; 
			relCursor++;
		}

		else  { //recvChar = normal key
			if(recvChar == dynamic_ll->string[relCursor]){ 
				addch(recvChar | COLOR_PAIR(3));
				dynamic_ll->ver_arr[relCursor] = 1;
			}else {
				addch(dynamic_ll->string[relCursor] | COLOR_PAIR(2));
				dynamic_ll->ver_arr[relCursor] = 0;
			}
			relCursor++;

		}
	}


	timeout(-1); // cancel timeout for getch()

	// initialize with spaces:
	float corrects = spaces;
	float charAmount = spaces;
	float correctWordChars = spaces;
	bool correctWord;

	for( ;printed_ll!=dynamic_ll->next_word; printed_ll=printed_ll->next_word){// stop enumeration when getting to word after the last
		correctWord = true;	
		for (int i =0; i<printed_ll->length; i++) {
			printw("%d ", printed_ll->ver_arr[i]);	
			if(printed_ll->ver_arr[i]==2){
				correctWord = false;
				break;
			}else if (printed_ll->ver_arr[i] ==1){
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
		printw("%s\n", printed_ll->string);
		if(correctWord){
			correctWordChars+= printed_ll->length;
		}


	}

	// wpm: all correct chars in correct words + spaces / 5 * 60/15
	// raw wpm: all chars + spaces /5 ) * 60/15
	// corrects: all correct chars + spaces 
	// acc: all corrects / all chars
	
	printw("wpm: %f, raw wpm: %f, accuracy: %f", ((correctWordChars)/5.0)*(60.0/((float)time_out)), (charAmount/5)*(60/((float)time_out)), (corrects/charAmount)*100);
	printw("\ncalcs-> correct chars in full words + spaces: %f, charAmount: %f,  corrects total: %f, spaces: %f", correctWordChars, charAmount, corrects, spaces);
	refresh(); 
	getch(); 
endwin();

	return 0;
}
// cols and rows still not figured out
