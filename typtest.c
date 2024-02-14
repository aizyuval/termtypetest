#include <curses.h>
#include <ncurses.h>
#include <cyaml/cyaml.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#define KEY_BACKTICK 59
#define KEY_TAB 9
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
struct _strings {
	char ** words_member;
	unsigned int words_member_count; // maybe I can gather all english categories in a single file, and draw how much i'd like when he asks. 200?ok. 1000? ok.
};
WINDOW *create_testwin();
struct _test_result spawn_test(WINDOW * testwin);
struct _word * init_text (struct _word * initializer_ll, unsigned int * wordAmount, struct _strings * strings, int line_number, int * existing_space, WINDOW * textWin, unsigned int * awaiting_words);

WINDOW *create_statswin();
//void spawn_stats(WINDOW * statswin);


void destroy_win(WINDOW *local_win);

void wrap_insch(WINDOW * win, int existing_space[], int line_number, int text_line_length, unsigned int * awaiting_words, char recvChar, int in_x, int in_y);
void wrap_delch(WINDOW * win, int existing_space[], int line_number, int text_line_length, unsigned int * awaiting_words,struct _strings * strings, int in_x);


struct _strings * start_keys(int *recvChar, unsigned * time_cyc,unsigned timeopts, long * time_out_num, long time_out[], WINDOW *timeWin, char **languages, unsigned * lang_cyc, unsigned old_lang_cyc, unsigned lang_amount, signed lang_diff, WINDOW * langWin, struct _word * printed_ll, unsigned int * wordAmount, struct _strings * strings, int line_number, int existing_space[], int text_line_length, WINDOW * testwin, WINDOW * textWin, unsigned int * awaiting_words, const cyaml_config_t * config, const cyaml_schema_value_t * top_schema);

struct _strings * restart_text(WINDOW * timeWin, WINDOW * langWin, long time_out_num,char** languages,unsigned time_cyc, struct _word * linked_ll, unsigned int * wordAmount, struct _strings * strings, int line_number, int * existing_space, int text_line_length, WINDOW * textWin, unsigned int * awaiting_words, char *lang,const cyaml_config_t * config, const cyaml_schema_value_t * top_schema);


long numDig(long number);
void shuffle(char* names[], size_t n);




