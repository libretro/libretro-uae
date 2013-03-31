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

#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "inputdevice.h"
#include "hotkeys.h"


static struct uae_hotkeyseq *hotkey_table;


void set_default_hotkeys (struct uae_hotkeyseq *keys)
{
    hotkey_table = keys;
    reset_hotkeys ();
}

void reset_hotkeys (void) {
    struct uae_hotkeyseq *seq = hotkey_table;
    int i;

    if (!seq)
	return;

    while (seq->event) {
	for (i=0; i < HOTKEYSEQ_MAXKEYS; i++)
	    seq->key_pressed[i] = 0;
	seq->complete = 0;
	seq++;
    }
}

int match_hotkey_sequence (int key, int state)
{
    struct uae_hotkeyseq *seq = hotkey_table;
    int event = 0;

    if (!seq)
    	return 0;

    if (state) {
	/* Key down */
	while (seq->event != 0) {
	    int i;
	    /* Check whether key is the next key in this sequence */
	    if (key == seq->keyseq[0])
		seq->key_pressed[0] = 1;
	    for (i=1; i < HOTKEYSEQ_MAXKEYS; i++) {
	        if ((seq->key_pressed[i-1] == 1) && (key == seq->keyseq[i])) {
		    seq->key_pressed[i] = 1;
		    /* Was it the final key in this sequence ? */
		    if ((i+1 == HOTKEYSEQ_MAXKEYS) || (seq->keyseq[i+1] == (uae_u16)-1)) {
		        /* Yes. Return the event */
		        event = seq->event;
		        seq->complete = 1;
			break;
		    }
		}
	    }
	    seq++;
	}
    } else {
	/* key up */
	while (seq->event != 0) {
	    int i, j;
	    for (i = 0; i < HOTKEYSEQ_MAXKEYS; i++) {
	        if ((key == seq->keyseq[i]) && (seq->key_pressed[i] == 1)) {
		    /* letting up a key - cancels any keys following it
		     * in the sequence that are pressed */
		    for (j=i; j < HOTKEYSEQ_MAXKEYS; j++)
		        seq->key_pressed[j] = 0;
		    /* if it was previously complete generate return the event
		     * (so that it can be used as a key-up event */
		    if (seq->complete) {
		        seq->complete = 0;
		        event = seq->event;
		    }
		}
	    }
	    seq++;
	}
    }
    return event;
}
