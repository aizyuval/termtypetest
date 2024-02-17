#include <curses.h>
#include <ncurses.h>
#include <cyaml/cyaml.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#define KEY_ENT 10
#define KEY_BACKTICK 96
#define KEY_TAB 9
#define KEY_SPACE 32 
#define MISTAKEN_CHAR 0
#define CORRECT_CHAR 1
#define	NONTYPED_CHAR 2
#define GREEN 3 
#define RED 2
#define homedi getenv("HOME")
#define homedi_len strlen(homedi)
#define _word_size sizeof(struct _word)

struct stats_metrics {// structure for storing a test metrics

	float wpm;
	float rwpm;
	float acc;
};
struct stats_cat { // structure for storing a time category stats
	struct stats_metrics best; // instance of stats metrics
	struct stats_metrics *tests; // array of stats metrics
	unsigned int tests_count;
};
struct stats_file {// top level structure for storing lanaguage stats
	struct stats_cat *times;
	unsigned int times_count;

};


struct stats_result { // structure for passing test data between functions
	struct stats_metrics statistics;
	char *language;
	bool pb;
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
struct stats_result * spawn_test(WINDOW * testwin,struct stats_file ** address_to_stats, struct stats_file * stats, const cyaml_config_t * config,const cyaml_schema_value_t *stats_file_schema_top, int * recvChar);
struct _word * init_text (struct _word * initializer_ll, unsigned int * wordAmount, struct _strings * strings, int line_number, int * existing_space, WINDOW * textWin, unsigned int * awaiting_words);

WINDOW *create_statswin();
void spawn_stats(WINDOW * statswin, struct stats_result *statisticsP, struct stats_file *stats, int * recvChar);



void destroy_win(WINDOW *local_win);

void wrap_insch(WINDOW * win, int existing_space[], int line_number, int text_line_length, unsigned int * awaiting_words, char recvChar, int in_x, int in_y);
void wrap_delch(WINDOW * win, int existing_space[], int line_number, int text_line_length, unsigned int * awaiting_words,struct _strings * strings, int in_x);


struct _strings * start_keys(struct stats_file ** address_to_stats,struct stats_file * stats,const cyaml_schema_value_t *stats_file_schema_top,int *recvChar, unsigned * time_cyc,unsigned timeopts, long * time_out_num, long time_out[], WINDOW *timeWin, char **languages, unsigned * lang_cyc, unsigned old_lang_cyc, unsigned lang_amount, signed lang_diff, WINDOW * langWin, struct _word * printed_ll, unsigned int * wordAmount, struct _strings * strings, int line_number, int existing_space[], int text_line_length, WINDOW * testwin, WINDOW * textWin, unsigned int * awaiting_words, const cyaml_config_t * config, const cyaml_schema_value_t * top_schema);

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

