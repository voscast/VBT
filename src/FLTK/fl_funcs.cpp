    // FLTK GUI related functions
//
// Copyright 2007-2018 by Daniel Noethen.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>

#ifndef WIN32
 #include <sys/wait.h>
#endif

#include "gettext.h"
#include "config.h"

#include "cfg.h"
#include "butt.h"
#include "util.h"
#include "port_audio.h"
#include "timer.h"
#include "flgui.h"
#include "fl_funcs.h"
#include "shoutcast.h"
#include "icecast.h"
#include "strfuncs.h"


int get_bitrate_index(int bitrate)
{
    int bitrate_idx[] = { 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112,
                        128, 160, 192, 224, 256, 320 };

    for(int i = 0; i < sizeof(bitrate_idx)/sizeof(int); i++)
        if(bitrate == bitrate_idx[i])
            return i;

    return 0;
}

int get_bitrate_index_vorbis_vbr(int bitrate)
{
    int bitrate_idx[] = { 0, 48, 56, 64, 80, 96, 112,
                        128, 160, 192, 224, 256, 320 };

    for(int i = 0; i < sizeof(bitrate_idx)/sizeof(int); i++)
        if(bitrate == bitrate_idx[i])
            return i;

    return 0;
}

void fill_cfg_widgets(void)
{
    int i;


#ifndef HAVE_LIBFDK_AAC
    fl_g->menu_item_cfg_aac->hide();
    fl_g->menu_item_rec_aac->hide();
    Fl::check();
#endif
  
    //fill the main section
    for(i = 0; i < cfg.audio.dev_count; i++)
    {
        unsigned long dev_name_len = strlen(cfg.audio.pcm_list[i]->name)+10;
        char *dev_name = (char*)malloc(dev_name_len);
        
        snprintf(dev_name, dev_name_len, "%d: %s", i, cfg.audio.pcm_list[i]->name);
        fl_g->choice_cfg_dev->add(dev_name);
        free(dev_name);
    }

    
    fl_g->choice_cfg_dev->value(cfg.audio.dev_num);
    

    fl_g->choice_cfg_act_srv->clear();
    fl_g->choice_cfg_act_srv->redraw();
    for(i = 0; i < cfg.main.num_of_srv; i++)
        fl_g->choice_cfg_act_srv->add(cfg.srv[i]->name);

    if(cfg.main.num_of_srv > 0)
    {
        fl_g->button_cfg_edit_srv->activate();
        fl_g->button_cfg_del_srv->activate();
        fl_g->choice_cfg_act_srv->activate();
    }
    else
    {
        fl_g->button_cfg_edit_srv->deactivate();
        fl_g->button_cfg_del_srv->deactivate();
        fl_g->choice_cfg_act_srv->deactivate();
    }

    fl_g->choice_cfg_act_icy->clear();
    fl_g->choice_cfg_act_icy->redraw();
    for(i = 0; i < cfg.main.num_of_icy; i++)
        fl_g->choice_cfg_act_icy->add(cfg.icy[i]->name);

    if(cfg.main.num_of_icy > 0)
    {
        fl_g->button_cfg_edit_icy->activate();
        fl_g->button_cfg_del_icy->activate();
        fl_g->choice_cfg_act_icy->activate();

    }
    else
    {
        fl_g->button_cfg_edit_icy->deactivate();
        fl_g->button_cfg_del_icy->deactivate();
        fl_g->choice_cfg_act_icy->deactivate();
    }

    fl_g->choice_cfg_act_srv->value(cfg.selected_srv);
    fl_g->choice_cfg_act_icy->value(cfg.selected_icy);


    fl_g->check_cfg_connect->value(cfg.main.connect_at_startup);
    fl_g->check_cfg_force_reconnecting->value(cfg.main.force_reconnecting);

    fl_g->input_log_filename->value(cfg.main.log_file);

    fl_g->check_update_at_startup->value(cfg.main.check_for_update);
    
    fl_g->check_start_agent->value(cfg.main.start_agent);
    fl_g->check_minimize_to_tray->value(cfg.main.minimize_to_tray);


    //fill the audio section
    if(!strcmp(cfg.audio.codec, "mp3"))
        fl_g->choice_cfg_codec->value(CHOICE_MP3);
    else if(!strcmp(cfg.audio.codec, "ogg"))
        fl_g->choice_cfg_codec->value(CHOICE_OGG);
    else if(!strcmp(cfg.audio.codec, "opus"))
        fl_g->choice_cfg_codec->value(CHOICE_OPUS);
    else if(!strcmp(cfg.audio.codec, "aac"))
        fl_g->choice_cfg_codec->value(CHOICE_AAC);
    else if(!strcmp(cfg.audio.codec, "flac"))
    {
        fl_g->choice_cfg_codec->value(CHOICE_FLAC);
        fl_g->choice_cfg_bitrate->hide();
    }
    
    if (cfg.audio.dev_remember == REMEMBER_BY_ID)
        fl_g->radio_cfg_ID->setonly();
    else
        fl_g->radio_cfg_name->setonly();

    if(cfg.audio.channel == 1)
        fl_g->choice_cfg_channel->value(CHOICE_MONO);
    else
        fl_g->choice_cfg_channel->value(CHOICE_STEREO);

    fl_g->choice_cfg_bitrate->value(get_bitrate_index(cfg.audio.bitrate));

    fl_g->input_cfg_present_level->value(-cfg.audio.signal_level);
    fl_g->input_cfg_absent_level->value(-cfg.audio.silence_level);

    fl_g->check_cfg_mono_to_stereo->value(cfg.audio.mono_to_stereo);
    
    fl_g->input_cfg_buffer->value(cfg.audio.buffer_ms);

    fl_g->choice_cfg_resample_mode->value(cfg.audio.resample_mode);
    
    // Fill stream section
    fl_g->check_song_update_active->value(cfg.main.song_update);
    fl_g->check_read_last_line->value(cfg.main.read_last_line);
    fl_g->choice_cfg_song_delay->value(cfg.main.song_delay/2);
    
    fl_g->input_cfg_song_file->value(cfg.main.song_path);
    fl_g->input_cfg_signal->value(cfg.main.signal_threshold);
    fl_g->input_cfg_silence->value(cfg.main.silence_threshold);
    
    fl_g->input_cfg_song_prefix->value(cfg.main.song_prefix);
    fl_g->input_cfg_song_suffix->value(cfg.main.song_suffix);

    
#if __APPLE__ && __MACH__
    fl_g->choice_cfg_app->add("iTunes\\/Music");
    fl_g->choice_cfg_app->add("Spotify");
    fl_g->choice_cfg_app->add("VOX");
    fl_g->check_cfg_use_app->value(cfg.main.app_update);
    fl_g->choice_cfg_app->value(cfg.main.app_update_service);
    if (cfg.main.app_artist_title_order == APP_ARTIST_FIRST)
        fl_g->radio_cfg_artist_title->setonly();
    else
        fl_g->radio_cfg_title_artist->setonly();
#elif (__linux__ || __FreeBSD__) && HAVE_DBUS
    fl_g->choice_cfg_app->add("Rhythmbox");
    fl_g->choice_cfg_app->add("Banshee");
    fl_g->choice_cfg_app->add("Clementine");
    fl_g->choice_cfg_app->add("Spotify");
    fl_g->choice_cfg_app->add("Cantata");
    fl_g->choice_cfg_app->add("Strawberry");
    fl_g->check_cfg_use_app->value(cfg.main.app_update);
    fl_g->choice_cfg_app->value(cfg.main.app_update_service);
    if (cfg.main.app_artist_title_order == APP_ARTIST_FIRST)
        fl_g->radio_cfg_artist_title->setonly();
    else
        fl_g->radio_cfg_title_artist->setonly();
#elif WIN32
    fl_g->choice_cfg_app->add(_("Not supported on Windows"));
    fl_g->choice_cfg_app->value(0);
    fl_g->check_cfg_use_app->value(0);
    fl_g->check_cfg_use_app->deactivate();
    fl_g->choice_cfg_app->deactivate();
    fl_g->radio_cfg_artist_title->deactivate();
    fl_g->radio_cfg_title_artist->deactivate();
#endif
    
    
    
    //fill the record section
    fl_g->input_rec_filename->value(cfg.rec.filename);
    fl_g->input_rec_folder->value(cfg.rec.folder);
    fl_g->input_rec_split_time->value(cfg.rec.split_time);

    if(!strcmp(cfg.rec.codec, "mp3"))
        fl_g->choice_rec_codec->value(CHOICE_MP3);
    else if(!strcmp(cfg.rec.codec, "ogg"))
        fl_g->choice_rec_codec->value(CHOICE_OGG);
    else if(!strcmp(cfg.rec.codec, "opus"))
        fl_g->choice_rec_codec->value(CHOICE_OPUS);
    else if(!strcmp(cfg.rec.codec, "aac"))
        fl_g->choice_rec_codec->value(CHOICE_AAC);
    else if(!strcmp(cfg.rec.codec, "flac"))
    {
        fl_g->choice_rec_codec->value(CHOICE_FLAC);
        fl_g->choice_rec_bitrate->hide();
    }
    else //wav
    {
        fl_g->choice_rec_codec->value(CHOICE_WAV);
        fl_g->choice_rec_bitrate->hide();
    }

    fl_g->choice_rec_bitrate->value(get_bitrate_index(cfg.rec.bitrate));

    if(cfg.rec.start_rec)
        fl_g->check_cfg_auto_start_rec->value(1);
    else
        fl_g->check_cfg_auto_start_rec->value(0);
    
    if(cfg.rec.stop_rec)
        fl_g->check_cfg_auto_stop_rec->value(1);
    else
        fl_g->check_cfg_auto_stop_rec->value(0);
    

    if(cfg.rec.rec_after_launch)
        fl_g->check_cfg_rec_after_launch->value(1);
    else
        fl_g->check_cfg_rec_after_launch->value(0);

    if(cfg.rec.sync_to_hour)
        fl_g->check_sync_to_full_hour->value(1);
    else
        fl_g->check_sync_to_full_hour->value(0);

    fl_g->input_rec_signal->value(cfg.rec.signal_threshold);
    fl_g->input_rec_silence->value(cfg.rec.silence_threshold);

    //fill the ssl/tls section
    fl_g->input_tls_cert_file->value(cfg.tls.cert_file);
    fl_g->input_tls_cert_dir->value(cfg.tls.cert_dir);


    update_samplerates();
    update_channel_lists();

    //fill the DSP section
    fl_g->check_activate_eq->value(cfg.dsp.equalizer);
    
    slider_equalizer1_cb(cfg.dsp.gain1);
    fl_g->equalizerSlider1->value(cfg.dsp.gain1);
    
    slider_equalizer2_cb(cfg.dsp.gain2);
    fl_g->equalizerSlider2->value(cfg.dsp.gain2);
    
    slider_equalizer3_cb(cfg.dsp.gain3);
    fl_g->equalizerSlider3->value(cfg.dsp.gain3);
    
    slider_equalizer4_cb(cfg.dsp.gain4);
    fl_g->equalizerSlider4->value(cfg.dsp.gain4);
    
    slider_equalizer5_cb(cfg.dsp.gain5);
    fl_g->equalizerSlider5->value(cfg.dsp.gain5);
    
    slider_equalizer6_cb(cfg.dsp.gain6);
    fl_g->equalizerSlider6->value(cfg.dsp.gain6);
    
    slider_equalizer7_cb(cfg.dsp.gain7);
    fl_g->equalizerSlider7->value(cfg.dsp.gain7);
    
    slider_equalizer8_cb(cfg.dsp.gain8);
    fl_g->equalizerSlider8->value(cfg.dsp.gain8);
    
    slider_equalizer9_cb(cfg.dsp.gain9);
    fl_g->equalizerSlider9->value(cfg.dsp.gain9);
    
    slider_equalizer10_cb(cfg.dsp.gain10);
    fl_g->equalizerSlider10->value(cfg.dsp.gain10);
	
	fl_g->check_activate_drc->value(cfg.dsp.compressor);
    fl_g->check_aggressive_mode->value(cfg.dsp.aggressive_mode);
	
	slider_threshold_cb(cfg.dsp.threshold);
    fl_g->thresholdSlider->value(cfg.dsp.threshold);
	
	slider_ratio_cb(cfg.dsp.ratio);
    fl_g->ratioSlider->value(cfg.dsp.ratio);
	
	slider_attack_cb(cfg.dsp.attack);
    fl_g->attackSlider->value(cfg.dsp.attack);
	
	slider_release_cb(cfg.dsp.release);
    fl_g->releaseSlider->value(cfg.dsp.release);
    
	slider_makeup_cb(cfg.dsp.makeup_gain);
    fl_g->makeupSlider->value(cfg.dsp.makeup_gain);
	
    //fill the GUI section
    fl_g->button_gui_bg_color->color(cfg.main.bg_color, fl_lighter((Fl_Color)cfg.main.bg_color));
    fl_g->button_gui_text_color->color(cfg.main.txt_color,fl_lighter((Fl_Color)cfg.main.txt_color));
    fl_g->check_gui_attach->value(cfg.gui.attach);
    fl_g->check_gui_ontop->value(cfg.gui.ontop);
    if(cfg.gui.ontop)
    {
        fl_g->window_main->stay_on_top(1);
        fl_g->window_cfg->stay_on_top(1);
    }
    fl_g->check_gui_hide_log_window->value(cfg.gui.hide_log_window);
    fl_g->check_gui_remember_pos->value(cfg.gui.remember_pos);

   
    fl_g->check_gui_lcd_auto->value(cfg.gui.lcd_auto);

    fl_g->check_gui_start_minimized->value(cfg.gui.start_minimized);

    
    fl_g->choice_gui_language->value(cfg.gui.lang);
    
    if (cfg.gui.vu_mode == VU_MODE_GRADIENT)
        fl_g->radio_gui_vu_gradient->setonly();
    else
        fl_g->radio_gui_vu_solid->setonly();


    //fill mp3 codec settings section
    fl_g->choice_stream_mp3_enc_quality->value(cfg.mp3_codec_stream.enc_quality);
    fl_g->choice_stream_mp3_stereo_mode->value(cfg.mp3_codec_stream.stereo_mode);
    fl_g->choice_stream_mp3_bitrate_mode->value(cfg.mp3_codec_stream.bitrate_mode);
    fl_g->choice_stream_mp3_vbr_quality->value(cfg.mp3_codec_stream.vbr_quality);
    fl_g->choice_stream_mp3_vbr_min_bitrate->value(get_bitrate_index(cfg.mp3_codec_stream.vbr_min_bitrate));
    fl_g->choice_stream_mp3_vbr_max_bitrate->value(get_bitrate_index(cfg.mp3_codec_stream.vbr_max_bitrate));

    fl_g->choice_rec_mp3_enc_quality->value(cfg.mp3_codec_rec.enc_quality);
    fl_g->choice_rec_mp3_stereo_mode->value(cfg.mp3_codec_rec.stereo_mode);
    fl_g->choice_rec_mp3_bitrate_mode->value(cfg.mp3_codec_rec.bitrate_mode);
    fl_g->choice_rec_mp3_vbr_quality->value(cfg.mp3_codec_rec.vbr_quality);
    fl_g->choice_rec_mp3_vbr_min_bitrate->value(get_bitrate_index(cfg.mp3_codec_rec.vbr_min_bitrate));
    fl_g->choice_rec_mp3_vbr_max_bitrate->value(get_bitrate_index(cfg.mp3_codec_rec.vbr_max_bitrate));


    //fill vorbis codec settings section
    fl_g->choice_stream_vorbis_bitrate_mode->value(cfg.vorbis_codec_stream.bitrate_mode);
    fl_g->choice_stream_vorbis_vbr_quality->value(cfg.vorbis_codec_stream.vbr_quality);
    fl_g->choice_stream_vorbis_vbr_min_bitrate->value(get_bitrate_index_vorbis_vbr(cfg.vorbis_codec_stream.vbr_min_bitrate));
    fl_g->choice_stream_vorbis_vbr_max_bitrate->value(get_bitrate_index_vorbis_vbr(cfg.vorbis_codec_stream.vbr_max_bitrate));

    fl_g->choice_rec_vorbis_bitrate_mode->value(cfg.vorbis_codec_rec.bitrate_mode);
    fl_g->choice_rec_vorbis_vbr_quality->value(cfg.vorbis_codec_rec.vbr_quality);
    fl_g->choice_rec_vorbis_vbr_min_bitrate->value(get_bitrate_index_vorbis_vbr(cfg.vorbis_codec_rec.vbr_min_bitrate));
    fl_g->choice_rec_vorbis_vbr_max_bitrate->value(get_bitrate_index_vorbis_vbr(cfg.vorbis_codec_rec.vbr_max_bitrate));
    
    //fill opus codec settings section
    fl_g->choice_stream_opus_bitrate_mode->value(cfg.opus_codec_stream.bitrate_mode);
    fl_g->choice_stream_opus_quality->value(cfg.opus_codec_stream.quality);
    fl_g->choice_stream_opus_audio_type->value(cfg.opus_codec_stream.audio_type);
    fl_g->choice_stream_opus_bandwidth->value(cfg.opus_codec_stream.bandwidth);

    fl_g->choice_rec_opus_bitrate_mode->value(cfg.opus_codec_rec.bitrate_mode);
    fl_g->choice_rec_opus_quality->value(cfg.opus_codec_rec.quality);
    fl_g->choice_rec_opus_audio_type->value(cfg.opus_codec_rec.audio_type);
    fl_g->choice_rec_opus_bandwidth->value(cfg.opus_codec_rec.bandwidth);
    

    //fill aac codec settings section
    fl_g->choice_stream_aac_bitrate_mode->value(cfg.aac_codec_stream.bitrate_mode);
    fl_g->choice_stream_aac_afterburner->value(cfg.aac_codec_stream.afterburner);
    fl_g->choice_stream_aac_profile->value(cfg.aac_codec_stream.profile);

    fl_g->choice_rec_aac_bitrate_mode->value(cfg.aac_codec_rec.bitrate_mode);
    fl_g->choice_rec_aac_afterburner->value(cfg.aac_codec_rec.afterburner);
    fl_g->choice_rec_aac_profile->value(cfg.aac_codec_rec.profile);

}

