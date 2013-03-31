#ifndef BE_WINDOW_H
#define BE_WINDOW_H
/***********************************************************/
//  UAE - The Un*x Amiga Emulator
//
//  BeOS port - graphics stuff
//
//  (c) 2000-2001 Axel Doerfler
//  (c) 1999 Be/R4 Sound - Raphael Moll
//  (c) 1998-1999 David Sowsy
//  (c) 1996-1998 Christian Bauer
//  (c) 1996 Patrick Hanevold
//
/***********************************************************/

#include <AppKit.h>
#include <InterfaceKit.h>
#include <DirectWindow.h>

#include <stdio.h>
#include <stdlib.h>


const uint32 kMsgRedraw = 'draw';
const uint32 kMsgToggleFullScreen = 'full';
const uint32 kMsgFullSizeWindow = 'w100';
const uint32 kMsgHalfSizeWindow = 'w050';
const uint32 kMsgThirdSizeWindow = 'w033';
const uint32 kMsgQuit = 'quit';
const uint32 kMsgAbout = 'abut';
const uint32 kMsgReset = 'rset';

const uint8 kWindowNone = 0;
const uint8 kWindowBitmap = 1;
const uint8 kWindowSingleLine = 2;
const uint8 kWindowFullScreen = 4;

const int MAX_CLIP_LIST_COUNT = 64;


// Global variables
extern UAEWindow *gWindow;
extern uint8 gWindowMode;
extern int	 inhibit_frame;


class BitmapView;

/*
 *  The window in which the Amiga graphics are displayed, handles I/O
 */

class UAEWindow : public BDirectWindow
{
	public:
		UAEWindow(BRect frame,bool useBitmap);

		bool QuitRequested(void);
		void MessageReceived(BMessage *msg);
		void FrameResized(float width,float height);

		void DirectConnected(direct_buffer_info *info);
		void UpdateBufferInfo(direct_buffer_info *info);
		void UnlockBuffer();
		uint8 *LockBuffer();

		void InitColors(color_space colorspace);
		void DrawBlock(int yMin,int yMax);
		void DrawLine(int y);
		void SetFullScreenMode(bool full);
		void UpdateMouse();

	private:
		BitmapView *fBitmapView;
		BBitmap	*fBitmap;
		bool	fIsConnected;
		int32	fAcquireFailed;

		sem_id	fDrawingLock;

		clipping_rect fWindowBounds;
		clipping_rect fClipList[MAX_CLIP_LIST_COUNT];
		uint32	fClipListCount;
		
		BPoint fScreenCentre; 
};

/*
 *  A simple view class for blitting a bitmap on the screen
 */

class BitmapView : public BView {
	public:
		BitmapView(BRect frame, BBitmap *bitmap);
		~BitmapView();

		void Draw(BRect update);
		void Pulse();
		void MouseMoved(BPoint point, uint32 transit, const BMessage *message);
		void WindowActivated(bool active);

	private:
		BBitmap *fBitmap;
};

#endif

