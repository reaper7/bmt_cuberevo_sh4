#ifndef SCIPTOMAN_H_
#define SCIPTOMAN_H_

//name & ver
#define PLUG_NAME "SCRIPTOMAN BMT"
#define PLUG_VERSION "1.0"

//fonts
#define FONT_SIZE						16
#define FONT_SIZE_TITLE			FONT_SIZE + 2

//buttons
#define MARGIN_X						5
#define MARGIN_Y						5
#define ONE_STEP						5
#define BUTTON_SIZE					18

//screen
#define SCREEN_W						1280
#define SCREEN_H						720
#define TITLE_H							(MARGIN_Y  + FONT_SIZE_TITLE)
#define NUM_LINES						5
#define BACKGROUND_COLOR		0x401f1f1f
#define BACKGROUND_W				220
#define BACKGROUND_H				(TITLE_H + (MARGIN_Y * 2) + (BUTTON_SIZE * NUM_LINES) + (ONE_STEP * (NUM_LINES-1)))
#define BACKGROUND_LEFT			((SCREEN_W / 2)-(BACKGROUND_W / 2)) 
#define BACKGROUND_TOP			((SCREEN_H / 2)-(BACKGROUND_H / 2))

//rect
#define RECT_GRAY						0xFF7F7F7F
#define LINE_SIZE						2

//text buttons caption
#define CAPTION_COLOR						0xFFCCCCCC
#define CAPTION_BGR_COLOR				0x702F2F2F
#define CAPTION_X_BGR_START			(MARGIN_X+BUTTON_SIZE)
#define CAPTION_X_BGR_STOP			(BACKGROUND_W - (MARGIN_X * 2) - BUTTON_SIZE)
#define CAPTION_X_START					((MARGIN_X * 2) + BUTTON_SIZE)

//title box
#define TITLE_X_POZ							MARGIN_X
#define TITLE_Y_POZ							(MARGIN_Y / 2)

//SCR BOX
#define SCR_X_LEFT_BUTTON_POZ		MARGIN_X
#define SCR_X_RIGHT_BUTTON_POZ	BACKGROUND_W-MARGIN_X-BUTTON_SIZE
#define SCR_Y_POZ								TITLE_H + MARGIN_Y
#define BUTTON_COLOR_SCR				0xFF7f7f7f

//RED button (STOP)
#define RED_X_POZ MARGIN_X
#define RED_Y_POZ SCR_Y_POZ+BUTTON_SIZE+ONE_STEP
#define BUTTON_COLOR_RED 0xFFB93C36

//GREEN button (START)
#define GREEN_X_POZ MARGIN_X
#define GREEN_Y_POZ RED_Y_POZ+BUTTON_SIZE+ONE_STEP
#define BUTTON_COLOR_GREEN 0xFF49B936

//YELLOW button (SET AS DEFAULT)
#define YELLOW_X_POZ MARGIN_X
#define YELLOW_Y_POZ GREEN_Y_POZ+BUTTON_SIZE+ONE_STEP
#define BUTTON_COLOR_YELLOW 0xFFB9B936

//BLUE button (RESTART)
#define BLUE_X_POZ MARGIN_X
#define BLUE_Y_POZ YELLOW_Y_POZ+BUTTON_SIZE+ONE_STEP
#define BUTTON_COLOR_BLUE 0xFF3C65B2

//scr buttons text area
#define SCR_X_LEFT_BUTTON_START	(SCR_X_LEFT_BUTTON_POZ + (MARGIN_X / 2))
#define SCR_X_RIGHT_BUTTON_START	(SCR_X_RIGHT_BUTTON_POZ + (MARGIN_X / 2))	
#define SCR_BUTTONS_COLOR 0xFF000000

//scr name text area
#define SCR_CAPTION_COLOR 0xFFEEEEEE
#define SCR_NAME_BGR_START	(SCR_X_LEFT_BUTTON_POZ + BUTTON_SIZE)
#define SCR_NAME_BGR_STOP	(BACKGROUND_W-(MARGIN_X *2)-(BUTTON_SIZE *2))
#define SCR_TEXT_X_START	(SCR_NAME_BGR_START + (MARGIN_X / 2))

#define SCRIPT_PATH		"/var/bin/"
#define CONF_FILE			"/var/etc/scr_conf"

/* global variable */
window_t *window = NULL;

struct datas
{
	char realname[256];
  char scrname[256];
  int scrtype;
};

struct datas *my_scrs=NULL;

int num_scrs=0;												//ilosc znalezionych plików "scr_xxxxx.sh" w katalogu /var/bin
int sel_scr=0;												//indeks wybranego przez uzytkownika scr
int act_scr=0;												//zapamietany index scr aktualnie uzywanego (w scr_conf)  
int sel_command=0;										//komenda wybrana przez uzytkownika (red=1,green=2,yellow=3,blue=4)
int t_index=0;

char * commands [4] = {"", " stop", " start", " restart"};

#endif /* SCIPTOMAN_H_ */

