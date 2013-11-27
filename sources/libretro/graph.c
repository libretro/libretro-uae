#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "graph.h"

extern unsigned char  msx[];

void printch(unsigned short *buffer,int x, int y, unsigned  short  couleur,unsigned char ch,int taille,int pl,int zde)
{          
	int i,j;
	unsigned char *font;
        int rectx,recty;
    
  	font=&msx[(int)ch * 8];
  	
	for(i=0;i<8;i++,font++)
	{
	     for(j=0;j<8;j++)
	     {
         	if ((*font &(128>>j)))
         	{
			rectx = x+(j<<0);
                 	if(taille==1)recty = y+(i<<1);
                 	else recty = y+(i<<0);
                
		 	buffer[recty* VIRTUAL_WIDTH  + rectx] = couleur; 
	             	if(pl==1) buffer[(recty+1)* VIRTUAL_WIDTH  + rectx] = couleur; 				

            	}
	     }
        }
}

void textpixel(unsigned  short *buffer,int x,int y,unsigned  short  color,int tail,int plein,int zdep, char *string,...)
{
   int boucle=0;  
   char	text[256];	   	
   va_list	ap;			
   
   if (string == NULL)return;		
		
   va_start(ap, string);		
      vsprintf(text, string, ap);	
   va_end(ap);	
   
   while(text[boucle]!=0){
     printch(buffer,x,y,color,text[boucle],tail,plein,zdep);
     boucle++;x+=8;//6;
   }
	
}

void textCLpixel(unsigned  short  *buffer,int lim,int x,int x2,int y,unsigned  short color,int tail,int plein,int zdep,char *string,...)
{
   int boucle=0;  
   char	text[256];	   	
   va_list	ap;			
   
   if (string == NULL)return;		
		
   va_start(ap, string);		
      vsprintf(text, string, ap);	
   va_end(ap);
   	
   while(text[boucle]!=0 && boucle<lim)boucle++;   
   boucle=(x2-x)/2 -(boucle*3);
   x=boucle;
   
   boucle=0;
   while(text[boucle]!=0 && boucle<lim){
     printch(buffer,x,y,color,text[boucle],tail,plein,zdep);
     boucle++;x+=8;//6;
   }
	
}

void textCpixel(unsigned  short *buffer,int x,int x2,int y,unsigned  short  color,int tail,int plein,int zdep,char *string,...)
{
   int boucle=0;  
   char	text[256];	   	
   va_list	ap;			
   
   if (string == NULL)return;		
		
   va_start(ap, string);		
      vsprintf(text, string, ap);	
   va_end(ap);
   	
   while(text[boucle]!=0)boucle++;   
   boucle=(x2-x)/2 -(boucle*3);
   x=boucle;
   
   boucle=0;
   while(text[boucle]!=0){
     printch(buffer,x,y,color,text[boucle],tail,plein,zdep);
     boucle++;x+=8;//6;
   }
	
}

void DrawFBoxBmp(unsigned  short  *buffer,int x,int y,int dx,int dy,unsigned  short color){
	
	int i,j,idx;
		
	for(i=x;i<x+dx;i++){
		for(j=y;j<y+dy;j++){
			
			idx=i+j*VIRTUAL_WIDTH;
			buffer[idx]=color;	
		}
	}
	
}

void DrawBoxBmp(unsigned  short  *buffer,int x,int y,int dx,int dy,unsigned  short  color){
	
	int i,j,idx;
		
	for(i=x;i<x+dx;i++){
		idx=i+y*VIRTUAL_WIDTH;
		buffer[idx]=color;
		idx=i+(y+dy)*VIRTUAL_WIDTH;
		buffer[idx]=color;
	}

	for(j=y;j<y+dy;j++){
			
		idx=x+j*VIRTUAL_WIDTH;
		buffer[idx]=color;	
		idx=(x+dx)+j*VIRTUAL_WIDTH;
		buffer[idx]=color;	
	}
	
}


void DrawHlineBmp(unsigned  short  *buffer,int x,int y,int dx,int dy,unsigned  short  color){
	
	int i,j,idx;
		
	for(i=x;i<x+dx;i++){
		idx=i+y*VIRTUAL_WIDTH;
		buffer[idx]=color;		
	}
}

void DrawVlineBmp(unsigned  short *buffer,int x,int y,int dx,int dy,unsigned  short  color){
	
	int i,j,idx;

	for(j=y;j<y+dy;j++){			
		idx=x+j*VIRTUAL_WIDTH;
		buffer[idx]=color;		
	}	
}

void DrawlineBmp(unsigned  short  *buffer,int x1,int y1,int x2,int y2,unsigned  short  color){
		
	int pixx, pixy;
 	int x, y;
 	int dx, dy; 	
 	int sx, sy;
 	int swaptmp;
 	int idx;

	dx = x2 - x1;
	dy = y2 - y1;
	sx = (dx >= 0) ? 1 : -1;
	sy = (dy >= 0) ? 1 : -1;

	if (dx==0) {
 		if (dy>0) {
 			DrawVlineBmp(buffer, x1, y1,0, dy, color);
			return;

 		} else if (dy<0) {
 			DrawVlineBmp(buffer, x1, y2,0, -dy, color);
			return;

 		} else {
			idx=x1+y1*VIRTUAL_WIDTH;
 			buffer[idx]=color;
			return ;
 		}
 	}
 	if (dy == 0) {
 		if (dx>0) {
 			DrawHlineBmp(buffer, x1, y1, dx, 0, color);
			return;

 		} else if (dx<0) {
 			DrawHlineBmp(buffer, x2, y1, -dx,0, color);
			return;
 		}
 	}

	dx = sx * dx + 1;
 	dy = sy * dy + 1;
	
	pixx = 1;
 	pixy = VIRTUAL_WIDTH;

 	pixx *= sx;
 	pixy *= sy;

 	if (dx < dy) {
	 	swaptmp = dx;
	 	dx = dy;
	 	dy = swaptmp;
	 	swaptmp = pixx;
	 	pixx = pixy;
	 	pixy = swaptmp;
 	}

	x = 0;
 	y = 0;

	idx=x1+y1*VIRTUAL_WIDTH;

	for (; x < dx; x++, idx +=pixx) {
		buffer[idx]=color;
 		y += dy;
 		if (y >= dx) {
 			y -= dx;
 			idx += pixy;
 		}
	}

}

