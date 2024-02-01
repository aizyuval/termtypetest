#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COLOR_TEXTF 256
#define COLOR_BACKGROUND 257//257
struct _word {
	char * string;
	char length;
	char correct;
	char mistake;
	char overword;
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
	struct _word *linked_list1, *linked_list2, *linked_list3;
	linked_list1 = malloc(_word_size);
	linked_list2 = linked_list1;
	linked_list3 = linked_list1;

	for(size_t i=0; i<wordAmount; linked_list1=linked_list1->next_word){
		// initialization of linked list

		linked_list1->string = malloc(strlen(strings[i]));
		strcpy(linked_list1->string, strings[i]);
		linked_list1->length = (char)strlen(linked_list1->string);
		linked_list1->correct = 0;
		linked_list1->mistake = 0;
		linked_list1->overword = 0;
		linked_list1->prev_word = malloc(_word_size);
		linked_list1->next_word = malloc(_word_size);
		i++;
		// later when enumerating for print, i can simply do while string!=NULL. it will be in the last linked element, that has allocated space but has no values.
	}
	// when printing a string. printw("%s "); -> space is very important
// print strings
	//
	// start while loop, 
	// compare recvChar with linked_list->string[relCursor]
	//
	// relCurser is used for comparison of the current char[relCurser] with recvChar. when he backspaces, it becomes relCurser --; when he backspaces a space, it becomes l->length (ast elemnt of string); when he spaces, it becomes 0;
	// relCurser is also a representation of the completed chars. it ++'s with ever addch.
	//
	// when he spaces:
	// if next_word->string is null, he is a god and finished everthing(what about mistakes?)
	//
	// if(relcurser<l->length ) -> space in the middle of the word
	// {move to (getxy) + length-relcurses (it's not length-1, because relcurses is on the key that he didn't type yet. thus, num of complete keys is actually relcurses
	// 
	// else -> space before space (good); {}
	// addch, space++, linked_list = linked_list->next_word. relCurser = 0; 
	//(CHECK - what is more costly, if'ing and sometimes moving, or moving all the time (to length-relcursor right?)
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
	//when he types a regular char:
	//if(relCursor >= l->length) -> he types when he should have space'd (overword, or simply space)
	// l->overword++;
	// l->mistakes++;
	//add the char, push all the text right or down, relCursor++
	//else // types where he should
		// if recvChar = l->string[relCurser]
//		//  addch(recvChar | green)
//		//  l->correct++;
//		// else 
//              // addch(recvChar | red)
//              // l->mistake++;
//              // 
//              // relcurser++ -> both;

	//
	// current global curser is: get(stdscr, row, col)
	// current global curser is: enumeration over linked_list lengths plus spaces

	// if l->correct != length then word is incorrect
	// else its all correct and word is complete
	// 

	printw("%s\n", string); //TEMP - should print dynamically?

	int count = 0, mistakes = 0, middleRun = 0, spaces = 0;
	move(0, 0); // move the cursor to where the first char of the string is printed //TEMP
	const long time_out = 15;

	int recvChar = getch(); // getting the first keystroke, and timing from there.
	long start =time(NULL);	
	while ((time(NULL)-start) < time_out) { 
		timeout(1000*(time_out - (time(NULL)-start)));	
		//if backspace, count --; else, check if similar
		if(middleRun){ 
			recvChar = getch();
			if(recvChar == ERR){
				break;
			}
		}else { // we already got the first key stroke.
			middleRun++;

		}

		// change the word/space in location count. This can be applied when I know how to represent char as a variable on the screen
		if (recvChar == KEY_BACKSPACE){
			// what if he backspaces a ' ' -> space?
			count--;// -1 location
			// if the key was correct, delete one correct. if it was mistake, delete mistake
			mvaddch(0, count, string[count]);
			move(0, count);
			continue;
		}
		else if( recvChar == ' ' || string[count] == ' ') { // if he key'd space, or should have;
			if(recvChar == string[count] && recvChar == ' '){ 
			// space regularly
				printw("%c", recvChar); // no colors. it's just a space.	
				spaces++;
			}else if (recvChar == ' ' && string[count] != recvChar){
				// implement when underlying structure is different
				//spaces++;
				// tab to the next word, location can be +3 or +2 or +6. add to mistakes the number of chars that is left.
				continue;// temp
			}else {
				// he didn't space when he should have. add to mistakes.
				// print the word and don't advance in location(continue). wait for him to type space.
				// implement when backspace is available
				continue;//temp
			}
		}else  {
			if(recvChar == string[count]){ // change the underlying process for determining location
				addch(recvChar | COLOR_PAIR(3));
			}else {
				addch(string[count] | COLOR_PAIR(2));// adds the same char, only red. //FIX - IS THERE A BETTER WAY, then to add the same char?
				mistakes++;

			}
		}
		count++;// move to next location (char)
	}
	attroff(COLOR_PAIR(3));
	attroff(COLOR_PAIR(2));
	timeout(-1);//cancelling the timeout!!
	start = time(NULL)-start;
	printw("\nstart after elapsation: %lu \n", start); 
	if (mistakes) { //1 mistake or more
		printw("\n you have  %f accuracy, and raw wpm of: %d ", 100*((((float)count)-((float)mistakes))/((float)count)), ((count-mistakes)/5)*(60/(int)time_out)); 
	}else{
		printw("\n you have 100%% accuracy, and raw wpm of: %d", ((count-mistakes)/5)*(60/(int)time_out)); 
	}
	printw("\nYou had, %d mistakes", mistakes);	

	refresh(); 
	getch(); // only leave the system if  he c-c, or other known key
	endwin();

	return 0;
}
