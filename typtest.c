#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COLOR_TEXTF 256
#define COLOR_BACKGROUND 257//257
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
	use_default_colors(); // use default background and foregrounds when inititalizing -> start_color(). Instead of using color 0;
	start_color();			//Start color
	
	// color each character independently, and dynamically
	init_pair(1, COLOR_TEXTF, -1);
	init_pair(2,COLOR_RED, -1); // wrong color pair (green + nothing
	init_pair(3,COLOR_GREEN, -1); // correct color pair (green + nothing)
	
	char string[99] = "word school thought man mongol horse paper china bottle child directly start mode setting headphone"; //TEMP
	
	char * strings[] = {"word", "school", "thought", "man", "mongol", "horse", "paper", "china", "bottle", "child", "directly", "start", "mode", "setting", "headphone"}; //TEMP
	size_t wordAmount = sizeof(strings)/sizeof(const char *);// temp, will be in data file
	size_t _word_size = sizeof(struct _word);
	// 3 linked lists that point to the same place, but are being used in different place, for convenience. initializer_ll is for initializing values in the ll. dynamic_ll is for changing these values as the user types. printed_ll is for caluclating the stats over the ll
	struct _word *initializer_ll, *dynamic_ll, *printed_ll;
	initializer_ll = malloc(_word_size);
	dynamic_ll = initializer_ll;
	printed_ll = initializer_ll;

	for(size_t i=0; i<wordAmount; initializer_ll=initializer_ll->next_word){
		// initialization of linked list

		initializer_ll->string = malloc(strlen(strings[i]));
		strcpy(initializer_ll->string, strings[i]);
		printw("%s ", initializer_ll->string);
		initializer_ll->ver_arr = malloc(strlen(strings[i]));
		memset(initializer_ll->ver_arr, 2, strlen(strings[i]));
		
		initializer_ll->length = (char)strlen(initializer_ll->string);
		initializer_ll->overword = 0;
		initializer_ll->prev_word = malloc(_word_size);
		initializer_ll->next_word = malloc(_word_size);
		i++;
		// later when enumerating for print, i can simply do while dynamic_ll->string!=NULL. it will be in the last linked element, that has allocated space but has no values.
	}

	// when printing a dynamic_ll->string. printw("%s "); -> space is very important