	//before spawning test, load data to structs for test/stats windows uses ( need to reload everytime I Save something to it and run a test/stats
	static const cyaml_schema_field_t stats_metrics_field_schema[] = {
		CYAML_FIELD_FLOAT("wpm",CYAML_FLAG_DEFAULT, struct stats_metrics, wpm),
		CYAML_FIELD_FLOAT("rwpm",CYAML_FLAG_DEFAULT, struct stats_metrics, rwpm),
		CYAML_FIELD_FLOAT("acc",CYAML_FLAG_DEFAULT, struct stats_metrics, acc),

		CYAML_FIELD_END
	};
	static const cyaml_schema_value_t stats_metrics_schema = {
		CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, struct stats_metrics, stats_metrics_field_schema),
	};

	static const cyaml_schema_field_t stats_cat_field_schema[] = {
		CYAML_FIELD_MAPPING(
			"best", CYAML_FLAG_DEFAULT, struct stats_cat, best, stats_metrics_field_schema),
		CYAML_FIELD_SEQUENCE("tests", CYAML_FLAG_POINTER, struct stats_cat, tests,&stats_metrics_schema,0,CYAML_UNLIMITED), // tests is sequence without known size
		CYAML_FIELD_END
	};
	static const cyaml_schema_value_t stats_cat_schema = {
		CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, struct stats_cat, stats_cat_field_schema),
	};


	static const cyaml_schema_field_t stats_file_field_schema[] = {
		CYAML_FIELD_SEQUENCE("times", CYAML_FLAG_POINTER, struct stats_file, times, &stats_cat_schema, 0,CYAML_UNLIMITED),
		CYAML_FIELD_END
	};

	static const cyaml_schema_value_t stats_file_schema_top = {
		CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct stats_file, stats_file_field_schema),
	};



	static const cyaml_config_t config = {
		.log_fn = cyaml_log,
		.mem_fn = cyaml_mem,
		.log_level = CYAML_LOG_WARNING,
	};
	int * recvChar = malloc(2);
	(*recvChar) = ' ';
	while ((*recvChar) != KEY_ENT) { // as long as he doesn't exit, this program runs

		(*recvChar) = ' ';
		struct stats_file *stats; 
		struct stats_result *statisticsP;

		if(homedi==NULL){printw("couldnt get your home dir");}

		char * englishstat = "/termtypetest/stats/english200.yaml";
		char * homedir = malloc(homedi_len+strlen(englishstat));

		strcpy(homedir, homedi);
		strcat(homedir,englishstat);

		cyaml_err_t stats_err = cyaml_load_file(homedir, &config,
					  &stats_file_schema_top, (cyaml_data_t **)&stats, NULL); //      not here!
		if(stats_err != CYAML_OK){
			printw("we cannot load stats file for some reason!");
			getch();
			endwin();
		}

		WINDOW * testwin = create_testwin();
		statisticsP = spawn_test(testwin, &stats,stats,&config,&stats_file_schema_top, recvChar);	
		destroy_win(testwin);	


		WINDOW * statswin = create_statswin();
		spawn_stats(statswin, statisticsP, stats, recvChar);
		while ((*recvChar)!=KEY_TAB&&((*recvChar)!=KEY_ENT)){
			(*recvChar) = getch(); // hanging until he goes back to the test
		}

		destroy_win(statswin);	
		cyaml_err_t err = cyaml_free(&config, &stats_file_schema_top, stats, 0); 
		if(err !=CYAML_OK){
			wprintw(stdscr, "error! cannot FREE loaded words");
		}
		free(statisticsP);
	}
endwin();
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

	bool outofwords = false;
	for(size_t i=0; i<(*wordAmount); initializer_ll=initializer_ll->next_word){

		int length = strlen(strings->words_member[i]);
		initializer_ll->length = (char)length;

		initializer_ll->string = malloc(length);
		strcpy(initializer_ll->string, strings->words_member[i]);

		// problem
		if(!outofwords){ // if im out of words, do nothing
			if(existing_space[line_number]<=length){
				if(line_number==2){
					outofwords = true;
				}else{
					line_number+=1;
					wmove(textWin, line_number, 0);
				}
			}
			if(existing_space[line_number]>length){// only > because if equal i can't fill it. need the user to space to move a line

				wprintw(textWin,"%s ", initializer_ll->string);
				existing_space[line_number] -= length+1;

				(*awaiting_words)++;// next word that is waiting to be printed.
				//
			}
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


struct _strings * start_keys(struct stats_file ** address_to_stats, struct stats_file * stats,const cyaml_schema_value_t *stats_file_schema_top, int *recvChar, unsigned * time_cyc,unsigned timeopts, long * time_out_num, long time_out[], WINDOW *timeWin, char **languages, unsigned * lang_cyc, unsigned old_lang_cyc, unsigned lang_amount, signed lang_diff, WINDOW * langWin, struct _word * printed_ll, unsigned int * wordAmount, struct _strings * strings, int line_number, int existing_space[], int text_line_length,WINDOW * testwin, WINDOW * textWin, unsigned int * awaiting_words, const cyaml_config_t * config, const cyaml_schema_value_t * top_schema){
	while ((*recvChar) == KEY_BACKSPACE || (*recvChar) == ' '|| (*recvChar) == KEY_TAB || (*recvChar) == KEY_BACKTICK || (*recvChar) == KEY_ENT) // or esc / Tab
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

			struct stats_file * temp = stats;
			cyaml_err_t err = cyaml_free(config, stats_file_schema_top, stats, 0); 
			if(err !=CYAML_OK){
				wprintw(stdscr, "error! cannot FREE loaded words");
				getch();
				endwin();
			}

			// change stats - free, create new one and then change it ((&stats)memcpy a new address to here
			
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

			// load new file to the stats pointer

			if(homedi==NULL){printw("couldnt get your home dir");}
			char * dir = "/termtypetest/stats/";
			char * ext = ".yaml";
			char *filep = malloc(homedi_len + strlen(dir) + strlen(languages[(*lang_cyc)]) + strlen(ext) +1);

			//memcpy(filep, homedi, homedi_len); // stats/
			strcpy(filep,homedi);
			strcat(filep, dir);
			strcat(filep, languages[(*lang_cyc)]); // stats/english200
			strcat(filep, ext); // stats/english200.yaml
			

			cyaml_err_t stats_err = cyaml_load_file(filep, config,

					   stats_file_schema_top, (cyaml_data_t **)&temp, NULL); 
			if(stats_err != CYAML_OK){
				printw("we cannot load stats file for some reason!");
				getch();
				endwin();
			}
			
			memcpy(address_to_stats, &temp, sizeof(struct stats_file *)); // changing the stats variable of the above function
			stats = temp; // changing the local variable to point to the newly created schema bbin


			strings = restart_text(timeWin, langWin,(*time_out_num), languages, (*time_cyc),printed_ll, wordAmount, strings, line_number, existing_space, text_line_length, textWin, awaiting_words, languages[(*lang_cyc)], config, top_schema); // update strings after freeing it!
			
		}else if((*recvChar) == KEY_TAB){

			strings = restart_text(timeWin, langWin, (*time_out_num), languages, (*time_cyc),printed_ll, wordAmount, strings, line_number, existing_space, text_line_length, textWin, awaiting_words, languages[(*lang_cyc)], config, top_schema);

		}else if ((*recvChar) == KEY_BACKTICK || (*recvChar) == KEY_ENT) {
			return NULL;
		}
		
		touchwin(testwin);
		wrefresh(testwin);
		wrefresh(textWin);
		(*recvChar) = getch();
	}

	return strings;

}



