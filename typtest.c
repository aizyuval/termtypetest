#include <bits/types/struct_timeval.h>
#include <ncurses.h>
#include <time.h>
int main()
{
	initscr();			/* Start curses mode 		*/
	//raw();	// prevent c-c c-o etc. from being passed to the terminal. rather, directly to us. 			/* Line buffering disabled	*/
	raw();
	noecho();			/* Don't echo() while we do getch */
//	nodelay(stdscr, TRUE);

	char string[99] = "word school thought man mongol horse paper china bottle child directly start mode setting headphone";
	int stringL = sizeof(string);
	printw("%s\n", string);

	int count = 0, mistakes = 0, middleRun = 0, spaces = 0;
	int getc = getch(); // getting the first keystroke, and timing from there.
	unsigned long time_out = 3;
	unsigned long start =time(NULL);	
	//long getch_time_out = time_out - time(NULL)-start;// timeout - elapsed time = remaining time = timing out the getch 
	while ((time(NULL)-start) < time_out) { //Temp; Should be while clock<Timeout
		//if backspace, count --; else, check if similar
		if(middleRun){ 
			//printw("\nhalf delay is: %d \n", (int)(time_out - time(NULL)-start));
	//		halfdelay((int)(time_out - (time(NULL)-start)));
			
			timeout(3000);	
			getc = getch();
			if(getc == ERR){
				printw("abortin!");
				timeout(-1);//cancelling the timeout!!
				break;
			}
		}else { // we already got the first key stroke.
			middleRun++;
			
		}

		//if (getc == KEY_BACKSPACE){
			// change the word/space in location count. This can be applied when I know how to represent char as a variable on the screen
		if(string[count] == ' ') { // CURRENT CHAR == SPACE. number 32?
			if(getc != string[count]){ // if he didn't typed space when he needed to, add to mistakes, print it, but do not move to the next word. he will stay in the same location
				// mistakes++; // only could be implemented when backspace is implemented
				//printw("%c", getc); // could only be implemented when backspace is implemented
				continue;
			}else{
				printw("%c", getc); // no colors. it's just a space.	
				spaces++;
			}
		}else {
			if(getc == string[count]){ // change the underlying process for determining location
				printw("%c", getc);// paint white
			}else {
				printw("%c", getc); // paint red
				mistakes++;

			}
		}
		count++;// move to next location (char)
	}
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
	getch();
	getch();
	getch();
	endwin();
	return 0;
}