//Updates the samplerate drop down menu for the audio
//device the user has selected
void update_samplerates(void)
{
    int i;
    int *sr_list;
    char sr_asc[10];

    fl_g->choice_cfg_samplerate->clear();

    sr_list = cfg.audio.pcm_list[cfg.audio.dev_num]->sr_list;

    for(i = 0; sr_list[i] != 0; i++)
    {
        snprintf(sr_asc, sizeof(sr_asc), "%dHz", sr_list[i]);
        fl_g->choice_cfg_samplerate->add(sr_asc);

        if(cfg.audio.samplerate == sr_list[i])
            fl_g->choice_cfg_samplerate->value(i);
    }
    if(i == 0)
    {
        fl_g->choice_cfg_samplerate->add("dev. not supported");
        fl_g->choice_cfg_samplerate->value(0);
    }
}

void update_channel_lists(void)
{
    
    int i;
    char ch_num_txt[16];
    
    int dev_num = fl_g->choice_cfg_dev->value();
    int num_of_channels = cfg.audio.pcm_list[dev_num]->num_of_channels;
    
    fl_g->choice_cfg_left_channel->clear();
    fl_g->choice_cfg_right_channel->clear();
    
    for (i = 1; i <= num_of_channels; i++)
    {
        snprintf(ch_num_txt, sizeof(ch_num_txt), "%d", i);
        fl_g->choice_cfg_left_channel->add(ch_num_txt);
        fl_g->choice_cfg_right_channel->add(ch_num_txt);
    }
    
    fl_g->choice_cfg_left_channel->value(cfg.audio.left_ch-1);
    fl_g->choice_cfg_right_channel->value(cfg.audio.right_ch-1);    
    
}