struct _strings * restart_text(WINDOW * timeWin, WINDOW * langWin, long time_out_num,char** languages,unsigned time_cyc, struct _word * linked_ll, unsigned int * wordAmount, struct _strings * strings, int line_number, int * existing_space, int text_line_length, WINDOW * textWin, unsigned int * awaiting_words, char *lang,const cyaml_config_t * config, const cyaml_schema_value_t * top_schema){
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


			if(homedi==NULL){printw("couldnt get your home dir");}
			char * dir = "/termtypetest/languages/";
	char * ext = ".yaml";
	char *file = malloc(homedi_len+ strlen(dir) + strlen(lang) + strlen(ext) +1);

	//memcpy(file, homedi, homedi_len); // languages/
	strcpy(file, homedi);
	strcat(file, dir);
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
			wmove(win, line_number, in_x);

		}else {

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

// mistyped spaces for when he mistypes discount from eventually space

unsigned start_test_loop(WINDOW * timeWin, WINDOW * langWin, int * recvChar, float * spaces, float * mistyped_spaces, int textstarty, int textstartx, long time_out_num, unsigned time_cyc, WINDOW * testwin, WINDOW * textWin, struct _word * dynamic_ll, struct _word * printed_ll, struct _strings * strings, unsigned int * wordAmount, int line_number, int * existing_space, int text_line_length, unsigned int * awaiting_words, char ** languages, const cyaml_config_t * config, const cyaml_schema_value_t * top_schema, struct _word ** dynamic_ll_ptr){



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
			(*recvChar) = getch();
			if((*recvChar) == ERR){
				continue;
			}
		}else { // we already got the first key stroke.
			middleRun++;

		}

		if ((*recvChar) == KEY_TAB) {
			// stop this test. return 0;
			timeout(-1);// cancel timeout as the test has stopped.
			return 0;
		}

		if ((*recvChar) == KEY_BACKSPACE){ 

			getyx(textWin, row, col);
			if(row == textstarty && col == textstartx){// cant backspace the start
				continue;
			}
			if(relCursor==0){// backspaces a space
				dynamic_ll = dynamic_ll->prev_word;
				relCursor = dynamic_ll->length; 
				col -=1;
				wmove(textWin,row,col);	
				(*spaces)--;

				// check if the word is overworded, or underworded
				if(dynamic_ll->overword){ // word is overworded
					relCursor += dynamic_ll->overword;
				}else
				{	// discover underworded position (if exists)
					for (unsigned char i=0; i<dynamic_ll->length; i++) {
						if (dynamic_ll->ver_arr[i]==NONTYPED_CHAR) {
							relCursor=i;
							wmove(textWin,row, col-(dynamic_ll->length-relCursor));
							(*mistyped_spaces)--;	
							(*spaces)++; // a space wasn't written to him, so he shouldn't lose one.
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

		else if((*recvChar) == ' '){
			// if next_word->dynamic_ll->string is null, he is a god and finished everthing(what about mistakes?)
			// drop line when there's space in the end

			if (relCursor<dynamic_ll->length){//he spaces in the middle of the word
				memset((dynamic_ll->ver_arr + relCursor), NONTYPED_CHAR, (dynamic_ll->length - relCursor)); // tab to the next word, non-type next chars.
				getyx(textWin, row, col);
				wmove(textWin,row, col+(int)(dynamic_ll->length - relCursor));// what if it drops a line? ERROR
				(*mistyped_spaces)++;
			}else {
				(*spaces)++;
			}


			getyx(textWin, row, col);

			if(col== (text_line_length-1-existing_space[line_number])){// if he is on the last space, move to next line
				if(row == 2){

					werase(textWin);
					wmove(textWin, 0, 0);
					line_number = 0;
					bool outofwords = false;

					existing_space[0] = text_line_length;
					existing_space[1] = text_line_length;
					existing_space[2] = text_line_length;

					while(outofwords==false){
						if(existing_space[line_number]<=strlen(strings->words_member[(*awaiting_words)])){
							if(line_number==2){
								outofwords=true;
							}
							line_number+=1;
							wmove(textWin, line_number, 0);
						}
						if(existing_space[line_number]>strlen(strings->words_member[(*awaiting_words)])){// only > because if equal i can't fill it. need the user to space to move a line
							wprintw(textWin,"%s ", strings->words_member[(*awaiting_words)]);
							existing_space[line_number] -= strlen(strings->words_member[(*awaiting_words)])+1;

							(*awaiting_words)++;// next word that is waiting to be printed.
						}
					}

					wmove(textWin, 0, 0);
					line_number = 0;
					touchwin(testwin);
					wrefresh(testwin);
					wrefresh(textWin);
					// last line, last col, typed ' '
					// clear all, print new
					// move first
					// line 0
				}else {

					wmove(textWin,row+1,0);
					line_number++;
				}
			}else{
				waddch(textWin, (*recvChar));
			}
			dynamic_ll=dynamic_ll->next_word;
			relCursor=0;
		}

		else if (relCursor >= dynamic_ll->length){//overwording 
			getyx(textWin, row, col);
			if(col == (text_line_length-1-existing_space[line_number]) || dynamic_ll->overword ==5){
				// if he is overwording by 8, stop insch'ing
				// if he is at the end of the line, don't allow overword
				continue;
			}
			wrap_insch(textWin, existing_space, line_number, text_line_length, awaiting_words, (*recvChar), row, col);
			wchgat(textWin,1, COLOR_PAIR(RED), RED,NULL);

			wmove(textWin,row,col+1);	
			dynamic_ll->overword += 1; 
			relCursor++;
		}

		else  { //recvChar = normal key
			// implement clearing first line, writing new line
			if((*recvChar) == dynamic_ll->string[relCursor]){ 
				waddch(textWin,(*recvChar) | COLOR_PAIR(GREEN));
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







struct stats_result * spawn_test(WINDOW *testwin, struct stats_file ** address_to_stats,struct stats_file * stats, const cyaml_config_t * config,const cyaml_schema_value_t *stats_file_schema_top, int * recvChar){
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
	WINDOW *exitWin = derwin(testwin,1, switches_length, textstarty+9,  (textstartx-1) + (text_line_length)/2- (switches_length)/2);// create a newwin inside the current one for better positioning and wrapping
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


	char * filep = "/termtypetest/languages/english200.yaml";
	char * hd = malloc(homedi_len+strlen(filep));
	strcpy(hd, homedi);

	if(hd==NULL){printw("couldnt get your home dir");}

	strcat(hd, filep);

	cyaml_err_t err = cyaml_load_file(hd, config,
				   &top_schema, (cyaml_data_t **)&strings, NULL); // load file onto the struct strings
	if(err!=CYAML_OK){
		printw("err. cannot load english 200 file");
	}

	shuffle(strings->words_member, strings->words_member_count);
	err = cyaml_save_file(hd, config, &top_schema, strings,0); // after shuffle, save the shuffled words to file. that way each time he shuffles he will have a random list to previous ones.
	//
	if(err!=CYAML_OK){
		printw("err. cannot save to the egnlish 200 yaml");
	}




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


	float spaces = 0, mistyped_spaces = 0;

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
	wprintw(togglesetWin, "stats [ ` ]");
	wprintw(exitWin, "exit [Enter]");

	touchwin(testwin); // only touching the father-window and then refreshing it to update all. then refreshing textWin to put the cursor back there.
	wrefresh(testwin);
	wrefresh(textWin);


	(*recvChar) = getch();

	struct _strings *protoS;
	protoS = start_keys(&stats, stats, stats_file_schema_top,recvChar, &time_cyc, timeopts, &time_out_num, time_out, timeWin, languages, &lang_cyc, old_lang_cyc, lang_amount, lang_diff, langWin, printed_ll, wordAmount, strings, line_number, existing_space, text_line_length , testwin,textWin,awaiting_words,config, &top_schema);
	memcpy(address_to_stats, &stats, sizeof(struct stats_file *)); // changing the global variable, stats, to point to the newly created schema.
	
	if(protoS == NULL)
	{

		//struct stats_result * statisticsP = malloc(sizeof(struct stats_result)); // save to this struct the result and return it to stats_win
		//statisticsP->language=languages[time_cyc];
		return NULL;
		// returning null meaning no test was committed and he wants to go to stats
	}


	strings = protoS;
	//delete backspace from view

	unsigned result;
	do{
		touchwin(testwin);
		wrefresh(testwin);
		wrefresh(textWin);
		result = start_test_loop(timeWin, langWin, recvChar, &spaces, &mistyped_spaces, textstarty, textstartx, time_out_num, time_cyc, testwin, textWin, dynamic_ll, printed_ll, strings, wordAmount, 0, existing_space, text_line_length, awaiting_words, languages, config, &top_schema, &dynamic_ll);
		//result is 0 if reinitiate test 
		//and 1 to finish

		refresh();
		if (result==0) {
			strings = restart_text(timeWin, langWin,time_out_num, languages, time_cyc, printed_ll, wordAmount, strings, 0, existing_space, text_line_length, textWin, awaiting_words, languages[time_cyc], config, &top_schema);
			// problem could start here. some pointer becomes unvalid or misused.
			werase(timeWin);
			werase(langWin);
			wprintw(timeWin, "%lu [Backspace]", time_out_num);
			wprintw(langWin, "%s [Space]", languages[lang_cyc]);
			touchwin(testwin);
			wrefresh(testwin);
			wrefresh(textWin);


			(*recvChar) = getch();

			struct _strings *protoS;
			protoS = start_keys(&stats, stats,stats_file_schema_top,recvChar, &time_cyc, timeopts, &time_out_num, time_out, timeWin, languages, &lang_cyc, old_lang_cyc, lang_amount, lang_diff, langWin, printed_ll, wordAmount, strings, line_number, existing_space, text_line_length , testwin,textWin,awaiting_words,config, &top_schema);
			memcpy(address_to_stats, &stats, sizeof(struct stats_file *)); // changing the global variable, stats, to point to the newly created schema.
			if(protoS==NULL){
				//struct stats_result * statisticsP = malloc(sizeof(struct stats_result)); // save to this struct the result and return it to stats_win
				//statisticsP->language=languages[time_cyc];
				return NULL;
			}
			strings = protoS;
		}else{
			break;
		}
	}while(true);

	timeout(-1); // cancel timeout for getch()
	//wmove(testwin, row+1, col);


	// initialize with spaces:

	float corrects = spaces;
	float charAmount = spaces + mistyped_spaces;
	float correctWordChars = 0;
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
			correctWordChars+= printed_ll->length + 1;// + 1 for correctly typed space
		}


	}
	// wpm: all correct chars in correct words + spaces / 5 * 60/15
	// raw wpm: all chars + spaces /5 ) * 60/15
	// corrects: all correct chars + spaces 
	// acc: all corrects / all chars
	//
	//
	//
	// important to verify: when backspacing, change the ver_arr of the thing to nontyped.

	struct stats_result * statisticsP = malloc(sizeof(struct stats_result)); // save to this struct the result and return it to stats_win

	statisticsP->statistics.wpm = ((correctWordChars)/5.0)*(60.0/((float)time_out_num));
	statisticsP->statistics.rwpm = (charAmount/5)*(60/((float)time_out_num));
	statisticsP->statistics.acc =  (corrects/charAmount)*100;

	// instead of strcpy that I don't know how it worked...
	statisticsP->language =  languages[lang_cyc];

	statisticsP->pb = false; // assuming it's not the pb

	// check if it had surpassed the best score and act upon it
	// time_cyc is my number here, our stats located in stats->times[time_cyc].best....
	if(statisticsP->statistics.wpm >= stats->times[time_cyc].best.wpm){

		if(statisticsP->statistics.wpm == stats->times[time_cyc].best.wpm){
			if(statisticsP->statistics.acc>stats->times[time_cyc].best.acc){
				stats->times[time_cyc].best = statisticsP->statistics; // if it has the same wpm and bigger acc, replace
				statisticsP->pb = true;
			}
		}else{
			stats->times[time_cyc].best = statisticsP->statistics; // it has bigger wpm, replace
			statisticsP->pb = true;
		}

	}

	unsigned int testsC = stats->times[time_cyc].tests_count;
	struct stats_metrics *temp = realloc(
			stats->times[time_cyc].tests,
			sizeof(*stats->times[time_cyc].tests) * (testsC + 1));

	if (temp == NULL) {
		printw("adding test to list failed");
		// Handle error
	}
	stats->times[time_cyc].tests = temp; // newly allocated memory


	stats->times[time_cyc].tests[testsC] = statisticsP->statistics; // it appends it in the local file but not in the yaml
	stats->times[time_cyc].tests_count++; // increment count

	//stats->times[time_cyc].tests[0].wpm = 1000; // prove for saving
	//
	// hh dd getenv is already defined
			char * dir = "/termtypetest/stats/";
			char * ext = ".yaml";
			char *file = malloc(homedi_len+ strlen(dir) + strlen(languages[lang_cyc]) + strlen(ext) +1);
			strcpy(file, homedi);// start with homedi
			
			strcat(file, dir);
	//
			strcat(file, languages[lang_cyc]); // stats/english200
			strcat(file, ext); // stats/english200.yaml
			

	err = cyaml_save_file(file, config, stats_file_schema_top, stats,0); 

	if(err!=CYAML_OK){
		printw("err dreaded, canno tsave to stats file");
	}
	refresh();


	err = cyaml_free(config, &top_schema, strings, 0); 
	if(err !=CYAML_OK){
		wprintw(stdscr, "error! cannot FREE loaded words");
		getch();
		endwin();
	}


	return statisticsP;
}
void spawn_stats(WINDOW * statswin, struct stats_result * statisticsP, struct stats_file *stats, int * recvChar){
	if(statisticsP == NULL){
		//wprintw(statswin, "language: %s", statisticsP->language); //  segmentation fault, no lang...
		wprintw(statswin, "this are general stats(for english200):\n \
time15: \n wpm: %f; raw wpm: %f; accuracy: %f\n \
time30: \n wpm: %f; raw wpm: %f; accuracy: %f\n \
time60: \n wpm: %f; raw wpm: %f; accuracy: %f\n \
time120: \n wpm: %f; raw wpm: %f; accuracy: %f\n", 
	  stats->times[0].best.wpm, stats->times[0].best.rwpm, stats->times[0].best.acc,
	  stats->times[1].best.wpm, stats->times[1].best.rwpm, stats->times[1].best.acc,
	  stats->times[2].best.wpm, stats->times[2].best.rwpm, stats->times[2].best.acc,
	  stats->times[3].best.wpm, stats->times[3].best.rwpm, stats->times[3].best.acc);
	}else {
		if(statisticsP->pb){
			wprintw(statswin, "\nTHIS IS A NEW Personal Best. CONGRATS!!!\n");
		}

		wprintw(statswin, "language: %s", statisticsP->language);
		wprintw(statswin, "RESULTS: \nwpm: %f\n raw wpm: %f\n accuracy: %f\n", statisticsP->statistics.wpm, statisticsP->statistics.rwpm, statisticsP->statistics.acc);
	}
	wprintw(statswin, "restart [Tab]\n exit [Enter]");
	wprintw(statswin, "\n:)");
	wrefresh(statswin);


}

void destroy_win(WINDOW *local_win)
{	
	werase(local_win);
	//werase(stdscr);
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}
// segment with 2 spaces!
