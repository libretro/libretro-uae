#ifndef VKBD_DEF_H
#define VKBD_DEF_H

#include "keyboard.h"

typedef struct {
	char norml[NLETT];
	char shift[NLETT];
	int val;	
} Mvk;

Mvk MVk[NPLGN * NLIGN * 2] = {

   // -11
   { "LMB" ,"LMB" ,-15 },
   { "RMB" ,"RMB" ,-16 },
   { {'M','O',30},{'M','O',30},-11 },
   { {'M','O',28},{'M','O',28},-12 },
   { {'M','O',27},{'M','O',27},-13 },
   { {'M','O',29},{'M','O',29},-14 },
   { "J/M" ,"J/M" ,-18 },
   { "TRBF","TRBF",-19 },
   { "ASPR","ASPR",-22 },
   { "STBR","STBR",-21 },
   { "RST" ,"RST" ,-20 },

   // 0 PG1
   { "Esc" ,"Esc" ,AK_ESC },
   { "F1"  ,"F1"  ,AK_F1 },
   { "F2"  ,"F2"  ,AK_F2 },
   { "F3"  ,"F3"  ,AK_F3 },
   { "F4"  ,"F4"  ,AK_F4 },
   { "F5"  ,"F5"  ,AK_F5 },
   { "F6"  ,"F6"  ,AK_F6 },
   { "F7"  ,"F7"  ,AK_F7 },
   { "F8"  ,"F8"  ,AK_F8 },
   { "F9"  ,"F9"  ,AK_F9 },
   { "F10" ,"F10" ,AK_F10 },

   // 11
   { "`"   ,"~"   ,AK_BACKQUOTE },
   { "1"   ,"!"   ,AK_1 },
   { "2"   ,"@"   ,AK_2 },
   { "3"   ,"#"   ,AK_3 },
   { "4"   ,"$"   ,AK_4 },
   { "5"   ,"%%"  ,AK_5 },
   { "6"   ,"^"   ,AK_6 },
   { "7"   ,"&"   ,AK_7 },
   { "8"   ,"*"   ,AK_8 },
   { "9"   ,"("   ,AK_9 },
   { "0"   ,")"   ,AK_0 },

   // 22
   { "Tab" ,"Tab" ,AK_TAB },
   { "q"   ,"Q"   ,AK_Q },
   { "w"   ,"W"   ,AK_W },
   { "e"   ,"E"   ,AK_E },
   { "r"   ,"R"   ,AK_R },
   { "t"   ,"T"   ,AK_T },
   { "y"   ,"Y"   ,AK_Y },
   { "u"   ,"U"   ,AK_U },
   { "i"   ,"I"   ,AK_I },
   { "o"   ,"O"   ,AK_O },
   { "p"   ,"P"   ,AK_P },

   // 33
   { "Ctrl","Ctrl",AK_CTRL },
   { "a"   ,"A"   ,AK_A },
   { "s"   ,"S"   ,AK_S },
   { "d"   ,"D"   ,AK_D },
   { "f"   ,"F"   ,AK_F },
   { "g"   ,"G"   ,AK_G },
   { "h"   ,"H"   ,AK_H },
   { "j"   ,"J"   ,AK_J },
   { "k"   ,"K"   ,AK_K },
   { "l"   ,"L"   ,AK_L },
   { ";"   ,":"   ,AK_SEMICOLON },

   // 44
   { "CapsLock","CapsLock",AK_CAPSLOCK },
   { "z"   ,"Z"   ,AK_Z },
   { "x"   ,"X"   ,AK_X },
   { "c"   ,"C"   ,AK_C },
   { "v"   ,"V"   ,AK_V },
   { "b"   ,"B"   ,AK_B },
   { "n"   ,"N"   ,AK_N },
   { "m"   ,"M"   ,AK_M },
   { ","   ,"<"   ,AK_COMMA },
   { "."   ,">"   ,AK_PERIOD },
   { "/"   ,"?"   ,AK_SLASH },

   // 55
   { "LSh" ,"LSh" ,AK_LSH },
   { "-"   ,"_"   ,AK_MINUS },
   { "="   ,"+"   ,AK_EQUAL },
   { "\\"  ,"|"   ,AK_BACKSLASH },
   { "["   ,"{"   ,AK_LBRACKET },
   { "]"   ,"}"   ,AK_RBRACKET },
   { "'"   ,"\""  ,AK_QUOTE },
   { "RSh" ,"RSh" ,AK_RSH },
   { "Del" ,"Del" ,AK_DEL },
   { {30}  ,{30}  ,AK_UP },
   { "Help","Help",AK_HELP },

   // 66
   { "LAlt","LAlt",AK_LALT },
   { "LAmi","LAmi",AK_LAMI },
   { "Spc" ,"Spc" ,AK_SPC },
   { "RAmi","RAmi",AK_RAMI },
   { "RAlt","RAlt",AK_RALT },
   { "NPAD","NPAD",-2 },
   { "<-"  ,"<-"  ,AK_BS },
   { "Rtrn","Rtrn",AK_RET },
   { {27}  ,{27}  ,AK_LF },
   { {28}  ,{28}  ,AK_DN },
   { {29}  ,{29}  ,AK_RT },



   // -11
   { "LMB" ,"LMB" ,-15 },
   { "RMB" ,"RMB" ,-16 },
   { {'M','O',30},{'M','O',30},-11 },
   { {'M','O',28},{'M','O',28},-12 },
   { {'M','O',27},{'M','O',27},-13 },
   { {'M','O',29},{'M','O',29},-14 },
   { "J/M" ,"J/M" ,-18 },
   { "TRBF","TRBF",-19 },
   { "ASPR","ASPR",-22 },
   { "STBR","STBR",-21 },
   { "RST" ,"RST" ,-20 },

   // 0 PG2
   { "Esc" ,"Esc" ,AK_ESC },
   { "F1"  ,"F1"  ,AK_F1 },
   { "F2"  ,"F2"  ,AK_F2 },
   { "F3"  ,"F3"  ,AK_F3 },
   { "F4"  ,"F4"  ,AK_F4 },
   { "F5"  ,"F5"  ,AK_F5 },
   { "F6"  ,"F6"  ,AK_F6 },
   { "F7"  ,"F7"  ,AK_F7 },
   { "F8"  ,"F8"  ,AK_F8 },
   { "F9"  ,"F9"  ,AK_F9 },
   { "F10" ,"F10" ,AK_F10 },

   // 11
   { "`"   ,"~"   ,AK_BACKQUOTE },
   { "NP1" ,"End" ,AK_NP1 },
   { "NP2" ,{'N','P',28},AK_NP2 },
   { "NP3" ,"PgDn",AK_NP3 },
   { "NP4" ,{'N','P',27},AK_NP4 },
   { "NP5" ," "   ,AK_NP5 },
   { "NP6" ,{'N','P',29},AK_NP6 },
   { "NP7" ,"Home",AK_NP7 },
   { "NP8" ,{'N','P',30},AK_NP8 },
   { "NP9" ,"PgUp",AK_NP9 },
   { "NP0" ,"Ins" ,AK_NP0 },

   // 22
   { "Tab" ,"Tab" ,AK_TAB },
   { "q"   ,"Q"   ,AK_Q },
   { "w"   ,"W"   ,AK_W },
   { "e"   ,"E"   ,AK_E },
   { "r"   ,"R"   ,AK_R },
   { "t"   ,"T"   ,AK_T },
   { "y"   ,"Y"   ,AK_Y },
   { "u"   ,"U"   ,AK_U },
   { "i"   ,"I"   ,AK_I },
   { "o"   ,"O"   ,AK_O },
   { "p"   ,"P"   ,AK_P },

   // 33
   { "Ctrl","Ctrl",AK_CTRL },
   { "a"   ,"A"   ,AK_A },
   { "s"   ,"S"   ,AK_S },
   { "d"   ,"D"   ,AK_D },
   { "f"   ,"F"   ,AK_F },
   { "g"   ,"G"   ,AK_G },
   { "h"   ,"H"   ,AK_H },
   { "j"   ,"J"   ,AK_J },
   { "k"   ,"K"   ,AK_K },
   { "l"   ,"L"   ,AK_L },
   { ";"   ,":"   ,AK_SEMICOLON },

   // 44
   { "CapsLock","CapsLock",AK_CAPSLOCK },
   { "z"   ,"Z"   ,AK_Z },
   { "x"   ,"X"   ,AK_X },
   { "c"   ,"C"   ,AK_C },
   { "v"   ,"V"   ,AK_V },
   { "b"   ,"B"   ,AK_B },
   { "n"   ,"N"   ,AK_N },
   { "m"   ,"M"   ,AK_M },
   { ","   ,"<"   ,AK_COMMA },
   { "."   ,">"   ,AK_PERIOD },
   { "/"   ,"?"   ,AK_SLASH },

   // 55
   { "LSh" ,"LSh" ,AK_LSH },
   { "NP-" ,"NP-" ,AK_NPSUB },
   { "NP+" ,"NP+" ,AK_NPADD },
   { "NP/" ,"NP/" ,AK_NPDIV },
   { "NP(" ,"NP(" ,AK_NPLPAREN },
   { "NP)" ,"NP)" ,AK_NPRPAREN },
   { "NP*" ,"NP*" ,AK_NPMUL },
   { "RSh" ,"RSh" ,AK_RSH },
   { "NPDl","NPDl",AK_NPDEL },
   { {'N','P',30} ,{'N','P',30},AK_NP8 },
   { "Help","Help",AK_HELP },

   // 66
   { "LAlt","LAlt",AK_LALT },
   { "LAmi","LAmi",AK_LAMI },
   { "Spc" ,"Spc" ,AK_SPC },
   { "RAmi","RAmi",AK_RAMI },
   { "RAlt","RAlt",AK_RALT },
   { "NPAD","NPAD",-2 },
   { "<-"  ,"<-"  ,AK_BS },
   { "Entr","Entr",AK_ENT },
   { {'N','P',27} ,{'N','P',27},AK_NP4 },
   { {'N','P',28} ,{'N','P',28},AK_NP2 },
   { {'N','P',29} ,{'N','P',29},AK_NP6 },

};

#endif