void write_log(const char* message)
{
    FILE *log_fd;

    if ((cfg.main.log_file != NULL) && (strlen(cfg.main.log_file) > 0))
    {
        log_fd = fl_fopen(cfg.main.log_file, "ab");
        if (log_fd != NULL)
        {
            fprintf(log_fd, "%s", message);
            fclose(log_fd);
        }
    }
        
}

void print_info(const char* info, int info_type)
{
    char timebuf[10];
    char logtimestamp[21];
    char* infotxt;
    FILE *log_fd;
    int len;

    time_t test;
    struct tm  *mytime;
    static struct tm time_bak;

    infotxt = strdup(info);


    test = time(NULL);
    mytime = localtime(&test);

    if( (time_bak.tm_min != mytime->tm_min) || (time_bak.tm_hour != mytime->tm_hour) )
    {
        time_bak.tm_min = mytime->tm_min;
        time_bak.tm_hour = mytime->tm_hour;
        strftime(timebuf, sizeof(timebuf), "\n%H:%M:", mytime);
        fl_g->info_buffer->append(timebuf);
    }

    fl_g->info_buffer->append((const char*)"\n");
    fl_g->info_buffer->append((const char*)info);

    //always scroll to the last line
    fl_g->info_output->scroll(fl_g->info_buffer->count_lines(0,     //count the lines from char 0 to the last character
                            fl_g->info_buffer->length()),           //returns the number of characters in the buffer
                            0);

    // log to log_file if defined
    if ((cfg.main.log_file != NULL) && (strlen(cfg.main.log_file) > 0))
    {
        strftime(logtimestamp, sizeof(logtimestamp), "%Y-%m-%d %H:%M:%S", mytime);
        log_fd = fl_fopen(cfg.main.log_file, "ab");
        if (log_fd != NULL) 
        {
            if(strchr(infotxt, ':'))
                strrpl(&infotxt, (char*)"\n", (char*)", ", MODE_ALL);
            else
                strrpl(&infotxt, (char*)"\n", (char*)" ", MODE_ALL);

            strrpl(&infotxt, (char*)":,", (char*)": ", MODE_ALL);
            strrpl(&infotxt, (char*)"\t", (char*)"", MODE_ALL);
            
            
            len = int(strlen(infotxt))-1;
            
            if (len > 0)
            {
                // remove trailing commas and spaces
                while (infotxt[len] == ',' || infotxt[len] == ' ')
                {
                    infotxt[len--] = '\0';
                    if (len < 0)
                        break;
                }
                
                fprintf(log_fd, "%s %s\n", logtimestamp, infotxt);
            }
            fclose(log_fd);
        }

    }
    
    free(infotxt);
}

