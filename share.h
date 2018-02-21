//
// Created by fof_z on 2/20/2018.
//

#ifndef SHARE_H
#define SHARE_H

enum state { idle, want_in, in_cs, empty, full, ready};

struct share {
    int  turn;
    char buf0[20];
    char buf1[20];
    char buf2[20];
    char buf3[20];
    char buf4[20];
    state bufFlag[5];
    state flag0[17];
    state flag1[17];
    state flag2[17];
    state flag3[17];
    state flag4[17];
};

#endif //SHARE_H
