// socket functions for VBT
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

#ifndef SOCKFUNCS_H
#define SOCKFUNCS_H

enum {
    READ = 0,
    WRITE = 1,
    RW = 2
};

enum {
    SOCK_ERR_CREATE = -1,
    SOCK_ERR_RESOLVE = -2,
    SOCK_TIMEOUT = -3,
    SOCK_INVALID = -4,
    SOCK_NO_MODE = -5,
    SOCK_ERR_SET_SBUF = -6,
    SOCK_ERR_SET_RBUF = -7,
    SOCK_ERR_BIND = -8,
    SOCK_ERR_LISTEN = -9
};

enum {
    CONN_TIMEOUT = 1000,
    SEND_TIMEOUT = 3000,
    RECV_TIMEOUT = 1000
};

void sock_init(void);
int sock_connect(const char *addr, unsigned int port, int timout_ms);
int sock_listen(int port, int *listen_sock);
int sock_setbufsize(int s, int send_size, int recv_size);
int sock_isdisconnected(int s);
int sock_send(int s, const char *buf, int len, int timout_ms);
int sock_recv(int s, char *buf, int len, int timout_ms);
int sock_select(int s, int timout_ms, int mode);
int sock_nonblock(int s);
int sock_block(int s);
int sock_isvalid(int s);
void sock_close(int s);
void sock_fdinit(int s);
void sock_fdclr(int s);
void sock_fdzero(void);


#endif

