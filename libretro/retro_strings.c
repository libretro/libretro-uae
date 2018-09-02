/* Copyright (C) 2018 
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "retro_strings.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Note: This function returns a pointer to a substring_left of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char* trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

// Returns a substring of 'str' that contains the 'len' leftmost characters of 'str'.
char* strleft(const char* str, int len)
{
	char* result = calloc(len + 1, sizeof(char));
	strncpy(result, str, len);
	return result;
}

// Returns a substring of 'str' that contains the 'len' rightmost characters of 'str'.
char* strright(const char* str, int len)
{
	int pos = strlen(str) - len;
	char* result = calloc(len + 1, sizeof(char));
	strncpy(result, str + pos, len);
	return result;
}

// Returns true if 'str' starts with 'start'
bool strstartswith(const char* str, const char* start)
{
	if (strlen(str) >= strlen(start))
		if(!strncasecmp(str, start, strlen(start)))
			return true;
		
	return false;
}

// Returns true if 'str' ends with 'end'
bool strendswith(const char* str, const char* end)
{
	if (strlen(str) >= strlen(end))
		if(!strcasecmp((char*)&str[strlen(str)-strlen(end)], end))
			return true;
		
	return false;
}
