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
   { {10,'L'},{10,'L'},-15 },
   { {10,'R'},{10,'R'},-16 },
   { {10,30},{10,30},-11 },
   { {10,28},{10,28},-12 },
   { {10,27},{10,27},-13 },
   { {10,29},{10,29},-14 },
   { "J/M" ,"J/M" ,-18 },
   { "TRBF","TRBF",-19 },
   { "ASPR","ASPR",-22 },
   { "STBR","STBR",-21 },
   { {17}  ,{17}  ,-20 }, // Reset

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
   { {11}  ,{11}  ,AK_TAB },
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
   { {12}  ,{12}  ,AK_LSH },
   { "-"   ,"_"   ,AK_MINUS },
   { "="   ,"+"   ,AK_EQUAL },
   { "\\"  ,"|"   ,AK_BACKSLASH },
   { "["   ,"{"   ,AK_LBRACKET },
   { "]"   ,"}"   ,AK_RBRACKET },
   { "'"   ,"\""  ,AK_QUOTE },
   { {12}  ,{12}  ,AK_RSH },
   { "Del" ,"Del" ,AK_DEL },
   { {30}  ,{30}  ,AK_UP },
   { "Help","Help",AK_HELP },

   // 66
   { "Alt" ,"Alt" ,AK_LALT },
   { {14}  ,{14}  ,AK_LAMI },
   { {18}  ,{18}  ,AK_SPC },
   { {13}  ,{13}  ,AK_RAMI },
   { "Alt" ,"Alt" ,AK_RALT },
   { {15}  ,{15}  ,-2 }, // Numpad
   { {25}  ,{25}  ,AK_BS },
   { {16}  ,{16}  ,AK_RET },
   { {27}  ,{27}  ,AK_LF },
   { {28}  ,{28}  ,AK_DN },
   { {29}  ,{29}  ,AK_RT },



   // -11
   { {10,'L'},{10,'L'},-15 },
   { {10,'R'},{10,'R'},-16 },
   { {10,30},{10,30},-11 },
   { {10,28},{10,28},-12 },
   { {10,27},{10,27},-13 },
   { {10,29},{10,29},-14 },
   { "J/M" ,"J/M" ,-18 },
   { "TRBF","TRBF",-19 },
   { "ASPR","ASPR",-22 },
   { "STBR","STBR",-21 },
   { {17}  ,{17}  ,-20 }, // Reset

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
   { "`"     ,"~"    ,AK_BACKQUOTE },
   { {15,'1'},"End"  ,AK_NP1 },
   { {15,'2'},{15,28},AK_NP2 },
   { {15,'3'},"PgDn" ,AK_NP3 },
   { {15,'4'},{15,27},AK_NP4 },
   { {15,'5'}," "    ,AK_NP5 },
   { {15,'6'},{15,29},AK_NP6 },
   { {15,'7'},"Home" ,AK_NP7 },
   { {15,'8'},{15,30},AK_NP8 },
   { {15,'9'},"PgUp" ,AK_NP9 },
   { {15,'0'},"Ins"  ,AK_NP0 },

   // 22
   { {11}  ,{11}  ,AK_TAB },
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
   { {12}  ,{12}  ,AK_LSH },
   { {15,'-'},{15,'-'},AK_NPSUB },
   { {15,'+'},{15,'+'},AK_NPADD },
   { {15,'/'},{15,'/'},AK_NPDIV },
   { {15,'('},{15,'('},AK_NPLPAREN },
   { {15,')'},{15,')'},AK_NPRPAREN },
   { {15,'*'},{15,'*'},AK_NPMUL },
   { {12}  ,{12}  ,AK_RSH },
   { {15,'.'},{15,'D','e','l'},AK_NPDEL },
   { {15,30} ,{15,30},AK_NP8 },
   { "Help","Help",AK_HELP },

   // 66
   { "Alt" ,"Alt" ,AK_LALT },
   { {14}  ,{14}  ,AK_LAMI },
   { {18}  ,{18}  ,AK_SPC },
   { {13}  ,{13}  ,AK_RAMI },
   { "Alt" ,"Alt" ,AK_RALT },
   { {15}  ,{15}  ,-2 }, // Numpad
   { {25}  ,{25}  ,AK_BS },
   { {15,16},{15,16},AK_ENT },
   { {15,27},{15,27},AK_NP4 },
   { {15,28},{15,28},AK_NP2 },
   { {15,29},{15,29},AK_NP6 },

};

#endif
