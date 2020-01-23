#ifndef SRC_INCLUDE_AKIKO_H_INCLUDED
#define SRC_INCLUDE_AKIKO_H_INCLUDED 1

#define AKIKO_BASE 0xb80000
#define AKIKO_BASE_END 0xb80040

extern void akiko_reset (void);
extern int akiko_init (void);
extern void akiko_free (void);

extern void AKIKO_hsync_handler (void);
extern void akiko_mute (int);

extern uae_u8 *extendedkickmemory;

extern void rethink_akiko (void);

#endif
