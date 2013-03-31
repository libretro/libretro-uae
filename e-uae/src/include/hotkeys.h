 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Target-independent support for parsing hotkey sequences
  *
  * Copyright 2004 Richard Drummond
  *
  * Based on the hotkey handling previously found
  * in x11win.c and sdlgfx.c
  */


/* Maximum number of keys in a sequence */
#define HOTKEYSEQ_MAXKEYS	4

struct uae_hotkeyseq
{
    uae_u16 keyseq [HOTKEYSEQ_MAXKEYS];		/* the keycodes in this sequence */
    uae_u16 key_pressed [HOTKEYSEQ_MAXKEYS];	/* a record of which have been pressed */
    int complete;                               /* true when all keys in this sequence are pressed */
    int event;					/* the event this sequence generates */
};

#define MAKE_HOTKEYSEQ(key1,key2,key3,key4,event) \
   {(key1), (key2), (key3), (key4)}, {0, 0, 0, 0}, 0, (event)

/* The last entry in a hotkey list must be marked */
#define HOTKEYS_END MAKE_HOTKEYSEQ (0,0,0,0,0)

extern void reset_hotkeys (void);

extern void set_default_hotkeys (struct uae_hotkeyseq *keys);

extern int match_hotkey_sequence (int key, int state);

static inline void handle_hotkey_event (int event, int state)
{
    handle_input_event (event, state, 0, 0);
}
