//define
#define _GNU_SOURCE

//include
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h> 
#include <fcntl.h>
#include <stddef.h>
#include <dirent.h>
#include <regex.h>						//reg expr.
#include <pthread.h>
//include cuberevo sdk
#include "plugin/plugin.h"
#include "gui/gui.h"
#include "directory.h"
#include "event/event.h"
#include "ui/wm.h"
#include "font/font.h"
#include "background.h"
//include cuberevo svn
#include "front_display/front_display.h"
#include "config_manager/config_manager.h"
//include main
#include "include/scriptoman.h"

plugin_info( author,    "Reaper7" );
plugin_info( rev,	PLUG_VERSION );
plugin_info( desc,	PLUG_NAME );
plugin_info( ldesc,     "SCRIPT Manager" );
//------------------------------------------------------------------------------------------------------------------
void make_screen(void)
{
	rect_t rect = { BACKGROUND_LEFT, BACKGROUND_TOP, BACKGROUND_W, BACKGROUND_H };

	/* create FB_WINDOW */
	window = wm_create(
			&rect,		/* coordinate */
			PLUG_NAME,	/* name of window */
			NULL,		/* redraw callback */
			NULL,		/* parent window */
			COLOR_TRANSPARENT /* color key for TRANSPARENT */
	);

	//background
	gc_set_fc(window->gc,BACKGROUND_COLOR); /* foreground color */
	gc_set_bc(window->gc,BACKGROUND_COLOR); /* foreground color */
	gui_fillrect(&window->gui,window->gc,0,0,BACKGROUND_W, BACKGROUND_H);

	//border lines
	gc_set_fc(window->gc,RECT_GRAY);
	gt_hfillrect( &window->gui, window->gc, 0, 0, BACKGROUND_W, LINE_SIZE );																													//up first
	gt_hfillrect( &window->gui, window->gc, 0, (TITLE_H-(LINE_SIZE/2)), BACKGROUND_W, LINE_SIZE );																		//up second
	gt_hfillrect( &window->gui, window->gc, 0, (BACKGROUND_H-LINE_SIZE), BACKGROUND_W, LINE_SIZE );																		//down last
	gt_vfillrect( &window->gui, window->gc, 0, 0, LINE_SIZE, BACKGROUND_H );																													//left
	gt_vfillrect( &window->gui, window->gc, (BACKGROUND_W-LINE_SIZE), 0, LINE_SIZE, BACKGROUND_H );																		//right

	//buttons
	//scr left & right
	gc_set_fc(window->gc,BUTTON_COLOR_SCR);
	gt_fillrect( &window->gui, window->gc, SCR_X_LEFT_BUTTON_POZ, SCR_Y_POZ, BUTTON_SIZE, BUTTON_SIZE);
	gt_fillrect( &window->gui, window->gc, SCR_X_RIGHT_BUTTON_POZ, SCR_Y_POZ, BUTTON_SIZE, BUTTON_SIZE);
	//red
	gc_set_fc(window->gc,BUTTON_COLOR_RED);
	gt_fillrect( &window->gui, window->gc, RED_X_POZ, RED_Y_POZ, BUTTON_SIZE, BUTTON_SIZE);
	//green
	gc_set_fc(window->gc,BUTTON_COLOR_GREEN);
	gt_fillrect( &window->gui, window->gc, GREEN_X_POZ, GREEN_Y_POZ, BUTTON_SIZE, BUTTON_SIZE);
	//yellow
	gc_set_fc(window->gc,BUTTON_COLOR_YELLOW);
	gt_fillrect( &window->gui, window->gc, YELLOW_X_POZ, YELLOW_Y_POZ, BUTTON_SIZE, BUTTON_SIZE);
	//blue
	gc_set_fc(window->gc,BUTTON_COLOR_BLUE);
	gt_fillrect( &window->gui, window->gc, BLUE_X_POZ, BLUE_Y_POZ, BUTTON_SIZE, BUTTON_SIZE);

	//text general
	gc_set_fc(window->gc,CAPTION_BGR_COLOR);
	//button caption box
	gt_fillrect( &window->gui, window->gc, CAPTION_X_BGR_START, RED_Y_POZ, CAPTION_X_BGR_STOP, BUTTON_SIZE);
	gt_fillrect( &window->gui, window->gc, CAPTION_X_BGR_START, GREEN_Y_POZ, CAPTION_X_BGR_STOP, BUTTON_SIZE);
	gt_fillrect( &window->gui, window->gc, CAPTION_X_BGR_START, YELLOW_Y_POZ, CAPTION_X_BGR_STOP, BUTTON_SIZE);
	gt_fillrect( &window->gui, window->gc, CAPTION_X_BGR_START, BLUE_Y_POZ, CAPTION_X_BGR_STOP, BUTTON_SIZE);
	//buttons caption text
	char buf[25];
	struct _font font_head0 = { .size = FONT_SIZE };
	struct _font font_head1 = { .size = FONT_SIZE_TITLE };
	gc_set_fc(window->gc,CAPTION_COLOR);
	//title
	sprintf(buf, "%s",PLUG_NAME);
	int title_len=font_width_str(&font_head1, buf, strlen(buf));
	font_draw_str(&font_head1, buf, strlen(buf), &window->gui, window->gc, ((BACKGROUND_W/2)-(title_len/2)), TITLE_Y_POZ);
	//others buttons
	gc_set_fc(window->gc,CAPTION_COLOR);
	sprintf(buf, "%s","Stop script");
	font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, CAPTION_X_START, RED_Y_POZ);	
	sprintf(buf, "%s","Start script");	
	font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, CAPTION_X_START, GREEN_Y_POZ);
	sprintf(buf, "%s","Set as default");	
	font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, CAPTION_X_START, YELLOW_Y_POZ);
	sprintf(buf, "%s","Restart script");	
	font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, CAPTION_X_START, BLUE_Y_POZ);
	//scr buttons
	gc_set_fc(window->gc,SCR_BUTTONS_COLOR);
	sprintf(buf, "%s","<");
	font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, SCR_X_LEFT_BUTTON_START, SCR_Y_POZ);
	sprintf(buf, "%s",">");
	font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, SCR_X_RIGHT_BUTTON_START, SCR_Y_POZ);
	
	rect.x = 0;
	rect.y = 0;
	rect.w = BACKGROUND_W;
	rect.h = BACKGROUND_H;
	wm_show(window);
}
//------------------------------------------------------------------------------------------------------------------
int get_scrs(void)
{
	DIR *dirscr;
  regex_t regex_scr;
  regex_t regex_scrplg;
	struct dirent *ep;
	char filename[255];
	int pos=0;
  int found_regex=0;

	//NO_SCR_POS_0
	sprintf(my_scrs[0].realname, "%s%s", SCRIPT_PATH, "no_script");	
	sprintf(my_scrs[0].scrname, "%s", "no_script");	

	char *pattern = "[scr_]{4}+[a-zA-Z_0-9]+[.sh]{3}";																																		//maska wyszukiwania dla skryptow scr: scr_xxxxx.sh
  char *pattern_scrplg = "[scrplg-]{7}+[a-zA-Z_0-9]+[.sh]{3}";                                                          //maska wyszukiwania dla skryptow scrplg: scrplg-xxxxx.sh

	if (regcomp(&regex_scr, pattern, REG_EXTENDED) != 0) return (-1);																											//sprawdzenie poprawnosci maski scr_
	if (regcomp(&regex_scrplg, pattern_scrplg, REG_EXTENDED) != 0) return (-1);																						//sprawdzenie poprawnosci maski scrplg-

	dirscr=opendir(SCRIPT_PATH);																																													//otwieranie katalogu /var/bin
	if (dirscr != NULL)  																																																	//jesli istnieje/otworzyl sie to...
	{
		printf("%s SEARCHING SCRIPT(s)...\n", PLUG_NAME);
		while ((ep=readdir(dirscr)) != NULL) 																																								//odczywywanie kolejnych plikow w katalogu
		{
			memset(&filename, 0, sizeof(filename));																																						//czyszczenie filename
			strcat(filename, ep->d_name);																																											//przepisywanie nazwy pliku

			if (regexec(&regex_scr, filename, 0, NULL, 0) == 0)																																//jesli aktualny plik pasuje do maski to...
        found_regex=1;                                                                                                  //pasuje do scr_
      else if (regexec(&regex_scrplg, filename, 0, NULL, 0) == 0)    
        found_regex=2;                                                                                                  //pasuje do scrplg
      else
        found_regex=0;                                                                                                  //nie pasuje

      if (found_regex != 0)                                                                                            //zapiszdo_tablicy_pasujące                                                        
			{
				pos++;																																																					//zwieksza ilosc znalezionych poprawnych plikow
        my_scrs[pos].scrtype=found_regex;                                                                               //zapis typu scr - 1->scr_ 2->scrplg-
				sprintf(my_scrs[pos].realname, "%s%s", SCRIPT_PATH, filename);																									//zapis pelniej sciezki pliku do tablicy pod nr pos
        if (found_regex==1)                                                                                             //zapis skroconej nazwy scr (xxxxx) do tablicy pod nr pos
				  sprintf(my_scrs[pos].scrname, "%s", strndup(filename + 4, (strlen(filename))-7));		
        else if (found_regex==2)			
          sprintf(my_scrs[pos].scrname, "%s", strndup(filename + 7, (strlen(filename))-10));													
				printf("%s POS=%d TYPE=%d NAME=%s REALNAME=%s\n", PLUG_NAME, pos, found_regex, my_scrs[pos].scrname, my_scrs[pos].realname);
			}
		}
    (void) closedir (dirscr);																																														//zamkniecie katalogu
		return (pos);																																																				//zwraca ilosc znalezionych poprawnych plikow
  }
  else																																																									//jesli nic nie zostalo znalezione to...
  {
		return (-1);																																																				//zwraca -1
  }
}
//------------------------------------------------------------------------------------------------------------------
void scr_name_update(int pos)
{
	char buf[25];
	int text_len;

	gc_set_fc(window->gc,CAPTION_BGR_COLOR);																																							//ustawienie koloru tla pod napis wybranego scr
	gt_fillrect( &window->gui, window->gc, SCR_NAME_BGR_START, SCR_Y_POZ, SCR_NAME_BGR_STOP, BUTTON_SIZE);								//wypelnienie kolorem obszaru pod napisem wybranego scr

	struct _font font_head0 = { .size = FONT_SIZE };																																			//wielkosc czcionki
	gc_set_fc(window->gc,SCR_CAPTION_COLOR);																																							//kolor czcionki
	if (pos==0) {																																																					//jesli nie ma domyslnego scr to...
		sprintf(buf, "%s","NO SCRIPT");																																												//wyswietla napis "NO SCRIPT"
	} else {																																																							//jesli jest domyslne scr to...
		sprintf(buf, "%s",my_scrs[pos].scrname);																																						//wyswietla skrocona nazwe scr
	}
	front_display_write_msg(buf, FRONT_DISPLAY_NOPAD);
	printf("%s CHANGES MENU TO: %s\n", PLUG_NAME, buf);
	text_len=font_width_str(&font_head0, buf, strlen(buf));
	font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, ((BACKGROUND_W/2)-(text_len/2)), SCR_Y_POZ);									

	rect_t rect = { SCR_NAME_BGR_START, SCR_Y_POZ, SCR_NAME_BGR_STOP, BUTTON_SIZE };
	wm_update_rect(window, &rect);
}
//------------------------------------------------------------------------------------------------------------------
void* _exec_command(void* unused)
{
	FILE *fcom;
	char answ[80];
	char tmp_ch[512];
	int tmp_scr=0;

	if (act_scr!=sel_scr) {
		tmp_scr=act_scr;
	} else {
		tmp_scr=sel_scr;
	}

	memset(&answ, 0, sizeof(answ));
	memset(&tmp_ch, 0, sizeof(tmp_ch));

	sprintf(tmp_ch, "%s%s", my_scrs[tmp_scr].realname, commands[sel_command]);
	printf("%s EXECUTE COMMAND[%d] ON SCRIPT[%d]: %s\n", PLUG_NAME, sel_command, tmp_scr, tmp_ch);

	if (!(fcom = popen(tmp_ch, "r"))) {
		printf("%s ERROR EXECUTING COMMAND\n", PLUG_NAME);
	} else {
		fgets(answ, sizeof(answ), fcom);
		if (strlen(answ)>0) {
			printf("%s MESSAGE: %s", PLUG_NAME, answ);
			memset(&tmp_ch, 0, sizeof(tmp_ch));
			sprintf(tmp_ch, "echo web_show_mess 1 \"%s\" >/dev/commander",answ);
			system(tmp_ch);
		}
		pclose(fcom);
	}
	t_index--;
	return NULL;
}
//------------------------------------------------------------------------------------------------------------------
int exec_command(void)
{
	//if ((act_scr!=0) || ((sel_scr==0) && (sel_command==1)))
	if (act_scr!=0)
	{	
		pthread_t proc_id[t_index];
		pthread_create(&proc_id[t_index++],NULL,&_exec_command,NULL);
		while (t_index!=0) {
			usleep(250000);	
		}
		printf("%s THREAD END\n", PLUG_NAME);
	}
	act_scr=sel_scr;
	return 0;
}
//------------------------------------------------------------------------------------------------------------------
int read_config(void)
{
	int pos;
	char *scr_from_conf;
	cm_file_read_configuration(CONF_FILE);
	scr_from_conf = cm_get_config_value_text("scr");
	if (scr_from_conf==NULL) return 0;
	for (pos=1; pos<=num_scrs; pos++) {
		if (strcmp(my_scrs[pos].scrname,scr_from_conf)==0) return pos;
	}
	return 0;
}
//------------------------------------------------------------------------------------------------------------------
void write_config(int pos)
{
	FILE *tmp_file = fopen(CONF_FILE, "w");
	if (tmp_file) {
		fprintf(tmp_file, "scr=%s\n", my_scrs[pos].scrname);
		fclose(tmp_file);
		printf("%s WRITING CONFIG FILE, SCRIPT=%s\n", PLUG_NAME, my_scrs[pos].scrname);
	}
}
//------------------------------------------------------------------------------------------------------------------
/* main func. */
int plugin_main(int argc, char *argv[])
{
	printf("%s START\n", PLUG_NAME);

	my_scrs = malloc(sizeof *my_scrs * 25);
	//memset(&my_scrs, 0, sizeof(my_scrs));

	num_scrs=get_scrs();
	printf("%s FOUND %d SCRIPT(s)\n", PLUG_NAME, num_scrs);

	act_scr=read_config();
	printf("%s READING CONFIG FILE, SCRIPT=%s\n", PLUG_NAME, my_scrs[act_scr].scrname);

	if (argc>1)
	{
		if(!strcmp(argv[1],"stop"))
			sel_command=1;
		else if(!strcmp(argv[1],"start"))
			sel_command=2;
		else if(!strcmp(argv[1],"reboot"))
			sel_command=3;
		if ((sel_command>=1) && (sel_command<=3))
		{
			printf("%s EXECUTING COMMAND [%s]\n", PLUG_NAME,argv[1]);
			goto execommand;
		}
	}

	int key_code=KEY_BACK;
	int key_value=-1;

	front_display_init();
	front_display_write_msg(PLUG_NAME, FRONT_DISPLAY_NOPAD);

	make_screen();

	scr_name_update(act_scr);
	sel_scr=act_scr;

	plug_event_init();

	while (1) {
		event_t event;
		if( plug_event_rcv( &event ) < 0 )	continue;
		if( event.type != event_type_key)	continue;

		key_value = event_key_2value(&event);
		key_code = event_key_2code(&event);

		if(key_code==KEY_BACK) break;

		//if key is pressed
		if(key_value>0) 
		{
			sel_command=0;
			switch(key_code)
			{
				case KEY_RED:
					sel_command=1;
					break;
				case KEY_GREEN:
					sel_command=2;
					break;
				case KEY_YELLOW:
					write_config(sel_scr);
					break;
				case KEY_BLUE:
					sel_command=3;
					break;
				case KEY_LEFT:
					if (num_scrs!=0) {
						if (sel_scr==0) {
							sel_scr=num_scrs;
						} else {
							sel_scr--;
						}
						scr_name_update(sel_scr);
					}
					break;
				case KEY_RIGHT:
					if (num_scrs!=0) {
						if (sel_scr==num_scrs) {
							sel_scr=0;
						} else {
							sel_scr++;
						}
						scr_name_update(sel_scr);
					}
					break;
				default:
					break;
			}
		}
		if (sel_command!=0) break;
	}

	plug_event_close();
	wm_destroy(window);
	front_display_write_msg("SCRIPTOMAN END", FRONT_DISPLAY_NOPAD);
	front_display_finish();


execommand:

	if (sel_command==1) {
		exec_command();
	} else if (sel_command==2) {
//		if ((act_scr!=sel_scr) && (sel_scr!=0)) {
		if (act_scr!=sel_scr) {
			sel_command=1;
			exec_command();
			write_config(sel_scr);
		}
			sel_command=2;
			exec_command();
	} else if (sel_command==3) {
//		if ((act_scr!=sel_scr) && (sel_scr!=0)) {
		if (act_scr!=sel_scr) {
			sel_command=1;
			exec_command();
			write_config(sel_scr);
			sel_command=2;
			exec_command();
		} else {
			exec_command();
		}
	}

	free(my_scrs);
	usleep(750000);
	printf("%s END\n", PLUG_NAME);
	return 0;
}