// print strings
	//
	//new variables: char relCurser, spaces, 
	// start while loop, 
	// compare recvChar with ->dynamic_ll->string[relCursor]
	//
	// relCurser is used for comparison of the current char[relCurser] with recvChar. when he backspaces, it becomes relCurser --; when he backspaces a space, it becomes l->length (ast elemnt of dynamic_ll->string); when he spaces, it becomes 0;
	// relCurser is also a representation of the completed chars. it ++'s with ever addch.
	//
	//
	//when he backspaces:
	//if relcursor=0:
	//space--, linked_list = linked_list->prev_word;
	//relcursor= l->overword + l->length
	// mvaddch, move one back 
	// 
	//if relcursor > l->length
	// he backspaces an overworded
	// delch
	//  l->overword--;
	//  relCursor--;
	// 
	// else // the curser is inside the word
	// relCursor--;
	// mvaddch 
	// moveone abck
	//
	//WHAT IF HE BACKSPACES THE FIRST CHAR?
	//
	//
	// current global curser is: get(stdscr, row, col)
	// current global curser is: enumeration over linked_list lengths plus spaces

	// if l->correct != length then word is incorrect
	// else its all correct and word is complete
	//  make sure everything is continued


	int middleRun = 0, spaces = 0, row = 0, col = 0;
	char relCursor = 0;
	move(0, 0); // move the cursor to where the first char of the dynamic_ll->string is printed //TEMP
	const long time_out = 15;
	// dynamic_ll->string[relCursor] becomes dynamic_ll->string[relcurser]
	int recvChar = getch(); // getting the first keystroke, and timing from there.
	long start =time(NULL);	
	while ((time(NULL)-start) < time_out) { 
		timeout(1000*(time_out - (time(NULL)-start)));	
		//if backspace, relCursor --; else, check if similar
		if(middleRun){ 
			recvChar = getch();
			if(recvChar == ERR){
				break;
			}
		}else { // we already got the first key stroke.
			middleRun++;

		}

		// change the word/space in location relCursor. This can be applied when I know how to represent char as a variable on the screen
		if (recvChar == KEY_BACKSPACE){
			// what if he backspaces a ' ' -> space?
	//when he backspaces:
	//if relcursor=0:
	//space--, linked_list = linked_list->prev_word;
	//relcursor= l->overword + l->length
	// mvaddch, move one back 
	// 
	//if relcursor > l->length
	// he backspaces an overworded
	// delch
	//  l->overword--;
	//  relCursor--;
	// 
	// else // the curser is inside the word
	// relCursor--;
	// mvaddch 
			// ALSO: if backspace and the word is not finished, go to where it didn't finish
			// moveone abck
			getyx(stdscr, row, col);
			if(relCursor==0){
				// he backspaces a space
				move(row,col-1);	
				spaces--;
				dynamic_ll = dynamic_ll->prev_word;
				relCursor = dynamic_ll->overword + dynamic_ll->length;
			}else if(relCursor<=dynamic_ll->length){
				// he is in the word
				relCursor--;	
				dynamic_ll->ver_arr[relCursor] = 2;// correct or not, it is deleted and gets 2
				mvaddch(row, col-1,dynamic_ll->string[relCursor] );
				move(row, col-1);
			}else{
				//backspace overword
				dynamic_ll->overword -= 1;
				delch(); // CHECK
				move(row, col-1);
			}
				
		}
		else if(recvChar == ' '){
	// if next_word->dynamic_ll->string is null, he is a god and finished everthing(what about mistakes?)
			if (relCursor<dynamic_ll->length){
				//he spaces in the middle of the word
				//tab to the next word, and add the rest of chars of the word as mistakes
				memset(dynamic_ll->ver_arr, 0, (dynamic_ll->length - relCursor)); // consider the rest of the chars as wrong
				getyx(stdscr, row, col);
				move(row, col+(int)(dynamic_ll->length - relCursor));// what if it drops a line? ERROR
			}
			addch(recvChar);
			dynamic_ll=dynamic_ll->next_word;
			spaces++;
			relCursor=0;
			continue;
		}
		else if (relCursor >= dynamic_ll->length){
			// recvChar isn't space although it should be.
			dynamic_ll->overword += 1; // will add overword to later mistakes
			relCursor++;
			insch(recvChar); //add the char and push all to the right
			//move(row, jj)
			//add the char, push all the text right or down, ERROR CURRENTLY
			continue;
			
		}
		else  { // normal key is needed, and normal key is given (cause he's not upon space, relCursor<d->length)
			if(recvChar == dynamic_ll->string[relCursor]){ // change the underlying process for determining location
				addch(recvChar | COLOR_PAIR(3));//green
				dynamic_ll->ver_arr[relCursor] = 1;
			}else {
				addch(dynamic_ll->string[relCursor] | COLOR_PAIR(2));// adds the same char, only red. //FIX - IS THERE A BETTER WAY, then to add the same char?
				dynamic_ll->ver_arr[relCursor] = 0;
			}
			relCursor++;
			
		}
	}
	attroff(COLOR_PAIR(3));
	attroff(COLOR_PAIR(2));
	timeout(-1);//cancelling the timeout!!
	start = time(NULL)-start;
	printw("\nstart after elapsation: %lu \n", start); 

	float mistakes = 0, corrects = 0;
	float charAmount = 0;
	bool correctWord;
	float correctWordChars = 0;
	for( ;printed_ll!=dynamic_ll; printed_ll=printed_ll->next_word){
		correctWord = true;	
		for (int i =0; i<printed_ll->length; i++) {
			if (printed_ll->ver_arr[i] ==1){
				corrects++;
			}else if (printed_ll->ver_arr[i] ==0){
				mistakes++;
				correctWord = false;
			}
			charAmount++;
		}
		if(correctWord){
			correctWordChars+= printed_ll->length;
		}

	}
	// calculations: raw wpm, wpm, accuracy.
	// raw wpm:	all chars/5 * 60/time_out
	// wpm: all chars in correct words/5 * 60/time_out
	// accuracy: correct chars/all chars * 100
	// attention to floats or so
	printw("wpm: %f, raw wpm: %f, accuracy: %f", (correctWordChars/5.0)*(60.0/((float)time_out)), (charAmount/5)*(60/((float)time_out)), (corrects/charAmount)*100);
	refresh(); 
	getch(); // only leave the system if  he c-c, or other known key
endwin();

	return 0;
}