void print_lcd(const char *text, int len, int home, int clear)
{
    /*
    if (!strcmp(text, _("idle")))
        fl_g->radio_co_logo->show();
    else
        fl_g->radio_co_logo->hide();
    */
    
    if(clear)
        fl_g->lcd->clear();

    fl_g->lcd->print((const uchar*)text, len);

    if(home)
        fl_g->lcd->cursor_pos(0);
}


void expand_string(char **str)
{
    int str_len;
    char expanded_str[1024];
    struct tm *date;
    const time_t t = time(NULL);

    // The %i (index number) place holder must be replaced with %%i
    // Otherwise strftime will replace %i with i and the index number will loose its function
    strrpl(str, (char*)"%i", (char*)"%%i", MODE_ALL);
    
    // %c, %x, %X specifiers are not allowed because they return illegal characters for file names
    // Therefore we make sure that strftime will ignore them
    strrpl(str, (char*)"%c", (char*)"%%c", MODE_ALL);
    strrpl(str, (char*)"%x", (char*)"%%x", MODE_ALL);
    strrpl(str, (char*)"%X", (char*)"%%X", MODE_ALL);

    date = localtime(&t);
    strftime(expanded_str, sizeof(expanded_str)-1, *str, date);
    
    str_len = strlen(expanded_str);
    *str = (char *)realloc(*str, str_len+1);
    strncpy(*str, expanded_str, str_len+1);
}

