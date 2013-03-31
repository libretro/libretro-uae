#ifndef GRAPH_H
#define GRAPH_H 1

#define VIRTUAL_WIDTH 640

typedef struct
{
	int x, y;
	int dx,dy;    	
} box;

extern void printch(unsigned short *buffer,int x, int y, unsigned  short  couleur,unsigned char ch,int taille,int pl,int zde);

extern void textpixel(unsigned  short *buffer,int x,int y,unsigned  short  color,int tail,int plein,int zdep, char *string,...);

extern void textCLpixel(unsigned  short  *buffer,int lim,int x,int x2,int y,unsigned  short color,int tail,int plein,int zdep,char *string,...);

extern void textCpixel(unsigned  short *buffer,int x,int x2,int y,unsigned  short  color,int tail,int plein,int zdep,char *string,...);

extern void DrawFBoxBmp(unsigned  short  *buffer,int x,int y,int dx,int dy,unsigned  short color);

extern void DrawBoxBmp(unsigned  short  *buffer,int x,int y,int dx,int dy,unsigned  short  color);

extern void DrawHlineBmp(unsigned  short  *buffer,int x,int y,int dx,int dy,unsigned  short  color);

extern void DrawVlineBmp(unsigned  short *buffer,int x,int y,int dx,int dy,unsigned  short  color);

extern void DrawlineBmp(unsigned  short  *buffer,int x1,int y1,int x2,int y2,unsigned  short  color);

extern void DrawBox(unsigned  short  *buf,box b,char t[],unsigned  short  color);

extern void DrawBoxF(unsigned  short  *buf,box b,char t[],unsigned short  color,unsigned  short  border);

extern void DrawCircle(unsigned short *buf,int x, int y, int radius,unsigned short rgba,int full);

extern void Draw_string(unsigned short *surf, signed short int x, signed short int y, const unsigned char *string, unsigned  short int maxstrlen, unsigned  short int xscale, unsigned  short int yscale,unsigned  short int fg, unsigned  short int bg);

extern void Draw_text(unsigned  short *buffer,int x,int y,unsigned  short  fgcol,unsigned  short int bgcol ,int scalex,int scaley , int max,char *string,...);


#endif

