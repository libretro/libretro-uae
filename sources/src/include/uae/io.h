#ifndef UAE_IO_H
#define UAE_IO_H

#include "uae/types.h"
#include <stdio.h>

#ifdef WINUAE
#define uae_tfopen _tfopen
#else
#define uae_tfopen _tfopen
#endif

#endif /* UAE_IO_H */