int main()
{
	initscr();			/* Start curses mode 		*/
	keypad(stdscr, TRUE);
	noecho();			/* Don't echo() while we do getch */
	use_default_colors();           // use default background and foregrounds 
	start_color();			//Start color
	init_pair(RED,COLOR_RED, -1); 
	init_pair(GREEN,COLOR_GREEN, -1);
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

long numDig(long number){
	if(number<10) return 1;
	if(number<100) return 2;
	if(number<1000) return 3;
	if(number<10000) return 4;
		else return 0;
}
void shuffle(char* names[], size_t n) {
	for (size_t i = 0; i < n;) {
		size_t j = i + rand() % (n - i);
		/* Reject this shuffle if the element we're about to place
     * is the same as the previous one
     */
		if (i > 0 && strcmp(names[j], names[i-1]) == 0)
			i = 0;
		else {
			/* Otherwise, place element i and move to the next one*/
			char* t = names[i];
			names[i] = names[j];
			names[j] = t;
			++i;
		}

	}

}
struct _word * init_text (struct _word * initializer_ll, unsigned int * wordAmount, struct _strings * strings, int line_number, int * existing_space, WINDOW * textWin, unsigned int * awaiting_words){

	// print a test-text on textWin

	for(size_t i=0; i<(*wordAmount); initializer_ll=initializer_ll->next_word){

		int length = strlen(strings->words_member[i]);
		initializer_ll->length = (char)length;

		initializer_ll->string = malloc(length);
		strcpy(initializer_ll->string, strings->words_member[i]);

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

		initializer_ll->ver_arr = malloc(strlen(strings->words_member[i]));
		memset(initializer_ll->ver_arr, NONTYPED_CHAR, strlen(strings->words_member[i]));

		initializer_ll->overword = 0;
		initializer_ll->next_word = malloc(_word_size);
		initializer_ll->next_word->prev_word = initializer_ll;
		i++;
	}
	wmove(textWin, 0, 0);
	return initializer_ll;
}


struct _strings * start_keys(int *recvChar, unsigned * time_cyc,unsigned timeopts, long * time_out_num, long time_out[], WINDOW *timeWin, char **languages, unsigned * lang_cyc, unsigned old_lang_cyc, unsigned lang_amount, signed lang_diff, WINDOW * langWin, struct _word * printed_ll, unsigned int * wordAmount, struct _strings * strings, int line_number, int existing_space[], int text_line_length,WINDOW * testwin, WINDOW * textWin, unsigned int * awaiting_words, const cyaml_config_t * config, const cyaml_schema_value_t * top_schema){
	while ((*recvChar) == KEY_BACKSPACE || (*recvChar) == ' '|| (*recvChar) == KEY_TAB || (*recvChar) == KEY_BACKTICK) // or esc / Tab
	{
		if((*recvChar)==KEY_BACKSPACE){
			// cycling between time opts
			(*time_cyc)+=1;
			if((*time_cyc)==timeopts){
				(*time_cyc)=0;
			}

			if(numDig((*time_out_num)) > numDig(time_out[(*time_cyc)])){// if previous num is bigger
				for(int i=0; i< (numDig((*time_out_num)) - numDig(time_out[(*time_cyc)])); i++){// delete the amount of spaces needed to cover the difference
					wmove(timeWin, 0, numDig((*time_out_num))-1);
					wdelch(timeWin);
				}

			}else if(numDig((*time_out_num)) < numDig(time_out[(*time_cyc)])){ // if old number is smaller
				for(int i=0; i< (numDig(time_out[(*time_cyc)])-numDig((*time_out_num))); i++){// insert the amount of spaces needed to cover the difference

					wmove(timeWin, 0, numDig(time_out[(*time_cyc)])-1);
					winsch(timeWin,' ');
				}
			}
			(*time_out_num) = time_out[(*time_cyc)];
			wmove(timeWin,0 , 0);
			wprintw(timeWin, "%lu", (*time_out_num));


		}else if((*recvChar) == KEY_SPACE){

			old_lang_cyc = (*lang_cyc);
			(*lang_cyc)+=1;
			if((*lang_cyc) == lang_amount){
				(*lang_cyc) = 0;
			}
			wmove(langWin, 0, strlen(languages[old_lang_cyc])-1); // end of the languege word
			lang_diff = strlen(languages[old_lang_cyc])-strlen(languages[(*lang_cyc)]); //signed


			// make sure new language doesn't override [Space]:
			if(lang_diff>0){
				// If last language is longer than the new one -> delch the difference 
				for (unsigned count = 1; lang_diff>0; count++){
					wdelch(langWin);
					lang_diff--;
					wmove(langWin, 0, strlen(languages[old_lang_cyc]) - 1 - count);

				}
			}else if(lang_diff<0){
				// If last language is shorter than new one -> insch the difference
				lang_diff = abs(lang_diff);
				while (lang_diff>0) {
					winsch(langWin, ' ');
					lang_diff--;
				}
			}
			wmove(langWin, 0, 0);
			wprintw(langWin,"%s", languages[(*lang_cyc)]);


			strings = restart_text(timeWin, langWin,(*time_out_num), languages, (*time_cyc),printed_ll, wordAmount, strings, line_number, existing_space, text_line_length, textWin, awaiting_words, languages[(*time_cyc)], config, top_schema); // update strings after freeing it!
		}else if((*recvChar) == KEY_TAB){
			//tab = restart text
			//printw("segmentation fault is not prob in keys! ");
			strings = restart_text(timeWin, langWin, (*time_out_num), languages, (*time_cyc),printed_ll, wordAmount, strings, line_number, existing_space, text_line_length, textWin, awaiting_words, languages[(*time_cyc)], config, top_schema);
		}else if ((*recvChar) == KEY_BACKTICK) {

			// spawn stats win. destroy textwin.
		}

		touchwin(testwin);
		wrefresh(testwin);
		wrefresh(textWin);
		(*recvChar) = getch();
	}

	return strings;

}



struct _strings * restart_text(WINDOW * timeWin, WINDOW * langWin, long time_out_num,char** languages,unsigned time_cyc, struct _word * linked_ll, unsigned int * wordAmount, struct _strings * strings, int line_number, int * existing_space, int text_line_length, WINDOW * textWin, unsigned int * awaiting_words, char *lang,const cyaml_config_t * config, const cyaml_schema_value_t * top_schema){
	//unsigned start_test_loop(int recvChar, int * spaces, int textstarty, int textstartx, long time_out_num, WINDOW * testwin, WINDOW * textWin, struct _word * dynamic_ll, struct _word * printed_ll, struct _strings * strings, unsigned int wordAmount, int line_number, int * existing_space, int text_line_length, int awaiting_words, char ** languages, cyaml_config_t * config, cyaml_schema_value_t * top_schema){
	// clear textWin, free pointer allocations, zero all pointers, reload file onto the same variable? (how to free in cyaml), shuffle, save file, do init_text
	// erase windows, re-print
	werase(textWin);


	struct _word * tmp = linked_ll;
	struct _word * linked_ll_initial = linked_ll; // through the free loop, linked_ll will become the last linked_ll element. linked_ll_initial preserves the starting point.
	// linked_ll initial should be the same as the general printed_ll; if not -> meaning i'm modifying what printed_ll points to
	for(size_t i=0; i<(*wordAmount);i++){// move through all structs and free all of them and their components
		tmp = linked_ll;
		linked_ll = linked_ll->next_word;
		free(tmp->string);
		free(tmp->ver_arr);
		if(i!=0){
			free(tmp);
		}
	}

	cyaml_err_t err = cyaml_free(config, top_schema, strings, 0);
	if(err !=CYAML_OK){
		wprintw(stdscr, "error! cannot FREE loaded words");
		getch();
		endwin();
	}



	char * dir = "languages/";
	char * ext = ".yaml";
	char *file = malloc(strlen(dir) + strlen(lang) + strlen(ext) +1);

	memcpy(file, dir, strlen(dir)); // languages/
	strcat(file, lang); // languages/english200
	strcat(file, ext); // languages/english200.yaml

	err = cyaml_load_file(file, config,
		       top_schema, (cyaml_data_t **)&strings, NULL); // load file onto the struct strings
	if(err !=CYAML_OK){
		wprintw(stdscr, "error! cannot reload file");
		getch();
		endwin();
	}

	shuffle(strings->words_member, strings->words_member_count);
	cyaml_save_file(file, config, top_schema, strings,0); // after shuffle, save the shuffled words to file. that way each time he shuffles he will have a random list to previous ones.
	if(err !=CYAML_OK){
		wprintw(stdscr, "error! cannot SAVE file");
		getch();
		endwin();
	}


	for (unsigned i =0; i<3; i++) {
		memcpy(&(existing_space[i]), &text_line_length, sizeof(int));
	}
	(*wordAmount) = strings->words_member_count;
	(*awaiting_words) = 0;
	init_text(linked_ll_initial, wordAmount, strings, line_number, existing_space, textWin, awaiting_words);

	return strings;
}






WINDOW *create_testwin(){

	int height = LINES/2;
	int width = COLS/1.5;
	int textstarty = (LINES - height) / 2; 
	int textstartx = (COLS - width) /2;
	WINDOW *testwin = newwin(height, width, textstarty, textstartx);
	keypad(testwin, TRUE);
	//box(testwin, 0, 0);

	wrefresh(testwin);

	return testwin;
}
WINDOW *create_newwin(int textstarty, int textstartx){
	int height = 1;
	int width = 30;
	WINDOW *win = newwin(height, width, textstarty, textstartx);
	wrefresh(win);
	return win;
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
void wrap_delch(WINDOW * win, int existing_space[], int line_number, int text_line_length, unsigned int * awaiting_words,struct  _strings * strings, int in_x){

	int next_word_length = 1; // + 1 for the space we are not going to count
	// if line!=2
	wdelch(win);
	existing_space[line_number]++;
	// now check if there's enough spacesto wrap
	if (line_number==2) {
		next_word_length += strlen(strings->words_member[(*awaiting_words)]);
		if(existing_space[line_number]>=next_word_length){
			//there's enough room for awaiting word. write it, then go back.
			wmove(win,line_number,  text_line_length-existing_space[line_number]);
			wprintw(win, "%s ", strings->words_member[(*awaiting_words)]);
			existing_space[line_number] -= next_word_length;
			(*awaiting_words)++;
			wmove(win, line_number, in_x);
		}
		else{
			//break;
		}
	}else{
		wmove(win,line_number+1,0);
		for(int x = 1; winch(win)!= ' '; x++)
		{
			next_word_length++;
			wmove(win, line_number+1, x);
		}

		int char_holder0;

		if (existing_space[line_number]<next_word_length) {
			//printw("next_word_length wont fit.\n");
			wmove(win, line_number, in_x);
			//break

		}else {

			//printw("trying to fit word!.\n");
			while (true) {

				wmove(win,line_number+1,0);
				char_holder0 = winch(win);

				wrap_delch(win, existing_space, line_number+1, text_line_length, awaiting_words, strings, 0);

				wmove(win,line_number,  text_line_length-existing_space[line_number]);

				waddch(win, char_holder0);
				existing_space[line_number]--;

				if (char_holder0 == ' ') {
					wmove(win, line_number, in_x); // move back
					break;

				}
			}



		}
	}
}

// check if space <nextline word+1
// else if line 2 restore
// else move it up



void wrap_insch(WINDOW * win, int existing_space[], int line_number, int text_line_length, unsigned int * awaiting_words, char recvChar, int in_y, int in_x){
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
				existing_space[line_number]++;
			}while((winch(win))!=KEY_SPACE);

			(*awaiting_words)--; // awaiting words is the previous one


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
				wrap_insch(win, existing_space, line_number+1, text_line_length, awaiting_words, charholder_0, line_number+1, 0);

				if(charholder_0 == ' '){ 
					space_occ=true;
				}

			}
		}
	}

}


