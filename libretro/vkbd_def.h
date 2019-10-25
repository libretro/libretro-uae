#ifndef VKBD_DEF_H
#define VKBD_DEF_H

#include "keyboard.h"

typedef struct {
	char norml[NLETT];
	char shift[NLETT];
	int val;	
} Mvk;

Mvk MVk[NPLGN * NLIGN * 2] = {

   // 0 PG1
   { "Esc","Esc",AK_ESC },
   { "F1" ,"F1" ,AK_F1 },
   { "F2" ,"F2" ,AK_F2 },
   { "F3" ,"F3" ,AK_F3 },
   { "F4" ,"F4" ,AK_F4 },
   { "F5" ,"F5" ,AK_F5 },
   { "F6" ,"F6" ,AK_F6 },
   { "F7" ,"F7" ,AK_F7 },
   { "F8" ,"F8" ,AK_F8 },
   { "F9" ,"F9" ,AK_F9 },
   { "F10","F10",AK_F10 },

   // 11
   { "`"  ,"~"  ,AK_BACKQUOTE },
   { "1"  ,"!"  ,AK_1 },
   { "2"  ,"@"  ,AK_2 },
   { "3"  ,"#"  ,AK_3 },
   { "4"  ,"$"  ,AK_4 },
   { "5"  ,"Prc",AK_5 },
   { "6"  ,"^"  ,AK_6 },
   { "7"  ,"&"  ,AK_7 },
   { "8"  ,"*"  ,AK_8 },
   { "9"  ,"("  ,AK_9 },
   { "0"  ,")"  ,AK_0 },

   // 22
   { "Tab","Tab",AK_TAB },
   { "q"  ,"Q"  ,AK_Q },
   { "w"  ,"W"  ,AK_W },
   { "e"  ,"E"  ,AK_E },
   { "r"  ,"R"  ,AK_R },
   { "t"  ,"T"  ,AK_T },
   { "y"  ,"Y"  ,AK_Y },
   { "u"  ,"U"  ,AK_U },
   { "i"  ,"I"  ,AK_I },
   { "o"  ,"O"  ,AK_O },
   { "p"  ,"P"  ,AK_P },

   // 33
   { "Ctrl","Ctrl" ,AK_CTRL },
   { "a"  ,"A"  ,AK_A },
   { "s"  ,"S"  ,AK_S },
   { "d"  ,"D"  ,AK_D },
   { "f"  ,"F"  ,AK_F },
   { "g"  ,"G"  ,AK_G },
   { "h"  ,"H"  ,AK_H },
   { "j"  ,"J"  ,AK_J },
   { "k"  ,"K"  ,AK_K },
   { "l"  ,"L"  ,AK_L },
   { ";"  ,":"  ,AK_SEMICOLON },

   // 44
   { "CapsLock","CapsLock" ,AK_CAPSLOCK },
   { "z"  ,"Z"  ,AK_Z },
   { "x"  ,"X"  ,AK_X },
   { "c"  ,"C"  ,AK_C },
   { "v"  ,"V"  ,AK_V },
   { "b"  ,"B"  ,AK_B },
   { "n"  ,"N"  ,AK_N },
   { "m"  ,"M"  ,AK_M },
   { ","  ,"<"  ,AK_COMMA },
   { "."  ,">"  ,AK_PERIOD },
   { "/"  ,"?"  ,AK_SLASH },

   // 55
   { "LShift","LShift" ,AK_LSH },
   { "-"  ,"_"  ,AK_MINUS },
   { "="  ,"+"  ,AK_EQUAL },
   { "\\" ,"|"  ,AK_BACKSLASH },
   { "["  ,"{"  ,AK_LBRACKET },
   { "]"  ,"}"  ,AK_RBRACKET },
   { "'"  ,"\"" ,AK_QUOTE },
   { "RShift","RShift" ,AK_RSH },
   { "Del","Del",AK_DEL },
   { "Up" ,"Up" ,AK_UP },
   { "Help","Help" ,AK_HELP },

   // 66
   { "LAlt","LAlt",AK_LALT },
   { "LAmiga","LAmiga",AK_LAMI },
   { "Space","Space",AK_SPC },
   { "RAmiga","RAmiga",AK_RAMI },
   { "RAlt","RAlt",AK_RALT },
   { "NUMPAD","NUMPAD",-2 },
   { "<-" ,"<-" ,AK_BS },
   { "Return","Return",AK_RET },
   { "Left","Left",AK_LF },
   { "Down","Down",AK_DN },
   { "Right","Right",AK_RT },



   // 0 PG2
   { "Esc","Esc",AK_ESC },
   { "F1" ,"F1" ,AK_F1 },
   { "F2" ,"F2" ,AK_F2 },
   { "F3" ,"F3" ,AK_F3 },
   { "F4" ,"F4" ,AK_F4 },
   { "F5" ,"F5" ,AK_F5 },
   { "F6" ,"F6" ,AK_F6 },
   { "F7" ,"F7" ,AK_F7 },
   { "F8" ,"F8" ,AK_F8 },
   { "F9" ,"F9" ,AK_F9 },
   { "F10","F10",AK_F10 },

   // 11
   { "`"  ,"~"   ,AK_BACKQUOTE },
   { "NP1","End" ,AK_NP1 },
   { "NP2","Down",AK_NP2 },
   { "NP3","PgDn",AK_NP3 },
   { "NP4","Left",AK_NP4 },
   { "NP5"," "   ,AK_NP5 },
   { "NP6","Right",AK_NP6 },
   { "NP7","Home",AK_NP7 },
   { "NP8","Up"  ,AK_NP8 },
   { "NP9","PgUp",AK_NP9 },
   { "NP0","Ins" ,AK_NP0 },

   // 22
   { "Tab","Tab",AK_TAB },
   { "q"  ,"Q"  ,AK_Q },
   { "w"  ,"W"  ,AK_W },
   { "e"  ,"E"  ,AK_E },
   { "r"  ,"R"  ,AK_R },
   { "t"  ,"T"  ,AK_T },
   { "y"  ,"Y"  ,AK_Y },
   { "u"  ,"U"  ,AK_U },
   { "i"  ,"I"  ,AK_I },
   { "o"  ,"O"  ,AK_O },
   { "p"  ,"P"  ,AK_P },

   // 33
   { "Ctrl","Ctrl" ,AK_CTRL },
   { "a"  ,"A"  ,AK_A },
   { "s"  ,"S"  ,AK_S },
   { "d"  ,"D"  ,AK_D },
   { "f"  ,"F"  ,AK_F },
   { "g"  ,"G"  ,AK_G },
   { "h"  ,"H"  ,AK_H },
   { "j"  ,"J"  ,AK_J },
   { "k"  ,"K"  ,AK_K },
   { "l"  ,"L"  ,AK_L },
   { ";"  ,":"  ,AK_SEMICOLON },

   // 44
   { "CapsLock","CapsLock" ,AK_CAPSLOCK },
   { "z"  ,"Z"  ,AK_Z },
   { "x"  ,"X"  ,AK_X },
   { "c"  ,"C"  ,AK_C },
   { "v"  ,"V"  ,AK_V },
   { "b"  ,"B"  ,AK_B },
   { "n"  ,"N"  ,AK_N },
   { "m"  ,"M"  ,AK_M },
   { ","  ,"<"  ,AK_COMMA },
   { "."  ,">"  ,AK_PERIOD },
   { "/"  ,"?"  ,AK_SLASH },

   // 55
   { "LShift","LShift" ,AK_LSH },
   { "NP-","NP-",AK_NPSUB },
   { "NP+","NP+",AK_NPADD },
   { "NP/","NP/",AK_NPDIV },
   { "NP(","NP(",AK_NPLPAREN },
   { "NP)","NP)",AK_NPRPAREN },
   { "NP*","NP*",AK_NPMUL },
   { "RShift","RShift",AK_RSH },
   { "NPDel","NPDel",AK_NPDEL },
   { "Up" ,"Up" ,AK_UP },
   { "Help","Help" ,AK_HELP },

   // 66
   { "LAlt","LAlt",AK_LALT },
   { "LAmiga","LAmiga",AK_LAMI },
   { "Space","Space",AK_SPC },
   { "RAmiga","RAmiga",AK_RAMI },
   { "RAlt","RAlt",AK_RALT },
   { "NUMPAD","NUMPAD",-2 },
   { "<-" ,"<-" ,AK_BS },
   { "Enter","Enter",AK_ENT },
   { "Left","Left",AK_LF },
   { "Down","Down",AK_DN },
   { "Right","Right",AK_RT },

};

#endif
