#include <ncurses.h>
#include <time.h>

//#define COLOR_TEXTF 1 //256
#define COLOR_BACKGROUND 257//257

int main()
{
	initscr();			/* Start curses mode 		*/
	noecho();			/* Don't echo() while we do getch */
	keypad(stdscr, TRUE);
	use_default_colors(); // use default background and foregrounds when inititalizing -> start_color(). Instead of using color 0;
	start_color();			//Start color
	
	init_pair(1,COLOR_RED, -1); // use red and nothing(-1)
	attron(COLOR_PAIR(1));
	
	char string[99] = "word school thought man mongol horse paper china bottle child directly start mode setting headphone"; //TEMP
	printw("%s\n", string); //TEMP - should print dynamically?

	int count = 0, mistakes = 0, middleRun = 0, spaces = 0;
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

		//if (recvChar == KEY_BACKSPACE){
		// change the word/space in location count. This can be applied when I know how to represent char as a variable on the screen
		if( recvChar == ' ' || string[count] == ' ') { // if he key'd space, or should have;
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
				printw("%c", recvChar);// paint white
			}else {
				printw("%c", recvChar); // paint red
				mistakes++;

			}
		}
		count++;// move to next location (char)
	}
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
	getch();
	endwin();

	return 0;
}