void test_file_extension(void)
{
    char *current_ext;

    current_ext = util_get_file_extension(cfg.rec.filename);

    // Append extension
    if(current_ext == NULL)
    {
        cfg.rec.filename = (char*)realloc(cfg.rec.filename, strlen(cfg.rec.filename)+strlen(cfg.rec.codec)+2);
        strcat(cfg.rec.filename, ".");
        strcat(cfg.rec.filename, cfg.rec.codec);
        fl_g->input_rec_filename->value(cfg.rec.filename);
    }
    // Replace extension
    else if(strcmp(current_ext, cfg.rec.codec))
    {
        strrpl(&cfg.rec.filename, current_ext, cfg.rec.codec, MODE_LAST);
        fl_g->input_rec_filename->value(cfg.rec.filename);
    }
}



void init_main_gui_and_audio(void)
{
    if(cfg.gui.remember_pos)
    {
        int VBT_x, VBT_y, VBT_w, VBT_h;

        VBT_x = cfg.gui.x_pos;
        VBT_y = cfg.gui.y_pos;
        VBT_w = fl_g->window_main->w();
        VBT_h = fl_g->window_main->h();
                
        int sx, sy, sw, sh;
        int is_visible = 0;
        for (int i = 0; i < Fl::screen_count(); i++)
        {
            Fl::screen_xywh(sx, sy, sw, sh, i);
            if ((VBT_x >= sx-VBT_w+50) && (VBT_x+50 < (sx+sw)) && (VBT_y >= sy-VBT_h+50) && (VBT_y+50 < (sy+sh))) {
                is_visible = 1;
            }
        }
        
        // Move VBT window to the saved position only if at least 50 pixel of VBT are visible on the screen
        if (is_visible)
            fl_g->window_main->position(cfg.gui.x_pos, cfg.gui.y_pos);
        
                
       // if( (VBT_x+VBT_w > 50) && (VBT_x+50 < total_screen_width) && (VBT_y+VBT_h > 50) && (VBT_y+50 < total_screen_height) )
    }
    
    
    fl_g->slider_gain->value(util_factor_to_db(cfg.main.gain));
    fl_g->window_main->redraw();
    
    if(cfg.gui.ontop)
        fl_g->window_main->stay_on_top(1);
    
    fl_g->button_info->label(_("Hide log"));
    if(cfg.gui.hide_log_window)
       button_info_cb();
          

    lame_stream.channel = cfg.audio.channel;
    lame_stream.bitrate = cfg.audio.bitrate;
    lame_stream.samplerate = cfg.audio.samplerate;
    lame_stream.enc_quality = cfg.mp3_codec_stream.enc_quality;
    lame_stream.stereo_mode = cfg.mp3_codec_stream.stereo_mode;
    lame_stream.bitrate_mode = cfg.mp3_codec_stream.bitrate_mode;
    lame_stream.vbr_quality = cfg.mp3_codec_stream.vbr_quality;
    lame_stream.vbr_min_bitrate = cfg.mp3_codec_stream.vbr_min_bitrate;
    lame_stream.vbr_max_bitrate = cfg.mp3_codec_stream.vbr_max_bitrate;
    lame_enc_reinit(&lame_stream);

    lame_rec.channel = cfg.audio.channel;
    lame_rec.bitrate = cfg.rec.bitrate;
    lame_rec.samplerate = cfg.audio.samplerate;
    lame_rec.enc_quality = cfg.mp3_codec_rec.enc_quality;
    lame_rec.stereo_mode = cfg.mp3_codec_rec.stereo_mode;
    lame_rec.bitrate_mode = cfg.mp3_codec_rec.bitrate_mode;
    lame_rec.vbr_quality = cfg.mp3_codec_rec.vbr_quality;
    lame_rec.vbr_min_bitrate = cfg.mp3_codec_rec.vbr_min_bitrate;
    lame_rec.vbr_max_bitrate = cfg.mp3_codec_rec.vbr_max_bitrate;
    lame_enc_reinit(&lame_rec);

    vorbis_stream.channel = cfg.audio.channel;
    vorbis_stream.bitrate = cfg.audio.bitrate;
    vorbis_stream.samplerate = cfg.audio.samplerate;
    vorbis_stream.bitrate_mode = cfg.vorbis_codec_stream.bitrate_mode;
    vorbis_stream.vbr_quality = 1-(cfg.vorbis_codec_stream.vbr_quality*0.1);
    vorbis_stream.vbr_min_bitrate = cfg.vorbis_codec_stream.vbr_min_bitrate;
    vorbis_stream.vbr_max_bitrate = cfg.vorbis_codec_stream.vbr_max_bitrate;
    vorbis_enc_reinit(&vorbis_stream);

    vorbis_rec.channel = cfg.audio.channel;
    vorbis_rec.bitrate = cfg.rec.bitrate;
    vorbis_rec.samplerate = cfg.audio.samplerate;
    vorbis_rec.bitrate_mode = cfg.vorbis_codec_rec.bitrate_mode;
    vorbis_rec.vbr_quality = 1-(cfg.vorbis_codec_rec.vbr_quality*0.1);
    vorbis_rec.vbr_min_bitrate = cfg.vorbis_codec_rec.vbr_min_bitrate;
    vorbis_rec.vbr_max_bitrate = cfg.vorbis_codec_rec.vbr_max_bitrate;
    vorbis_enc_reinit(&vorbis_rec);
    
    opus_stream.channel = cfg.audio.channel;
    opus_stream.bitrate = cfg.audio.bitrate*1000;
    opus_stream.samplerate = cfg.audio.samplerate;
    opus_stream.bitrate_mode = cfg.opus_codec_stream.bitrate_mode;
    opus_stream.audio_type = cfg.opus_codec_stream.audio_type;
    opus_stream.quality = cfg.opus_codec_stream.quality;
    opus_stream.bandwidth = cfg.opus_codec_stream.bandwidth;
    opus_enc_reinit(&opus_stream);
    
    opus_rec.channel = cfg.audio.channel;
    opus_rec.bitrate = cfg.rec.bitrate*1000;
    opus_rec.samplerate = cfg.audio.samplerate;
    opus_rec.bitrate_mode = cfg.opus_codec_rec.bitrate_mode;
    opus_rec.audio_type = cfg.opus_codec_rec.audio_type;
    opus_rec.quality = cfg.opus_codec_rec.quality;
    opus_rec.bandwidth = cfg.opus_codec_rec.bandwidth;
    opus_enc_reinit(&opus_rec);

#ifdef HAVE_LIBFDK_AAC
    if (g_aac_lib_available == 1)
    {
        aac_stream.channel = cfg.audio.channel;
        aac_stream.bitrate = cfg.audio.bitrate;
        aac_stream.samplerate = cfg.audio.samplerate;
        aac_stream.bitrate_mode = cfg.aac_codec_stream.bitrate_mode;
        aac_stream.profile = cfg.aac_codec_stream.profile;
        aac_stream.afterburner = cfg.aac_codec_stream.afterburner == 0 ? 1 : 0;
        aac_enc_reinit(&aac_stream);
        
        aac_rec.channel = cfg.audio.channel;
        aac_rec.bitrate = cfg.rec.bitrate;
        aac_rec.samplerate = cfg.audio.samplerate;
        aac_rec.bitrate_mode = cfg.aac_codec_rec.bitrate_mode;
        aac_rec.profile = cfg.aac_codec_rec.profile;
        aac_rec.afterburner = cfg.aac_codec_rec.afterburner == 0 ? 1 : 0;
        aac_enc_reinit(&aac_rec);
    }
#endif
    
    flac_stream.channel = cfg.audio.channel;
    flac_stream.samplerate = cfg.audio.samplerate;
    flac_stream.enc_type = FLAC_ENC_TYPE_STREAM;
    flac_enc_reinit(&flac_stream);

    flac_rec.channel = cfg.audio.channel;
    flac_rec.samplerate = cfg.audio.samplerate;
    flac_rec.enc_type = FLAC_ENC_TYPE_REC;
    flac_enc_reinit(&flac_rec);
}


