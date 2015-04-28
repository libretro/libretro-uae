#ifndef VKBD_DEF_H
#define VKBD_DEF_H

#include "keyboard.h"

typedef struct {
	char norml[NLETT];
	char shift[NLETT];
	int val;	
} Mvk;

Mvk MVk[NPLGN*NLIGN*2]= {

   { "PG2","PG2" ,-2}, //0
   { "Esc","Esc" ,1 },
   { " q" ," Q"  ,AK_Q},
   { " w" ," W"  ,AK_W},
   { " e" ," E"  ,AK_E},
   { " r" ," R"  ,AK_R},
   { " t" ," T"  ,AK_T},
   { " y" ," Y"  ,AK_Y},
   { " u"," U"   ,AK_U},
   { " i"," I"   ,AK_I},

   { " o"," O"   ,AK_O}, //10
   { " p" ," P"  ,AK_P},
   { " a" ," A"  ,AK_A},
   { " s" ," S"  ,AK_S},
   { " d" ," D"  ,AK_D},
   { " f" ," F"  ,AK_F},
   { " g" ," G"  ,AK_G},
   { " h" ," H"  ,AK_H},
   { " j" ," J"  ,AK_J},
   { " k" ," K"  ,AK_K},

   { " l" ," L"  ,AK_L},//20
   { " z" ," Z"  ,AK_Z},
   { " x" ," X"  ,AK_X},
   { " c" ," C"  ,AK_C},
   { " v" ," V"  ,AK_V},
   { " b" ," B"  ,AK_B},
   { " n" ," N"  ,AK_N},
   { " m"," M"   ,AK_M},
   { "F1" ,"F1"  ,AK_F1},
   { "F2" ,"F2"  ,AK_F2},

   { "F3" ,"F3"  ,AK_F3},//30
   { "F4" ,"F4"  ,AK_F4},
   { "F5" ,"F5"  ,AK_F5},
   { "F6" ,"F6"  ,AK_F6},
   { "F7" ,"F7"  ,AK_F7},
   { "F8" ,"F8"  ,AK_F8},	
   { "F9" ,"F9"  ,AK_F9},
   { "F10","F10" ,AK_F10},
   { "F11","F11" ,-1},
   { "F12","F12" ,-1},

   { " 1" ," !" , AK_1},//40
   { " 2" ," \"" ,AK_2 },
   { " 3" ," 3"  ,AK_3 },
   { " 4" ," $"  ,AK_4 },
   { " 5" ," %"  ,AK_5 },
   { " 6" ," ^"  ,AK_6 },
   { " 7" ," &"  ,AK_7 },
   { " 8" ," *"  ,AK_8 },
   { " 9" ," ("  ,AK_9},
   { " 0" ," )"  ,AK_0},

   { "PG2","PG2" ,-2}, //50
   { " `" ," ` " ,-1}, 
   { " -" ," _" , AK_MINUS},
   { " =" ," +" , AK_EQUAL},
   { "Bks","Bks" ,AK_BS},
   { "Scr","Scr" ,-1/*AKS_SCREENSHOT*/},
   { "Pse","Pse" ,AKS_PAUSE},
   { "Ins","Ins" ,AK_HELP},
   { "Hme","Hme" ,AK_NPLPAREN},
   { "PgU","PgU" ,AK_RAMI},

   { "Tab" ,"Tab",AK_TAB}, //60	
   { " [" ,"  {" ,AK_LBRACKET},
   { " ]" ,"  }" ,AK_RBRACKET},
   { "Ent" ,"Ent",AK_RET},
   { "Del" ,"Del",AK_DEL},
   { "End" ,"End",AK_NPRPAREN},
   { "PgD" ,"PgD",AK_LAMI},
   { "SHF" ,"SHF",AK_LSH},
   { " \\" ," |" ,AK_BACKSLASH},	
   { " Dw" ," Dw",AK_DN},	

   { "Cps","Cps" ,AK_CAPSLOCK}, //70
   { " ;"," :"   ,AK_SEMICOLON},
   { " '"," @"   ,AK_QUOTE},
   { " #"," ~"   ,43},
   { "Ctr" ,"Ctr",AK_CTRL},
   { "Alt" ,"Alt",AK_LALT},
   { "Spc" ,"Spc",AK_SPC},
   { "Lft"," 4"  ,AK_LF},
   { " Up"," 8"  ,AK_UP},
   { "Rgt" ," 6" ,AK_RT},

   { " ,"," <"   ,AK_COMMA},//80
   { " ."," >"   ,AK_PERIOD},
   { " /"," ?"   ,AK_SLASH},
   { " |" ," ^"  ,54},
   { " Num"," Num",69},
   { " /" ," /"  ,AK_NPDIV},
   { " *" ," *"  ,AK_NPMUL},
   { " -" ," -"  ,AK_NPSUB},
   { " Hme" ," 7",AK_NPLPAREN},
   { " ^"," Y"   ,72},

   { " PgU"," U" ,AK_RAMI},//90
   { " +"," I"   ,AK_NPADD},
   { " 4","Lft"  ,AK_NP4},
   { " 5" ," 5"  ,AK_NP5},
   { " 6" ,"Rgt" ,AK_NP6},
   { " 1" ,"End" ,AK_NP1},
   { "Njoy","Njoy",-5},
   { "Col" ,"Col",-3},
   { "Ent" ,"Ent",AK_RET},
   { "Kbd" ,"Kbd",-4},

} ;

#endif
