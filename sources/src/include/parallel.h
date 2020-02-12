#ifndef UAE_PARALLEL_H
#define UAE_PARALLEL_H

extern int isprinter (void);
extern void doprinter (uae_u8);
extern void flushprinter (void);
extern void closeprinter (void);
extern int isprinteropen (void);
extern void initparallel (void);
extern int paraport_init (void);

#endif /* UAE_PARALLEL_H */
