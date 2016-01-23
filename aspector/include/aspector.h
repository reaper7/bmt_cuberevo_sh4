#ifndef ASPECTOR_H_
#define ASPECTOR_H_

//name & ver
#define PLUG_NAME						"ASPECTOR BMT"
#define PLUG_VERSION				"2.1"

//SPECIAL DEFINITIONS
#define PLATFORM_BMT
//#define TEST_BMT

//fonts
#define FONT_SIZE						16
//#define FONT_BOTTOM_TEXT		FONT_SIZE - 2
#define FONT_BOTTOM_TEXT		FONT_SIZE
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
#define NUM_LINES						12
#define NUM_NO_BUTTONS			1
#define NUM_BUTTONS_ROW			(NUM_LINES - NUM_NO_BUTTONS)
#define BACKGROUND_COLOR		0x401f1f1f
#define BACKGROUND_W				300
#define BACKGROUND_H				(TITLE_H + (MARGIN_Y * 2) + (BUTTON_SIZE * NUM_LINES) + (ONE_STEP * (NUM_LINES)))
#define BACKGROUND_LEFT			((SCREEN_W / 2)-(BACKGROUND_W / 2)) 
#define BACKGROUND_TOP			((SCREEN_H / 2)-(BACKGROUND_H / 2))

//rect
#define RECT_GRAY						0xFF7F7F7F				//moje
#define LINE_SIZE						2

#define CAPTION_BUTTON_COLOR	0xFF000000
#define CAPTION_COLOR 				0xFFCCCCCC
#define CAPTION_BGR_COLOR 		0x702F2F2F
#define CAPTION_X_BGR_START		(MARGIN_X+BUTTON_SIZE)
#define CAPTION_X_BGR_STOP		(BACKGROUND_W - (MARGIN_X * 2) - BUTTON_SIZE)
//#define CAPTION_X_START				(MARGIN_X + (MARGIN_X / 2) + BUTTON_SIZE)

//title box
#define TITLE_X_POZ						MARGIN_X
#define TITLE_Y_POZ						(MARGIN_Y / 2)

//BUTTON
#define BUTTON_X_SIZE					50

#define BUTTON_COLOR_RED			0x70B93C36
#define BUTTON_COLOR_GREEN 		0x7049B936
#define BUTTON_COLOR_YELLOW 0xFFB9B936
//#define BUTTON_COLOR_BLUE 	0xFF3C65B2
//#define BUTTON_COLOR_GREY 	0x70555555
#define BUTTON_COLOR_GREY 		0x70CCCCCC

//buttons pos
#define BUTTONS_Y_START			TITLE_H + MARGIN_Y
#define BUTTON_LCOL_X_POZ		MARGIN_X

//POSITION TEXT
#define POS_TEXT_COLOR			0xFFCCCCCC
#define POS_TEXT_COLOR_REC 	0xFFB9B936
#define DIM_X_BGR_STOP			(BACKGROUND_W - (MARGIN_X * 2))
#define DIM_STEP 						(DIM_X_BGR_STOP / 4)
#define DIM_Y_POZ						(((BUTTON_SIZE + ONE_STEP) * NUM_BUTTONS_ROW) + BUTTONS_Y_START + MARGIN_Y)

/* global */
#define STEP_XY							4
#define STEP_WH							(STEP_XY * 2)
#define CONF_FILE						"/var/plug-in/aspector.conf"
#define PRIV_ASPECT_FILE		"/var/private_aspect_info"
#define PRIV_ASPECT_TMP			"/var/private_aspect_tmp"
#define MAX_BANK						10

//
#define MIN_X								(-SCREEN_W+STEP_XY)
#define MAX_X								(SCREEN_W-STEP_XY)
#define MIN_Y								(-SCREEN_H+STEP_XY)
#define MAX_Y								(SCREEN_H+STEP_XY)
#define MIN_W								160
#define MAX_W								((2*SCREEN_W)-STEP_WH)
#define MIN_H								96
#define MAX_H								((2*SCREEN_H)-STEP_WH)
//

//mini window
#define MINIW_LINES					7
#define PROP_TO_WIN					ONE_STEP
#define PICON_W							100
#define PICON_H							(PICON_W * 0.6)
#define MINIW_W							BACKGROUND_W
#define MINIW_H							(((BUTTON_SIZE + ONE_STEP) * MINIW_LINES) + (MARGIN_Y * 2) + PICON_H + PROP_TO_WIN)
#define MINIW_LEFT					(BACKGROUND_LEFT + BACKGROUND_W + 10)
#define MINIW_TOP						(BACKGROUND_TOP + TITLE_H - (LINE_SIZE / 2))
//#define MINIW_LEFT					((SCREEN_W / 2) - (MINIW_W / 2) - (PICON_W / 2) - MARGIN_X + (LINE_SIZE / 2))
//#define MINIW_TOP						(BACKGROUND_TOP + BACKGROUND_H  + ONE_STEP)
#define BAR_START_X					MARGIN_X
#define BAR_START_X_TEXT		BAR_START_X + MARGIN_X
#define BAR_W								(MINIW_W - BAR_START_X - MARGIN_X)
#define BAR_START_Y_TEXT		(MARGIN_Y + PICON_H + ONE_STEP + PROP_TO_WIN)
#define MINIW_FONT_SIZE			(FONT_SIZE - 2)
#define BAR_STEP_4					(BAR_W / 4)
#define V_CENTER_FONT				((BUTTON_SIZE - MINIW_FONT_SIZE) / 2)
//

