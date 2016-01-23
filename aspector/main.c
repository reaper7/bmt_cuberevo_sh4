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
//include cuberevo sdk
#include "plugin/plugin.h"
#include "gui/gui.h"
#include "ui/wm.h"
#include "ui/callback.h"
#include "event/event.h"
#include "font/font.h"
#include "background.h"
#include "directory.h"
#include "volume.h"
#include "ch/ch.h"
#include "db/db.h"
#include "db/sat.h"
#include "st_drv_inf/video.h"
#include "libucsi/mpeg/types.h"
//include cuberevo svn
#include "front_display/front_display.h"
#include "config_manager/config_manager.h"
//include main
#include "include/aspector.h"
#include "include/tp.h"


extern char *vkeyboard_with_maxlen( const char *title, const char *dfl_input, const char *style, const char *help_style ,int max_length, vkeyboard_mode_e mode);
extern int vid_main_move(int x, int y, int w, int h);
//extern int vid_get_screen_size(int dev, int *w, int *h);
extern char *db_opt_aspect_ratio;
extern int db_sat_select_info_by_id( sqlite3 *db, uint16_t id, satinfo_t *satinfo );
extern void db_sat_name_free( satinfo_t *satinfo );
extern channelinfo_t *ch_watching_info( ch_mode_e decoder );

extern si_rawsection_t *ch_watching_raw_pmt( ch_mode_e decoder );

extern tuner_id_e ch_watching_tuner_id( ch_mode_e decoder );
extern uint16_t ch_watching_video_pid( ch_mode_e decoder );
extern uint16_t ch_watching_audio_pid( ch_mode_e decoder );
extern int db_channel_get_id_by_seq(sqlite3 *db,int seq,int service_type);
int is_vid_decoding_now( void );
extern int gc_set_alpha( gc_t gc, unsigned int alpha );

#ifdef PLATFORM_BMT
extern int is_sync_event_now( void );
#endif

#ifdef TEST_BMT
extern int bmt_db_settings_set( sqlite3 *dbbmt, const char *name, char *value );
extern int bmt_db_settings_get( sqlite3 *dbbmt, const char *name, char **value );
#endif

//OBSAUGA WIELU JEZYKOW
//extern char *multilang_gettext( const char *str );
//#define _(str)        multilang_gettext(str)
//OBSAUGA WIELU JEZYKOW

//------------------------------------------------------------------------------------------------------------------
void make_screen(void)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  char buf[40];
  int text_len;
  struct _font font_head1 = { .size = FONT_SIZE_TITLE };
  rect_t rect = { BACKGROUND_LEFT, BACKGROUND_TOP, BACKGROUND_W, BACKGROUND_H };

  /* create FB_WINDOW */
  window = wm_create(
    &rect,          /* coordinate */
    PLUG_NAME,      /* name of window */
    NULL,           /* redraw callback */
    NULL,           /* parent window */
    COLOR_TRANSPARENT /* color key for TRANSPARENT */
  );

  //background
  gc_set_fc(window->gc,BACKGROUND_COLOR); /* foreground color */
  gc_set_bc(window->gc,BACKGROUND_COLOR); /* foreground color */
  gui_fillrect(&window->gui,window->gc,0,0,BACKGROUND_W, BACKGROUND_H);

  //border lines
  gc_set_fc(window->gc,RECT_GRAY);
  gt_hfillrect( &window->gui, window->gc, 0, 0, BACKGROUND_W, LINE_SIZE );                                                                                                                                                                                                                                        //up first
  gt_hfillrect( &window->gui, window->gc, 0, (TITLE_H-(LINE_SIZE/2)), BACKGROUND_W, LINE_SIZE );                                                                                                                                          //up second
  gt_hfillrect( &window->gui, window->gc, 0, (BACKGROUND_H-MARGIN_Y-(BUTTON_SIZE+ONE_STEP)-(LINE_SIZE/2)), BACKGROUND_W, LINE_SIZE );     //down first
  gt_hfillrect( &window->gui, window->gc, 0, (BACKGROUND_H-LINE_SIZE), BACKGROUND_W, LINE_SIZE );                                                                                                                                         //down last
  gt_vfillrect( &window->gui, window->gc, 0, 0, LINE_SIZE, BACKGROUND_H );                                                                                                                                                                                                                                        //left
  gt_vfillrect( &window->gui, window->gc, (BACKGROUND_W-LINE_SIZE), 0, LINE_SIZE, BACKGROUND_H );                                                                                                                                         //right

  //title
  gc_set_fc(window->gc,CAPTION_COLOR);
  sprintf(buf, "%s",PLUG_NAME);
  text_len=font_width_str(&font_head1, buf, strlen(buf));
  font_draw_str(&font_head1, buf, strlen(buf), &window->gui, window->gc, ((BACKGROUND_W/2)-(text_len/2)), TITLE_Y_POZ);  

  //wm_show(window);
}
//------------------------------------------------------------------------------------------------------------------
void picon_name()
{
  if (piconname==NULL)
  {
    if (miniw_hide_show!=0)
    {
      struct _font font_head1 = { .size = FONT_SIZE + 8 };
      char picon_file[256];
      int text_len=0;
      sprintf(picon_file, "Picon name for the channel: 1_0_%X_%X_%X_%X_%X0000_0_0_0.png",myinfo->ch_servtype,myinfo->ch_sid,myinfo->ch_tsid,myinfo->ch_onid,myinfo->sat_angle);
      if (deb>=3) printf("%s PICON NAME[%s]\n", PLUG_NAME, picon_file);

      rect_t piconrect = { 0, 30, 1280, 30 };

      piconname = wm_create(
        &piconrect,             /* coordinate */
        "piconname",    /* name of window */
        NULL,           /* redraw callback */
        NULL,           /* parent window */
        COLOR_TRANSPARENT /* color key for TRANSPARENT */
      );

      //background
      gc_set_fc(piconname->gc,BACKGROUND_COLOR); /* foreground color */
      gc_set_bc(piconname->gc,BACKGROUND_COLOR); /* foreground color */
      gui_fillrect(&piconname->gui,piconname->gc,0,0,1280,30);

      //text
      gc_set_fc(piconname->gc,CAPTION_COLOR);
      text_len=font_width_str(&font_head1, picon_file, strlen(picon_file));
      if (text_len>0) font_draw_str(&font_head1, picon_file, strlen(picon_file), &piconname->gui, piconname->gc, (1280/2)-(text_len/2) ,0);

      wm_redraw(piconname);
      wm_show(piconname);
    }
  }
  else
  {
    wm_destroy(piconname);
    piconname = NULL;
  }
}
//------------------------------------------------------------------------------------------------------------------
int mini_win(int de_cr)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  if (de_cr>=1)
  {
    int text_len;
    char buf1[40];
    rect_t minirect = { MINIW_LEFT, MINIW_TOP, MINIW_W, MINIW_H };

    struct _font font_head0 = { .size = MINIW_FONT_SIZE };
    struct _font font_head1 = { .size = FONT_SIZE + 2 };
    struct _font font_head2 = { .size = FONT_SIZE + 34 };

    if (miniwindow==NULL)
    {
      miniwindow = wm_create(
        &minirect,              /* coordinate */
        "mini win",     /* name of window */
        NULL,           /* redraw callback */
        NULL,           /* parent window */
        COLOR_TRANSPARENT /* color key for TRANSPARENT */
      );
    }

    //background
    gc_set_fc(miniwindow->gc,BACKGROUND_COLOR); /* foreground color */
    gc_set_bc(miniwindow->gc,BACKGROUND_COLOR); /* foreground color */
    gui_fillrect(&miniwindow->gui,miniwindow->gc,0,0,MINIW_W, MINIW_H);

    //border lines
    gc_set_fc(miniwindow->gc,RECT_GRAY);
    gt_hfillrect( &miniwindow->gui, miniwindow->gc, 0, 0, MINIW_W, LINE_SIZE );                                                                                                                                                                                                                                                             //up
    gt_hfillrect( &miniwindow->gui, miniwindow->gc, 0, (MINIW_H-LINE_SIZE), MINIW_W, LINE_SIZE );                                                                                                                                                                                   //down
    gt_vfillrect( &miniwindow->gui, miniwindow->gc, 0, 0, LINE_SIZE, MINIW_H );                                                                                                                                                                                                                                                             //left
    gt_vfillrect( &miniwindow->gui, miniwindow->gc, (MINIW_W-LINE_SIZE), 0, LINE_SIZE, MINIW_H );                                                                                                                                                                                   //right

    //picon file
    struct stat buf;
    sprintf(picon_file, "/usr/share/sbox/image/picons/1_0_%X_%X_%X_%X_%X0000_0_0_0.png",myinfo->ch_servtype,myinfo->ch_sid,myinfo->ch_tsid,myinfo->ch_onid,myinfo->sat_angle);
    if (stat(picon_file, &buf)!=0)
      sprintf(picon_file, "/usr/share/sbox/image/dark/picon0.png");
    if (stat(picon_file, &buf)==0)
      piconimg = gui_open_image_by_path( picon_file, gui_image_type_png );
    else
      piconimg = NULL;

    //bar color
    gc_set_fc(miniwindow->gc,CAPTION_BGR_COLOR);                                                                                                                                                                                                                                                                                                                                                                                    //caption bgr color
    //draw bar rows background
    int pos;
    for( pos = 0; pos < MINIW_LINES; pos++ ) {
      gt_fillrect( &miniwindow->gui, miniwindow->gc, BAR_START_X, BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * pos), BAR_W, BUTTON_SIZE);                   //long caption bgr
    }

    //chnum text
    sprintf(buf1, "%d",myinfo->ch_seq);
    text_len=font_width_str(&font_head2, buf1, strlen(buf1));

    //draw background, image, ch number
    if (piconimg != NULL)
    {
      gt_fillrect( &miniwindow->gui, miniwindow->gc, MARGIN_X, MARGIN_Y, ((BAR_W / 2) - (ONE_STEP / 2)), (PICON_H + PROP_TO_WIN));                                                    //number
      gt_fillrect( &miniwindow->gui, miniwindow->gc, MARGIN_X + ((BAR_W / 2) + (ONE_STEP / 2)), MARGIN_Y, ((BAR_W / 2) - (ONE_STEP / 2)), (PICON_H + PROP_TO_WIN));           //picon
      gc_set_alpha(miniwindow->gc,0x80);
      gui_draw_image(&miniwindow->gui,piconimg,miniwindow->gc, MARGIN_X + ((BAR_W / 2) + (ONE_STEP / 2)) + ((((BAR_W / 2) - (ONE_STEP / 2)) / 2) - (PICON_W / 2)), (MARGIN_Y + (PROP_TO_WIN / 2)), PICON_W, PICON_H, 0);
      gui_close_image(piconimg);
      piconimg = NULL;
      gc_set_alpha(miniwindow->gc,0xFF);
      gc_set_fc(miniwindow->gc,CAPTION_COLOR);                                                                                                                                                                                                                                                                                                                                                                                                                        //caption color
      if (text_len>0) font_draw_str(&font_head2, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, MARGIN_X +((((BAR_W / 2) - (ONE_STEP / 2))/2)-(text_len/2)) ,(MARGIN_Y + ((PICON_H + PROP_TO_WIN) / 2) - (font_height(&font_head2) / 2)));                                                                                                                                                                                                                                                                                                                                                                             //ch number on left
    }
    else
    {
      gt_fillrect( &miniwindow->gui, miniwindow->gc, MARGIN_X, MARGIN_Y, BAR_W , (PICON_H + PROP_TO_WIN));                                                                                                                                                    //number wide - no picon file
      gc_set_fc(miniwindow->gc,CAPTION_COLOR);                                                                                                                                                                                                                                                                                                                                                                                                        //caption color
      if (text_len>0) font_draw_str(&font_head2, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, MARGIN_X + ((BAR_W / 2) - (text_len/2)) ,(MARGIN_Y + ((PICON_H + PROP_TO_WIN) / 2) - (font_height(&font_head2) / 2)));                                                                                                                                                                                                                                                                                                                                                                         //ch number on center
    }

    int row_ind=0;
    //line 1 - channel name        
    sprintf(buf1, "%s",myinfo->ch_name);
    text_len=font_width_str(&font_head1, buf1, strlen(buf1));
    if (text_len>0) font_draw_str(&font_head1, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, BAR_START_X +((BAR_W/2)-(text_len/2)) ,(BAR_START_Y_TEXT - 2 + ((BUTTON_SIZE + ONE_STEP) * row_ind)));

    //line 2 - sat info 1
    row_ind++;
    sprintf(buf1, "%s",myinfo->sat_name);  
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 0)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));
    sprintf(buf1, "%s",myinfo->sat_pos);
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 2)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));
    sprintf(buf1, "TUNER %d",myinfo->tuner_num);
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 3)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));

    //line 3 - sat info 2
    row_ind++;
    sprintf(buf1, "SR %s",myinfo->sr);
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 0)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));
    sprintf(buf1, "FREQ %s",myinfo->tpfreq);       
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 2)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));

    //line 4 - ch info 1
    row_ind++;
    sprintf(buf1, "NID %04X [%d]",myinfo->ch_onid,myinfo->ch_onid);                                                                                                                                                                                                                                                                                                                
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 0)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));
    sprintf(buf1, "TSID %04X [%d]",myinfo->ch_tsid,myinfo->ch_tsid);
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 2)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));

    //line 5 - ch info 2
    row_ind++;
    sprintf(buf1, "SID %04X [%d]",myinfo->ch_sid,myinfo->ch_sid);  
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 0)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));
    if (de_cr==3)
      goto redraw1;
    sprintf(buf1, "TYPE %s",myinfo->scr);
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 2)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));

    //line 6 - pids info
    row_ind++;
    sprintf(buf1, "VPID %04X [%d]",myinfo->vidpid,myinfo->vidpid);
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 0)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));
    sprintf(buf1, "APID %04X [%d]",myinfo->audpid,myinfo->audpid); 
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 2)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));

    //line 7 - stream info
    row_ind++;
    sprintf(buf1, "%s",myinfo->ch_resolution);                                                                                                                                                                                                                                                                                                                     
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 0)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));
    sprintf(buf1, "%s",myinfo->stream_aspect);     
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 2)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));
    sprintf(buf1, "%s",myinfo->stream_fps);
    font_draw_str(&font_head0, buf1, strlen(buf1), &miniwindow->gui, miniwindow->gc, (BAR_START_X_TEXT + (BAR_STEP_4 * 3)), (V_CENTER_FONT + BAR_START_Y_TEXT + ((BUTTON_SIZE + ONE_STEP) * row_ind)));
               
