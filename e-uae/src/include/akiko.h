

#define AKIKO_BASE 0xb80000
#define AKIKO_BASE_END 0xb80100 /* ?? */

extern void akiko_reset (void);
extern int akiko_init (void);
extern void akiko_free (void);
extern int cd32_enabled;

extern void akiko_entergui (void);
extern void akiko_exitgui (void);
extern void AKIKO_hsync_handler (void);

extern uae_u32 akiko_lget (uaecptr addr) REGPARAM;
extern uae_u32 akiko_wget (uaecptr addr) REGPARAM;
extern uae_u32 akiko_bget (uaecptr addr) REGPARAM;
extern void akiko_bput (uaecptr addr, uae_u32 value) REGPARAM;
extern void akiko_wput (uaecptr addr, uae_u32 value) REGPARAM;
extern void akiko_lput (uaecptr addr, uae_u32 value) REGPARAM;


extern const uae_u8 *restore_akiko (const uae_u8 *src);
extern       uae_u8 *save_akiko    (uae_u32 *len);
