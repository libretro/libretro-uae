#ifndef PUAE_REGISTRY_H
#define PUAE_REGISTRY_H

extern int getregmode (void);
extern int reginitializeinit (const char *path);
extern void regstatus (void);

extern int regsetstr (const char *name, const char *str);
extern int regsetint (const char *name, int val);
extern int regqueryint (const char *name, int *val);
extern int regquerystr (const char *name, char *str, int *size);

extern int regdelete (const char *name);
extern void regdeletetree (const char *name);

extern int regexists (const char *name);
extern int regexiststree (const char *name);

extern int regquerydatasize (const char *name, int *size);
extern int regsetdata (const char *name, const void *str, int size);
extern int regquerydata (const char *name, void *str, int *size);

extern int regenumstr (int idx, char *name, int *nsize, char *str, int *size);

#endif // PUAE_REGISTRY_H