redraw1:

    wm_redraw(miniwindow);
    wm_show(miniwindow);
    return de_cr;                                                                                                                                   //window create and/or show;
  } 
  else if (de_cr==0) 
  {
    if (miniwindow!=NULL)
    {
      if (piconimg != NULL)
      {
        gui_close_image(piconimg);
        piconimg = NULL;
      }      
      wm_destroy(miniwindow);
      miniwindow = NULL;
      return 0;                                                                                                                                                       //window destroy
    }
    else
      return de_cr;
  } 
  else
    return -1;
}
//------------------------------------------------------------------------------------------------------------------
void row_repaint(int button_number, int update_win, int mode)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  struct _font font_head0 = { .size = FONT_SIZE };
  int col;
  int text_len;
  char buf1[40];
  col=(((BUTTON_SIZE + ONE_STEP) * (button_number - 1)) + BUTTONS_Y_START);

  if (mode==1)
  {
    char buf3[40];
               
    if (((aspect_43==1) && (button_number==6)) || ((exit_after==1) && (button_number==7)) || ((save_flag==1) && (button_number==8))) 
    {
      gc_set_fc(window->gc,BUTTON_COLOR_RED);
      gt_fillrect( &window->gui, window->gc, BUTTON_LCOL_X_POZ, col, BUTTON_X_SIZE, BUTTON_SIZE);
      gc_set_fc(window->gc,CAPTION_COLOR);
      sprintf(buf1, "%s",bn1[button_number - 1]);
      sprintf(buf3, "%s %s","Disable",bn3[button_number - 1]);
    }
    else if ((save_flag==1) && (button_number==9))
    {
      gc_set_fc(window->gc,BUTTON_COLOR_GREY);
      gt_fillrect( &window->gui, window->gc, BUTTON_LCOL_X_POZ, col, BUTTON_X_SIZE, BUTTON_SIZE);
      gc_set_fc(window->gc,CAPTION_BUTTON_COLOR);
      sprintf(buf1, "%s",bn1[button_number - 1]);
      sprintf(buf3, "%s",bn3[button_number - 1]);
    }
    else if ((save_flag==0) && (button_number==9))
    {
      gc_set_fc(window->gc,CAPTION_BGR_COLOR);                                                                                                                                                                                                                                                                                                                                                                                               
      gt_fillrect( &window->gui, window->gc, BUTTON_LCOL_X_POZ, col , BUTTON_X_SIZE, BUTTON_SIZE);   
      sprintf(buf1, "%s","");
      sprintf(buf3, "%s","");
    }
    else
    {
      gc_set_fc(window->gc,BUTTON_COLOR_GREY);
      gt_fillrect( &window->gui, window->gc, BUTTON_LCOL_X_POZ, col, BUTTON_X_SIZE, BUTTON_SIZE);
      gc_set_fc(window->gc,CAPTION_BUTTON_COLOR);
      sprintf(buf1, "%s",bn1[button_number - 1]);
      sprintf(buf3, "%s %s","Enable",bn3[button_number - 1]);
    }
    text_len=font_width_str(&font_head0, buf1, strlen(buf1));
    if (text_len>0)
      font_draw_str(&font_head0, buf1, strlen(buf1), &window->gui, window->gc, (BUTTON_LCOL_X_POZ + (BUTTON_X_SIZE/2) - (text_len/2)), col);

    gc_set_fc(window->gc,CAPTION_BGR_COLOR);                                                                                                                                                                                                                                                                                                                                                                                       
    gt_fillrect( &window->gui, window->gc, (BUTTON_X_SIZE + MARGIN_X), col, (BACKGROUND_W - (2 * MARGIN_X) - BUTTON_X_SIZE), BUTTON_SIZE);                 
    gc_set_fc(window->gc,CAPTION_COLOR);
    text_len=font_width_str(&font_head0, buf3, strlen(buf3));
    if (text_len>0)
      font_draw_str(&font_head0, buf3, strlen(buf3), &window->gui, window->gc, (MARGIN_X * 2) + BUTTON_X_SIZE,col);
  }
  else if (mode==0)
  {
    if ((konfig[button_number].in_priv==1) && (konfig[button_number].cx==dx) && (konfig[button_number].cy==dy) && (konfig[button_number].cw==dw) && (konfig[button_number].ch==dh))
      gc_set_fc(window->gc,BUTTON_COLOR_YELLOW);                                                                                                                                                                                                                                                                                                                                                                                             
    else if ((konfig[button_number].in_priv!=1) && (last_button==button_number) && (konfig[button_number].cx==dx) && (konfig[button_number].cy==dy) && (konfig[button_number].cw==dw) && (konfig[button_number].ch==dh))
      gc_set_fc(window->gc,BUTTON_COLOR_GREEN);
    else
      gc_set_fc(window->gc,BUTTON_COLOR_GREY);       

    gt_fillrect( &window->gui, window->gc, BUTTON_LCOL_X_POZ, col , BUTTON_X_SIZE, BUTTON_SIZE);
    gc_set_fc(window->gc,CAPTION_BUTTON_COLOR);
    sprintf(buf1, "%d",button_number);
    text_len=font_width_str(&font_head0, buf1, strlen(buf1));
    if (text_len>0) font_draw_str(&font_head0, buf1, strlen(buf1), &window->gui, window->gc, (BUTTON_LCOL_X_POZ + (BUTTON_X_SIZE/2) - (text_len/2)), col);
  }

  if (update_win==1)
  {
    rect_t rect;
    rect.x=BUTTON_LCOL_X_POZ;
    rect.y=col;
    rect.h=BUTTON_SIZE;
    
    if (mode==0)
      rect.w=BUTTON_X_SIZE;
    else
      rect.w=(BACKGROUND_W - (2 * MARGIN_X));

    wm_update_rect(window, &rect);
  }
}
//------------------------------------------------------------------------------------------------------------------
void make_list_menu(int mode)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  int update_show=0;
  if (window==NULL)
  {
    make_screen();
    update_show=1;
  }

  char buf1[40], buf2[40], buf3[40];
  int text_len;
  int row,col;
  struct _font font_head0 = { .size = FONT_SIZE };
  struct _font font_head1 = { .size = FONT_SIZE_TITLE };
       
  if (mode==0)
  {
    for( row = 0; row < NUM_BUTTONS_ROW; row++ )
    {
      switch(row)
      {
        case 0 ... 8:
          sprintf(buf1, "%d",row+1);
          sprintf(buf2, "%s","");
          sprintf(buf3, "%s",konfig[row+1].preset_name);
          break;
        case 9:
          sprintf(buf1, "%s",bn1[row]);
          sprintf(buf2, "%s","");
          sprintf(buf3, "%s",bn3[row]);
          break;
        case 10:
          sprintf(buf1, "%s",bn1[row]);
          sprintf(buf2, "%s","");
          sprintf(buf3, "%s","SETTINGS");
          break;
        default:
          sprintf(buf1, "%s","");
          sprintf(buf2, "%s","");
          sprintf(buf3, "%s","");
          break;
      }      

      col=((BUTTON_SIZE * row) + (ONE_STEP * row) + BUTTONS_Y_START);

      if (strcmp(buf1,"")!=0)                
      {
        //buttons
        if ((row>=0) && (row<=8))
        {
          row_repaint(row+1, 0, 0);
        }
        else
        {
          gc_set_fc(window->gc,BUTTON_COLOR_GREY);                                                                                                                                                                                                                                                                                                                                                                                                        //buttons color grey
          gt_fillrect( &window->gui, window->gc, BUTTON_LCOL_X_POZ, col , BUTTON_X_SIZE, BUTTON_SIZE);                                                                                                                                                                                            //left buttons
          //button caption
          gc_set_fc(window->gc,CAPTION_BUTTON_COLOR);
          text_len=font_width_str(&font_head0, buf1, strlen(buf1));
          if (text_len>0) font_draw_str(&font_head0, buf1, strlen(buf1), &window->gui, window->gc, (BUTTON_LCOL_X_POZ + (BUTTON_X_SIZE/2) - (text_len/2)), col);
          text_len=font_width_str(&font_head0, buf2, strlen(buf2));
          if (text_len>0) font_draw_str(&font_head0, buf2, strlen(buf2), &window->gui, window->gc, (BACKGROUND_W - MARGIN_X - (BUTTON_X_SIZE/2) - (text_len/2)), col);
        }
        //bar
        gc_set_fc(window->gc,CAPTION_BGR_COLOR);                                                                                                                                                                                                                                                                                                                                                                                                                //caption bgr color
        gt_fillrect( &window->gui, window->gc, (BUTTON_X_SIZE + MARGIN_X), col, (BACKGROUND_W - (2 * MARGIN_X) - BUTTON_X_SIZE), BUTTON_SIZE);                  //long caption bgr(without right buttun)
        //bar caption
        gc_set_fc(window->gc,CAPTION_COLOR);
        text_len=font_width_str(&font_head0, buf3, strlen(buf3));
        if (text_len>0) font_draw_str(&font_head0, buf3, strlen(buf3), &window->gui, window->gc, (MARGIN_X * 2) + BUTTON_X_SIZE,col);
      }
      else
      {
        gc_set_fc(window->gc,CAPTION_BGR_COLOR);                                                                                                                                                                                                                                                                                                                                                                                               
        gt_fillrect( &window->gui, window->gc, BUTTON_LCOL_X_POZ, col , (BACKGROUND_W - (2 * MARGIN_X)), BUTTON_SIZE); 
      }
    }
  }
  else
  {
    for( row = 0; row < NUM_BUTTONS_ROW; row++ )
    {
      sprintf(buf1, "%s",bn1[row]);
      sprintf(buf2, "%s",bn2[row]);
      sprintf(buf3, "%s",bn3[row]);

      col=((BUTTON_SIZE * row) + (ONE_STEP * row) + BUTTONS_Y_START);

      if (strcmp(buf1,"")!=0)                
      {
        //buttons      
        gc_set_fc(window->gc,BUTTON_COLOR_GREY);                                                                                                                                                                                                                                                                                                                                                                                                                        //buttons color
        gt_fillrect( &window->gui, window->gc, BUTTON_LCOL_X_POZ, col , BUTTON_X_SIZE, BUTTON_SIZE);                                                                                                                                                                                                    //left buttons
        if (strcmp(buf2,"")!=0) gt_fillrect( &window->gui, window->gc, (BACKGROUND_W - MARGIN_X - BUTTON_X_SIZE), col, BUTTON_X_SIZE, BUTTON_SIZE);             //right buttons
        //bar
        gc_set_fc(window->gc,CAPTION_BGR_COLOR);                                                                                                                                                                                                                                                                                                                                                                                                                        //caption bgr color
        if (strcmp(buf2,"")!=0)
          gt_fillrect( &window->gui, window->gc, (BUTTON_X_SIZE + MARGIN_X), col, (BACKGROUND_W - (2 * MARGIN_X) - (2 * BUTTON_X_SIZE)), BUTTON_SIZE);//short caption bgr(with right buttons)    
        else
          gt_fillrect( &window->gui, window->gc, (BUTTON_X_SIZE + MARGIN_X), col, (BACKGROUND_W - (2 * MARGIN_X) - BUTTON_X_SIZE), BUTTON_SIZE);                  //long caption bgr(without right buttun)       
        //button caption
        gc_set_fc(window->gc,CAPTION_BUTTON_COLOR);
        text_len=font_width_str(&font_head0, buf1, strlen(buf1));
        if (text_len>0)
        {
          if (strcmp(buf1,"v")!=0)
            font_draw_str(&font_head0, buf1, strlen(buf1), &window->gui, window->gc, (BUTTON_LCOL_X_POZ + (BUTTON_X_SIZE/2) - (text_len/2)), col);
          else
            font_draw_str(&font_head0, buf1, strlen(buf1), &window->gui, window->gc, (BUTTON_LCOL_X_POZ + (BUTTON_X_SIZE/2) - (text_len/2)), col-2);
        }
        text_len=font_width_str(&font_head0, buf2, strlen(buf2));
        if (text_len>0) 
        {
          if (strcmp(buf2,"^")!=0)
            font_draw_str(&font_head0, buf2, strlen(buf2), &window->gui, window->gc, (BACKGROUND_W - MARGIN_X - (BUTTON_X_SIZE/2) - (text_len/2)), col);
          else
            font_draw_str(&font_head1, buf2, strlen(buf2), &window->gui, window->gc, (BACKGROUND_W - MARGIN_X - (BUTTON_X_SIZE/2) - (text_len/2)), col+2);
        }
        //bar caption
        gc_set_fc(window->gc,CAPTION_COLOR);
        text_len=font_width_str(&font_head0, buf3, strlen(buf3));
        if (text_len>0) font_draw_str(&font_head0, buf3, strlen(buf3), &window->gui, window->gc, (MARGIN_X * 2) + BUTTON_X_SIZE,col);
      }
      else
      {
        gc_set_fc(window->gc,CAPTION_BGR_COLOR);                                                                                                                                                                                                                                                                                                                                                                                               
        gt_fillrect( &window->gui, window->gc, BUTTON_LCOL_X_POZ, col , (BACKGROUND_W - (2 * MARGIN_X)), BUTTON_SIZE);
      }
    }
    row_repaint(6,0,1);
    row_repaint(7,0,1);
    row_repaint(8,0,1);
    row_repaint(9,0,1);
  }

  if (update_show==0)
  {
    rect_t rect = { BUTTON_LCOL_X_POZ, BUTTONS_Y_START, (BACKGROUND_W - (2 * MARGIN_X)), ((BUTTON_SIZE + ONE_STEP) * NUM_BUTTONS_ROW) };
    wm_update_rect(window, &rect);
  }
  else
    wm_show(window);
}
//------------------------------------------------------------------------------------------------------------------
void screen_pos_text(int changes_flag)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  char buf[40];

  gc_set_fc(window->gc,CAPTION_BGR_COLOR);
  gt_fillrect( &window->gui, window->gc, MARGIN_X, DIM_Y_POZ, DIM_X_BGR_STOP, BUTTON_SIZE);

  struct _font font_head0 = { .size = FONT_BOTTOM_TEXT };                                                                                                                                                                                                                                                                                 //wielkosc czcionki
  if (changes_flag==0)   
    gc_set_fc(window->gc,POS_TEXT_COLOR);                                                                                                                                                                                                                                                                                                                                           //kolor czcionki
  else
    gc_set_fc(window->gc,POS_TEXT_COLOR_REC);

  //line 1
  sprintf(buf, "X:%d",dx);                                                                                                                                                                                                                                                                                                                                       
  font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, (MARGIN_X + (DIM_STEP * 0)), DIM_Y_POZ);        
  sprintf(buf, "Y:%d",dy);                                                                                                                                                                                                                                                                                                                                       
  font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, (MARGIN_X + (DIM_STEP * 1)), DIM_Y_POZ);        
  sprintf(buf, "W:%d",dw);                                                                                                                                                                                                                                                                                                                                       
  font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, (MARGIN_X + (DIM_STEP * 2)), DIM_Y_POZ);
  sprintf(buf, "H:%d",dh);                                                                                                                                                                                                                                                                                                                                       
  font_draw_str(&font_head0, buf, strlen(buf), &window->gui, window->gc, (MARGIN_X + (DIM_STEP * 3)), DIM_Y_POZ);

  rect_t rect = { MARGIN_X, DIM_Y_POZ, DIM_X_BGR_STOP, BUTTON_SIZE };
  wm_update_rect(window, &rect);
}
//------------------------------------------------------------------------------------------------------------------
void vid_change(int command)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  switch(command)
  {
    case -1:
      dx=0;
      dy=0;
      dw=1280;
      dh=720;
      last_button=-1;
      if ((list_menu==0) && (window!=NULL))
        make_list_menu(0);
      break;
    case 0:
      dx=konfig[0].cx;
      dy=konfig[0].cy;
      dw=konfig[0].cw;
      dh=konfig[0].ch;
      last_button=-1;
      if ((list_menu==0) && (window!=NULL))
        make_list_menu(0);
      break;
    case 1:         //left
      if (dx>=MIN_X)
        dx-=STEP_XY;
      break;
    case 2:         //right
      if (dx<=MAX_X)
        dx+=STEP_XY;
      break;
    case 3:         //up
      if (dy>=MIN_Y)
        dy-=STEP_XY;
      break;
    case 4:         //down
      if (dy<=MAX_Y)
        dy+=STEP_XY;
      break;
    case 5:         //W_down
      if ((dw>=MIN_W+STEP_WH) && (dx<=MAX_X)) 
      {
        dw-=STEP_WH;
        dx+=STEP_XY;
      }
      break;
    case 6:         //W_up
      if ((dw<=MAX_W) && (dx>=MIN_X))
      {
        dw+=STEP_WH;
        dx-=STEP_XY;
      }
      break;
    case 7:         //H_down
      if ((dh>=MIN_H+STEP_WH) && (dy<=MAX_Y))
      {
        dh-=STEP_WH;
        dy+=STEP_XY;
      }
      break;
    case 8:         //H_up
      if ((dh<=MAX_H) && (dy>=MIN_Y))
      {
        dh+=STEP_WH;
        dy-=STEP_XY;
      }
      break;
    case 10 ... 19:
      dx=konfig[command-MAX_BANK].cx;
      dy=konfig[command-MAX_BANK].cy;
      dw=konfig[command-MAX_BANK].cw;
      dh=konfig[command-MAX_BANK].ch;
      last_button=command-MAX_BANK;
      make_list_menu(0);
      break;
    case 20:
      //NOP for command line preset
      break;
    case 21:        //RESIZE PROPORTIONAL down
      if ((dw>=MIN_W+STEP_WH) && (dx<=MAX_X) && (dh>=MIN_H+STEP_WH) && (dy<=MAX_Y))
      {
        dw-=STEP_WH;
        dx+=STEP_XY;
        if (aspect_43==1)
        {
          dh-=(int)STEP_WH*3/4;
          dy+=(int)STEP_XY*3/4;
        }
        else
        {
          dh-=(int)STEP_WH*9/16;
          dy+=(int)STEP_XY*9/16;
        }
        //dh-=STEP_WH;
        //dy+=STEP_XY;
      }
      break;
    case 22:        //RESIZE PROPORTIONAL up
      if ((dw<=MAX_W) && (dx>=MIN_X) && (dh<=MAX_H) && (dy>=MIN_Y))
      {
        dw+=STEP_WH;
        dx-=STEP_XY;
        if (aspect_43==1)
        {
          dh+=(int)STEP_WH*3/4;
          dy-=(int)STEP_XY*3/4;
        }
        else
        {
          dh+=(int)STEP_WH*9/16;
          dy-=(int)STEP_XY*9/16;
        }
        //dh+=STEP_WH;
        //dy-=STEP_XY;
      }
      break;
  }
  if ((command >= 10) && (command <= 19))
    screen_pos_text(0);
  else if (((command > 0) && (command < 10)) || (command >= 21))
    screen_pos_text(1);
  if (command==-1)
    vid_main_move(0, 0, 1280, 720);
  else
    vid_main_move(dx, dy, dw, dh);
}
//------------------------------------------------------------------------------------------------------------------
/*
static int check_ch( void *arg, void *calldata )
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  int wch=0;
  while (wch<=30)
  {
    opened=is_vid_decoding_now();
    if (opened==1)
    {
      if (deb>=2) printf("%s DECODING OK [WAIT %dms]\n", PLUG_NAME,(wch*100));
      break;
    }
    wch++;
    if (deb>=3) printf("%s NO DECODE [WAIT %dms]\n", PLUG_NAME,(wch*100));
    usleep(100000);
  }

  return opened;
}
*/
//------------------------------------------------------------------------------------------------------------------
void scr_ch(int mode)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);

  if (isinfo==1)
    opened=is_vid_decoding_now();
  else
    opened=0;

  if (deb>=3) printf("%s SCRAMBLED TYPE[%d] OPENED[%d]\n", PLUG_NAME, scrambled, opened);
  if (scrambled==1)
    scrambled+=opened;
  switch(scrambled)
  {
    case 0:
      sprintf(myinfo->scr, "%s","UNSCRAMBL.");
      break;
    case 1:
      sprintf(myinfo->scr, "%s","SCRAMBLED ");
      break;
    case 2:
      sprintf(myinfo->scr, "%s","DESCRAMBL.");
      break;
    default:
      sprintf(myinfo->scr, "%s","UNKNOWN   ");
      break;
  }
       
  if (mode!=0)
    miniw_hide_show=mini_win(2);
}
//------------------------------------------------------------------------------------------------------------------
void check_ch(void)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  int wch=0;
  while (wch<=30)
  {
    opened=is_vid_decoding_now();
    if (opened==1)
    {
      if (deb>=2) printf("%s DECODING OK [WAIT %dms]\n", PLUG_NAME,(wch*100));
        break;
    }
    wch++;
    if (deb>=3) printf("%s NO DECODE [WAIT %dms]\n", PLUG_NAME,(wch*100));
      usleep(100000);
  }
}
//------------------------------------------------------------------------------------------------------------------
void sync_ch(void)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  issync=0;
#ifdef PLATFORM_BMT
  int wch=0;
  while (wch<=30)
  {
    issync=is_sync_event_now();
    if (issync==1)
    {
      if (deb>=2) printf("%s SYNC OK [WAIT %dms]\n", PLUG_NAME,(wch*100));
        break;
    }
    wch++;
    if (deb>=3) printf("%s NO SYNC [WAIT %dms]\n", PLUG_NAME,(wch*100));
      usleep(100000);
  }