unsigned start_test_loop(WINDOW * timeWin, WINDOW * langWin, int recvChar, float * spaces, int textstarty, int textstartx, long time_out_num, unsigned time_cyc, WINDOW * testwin, WINDOW * textWin, struct _word * dynamic_ll, struct _word * printed_ll, struct _strings * strings, unsigned int * wordAmount, int line_number, int * existing_space, int text_line_length, unsigned int * awaiting_words, char ** languages, const cyaml_config_t * config, const cyaml_schema_value_t * top_schema, struct _word ** dynamic_ll_ptr){



	werase(timeWin);
	wprintw(timeWin, "%lu", time_out_num);
	werase(langWin);
	//wprintw(langWin, "%s", languages[time_cyc]);

	// 1 for success 
	// 0 for stop
	long start =time(NULL);	
	int time_remaining = (int)time_out_num;
	int middleRun = 0, row = 0, col = 0;
	unsigned char relCursor = 0;

	timeout(1001);
	while ((time(NULL)-start) < time_out_num) { 
		time_remaining = time_out_num - (time(NULL)-start);
		//timeout(1000*(time_remaining));	// relative timeout

		wmove(testwin, 0, numDig(time_remaining));
		if(winch(testwin)!=' '){// if there's a number that shouldn't be there (from previous time counter) delch it.
			wdelch(testwin);
		}

		wmove(testwin, 0, 0);
		wprintw(testwin, "%d", time_remaining);
		touchwin(testwin);
		wrefresh(testwin);
		wrefresh(textWin);

		getyx(textWin, row, col);
		// if he is in the end of the line:
		if(middleRun){ 
			recvChar = getch();
			if(recvChar == ERR){
				continue;
			}
		}else { // we already got the first key stroke.
			middleRun++;

		}

		if (recvChar == KEY_TAB) {
			// stop this test. return 0;
			timeout(-1);// cancel timeout as the test has stopped.
			return 0;
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
				wmove(textWin, row, col-1);
				wrap_delch(textWin, existing_space, line_number, text_line_length, awaiting_words, strings, col-1);
				//mvwdelch(textWin,row, col-1);
				relCursor--;
			}

		}

		else if(recvChar == ' '){
			// if next_word->dynamic_ll->string is null, he is a god and finished everthing(what about mistakes?)
			// drop line when there's space in the end

			if (relCursor<dynamic_ll->length){//he spaces in the middle of the word
				memset((dynamic_ll->ver_arr + relCursor), NONTYPED_CHAR, (dynamic_ll->length - relCursor)); // tab to the next word, non-type next chars.
				getyx(textWin, row, col);
				wmove(textWin,row, col+(int)(dynamic_ll->length - relCursor));// what if it drops a line? ERROR
			}


			getyx(textWin, row, col);
			if(col== (text_line_length-1-existing_space[line_number])){// if he is on the last space, move to next line
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
			if(col == (text_line_length-1-existing_space[line_number]) || dynamic_ll->overword ==8){
				// if he is overwording by 8, stop insch'ing
				// if he is at the end of the line, don't allow overword
				continue;
			}
			wrap_insch(textWin, existing_space, line_number, text_line_length, awaiting_words, recvChar, row, col);
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
		wrefresh(testwin);
		touchwin(testwin);
		wrefresh(textWin);
	}


	(*dynamic_ll_ptr) = dynamic_ll; // update dynamic_ll after function usage. 
	return 1;
}







struct _test_result spawn_test(WINDOW *testwin){
	// destroy other wins.

	int height = LINES/2;
	int width = COLS/1.5;
	int textstarty = height/3;
	int textstartx = width/8;
	wmove(testwin, textstarty, textstartx);// 0,0 of textwin
	int text_line_length = width*0.75;

	int switches_length = 20;

	WINDOW *textWin = derwin(testwin,3, text_line_length, textstarty-1, textstartx-1);// create a newwin inside the current one for better positioning and wrapping
	WINDOW *timeWin = derwin(testwin,1, switches_length, 0, 0);// create a newwin inside the current one for better positioning and wrapping
	WINDOW *langWin = derwin(testwin,1, switches_length, 0, 35);// create a newwin inside the current one for better positioning and wrapping
	WINDOW *restartWin = derwin(testwin,1, switches_length, textstarty+7, (textstartx-1) + (text_line_length)/2- (switches_length)/2);// create a newwin inside the current one for better positioning and wrapping
	WINDOW *togglesetWin = derwin(testwin,1, switches_length, textstarty+8,  (textstartx-1) + (text_line_length)/2- (switches_length)/2);// create a newwin inside the current one for better positioning and wrapping
	box(timeWin,0,0);box(langWin,0,0);box(restartWin,0,0);box(togglesetWin,0,0);
	touchwin(testwin);
	wrefresh(testwin);
	wmove(textWin, 0, 0);


	// INITIALIZE words from file, shuffle them.
	static const cyaml_schema_value_t word_entry = {
		CYAML_VALUE_STRING(CYAML_FLAG_POINTER, char*, 0, CYAML_UNLIMITED),
	};
	static const cyaml_schema_field_t top_mapping_scheme[] = { 
		CYAML_FIELD_SEQUENCE("words", CYAML_FLAG_POINTER, struct _strings, words_member,  &word_entry, 0, CYAML_UNLIMITED),// cyaml unlimited specifies size. can be 200, 1000.
		CYAML_FIELD_END
	};

	static const cyaml_schema_value_t top_schema = {
		CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct _strings, top_mapping_scheme),
	};


	struct _strings *strings; // every strings is strings->words_member[n]

	static const cyaml_config_t config = {
		.log_fn = cyaml_log,
		.mem_fn = cyaml_mem,
		.log_level = CYAML_LOG_WARNING,
	};

	//char * defaultfile = "languages/english200.yaml";
	cyaml_err_t err = cyaml_load_file("languages/english200.yaml", &config,
				   &top_schema, (cyaml_data_t **)&strings, NULL); // load file onto the struct strings

	shuffle(strings->words_member, strings->words_member_count);
	cyaml_save_file("languages/english200.yaml", &config, &top_schema, strings,0); // after shuffle, save the shuffled words to file. that way each time he shuffles he will have a random list to previous ones.

	unsigned int * wordAmount = malloc(sizeof(unsigned int));
	(*wordAmount) = strings->words_member_count;

	unsigned char line_number = 0;
	int existing_space[3] = {text_line_length,text_line_length,text_line_length}; 

	unsigned int val=0;
	unsigned int * awaiting_words = &val;

	// 3 linked lists that point to the same place, but are used in different place.
	struct _word *initializer_ll, *dynamic_ll, *printed_ll;

	initializer_ll = malloc(_word_size);
	dynamic_ll = initializer_ll;
	printed_ll = initializer_ll;

	init_text(initializer_ll, wordAmount, strings, line_number, existing_space, textWin, awaiting_words); // initialize linked list, and print words on screen.

	touchwin(testwin);
	wrefresh(testwin);
	wrefresh(textWin);


	float spaces = 0;
	int middleRun = 0, row = 0, col = 0;
	unsigned char relCursor = 0;

	long time_out[] = {15,30,60,120};
	long time_out_num = 15;
	unsigned timeopts = 4;
	unsigned time_cyc = 0;

	char ** languages;
	unsigned lang_cyc = 0;
	unsigned old_lang_cyc = 0;
	unsigned lang_amount = 2;
	signed lang_diff = 0;
	languages = malloc(lang_amount*sizeof(char*));
	for (unsigned i = 0; i<lang_amount; i++) {
		languages[i]=malloc(sizeof(char*));
	}


	strcpy(languages[0], "english200");
	strcpy(languages[1], "english1000");

	wprintw(timeWin, "%lu [Backspace]", time_out_num);
	wprintw(langWin, "%s [Space]", languages[0]);
	wprintw(restartWin, "restart [Tab]");
	wprintw(togglesetWin, "stats [Esc]");

	touchwin(testwin); // only touching the father-window and then refreshing it to update all. then refreshing textWin to put the cursor back there.
	wrefresh(testwin);
	wrefresh(textWin);

	int recvChar = getch(); // getting the first keystroke, and acting upon it

	strings = start_keys(&recvChar, &time_cyc, timeopts, &time_out_num, time_out, timeWin, languages, &lang_cyc, old_lang_cyc, lang_amount, lang_diff, langWin, printed_ll, wordAmount, strings, line_number, existing_space, text_line_length , testwin,textWin,awaiting_words,&config, &top_schema);

	//delete backspace from view

	unsigned result;
	do{
		touchwin(testwin);
			wrefresh(testwin);
			wrefresh(textWin);
		result = start_test_loop(timeWin, langWin, recvChar, &spaces, textstarty, textstartx, time_out_num, time_cyc, testwin, textWin, dynamic_ll, printed_ll, strings, wordAmount, 0, existing_space, text_line_length, awaiting_words, languages, &config, &top_schema, &dynamic_ll);
		//result is 0 if reinitiate test 
		//and 1 to finish

		if (result==0) {
			strings = restart_text(timeWin, langWin,time_out_num, languages, time_cyc, printed_ll, wordAmount, strings, 0, existing_space, text_line_length, textWin, awaiting_words, languages[time_cyc], &config, &top_schema);
			// problem could start here. some pointer becomes unvalid or misused.
			werase(timeWin);
			werase(langWin);
			wprintw(timeWin, "%lu [Backspace]", time_out_num);
			wprintw(langWin, "%s [Space]", languages[time_cyc]);
			touchwin(testwin);
			wrefresh(testwin);
			wrefresh(textWin);


			recvChar = getch();
			strings = start_keys(&recvChar, &time_cyc, timeopts, &time_out_num, time_out, timeWin, languages, &lang_cyc, old_lang_cyc, lang_amount, lang_diff, langWin, printed_ll, wordAmount, strings, line_number, existing_space, text_line_length , testwin,textWin,awaiting_words,&config, &top_schema);
		}else{
			break;
		}
	}while(true);

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
		if(correctWord){
			correctWordChars+= printed_ll->length;
		}


	}
	// wpm: all correct chars in correct words + spaces / 5 * 60/15
	// raw wpm: all chars + spaces /5 ) * 60/15
	// corrects: all correct chars + spaces 
	// acc: all corrects / all chars

	wprintw(testwin,"wpm: %f, raw wpm: %f, accuracy: %f", ((correctWordChars)/5.0)*(60.0/((float)time_out_num)), (charAmount/5)*(60/((float)time_out_num)), (corrects/charAmount)*100);
	wprintw(testwin,"\ncalcs-> correct chars in full words + spaces: %f, charAmount: %f,  corrects total: %f, spaces: %f", correctWordChars, charAmount, corrects, spaces);
	wprintw(testwin, "\nlanguage: %s; time: %lu", languages[time_cyc], time_out_num );

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
//everything should pass like a documentation. 
