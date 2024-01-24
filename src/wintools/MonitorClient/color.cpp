// Author:      SFr frydaysoft@gmx.de

#include "stdafx.h"
#include "color.h"

ColorList colList[COLLIST_MAX]=
{
    {"DEF_BACKGROUND",DEF_BACKGROUND},
    {"DEF_TEXT",DEF_TEXT},
    {"DEF_SELBACKGROUND",DEF_SELBACKGROUND},
    {"DEF_SELTEXT",DEF_SELTEXT},
    {"DEF_DESELBACKGROUND",DEF_DESELBACKGROUND},
    {"DEF_DESELTEXT",DEF_DESELTEXT},
    {"BLACK",BLACK},
    {"BLUE",BLUE},
    {"GREEN",GREEN},
    {"CYAN",CYAN},
    {"RED",RED},
    {"MANGENTA",MANGENTA},
    {"YELLOW",YELLOW},
    {"WHITE",WHITE},
    {"DARKBLUE",DARKBLUE},
    {"DARKGREEN",DARKGREEN},
    {"BLUEGREEN",BLUEGREEN},
    {"BROWN",BROWN},
    {"PURPUR",PURPUR},
    {"OLIV",OLIV},
    {"DARKGRAY",DARKGRAY},
    {"LIGHTGRAY",LIGHTGRAY},
    {"LIGHTBLUE",LIGHTBLUE},
    {"DARKWHITE",DARKWHITE},
    {"MIDDLEGRAY",MIDDLEGRAY}
};

COLORREF GetColorRef(ITEM_COLOR lParam)	
{
	    switch (lParam)
	    {
            case DEF_BACKGROUND:        return ::GetSysColor(COLOR_WINDOW);               break;
            case DEF_TEXT:              return ::GetSysColor(COLOR_WINDOWTEXT);           break;
            case DEF_SELBACKGROUND:     return ::GetSysColor(COLOR_ACTIVECAPTION);        break;
            case DEF_SELTEXT:           return ::GetSysColor(COLOR_HIGHLIGHTTEXT);        break;
            case DEF_DESELBACKGROUND:   return ::GetSysColor(COLOR_INACTIVECAPTION);      break;
            case DEF_DESELTEXT:         return ::GetSysColor(COLOR_INACTIVECAPTIONTEXT ); break;
		    case BLACK:         return RGB(0,0,0);      break; //black
		    case BLUE:          return RGB(0,0,255);    break; //blue
		    case GREEN:         return RGB(0,255,0);    break; //green
		    case CYAN:          return RGB(0,255,255);  break; //cyan
		    case RED:           return RGB(255,0,0);    break; //red
		    case MANGENTA:      return RGB(255,0,255);  break; //mangenta
		    case YELLOW:        return RGB(255,255,0);  break; //yellow
		    case WHITE:         return RGB(255,255,255);break; //white
		    case DARKBLUE:      return RGB(0,0,128);    break; //dark-blue
		    case DARKGREEN:     return RGB(0,128,0);    break; //dark-green
		    case BLUEGREEN:     return RGB(0,128,128);  break; //blue-green
		    case BROWN:         return RGB(128,0,0);    break; //brown
		    case PURPUR:        return RGB(128,0,128);  break; //purpur
		    case OLIV:          return RGB(128,128,0);  break; //oliv
		    case DARKGRAY:      return RGB(128,128,128);break; //dark-gray
		    case LIGHTGRAY:     return RGB(192,192,192);break; //light-gray
		    case LIGHTBLUE:     return RGB(166,202,240);break; //light-blue
		    case DARKWHITE:     return RGB(255,251,240);break; //dark-white
		    case MIDDLEGRAY:    return RGB(160,160,164);break; //middle-gray
		    default:            return RGB(0,128,0);    break; //dark-green
	    }
}