//EXTERNAL
typedef struct _t_lnb 
{
	short			low_freq;
	short			high_freq;
	int			lnb_type;
	int		sat_position;
	int		diseqc_10;
	int		diseqc_11;
	int			khz22;
	int		toneburst;
	int		diseqc_repeat;
	int	sequence_repeat;
	int		swap_command;
	int		polarization;
	uint16_t		motor;
	int		lnb_power;	/* lnb power of each tuner */
} lnbinfo_t;

typedef struct t_satinfo
{
	uint16_t	id;		/* 위성 ID */
	int		angle;		/* 위성 위치 */
	char		*name;		/* 위성 이름 */
	int	tuner_id;	/* 현재 튜너 */
	uint16_t	default_tp;	/* current TP frequency ID */
	int		posi_index;	/* satellite position index for diseqc1.2 */
	//uint8_t		checked;	/* flag for check mark in lnb setting */
	lnbinfo_t	lnb[2];		/* lnb[0] : tuner1, lnb[1] : tuner2 => twin tuner LNB Setting structure */
}satinfo_t;

typedef enum
{
	vkeyboard_mode_sbox,
	vkeyboard_mode_plugin,
} vkeyboard_mode_e;


struct _si_rawsection
{
	int size;			/**< size of section */
	unsigned char* section;		/**< section data pointer */
};
typedef struct _si_rawsection si_rawsection_t;
//EXTERNAL

plugin_info( author,    "Reaper7" );
plugin_info( rev,	PLUG_VERSION );
plugin_info( desc,	PLUG_NAME );
plugin_info( ldesc,     "Aspect changer" );

window_t *window = NULL;
window_t *miniwindow = NULL;
window_t *piconname = NULL;
gui_image_t piconimg=NULL;
char picon_file[256];
sqlite3 *db;

#ifdef TEST_BMT
#define bmt_db							"bmt.dat"
#endif

int deb=2;

int dx,dy,dw,dh;
char pname[50];
int save_flag=0;
int exit_after=1;
int list_menu=0;
int aspect_43=0;
int found_in_priv=0;
int last_button=-1;
int miniw_hide_show=0;
int mode_info=0;
int opened=0;
int scrambled=0;
int issync=0;
int isinfo=0;
int max_ch=10;

typedef struct t_myinfo
{
	//chinfo
	uint16_t ch_id;						//ch id
	int ch_seq;								//ch number by seq
	int ch_onid;							//org network id
	int ch_tsid;
	int ch_sid;								//service id
	int ch_servtype;					//service type
	char scr[15];							//scramble text
	char ch_resolution[50];		//ch_resolution
	char ch_name[50];					//ch name
	//sat info
	int sat_angle;						//sat angle
	char sat_pos[10];					//sat position text
	char sat_name[50];				//sat name
	//tunerinfo
	int tuner_num;						//tuner number
	char sr[15];							//srate
	char tpfreq[15];					//tp frequency
	//streaminfo
	int stream_width;					//img width
	int stream_height;				//img height
	int stream_ar;						//aspect ratio
	char stream_fps[10];			//frame per sec
	char stream_aspect[10];		//aspect text
	//pidinfo
	int vidpid;								//video pid
	int audpid;								//audio pid
}myinfo_t;

static myinfo_t *myinfo;

typedef struct t_my_config
{
	char preset_name[50];
  int cx;
	int cy;
	int cw;
	int ch;
	int in_priv;
}my_config_t;

static my_config_t *konfig;

char *bn1[]={"Pg-", "Vol-", "<<", "<", "v", "FILE", "BOOK", "REC", "1...9", "OK", "MENU"};
char *bn2[]={"Pg+", "Vol+", ">>", ">", "^", "\0", "\0", "\0", "\0", "\0", "\0"};
char *bn3[]={"Dec         WIDTH         Inc", "Dec        HEIGHT        Inc", "Dec        RESIZE        Inc", "Left        MOVE      Right", "Down      MOVE           Up", "4:3 Resize ASPECT", "EXIT After Change", "SAVE Mode", "PRESET Number", "RESTORE Default Size", "PRESET List"};

#endif /* ASPECTOR_H_ */

