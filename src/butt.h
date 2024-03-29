// VBT - Vos Broadcast Tool
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

#ifndef BUTT_H
#define BUTT_H

#include "config.h"
#include "timer.h"
#include "lame_encode.h"
#include "vorbis_encode.h"
#include "opus_encode.h"
#include "flac_encode.h"
#include "aac_encode.h"

extern bool record;
extern bool recording;      //TRUE if VBT is recording
extern bool connected;      //TRUE if VBT is connected to server
extern bool disconnect;     //TRUE if VBT should disconnect
extern bool try_connect;    //but will try to connect to a server while TRUE
extern bool streaming;
extern bool song_timeout_running; //TRUE if automatic song updating is running
extern bool app_timeout_running;  //TRUE if automatic song updating from some app is running


extern int stream_socket;
extern double kbytes_sent;
extern double kbytes_written;

extern unsigned int record_start_hour; // the hour when last recording started

extern sec_timer rec_timer;
extern sec_timer stream_timer;

extern lame_enc lame_stream;
extern lame_enc lame_rec;
extern vorbis_enc vorbis_stream;
extern vorbis_enc vorbis_rec;
extern opus_enc opus_stream;
extern opus_enc opus_rec;
extern flac_enc flac_rec;
extern flac_enc flac_stream;
#ifdef HAVE_LIBFDK_AAC
 extern aac_enc aac_stream;
 extern aac_enc aac_rec;
#endif

#endif
