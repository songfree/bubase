// Author:      SFr frydaysoft@gmx.de

#ifndef AFX_COLOR_H__9FC7D36B_FC88_42FD_8DAA_2F70016038BA__INCLUDED_
#define AFX_COLOR_H__9FC7D36B_FC88_42FD_8DAA_2F70016038BA__INCLUDED_

    #include "windows.h"

    typedef enum 
    {
        DEF_BACKGROUND=0, DEF_TEXT, DEF_SELBACKGROUND, DEF_SELTEXT, DEF_DESELBACKGROUND, DEF_DESELTEXT,
        BLACK,BLUE,GREEN,CYAN,RED,
        MANGENTA,YELLOW,WHITE,DARKBLUE,DARKGREEN,
        BLUEGREEN,BROWN,PURPUR,OLIV,DARKGRAY,
        LIGHTGRAY,LIGHTBLUE,DARKWHITE,MIDDLEGRAY
    }
    ITEM_COLOR;

    typedef struct 
    {
        char        str[50];
        ITEM_COLOR      col;
    }ColorList;

    #define COLLIST_MAX 25

    extern ColorList colList[COLLIST_MAX];

    COLORREF GetColorRef(ITEM_COLOR lParam);

#endif