char *ask_user_msg = NULL;
char *ask_user_hash = NULL;
int ask_user_has_clicked = 0;
int ask_user_answer = 0;

void ask_user_reset(void)
{
    if (ask_user_msg != NULL)
    {
        free(ask_user_msg);
        ask_user_msg = NULL;
    }
    if (ask_user_hash != NULL)
    {
        free(ask_user_hash);
        ask_user_hash = NULL;
    }
    ask_user_has_clicked = 0;
}

int ask_user_get_has_clicked(void)
{
    return ask_user_has_clicked;
}

int ask_user_get_answer(void)
{
    return ask_user_answer;
}

void ask_user_set_msg(char *msg)
{
    int len = strlen(msg);
    ask_user_msg = (char*)calloc(len+1, sizeof(char));
    strncpy(ask_user_msg, msg, len);
}

void ask_user_set_hash(char *hash)
{
    int len = strlen(hash);
    ask_user_hash = (char*)calloc(len+1, sizeof(char));
    strncpy(ask_user_hash, hash, len);
}

void ask_user_ask(void)
{
    if (fl_choice("%s", _("TRUST"), _("No"), NULL, ask_user_msg) == 1)
    { // No
        ask_user_answer = IC_ABORT;
    }
    else
    { // TRUST
	int len = strlen(ask_user_hash);
        cfg.srv[cfg.selected_srv]->cert_hash =
            (char*)realloc(cfg.srv[cfg.selected_srv]->cert_hash, len+1);
        
        memset(cfg.srv[cfg.selected_srv]->cert_hash, 0, len+1);
        strncpy(cfg.srv[cfg.selected_srv]->cert_hash, ask_user_hash, len);
        ask_user_answer = IC_RETRY;
    }
    
    ask_user_has_clicked = 1;
}