#else
  if((myinfo->stream_height>720 )&&(myinfo->stream_height<=1088))
    usleep(1400000);
  else
    usleep(1200000);
#endif
}
//------------------------------------------------------------------------------------------------------------------
int read_ch(void)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);

  //destroy piconname banner if exist
  if (piconname != NULL)
    picon_name();

  channelinfo_t *chinfo = NULL;
  //channelinfo_t *chinfo = malloc(sizeof(channelinfo_t));
  chinfo = ch_watching_info( ch_mode_live );

  myinfo->ch_seq=chinfo->seq;
  myinfo->ch_onid=chinfo->org_network_id;
  myinfo->ch_tsid=chinfo->ts_id;
  myinfo->ch_sid=chinfo->service_id;
  myinfo->ch_servtype=chinfo->service_type;
  scrambled=chinfo->scramble;
  sprintf(myinfo->ch_name, "%s",chinfo->name);

  db = db_open(type_db);
  if( db )
  {
    //sat info
    satinfo_t satinfo;
    if (db_sat_select_info_by_id(db, chinfo->sat_id, &satinfo)==0)
    {
      if (satinfo.angle!=0)
      {
        myinfo->sat_angle=satinfo.angle;
        sprintf(myinfo->sat_pos, "%.1f %s",(double)satinfo.angle/10,(satinfo.angle<0) ? "W" : "E");
      }
      else
      {
        myinfo->sat_angle=0xffff;
        sprintf(myinfo->sat_pos, "%s","C/T");  
      }
      sprintf(myinfo->sat_name, "%s",(char*)strtok(satinfo.name," "));
      db_sat_name_free( &satinfo );
    }

    //tp info
    tpinfo_t tpinfo;
    if (db_tp_select_info_by_id(db, chinfo->tp_id, &tpinfo )==0)
    {
      char tppol[3];

      if( tpinfo.polarization == polarization_vertical )
        sprintf( tppol, "V" );
      else if( tpinfo.polarization == polarization_horizontal )
        sprintf( tppol, "H" );
      else if( tpinfo.polarization ==  polarization_left )
        sprintf( tppol, "L" );
      else if( tpinfo.polarization == polarization_right )
        sprintf( tppol, "R" );
      else
        sprintf( tppol, "A" );

      if( chinfo->sat_id < 300 )
      {
        sprintf(myinfo->tpfreq, "%dMHz/%s", tpinfo.frequency, tppol );
        sprintf(myinfo->sr, "%dKsps", tpinfo.symbol_rate );
      }
      else if( chinfo->sat_id >= 300 && chinfo->sat_id < 400 )
      {
        sprintf(myinfo->tpfreq, "%dKHz/%s", tpinfo.frequency, tppol );
        sprintf(myinfo->sr, "%dMHz", tpinfo.symbol_rate );
      }
      else
      {
        sprintf(myinfo->tpfreq, "%dKHz/%s", tpinfo.frequency, tppol );
        sprintf(myinfo->sr, "%dKsps", tpinfo.symbol_rate );
      }

    }
    else
    {
      strcpy(myinfo->tpfreq, "Unk.MHz/Unk.Pol");
      strcpy(myinfo->sr, "Unk.Ksps");
    }

    db_close(db);
    
  }
  //free(chinfo);

  //tuner info
  myinfo->tuner_num = ch_watching_tuner_id( ch_mode_live );

  //show some info on miniw
  if (mode_info==1)
    miniw_hide_show=mini_win(3);

  //pid info
  si_rawsection_t *pmtraw=NULL;
  pmtraw=ch_watching_raw_pmt( ch_mode_live );
  if (pmtraw->section != NULL)
  {
    myinfo->vidpid=ch_watching_video_pid( ch_mode_live );
    myinfo->audpid=ch_watching_audio_pid( ch_mode_live );
  }
  else
  {
    myinfo->vidpid=0;
    myinfo->audpid=0;
  }
       
  if (myinfo->vidpid != 0)
  {
    //stream_info
    vid_stream_info_t *stream_info = malloc(sizeof(vid_stream_info_t));

    int probe=0;
    isinfo=0;
    while (probe <= 35)
    {
      ch_watching_video_stream_info(ch_mode_live, stream_info);
      if ((stream_info->height>262) && (stream_info->height<=1088))
      {
        if (deb>=2) printf("%s STREAM INFO OK [WAIT %dms]\n", PLUG_NAME,(probe*100));
        isinfo=1;
        break;
      }
        probe++;
        if (deb>=3) printf("%s NO STREAM INFO [WAIT %dms]\n", PLUG_NAME, (probe*100));
        usleep(100000);
    }

    scr_ch(0);

    if ((stream_info->fps >= 1) && (stream_info->fps <= 100000))
      sprintf(myinfo->stream_fps, "FPS %d",(int)stream_info->fps / 1000);
    else
      sprintf(myinfo->stream_fps, "%s","");

    myinfo->stream_ar=stream_info->aspect_ratio;

    myinfo->stream_width=stream_info->width;
    myinfo->stream_height=stream_info->height;

    //detect SD/HD by img dimension
    if((stream_info->height<=576)&&(stream_info->height>0))
      sprintf(myinfo->ch_resolution, "%dx%d   [ %s]",stream_info->width,stream_info->height,"576i");
    else if( (576<stream_info->height)&&(stream_info->height<=720) )
      sprintf(myinfo->ch_resolution, "%dx%d   [ %s]",stream_info->width,stream_info->height,"720p");
    else if(( stream_info->height>720 )&&( stream_info->height<2000 ))
      sprintf(myinfo->ch_resolution, "%dx%d [%s]",stream_info->width,stream_info->height,"1080i");
    else if (scrambled==1)
    {
      sprintf(myinfo->ch_resolution, "%s","CHANNEL SCRAMBLED");
      myinfo->stream_ar=0;
      myinfo->stream_width=0;
      myinfo->stream_height=0;
    }
    else
    {
      sprintf(myinfo->ch_resolution, "%s","RESOLUTION UNKNOWN");
      myinfo->stream_ar=0;
      myinfo->stream_width=0;
      myinfo->stream_height=0;
    }

    free(stream_info);

  }
  else
  {
    sprintf(myinfo->ch_resolution, "%s","RADIO OR OTHER CHANNEL");
    sprintf(myinfo->stream_fps, "%s","");
    myinfo->stream_ar=0;
    myinfo->stream_width=0;
    myinfo->stream_height=0;
    scr_ch(0);
  }

  //if current channel aspect ratio is 4:3 then set default resize proportion to 4:3
  switch(myinfo->stream_ar)
  {
    case 0:
      aspect_43=0;
      sprintf(myinfo->stream_aspect, "%s","");
      break;
    case 1:
      aspect_43=0;
      sprintf(myinfo->stream_aspect, "AR %s","16:9");
      break;
    case 2:
      aspect_43=1;
      sprintf(myinfo->stream_aspect, "AR %s","4:3");
      break;
    case 4:
      aspect_43=0;
      sprintf(myinfo->stream_aspect, "AR %s","2.21:1");
      break;
    case 8:
      aspect_43=0;
      sprintf(myinfo->stream_aspect, "AR %s","SQ.");
      break;
    default:
      aspect_43=0;
      sprintf(myinfo->stream_aspect, "AR %s","UNK.");
      break;
  }

  //printf("%s CH[%d] ONId[%d] TSId[%d] SId[%d]  VID_%s W[%d] H[%d]\n", PLUG_NAME, ch_id, ch_onid, ch_tsid, ch_sid, stream_aspect, stream_width, stream_height);
  return 0;
}
//------------------------------------------------------------------------------------------------------------------
int private_aspect(int amode)
{
        if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
        int ret=0;
        char filename[ strlen(PRIV_ASPECT_FILE) + 1 ];
        sprintf( filename, "%s", PRIV_ASPECT_FILE );
        //open private_aspect_info
        FILE *fp = fopen( filename, "rt" );
        if( fp == NULL )
        {
                if (deb>=1) printf("%s private_aspect_info fopen failed\n", PLUG_NAME);
                return -1;
        }

        read_ch();

        char line[128], *stmp;
        int tmp_orgnetid,tmp_tsid,tmp_sid,tmp_char;

        if ((amode==0) && (strcmp(db_opt_aspect_ratio,"16:9")==0))
        {
                int tmpx,tmpy,tmpw,tmph;               
                int filepos=0;

                while(fgets(line, sizeof line, fp) != NULL)                                                                                                                                                                                                                             //read config file line by line
                {
                        filepos++;
                        if(((stmp = strchr(line,'\n')) != NULL) && (strncmp(line,"#",1)!=0))
                        {                                                                                                      
                                *stmp = '\0';
                                if (8==sscanf(line,"%d|%d|%d|%d|x:%d|y:%d|w:%d|h:%d|",&tmp_orgnetid,&tmp_tsid,&tmp_sid,&tmp_char,&tmpx,&tmpy,&tmpw,&tmph))
                                {
                                        if (((tmp_orgnetid >= 0) && (tmp_orgnetid <= 65535)) && ((tmp_tsid >= 0) && (tmp_tsid <= 65535)) && ((tmp_sid >= 0) && (tmp_sid <= 65535)) && ((tmp_char >= 0) && (tmp_char <= 8)) && ((tmpx >= (-1280)) && (tmpx <= 1280)) && ((tmpy >= (-720)) && (tmpy <= 720)) && ((tmpw >= 160) && (tmpw <= (2*1280))) && ((tmph >= 96) && (tmph <= (2*720))))
                                        {
                                                if ((tmp_orgnetid==myinfo->ch_onid) && (tmp_tsid==myinfo->ch_tsid) && (tmp_sid==myinfo->ch_sid) && (tmp_char==0))
                                                {
                                                        if (deb>=1) printf("%s ch found -> flag [ignore] -> break\n", PLUG_NAME);
                                                        break;
                                                } else if (((tmp_orgnetid==myinfo->ch_onid) && (tmp_tsid==myinfo->ch_tsid) && (tmp_sid==myinfo->ch_sid) && (tmp_char==myinfo->stream_ar)) || ((tmp_orgnetid==0) && (tmp_tsid==0) && (tmp_sid==0) && (tmp_char==myinfo->stream_ar)))
                                                {
                                                        if (deb>=1) printf("%s ch found -> change default value to x:%d y:%d w:%d h%d -> break\n", PLUG_NAME, tmpx, tmpy, tmpw, tmph);
                                                        konfig[0].cx=tmpx;
                                                        konfig[0].cy=tmpy;
                                                        konfig[0].cw=tmpw;
                                                        konfig[0].ch=tmph;
                                                        ret=1;
                                                        break;
                                                }
                                        }       else {
                                                if (deb>=1) printf("%s some parameter out of range in line: %d\n", PLUG_NAME, filepos);
                                        }
                                }
                        }
                }

                fclose( fp );
        } else if ((amode==1) || (amode==2)) {
               
                int flag_writed=0;                                                                                                                                                                                                                              //juz zapisane 0:1
                char fileout[ strlen(PRIV_ASPECT_TMP) + 1 ];
                sprintf( fileout, "%s", PRIV_ASPECT_TMP );     
                FILE *fo = fopen( fileout, "w");

                //org_network_id|ts_id|service_id|ch_ar|x:xxxx|y:xxxx|w:xxxx|h:xxxx|your name

                while(fgets(line, sizeof line, fp) != NULL)    
                {
                        if ((stmp = strchr(line,'\n')) != NULL)
                        {      
                                if ((strncmp(line,"#",1)!=0) && (flag_writed==0))
                                {                                                                                              
                                        *stmp = '\0';
                                        if (4==sscanf(line,"%d|%d|%d|%d|",&tmp_orgnetid,&tmp_tsid,&tmp_sid,&tmp_char))
                                        {
                                                if (((tmp_orgnetid >= 0) && (tmp_orgnetid <= 65535)) && ((tmp_tsid >= 0) && (tmp_tsid <= 65535)) && ((tmp_sid >= 0) && (tmp_sid <= 65535)) && ((tmp_char >= 0) && (tmp_char <= 8)))
                                                {

                                                        if ((tmp_orgnetid==myinfo->ch_onid) && (tmp_tsid==myinfo->ch_tsid) && (tmp_sid==myinfo->ch_sid) && (tmp_char==myinfo->stream_ar))

                                                        {

                                                                //if ((dx!=0) && (dy!=0) && (dw!=1280) && (dh!=720))
                                                                if ((amode==1) && (! ((dx==0) && (dy==0) && (dw==1280) && (dh==720))))
                                                                        fprintf(fo, "%d|%d|%d|%d|x:%d|y:%d|w:%d|h:%d|#%s\n",myinfo->ch_onid,myinfo->ch_tsid,myinfo->ch_sid,myinfo->stream_ar,dx,dy,dw,dh,vkeyboard_with_maxlen(myinfo->ch_name,myinfo->ch_name,"vkeyboard","help_vkeyboard" ,28, vkeyboard_mode_plugin));      

                                                                flag_writed=1;

                                                        }
                                                        else if ((tmp_orgnetid==0) && (tmp_tsid==0) && (tmp_sid==0))
                                                        {

                                                                //if ((dx!=0) && (dy!=0) && (dw!=1280) && (dh!=720))
                                                                if ((amode==1) && (! ((dx==0) && (dy==0) && (dw==1280) && (dh==720))))
                                                                        fprintf(fo, "%d|%d|%d|%d|x:%d|y:%d|w:%d|h:%d|#%s\n",myinfo->ch_onid,myinfo->ch_tsid,myinfo->ch_sid,myinfo->stream_ar,dx,dy,dw,dh,vkeyboard_with_maxlen(myinfo->ch_name,myinfo->ch_name,"vkeyboard","help_vkeyboard" ,28, vkeyboard_mode_plugin));      

                                                                fprintf(fo, "%s\n",line);
                                                                flag_writed=1;

                                                        }
                                                        else
                                                        {
                                                                fprintf(fo, "%s\n",line);
                                                        }
                                                }
                                        }
                                }
                                else
                                {

                                        fputs(line, fo);

                                }
                        }
                }

                //if channel don't exist and universal settings don't exist in private_aspect_info then write at the end of file
                if ((amode==1) && (flag_writed==0) && (! ((dx==0) && (dy==0) && (dw==1280) && (dh==720))))
                        fprintf(fo, "%d|%d|%d|%d|x:%d|y:%d|w:%d|h:%d|#%s\n",myinfo->ch_onid,myinfo->ch_tsid,myinfo->ch_sid,myinfo->stream_ar,dx,dy,dw,dh,vkeyboard_with_maxlen(myinfo->ch_name,myinfo->ch_name,"vkeyboard","help_vkeyboard" ,28, vkeyboard_mode_plugin));      

                fflush( fo );
                fclose( fo );
                fclose( fp );
                remove(filename);
                rename(fileout, filename);
        }
        return ret;
}
//------------------------------------------------------------------------------------------------------------------
void read_config(int mode)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  int pos, tmp;
  char buf[40];

  cm_file_read_configuration(CONF_FILE);

  switch(mode)
  {
    case 0:
      tmp=cm_get_config_value_int("exitafterset",1);
      if ((tmp==0) || (tmp==1))
        exit_after=tmp;
      else
        exit_after=1;

      for( pos = 1; pos < MAX_BANK; pos++ )
      {
        sprintf(buf, "x%d",pos);
        tmp=cm_get_config_value_int(buf,0);
        if ((tmp < -(SCREEN_W)) || (tmp > SCREEN_W))
          tmp=0;
        konfig[pos].cx=tmp;

        sprintf(buf, "y%d",pos);
        tmp=cm_get_config_value_int(buf,0);
        if ((tmp < -(SCREEN_H)) || (tmp > SCREEN_H))
          tmp=0;
        konfig[pos].cy=tmp;

        sprintf(buf, "w%d",pos);
        tmp=cm_get_config_value_int(buf,SCREEN_W);
        if ((tmp < MIN_W) || (tmp > (2*SCREEN_W)))
          tmp=SCREEN_W;
        konfig[pos].cw=tmp;

        sprintf(buf, "h%d",pos);
        tmp=cm_get_config_value_int(buf,SCREEN_H);
        if ((tmp < MIN_H) || (tmp > (2*SCREEN_H)))
          tmp=SCREEN_H;
        konfig[pos].ch=tmp;

        sprintf(buf, "preset%dname",pos);
        memset(&konfig[pos].preset_name, 0, sizeof(konfig[pos].preset_name));
        if (cm_get_config_value_text(buf)==NULL)
          sprintf(konfig[pos].preset_name, "x:%d y:%d w:%d h:%d",konfig[pos].cx,konfig[pos].cy,konfig[pos].cw,konfig[pos].ch);
        else
          sprintf(konfig[pos].preset_name, "%s",cm_get_config_value_text(buf));

        if ((found_in_priv==1) && (konfig[pos].cx==konfig[0].cx) && (konfig[pos].cy==konfig[0].cy) && (konfig[pos].cw==konfig[0].cw) && (konfig[pos].ch==konfig[0].ch))
          konfig[pos].in_priv=1;
        else
          konfig[pos].in_priv=0;
      }
      break;
    case 1:
      tmp=cm_get_config_value_int("channelssw",10);
      if ((tmp>=1) && (tmp<=100))
        max_ch=tmp;
      else
        max_ch=10;
      break;
    case 10:
      dx=0;
      dy=0;
      dw=1280;
      dh=720;
      break;
    case 11 ... 19:
      sprintf(buf, "x%d",mode-10);
      tmp=cm_get_config_value_int(buf,0);
      if ((tmp < -(SCREEN_W)) || (tmp > SCREEN_W))
        tmp=0;
      dx=tmp;

      sprintf(buf, "y%d",mode-10);
      tmp=cm_get_config_value_int(buf,0);
      if ((tmp < -(SCREEN_H)) || (tmp > SCREEN_H))
        tmp=0;
      dy=tmp;

      sprintf(buf, "w%d",mode-10);
      tmp=cm_get_config_value_int(buf,SCREEN_W);
      if ((tmp < MIN_W) || (tmp > (2*SCREEN_W)))
        tmp=SCREEN_W;
      dw=tmp;

      sprintf(buf, "h%d",mode-10);
      tmp=cm_get_config_value_int(buf,SCREEN_H);
      if ((tmp < MIN_H) || (tmp > (2*SCREEN_H)))
        tmp=SCREEN_H;
      dh=tmp;
      break;
  }
}
//------------------------------------------------------------------------------------------------------------------
void save_config(void)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  int pos;

  FILE *tmp_file = fopen(CONF_FILE, "w");

  fprintf(tmp_file, "exitafterset=%d\n",exit_after);
  fprintf(tmp_file, "channelssw=%d\n",max_ch);
  //fprintf(tmp_file, "resizeaspect43=%d\n",aspect_43);

  for( pos = 1; pos < MAX_BANK; pos++ )
  {
    fprintf(tmp_file, "preset%dname=%s\n",pos , konfig[pos].preset_name);
    fprintf(tmp_file, "x%d=%d\n",pos , konfig[pos].cx);
    fprintf(tmp_file, "y%d=%d\n",pos , konfig[pos].cy);
    fprintf(tmp_file, "w%d=%d\n",pos , konfig[pos].cw);
    fprintf(tmp_file, "h%d=%d\n",pos , konfig[pos].ch);
  }
  fflush(tmp_file);
  fclose(tmp_file);
}
//------------------------------------------------------------------------------------------------------------------
void write_table(int tmppos)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  if ((tmppos>=-1) && (tmppos<=9))
  {
    int temp_pos;
    if (tmppos==-1)
      temp_pos=0;
    else
      temp_pos=tmppos;

    konfig[temp_pos].cx=dx;
    konfig[temp_pos].cy=dy;
    konfig[temp_pos].cw=dw;
    konfig[temp_pos].ch=dh;
    save_flag=0;
    row_repaint(8,1,1);
    row_repaint(9,1,1);
    screen_pos_text(0);
  }

  if ((tmppos>=1) && (tmppos<=9))
  {
    int tmpx,tmpy,tmpw,tmph;
    if ((4==sscanf(konfig[tmppos].preset_name,"x:%d y:%d w:%d h:%d",&tmpx,&tmpy,&tmpw,&tmph)) || (strcmp(konfig[tmppos].preset_name,"")==0))
    {
      if ((tmpx!=dx) || (tmpy!=dy) || (tmpw!=dw) || (tmpx!=dx))
        sprintf(konfig[tmppos].preset_name, "x:%d y:%d w:%d h:%d",dx,dy,dw,dh);
    }
    char buf[40];
    sprintf(buf, "PRESET %d NAME",tmppos);         
    sprintf(konfig[tmppos].preset_name,"%s",vkeyboard_with_maxlen(buf,konfig[tmppos].preset_name,"vkeyboard","help_vkeyboard" ,28, vkeyboard_mode_plugin));
    if (strcmp(konfig[tmppos].preset_name,"")==0)
      sprintf(konfig[tmppos].preset_name, "x:%d y:%d w:%d h:%d",dx,dy,dw,dh);

    save_config();
  }
  else if (tmppos<=0)
  {
    if (myinfo->ch_id != 0)
    {
      if (tmppos==-1)
        private_aspect(2);
      else
        private_aspect(1);

      found_in_priv=private_aspect(0);
      if (found_in_priv==1)
      {
        int pos;
        for( pos = 1; pos < MAX_BANK; pos++ )
        {

          //if (((konfig[pos].cx==dx) && (konfig[pos].cy==dy) && (konfig[pos].cw==dw) && (konfig[pos].ch==dh)) && ((konfig[pos].cx!=0) && (konfig[pos].cy!=0) && (konfig[pos].cw!=1280) && (konfig[pos].ch!=720)))
          if (((konfig[pos].cx==dx) && (konfig[pos].cy==dy) && (konfig[pos].cw==dw) && (konfig[pos].ch==dh)) && (! ((konfig[pos].cx==0) && (konfig[pos].cy==0) && (konfig[pos].cw==1280) && (konfig[pos].ch==720))))
            konfig[pos].in_priv=1;
          else
            konfig[pos].in_priv=0;

        }
      }
    }
  }
}
//------------------------------------------------------------------------------------------------------------------
int ch_change(int new_seq)
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  db = db_open(type_db);
  if( db )
  {
    int new_id = db_channel_get_id_by_seq(db,new_seq,myinfo->ch_servtype);
    db_close(db);
    if (new_id > 0)
    {
      /*
      //WITH CALLBACK
      callback_t check_ch_cb;
      check_ch_cb.function = check_ch;
      check_ch_cb.arg = NULL;

      ch_change_fg( ch_mode_live , new_id, &check_ch_cb);
      */

      ch_change_fg( ch_mode_live , new_id, NULL);

      myinfo->ch_id = ch_watching_id(ch_mode_live);
      read_ch();
      //miniw_hide_show=mini_win(2);
      if ((isinfo==1) && (myinfo->vidpid!=0))
      {
        sync_ch();
        if (issync==1)
        {
          check_ch();
          scr_ch(1);
        }
      }
    }
  }
  else
    return -1;

  return 0;
}
//------------------------------------------------------------------------------------------------------------------
long long timeval_diff(struct timeval *difference, struct timeval *end_time, struct timeval *start_time)
{
  struct timeval temp_diff;

  if(difference==NULL)
  {
    difference=&temp_diff;
  }

  difference->tv_sec =end_time->tv_sec -start_time->tv_sec ;
  difference->tv_usec=end_time->tv_usec-start_time->tv_usec;

  /* Using while instead of if below makes the code slightly more robust. */

  while(difference->tv_usec<0)
  {
    difference->tv_usec+=1000000;
    difference->tv_sec -=1;
  }

  return 1000000LL*difference->tv_sec+
                   difference->tv_usec;

} /* timeval_diff() */
//------------------------------------------------------------------------------------------------------------------
int test_irk(void)
{
  if (deb>=1) printf("%s START[%s]\n", PLUG_NAME, __func__);

  struct timeval one_start;
  struct timeval one_stop;

  typedef struct t_time_test
  {
    int pozycja;
    struct timeval one;
    char ch_res[51];                                //ch_resolution
    char ch_n[51];                                  //ch name
    char scramble[16];                             
  }time_test_t;

  time_test_t *time_test;
  time_test = malloc(sizeof(time_test_t) * (max_ch + 1));

  int pos=1;
  int first_ch_seq=myinfo->ch_seq;        //save first channel

  int only_scr=0;
  while ((pos<=max_ch) && (only_scr<5))
  {
    gettimeofday(&one_start,NULL);
    ch_change(myinfo->ch_seq+1);

    if ((isinfo==1) && (issync==1) && (myinfo->vidpid!=0))
    {
      gettimeofday(&one_stop,NULL);
      timeval_diff(&time_test[pos].one,&one_stop,&one_start);
      time_test[pos].pozycja=pos;
      sprintf(time_test[pos].ch_res, "%s",myinfo->ch_resolution);
      sprintf(time_test[pos].ch_n, "%s",myinfo->ch_name);
      sprintf(time_test[pos].scramble, "%s",myinfo->scr);
      pos++;
      only_scr=0;
    }
    else
      only_scr++;
  }

  ch_change(first_ch_seq);                        //restore channel

  pos--;
  int old_pos=pos;
  if (deb>=3) printf("%s COUNTED CH[%d]  SCRAMBLED CH(not counted)[%d]\n", PLUG_NAME,old_pos, only_scr);

  if (pos>1)
  {
    FILE *time_file = fopen("/tmp/switch_time.log", "w");
    int mindiff=0;
    int secdiff=0;
    double usecdiff;

    if (time_file)
    {
      double timediff=0;

      for( pos = 1; pos <= old_pos; pos++ )
      {
        if (deb>=2) printf("%s %02d> %ld.%06.0fsec RES:%s T:%s N:%s\n",PLUG_NAME, pos, time_test[pos].one.tv_sec, (double)time_test[pos].one.tv_usec,time_test[pos].ch_res,time_test[pos].scramble,time_test[pos].ch_n);
        fprintf(time_file, " %02d> %ld.%06.0fsec RES:%s T:%s N:%s\n",pos, time_test[pos].one.tv_sec, (double)time_test[pos].one.tv_usec,time_test[pos].ch_res,time_test[pos].scramble,time_test[pos].ch_n);

        timediff+=time_test[pos].one.tv_sec + (time_test[pos].one.tv_usec / 1000000.0);
      }

      mindiff=(int)timediff / 60;
      secdiff=(int)timediff % 60;
      usecdiff=timediff-(mindiff * 60)-secdiff;

      if (deb>=2) printf("%s SWITCH %d CHANNELS -> %d:%2.6f [m:s.usec]\n",PLUG_NAME,old_pos, mindiff,(double)secdiff+usecdiff);
      fprintf(time_file, "SWITCH %d CHANNELS -> %d:%2.6f [m:s.usec]\n",old_pos, mindiff,(double)secdiff+usecdiff);

      fflush(time_file);
      fclose(time_file);

      char tmp_ch[255];
      sprintf(tmp_ch, "echo web_show_mess 1 \"SWITCH %d CHANNELS -> %d:%2.6f [m:s.usec]\" >/dev/commander",old_pos, mindiff,(double)secdiff+usecdiff);
      system(tmp_ch);
    }
  }

  free(time_test);

  return 0;
}
//------------------------------------------------------------------------------------------------------------------
/* main func. */
int plugin_main(int argc, char *argv[])
{
  if (deb>=5) printf("%s START[%s]\n", PLUG_NAME, __func__);
  if (deb>=1) printf("%s START\n", PLUG_NAME);

#ifdef TEST_BMT
  sqlite3 *dbbmt = db_open(bmt_db);
  if( !dbbmt )
  {
    printf("%s unable to open bmt_db\n", PLUG_NAME);
    //return -1;
  }
  else
  {
    printf("%s open bmt_db\n", PLUG_NAME);

    //write test
    int wynik=bmt_db_settings_set(dbbmt,"test1","to_jest_wpis");
    printf("%s WRITE %s\n", PLUG_NAME,(wynik==0) ? "OK":"ERROR");

    //read test
    char *rezultat;
    if (bmt_db_settings_get(dbbmt, "test1", &rezultat )==0)
      printf("%s READ [%s]\n", PLUG_NAME,rezultat);
    else
      printf("%s READ ERROR\n", PLUG_NAME);
    free(rezultat);

    db_close(dbbmt);
  }
#endif

  int key_code=-1;
  int key_value=-1;
  int do_exit=0;
       
  front_display_init();

  myinfo = malloc(sizeof(myinfo_t));

  myinfo->ch_id = ch_watching_id(ch_mode_live);

  if (argc > 1)
  {
    if((!strcmp(argv[1],"info")) && (myinfo->ch_id != 0))
    {
      mode_info=1;
      read_ch();
      miniw_hide_show=mini_win(1);
      front_display_write_msg("INFO BOX", FRONT_DISPLAY_NOPAD);
      read_config(mode_info);
      goto keyget;
    }
    else if((!strcmp(argv[1],"p")) && (argc == 3)) 
    {
      if((atoi(argv[2])>=0) && (atoi(argv[2])<=9))
      {
        int conf_pos=(int)atoi(argv[2])+10;
        read_config(conf_pos);
        vid_change(20);
      }
      goto exit1;
    } 
  }

  konfig = malloc(sizeof(my_config_t) * MAX_BANK);

  int pos;
  for( pos = 0; pos < MAX_BANK; pos++ )
  {
    memset(&konfig[pos].preset_name, 0, sizeof(konfig[pos].preset_name));
    memset(&konfig[pos].cx, 0, sizeof(konfig[pos].cx));
    memset(&konfig[pos].cy, 0, sizeof(konfig[pos].cy));
    memset(&konfig[pos].cw, 0, sizeof(konfig[pos].cw));
    memset(&konfig[pos].ch, 0, sizeof(konfig[pos].ch));
    memset(&konfig[pos].in_priv, 0, sizeof(konfig[pos].in_priv));
  }

  front_display_init();
  front_display_write_msg(PLUG_NAME, FRONT_DISPLAY_NOPAD);

  if (myinfo->ch_id != 0)
    found_in_priv=private_aspect(0);

  if (found_in_priv==1)
  {
    //vid_change(0);
    dx=konfig[0].cx;
    dy=konfig[0].cy;
    dw=konfig[0].cw;
    dh=konfig[0].ch;
  }
  else
  {
    konfig[0].cx=0;
    konfig[0].cy=0;
    konfig[0].cw=1280;
    konfig[0].ch=720;

    dx=konfig[0].cx;
    dy=konfig[0].cy;
    dw=konfig[0].cw;
    dh=konfig[0].ch;
  }

  read_config(mode_info);
  make_list_menu(list_menu);

/*
  if ((dw==0) || (dh==0))
  {
    int w,h;
    vid_get_screen_size(0, &w, &h);
    dw = (int)(w/1.5);
    dh = (int)(h/1.5);
  }
*/
  screen_pos_text(0);

keyget:

  plug_event_init();

  while (1)
  {
    event_t event;
    if( plug_event_rcv( &event ) < 0 )      continue;
    if( event.type != event_type_key)       continue; /* in plugin only event_type_key is valid */

    key_value = event_key_2value(&event);
    key_code = event_key_2code(&event);

    if(key_code==KEY_BACK) break;

    //if key is pressed mode info
    if((key_value>0) && (mode_info==1))
    {
      switch(key_code)
      {
        case KEY_UP:                         //ch up
        case KEY_PAGEUP:
          ch_change(myinfo->ch_seq+1);
          break;
        case KEY_DOWN:                      //ch down
        case KEY_PAGEDOWN:
          ch_change(myinfo->ch_seq-1);
          break;
        case KEY_OK:                        //same ch
          ch_change(myinfo->ch_seq);
          break;
        case KEY_RIGHT:                     //vol up
        case KEY_F17:
          volume_up();
          break;
        case KEY_LEFT:                      //vol down
        case KEY_F18:
          volume_down();
          break;
        case KEY_MUTE:                     //vol mute
          volume_mute();
          break;
        case KEY_SLOW:
          test_irk();
          break;
        case KEY_F4:                       //key_photo
          picon_name();
          break;
      }
    }

                //if key is pressed mode normal
    if((key_value>0) && (mode_info==0))
    {
      switch(key_code)
      {
        case KEY_MENU:
          if (list_menu==0)                                      
            list_menu=1;
          else
          {
            if (save_flag==1)
            {                              
              save_flag=0;
              screen_pos_text(0);
            }
            list_menu=0;
          }
          make_list_menu(list_menu);
          break;
        case KEY_OK:
          vid_change(0);
          screen_pos_text(0);
          if ((save_flag==1) && (list_menu==1))
          {
            save_flag=0;
            row_repaint(8,1,1);
            row_repaint(9,1,1);
          }
          if ((exit_after==1) && (list_menu==0)) do_exit=1;
          break;
        case KEY_INFO:
          if (myinfo->ch_id != 0)
          {
            if (miniw_hide_show==0)        
            {      
              read_ch();     
              miniw_hide_show=1;
            }
            else if (miniw_hide_show==1)
              miniw_hide_show=0;
            miniw_hide_show=mini_win(miniw_hide_show);
          }
          break;
        case KEY_F4:                    //key_photo
          picon_name();
          break;
        case KEY_0:
          /*
          konfig[0].cx=0;
          konfig[0].cy=0;
          konfig[0].cw=1280;
          konfig[0].ch=720;
          vid_change(0);
          */
          vid_change(-1);
          screen_pos_text(0);
          if ((save_flag==1) && (list_menu==1))
          {
            save_flag=0;
            row_repaint(8,1,1);
            row_repaint(9,1,1);
          }
          if ((exit_after==1) && (list_menu==0)) do_exit=1;      
          break;
      }

      if (list_menu==0) 
      {
        switch(key_code)
        {
          case KEY_1:
            vid_change(MAX_BANK+1);
            if (exit_after==1) do_exit=1;
            break;
          case KEY_2:
            vid_change(MAX_BANK+2);
            if (exit_after==1) do_exit=1;
            break;
          case KEY_3:
            vid_change(MAX_BANK+3);
            if (exit_after==1) do_exit=1;
            break;
          case KEY_4:
            vid_change(MAX_BANK+4);
            if (exit_after==1) do_exit=1;
            break;
          case KEY_5:
            vid_change(MAX_BANK+5);
            if (exit_after==1) do_exit=1;
            break;
          case KEY_6:
            vid_change(MAX_BANK+6);
            if (exit_after==1) do_exit=1;
            break;
          case KEY_7:
            vid_change(MAX_BANK+7);
            if (exit_after==1) do_exit=1;
            break;
          case KEY_8:
            vid_change(MAX_BANK+8);
            if (exit_after==1) do_exit=1;
            break;
          case KEY_9:
            vid_change(MAX_BANK+9);
            if (exit_after==1) do_exit=1;
            break;
        }
        if (do_exit==1) break;
      }
      else
      {
        switch(key_code)
        {
          case KEY_1:
            if (save_flag==1) write_table(1);
            break;
          case KEY_2:
            if (save_flag==1) write_table(2);
            break;
          case KEY_3:
            if (save_flag==1) write_table(3);
            break;
          case KEY_4:
            if (save_flag==1) write_table(4);
            break;
          case KEY_5:
            if (save_flag==1) write_table(5);
            break;
          case KEY_6:
            if (save_flag==1) write_table(6);
            break;
          case KEY_7:
            if (save_flag==1) write_table(7);
            break;
          case KEY_8:
            if (save_flag==1) write_table(8);
            break;
          case KEY_9:
            if (save_flag==1) write_table(9);
            break;
          case KEY_STOP:                                                                                                          //Delete channel from private_aspect_file
            if (save_flag==1) write_table(-1);
            break;
          case KEY_PLAY:                                                                                                          //Add channel to private_aspect_file
            if (save_flag==1) write_table(0);
            break;
          case KEY_LEFT:
            vid_change(1);  //left
            break;
          case KEY_RIGHT:
            vid_change(2);  //right
            break;
          case KEY_UP:
            vid_change(3);  //up
            break;
          case KEY_DOWN:
            vid_change(4);  //down
            break;
          case KEY_PAGEDOWN:
          case KEY_RED:
            vid_change(5);  //W down
            break;
          case KEY_PAGEUP:
          case KEY_GREEN:
            vid_change(6);  //W_up
            break;
          case KEY_F18:
          case KEY_YELLOW:
            vid_change(7);  //H down
            break;
          case KEY_F17:
          case KEY_BLUE:
            vid_change(8);  //H_up
            break;
          case KEY_REWIND:
            vid_change(21); //RESIZE PROPORTIONAL down
            break;
          case KEY_FASTFORWARD:
            vid_change(22); //RESIZE PROPORTIONAL up
            break;
          case KEY_FILE:
            if (aspect_43==0)                                      
              aspect_43=1;
            else
              aspect_43=0;
            row_repaint(6,1,1);
            break;
          case KEY_F5:
            if (exit_after==0)                                     
              exit_after=1;
            else
              exit_after=0;
            row_repaint(7,1,1);
            save_config();
            break;
          case KEY_RECORD:
            if (save_flag==0)                                      
              save_flag=1;
            else
              save_flag=0;
            row_repaint(8,1,1);
            row_repaint(9,1,1);
            break;
        }
      }
    }
  }

  //plug_event_close();

  if (piconname != NULL)
    picon_name();

  if (miniwindow != NULL)
    mini_win(0);

  if (window != NULL)
    wm_destroy(window);

  if (mode_info==0)
  {
    //save_config();
    free(konfig);
  }

exit1:

  plug_event_close();

  free(myinfo);

  //usleep(500000);
  //front_display_write_msg("ASPECTOR END", FRONT_DISPLAY_NOPAD);
  usleep(900000);
  front_display_finish();
  if (deb>=1) printf("%s END\n", PLUG_NAME);

  return 0;
}
