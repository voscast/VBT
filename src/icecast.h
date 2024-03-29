// icecast functions for VBT
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

#ifndef ICECAST_H
#define ICECAST_H

enum {
    IC_OK = 0,
    IC_RETRY = 1,
    IC_ABORT = 2,
    IC_ASK = 3,
};

int ic_init(void);
int ic_connect(void);
int ic_send(char *buf, int buf_len);
int ic_update_song(char *song_name);
void ic_disconnect(void);

#endif