void DrawBox(unsigned  short  *buf,box b,char t[],unsigned  short  color){

	DrawBoxBmp(buf,b.x,b.y,b.dx,b.dy,color); 	
	textCpixel(buf,b.x, 3*b.x + b.dx ,b.y+2,color,1,1,4,"%s",t);

}

void DrawBoxF(unsigned  short  *buf,box b,char t[],unsigned short  color,unsigned  short  border){

	int ydec=b.y+(b.dy/2)-4;

	if(ydec<b.y+2)ydec=b.y+2;

	DrawBoxBmp(buf,b.x,b.y,b.dx,b.dy,border);
	DrawFBoxBmp(buf,b.x+1,b.y+1,b.dx-2,b.dy-2,color);
 	
	textCpixel(buf,b.x, 3*b.x + b.dx , ydec ,border,1,1,4,"%s",t);

}


const float DEG2RAD = 3.14159/180;

void DrawCircle(unsigned short *buf,int x, int y, int radius,unsigned short rgba,int full)
{ 
	int i;
	float degInRad; 
	int x1,y1;

   	for ( i=0; i < 360; i++){

		degInRad = i*DEG2RAD;
   		x1=x+cos(degInRad)*radius;
		y1=y+sin(degInRad)*radius;

		if(full)DrawlineBmp(buf,x,y, x1,y1,rgba); 
		else {
			buf[x1+y1*VIRTUAL_WIDTH]=rgba;
		}
     		
     	}
    	
}

//UINT16
void filter_scale2x(unsigned char *srcPtr, unsigned srcPitch, 
                      unsigned char *dstPtr, unsigned dstPitch,
		      int width, int height)
{
	unsigned int nextlineSrc = srcPitch / sizeof(short);
	short *p = (short *)srcPtr;

	unsigned int nextlineDst = dstPitch / sizeof(short);
	short *q = (short *)dstPtr;
  
	while(height--) {
		int i = 0, j = 0;
		for(i = 0; i < width; ++i, j += 2) {
			short B = *(p + i - nextlineSrc);
			short D = *(p + i - 1);
			short E = *(p + i);
			short F = *(p + i + 1);
			short H = *(p + i + nextlineSrc);

			*(q + j) = D == B && B != F && D != H ? D : E;
			*(q + j + 1) = B == F && B != D && F != H ? F : E;
			*(q + j + nextlineDst) = D == H && D != B && H != F ? D : E;
			*(q + j + nextlineDst + 1) = H == F && D != H && B != F ? F : E;
		}
		p += nextlineSrc;
		q += nextlineDst << 1;
	}
}
 
#include "font2.c"

void Draw_string(unsigned short *surf, signed short int x, signed short int y, const unsigned char *string,unsigned short maxstrlen,unsigned short xscale, unsigned short yscale, unsigned short fg, unsigned short bg)
{
    	int k,strlen;
    	unsigned char *linesurf;
    	signed short int ypixel;
    	unsigned short *yptr; 
    	int col, bit;
    	unsigned char b;

    	int xrepeat, yrepeat;

    	if(string==NULL)return;
    	for(strlen = 0; strlen<maxstrlen && string[strlen]; strlen++) {}

	int surfw=strlen * 7 * xscale;
	int surfh=8 * yscale;
	
        linesurf =malloc(sizeof(unsigned short)*surfw*surfh );

    	yptr = (unsigned short *)&linesurf[0];
    
	for(ypixel = 0; ypixel<8; ypixel++) {

        	for(col=0; col<strlen; col++) {
//            		b = font_array[string[col]^0x80][ypixel];
            		b = font_array[(string[col]^0x80)*8 + ypixel];

            		for(bit=0; bit<7; bit++, yptr++) {              
				*yptr = (b & (1<<(7-bit))) ? fg : bg;
                		for(xrepeat = 1; xrepeat < xscale; xrepeat++, yptr++)
                    			yptr[1] = *yptr;
                        }
        	}

        	for(yrepeat = 1; yrepeat < yscale; yrepeat++) 
            		for(xrepeat = 0; xrepeat<surfw; xrepeat++, yptr++)
                		*yptr = yptr[-surfw];
           
    	}

    	yptr = (unsigned short*)&linesurf[0];

    	for(yrepeat = y; yrepeat < y+ surfh; yrepeat++) 
        	for(xrepeat = x; xrepeat< x+surfw; xrepeat++,yptr++)
             		if(*yptr!=0)surf[xrepeat+yrepeat*VIRTUAL_WIDTH] = *yptr;

	free(linesurf);

}


void Draw_text(unsigned  short *buffer,int x,int y,unsigned  short  fgcol,unsigned  short int bgcol ,int scalex,int scaley , int max,char *string,...)
{
	int boucle=0;  
   	char text[256];	   	
   	va_list	ap;			
   
   	if (string == NULL)return;		
		
   	va_start(ap, string);		
      		vsprintf(text, string, ap);	
   	va_end(ap);	

   	Draw_string(buffer, x,y, text,max, scalex, scaley,fgcol,bgcol);	
}

