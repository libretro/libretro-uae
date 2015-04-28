#include "libretro-glue.h"
#include "graph.h"

#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

int ndsknum=0;
static int entries; 

extern unsigned short int bmp[TEX_WIDTH * TEX_HEIGHT];

#ifdef __CELLOS_LV2__
#define S_ISDIR(x) (x & CELL_FS_S_IFDIR)
#endif

#define PATHSEP '/'

int alphasort2(const struct dirent **d1, const struct dirent **d2)
{
   const struct dirent *c1;
   const struct dirent *c2;
   c1 = *d1;
   c2 = *d2;
   return strcmp(c1->d_name, c2->d_name);
}

int scandir(const char *dirp, struct dirent ***namelist,
      int (*filter)(const struct dirent *),
      int (*compar)(const struct dirent **, const struct dirent **))
{
   DIR *dp = opendir (dirp);
   struct dirent *current;
   struct dirent **names = NULL;
   size_t names_size = 0, pos;
   int save;

   (void)save;

   if (dp == NULL)
      return -1;

   save  = errno;
   errno = 0;
   pos   = 0;

   while ((current = readdir (dp)) != NULL)
      if (filter == NULL || (*filter) (current))
      {
         struct dirent *vnew;
         size_t dsize;

         if (pos == names_size)
         {
            struct dirent **new;
            if (names_size == 0)
               names_size = 10;
            else
               names_size *= 2;
            new = (struct dirent **) realloc (names, names_size * sizeof (struct dirent *));
            if (new == NULL)
               break;
            names = new;
         }

         dsize = &current->d_name[strlen(current->d_name)+1] - (char *) current;
         vnew = (struct dirent *) malloc (dsize);
         if (vnew == NULL)
            break;

         names[pos++] = (struct dirent *) memcpy (vnew, current, dsize);
      }

   if (errno != 0)
   {
      save = errno;
      closedir (dp);
      while (pos > 0)
         free (names[--pos]);
      free (names);

      return -1;
   }

   closedir (dp);


   /* Sort the list if we have a comparison function to sort with.  */
   if (compar != NULL)
      qsort (names, pos, sizeof (struct dirent *), compar);
   *namelist = names;
   return pos;
}


/*-----------------------------------------------------------------------*/
/**
 * Free file entries
 */
static struct dirent **files_free(struct dirent **files)
{
   int i;
   if (files != NULL)
   {
      for(i=0; i<entries; i++)
         free(files[i]);
      free(files);
   }
   return NULL;
}

void File_HandleDotDirs(char *path)
{
   int len = strlen(path);
   if (len >= 2 &&
         path[len-2] == PATHSEP &&
         path[len-1] == '.')
   {
      /* keep in same dir */
      path[len-1] = '\0';
   }
   else if (len >= 3 &&
         path[len-3] == PATHSEP &&
         path[len-2] == '.' &&
         path[len-1] == '.')
   {
      /* go one dir up */
      if (len == 3) {
         path[1] = 0;		/* already root */
      } else {
         char *ptr;
         path[len-3] = 0;
         ptr = strrchr(path, PATHSEP);
         if (ptr)
            *(ptr+1) = 0;
      }
   }
}

bool File_DirExists(const char *pathdir)
{
	struct stat buf;
	return (stat(pathdir, &buf) == 0 && S_ISDIR(buf.st_mode));
}

int HandleExtension(char *path,char *ext)
{
   int len = strlen(path);

   if (len >= 4 &&
         path[len-4] == '.' &&
         path[len-3] == ext[0] &&
         path[len-2] == ext[1] &&
         path[len-1] == ext[2])
   {
      return 1;
   }

   return 0;
}

//basic filebrowser
#define MAXSEL 10

char *filebrowser(const char *path_and_name)
{
   static char seldsk[512];	 
   static char path[FILENAME_MAX];                 /* The actual path names */
   static struct dirent **files = NULL;
   static bool reloaddir = true;
   static int first=0;

   int i,LIMSEL=MAXSEL;
   static int firstin=0,fselect=0,select=0,pselect=0;
   static int padcountdown = 0;
   static int pad_held_down = 0;

   (void)firstin;

   if(first==0)
   {
      entries = 0;		

      if (path_and_name && path_and_name[0])
         sprintf(path,"%s",path_and_name);			

      first++;
   }

   if (reloaddir)
   {
      files = files_free(files);
      entries = scandir(path, &files, 0, alphasort2);

      if (entries < 0)
      {
         fprintf(stderr, "Path not found. :(\n");
         first=0;			

         return "EMPTY";
      }

      reloaddir = false;

   }

   if(entries==0)
      return "EMPTY";

   ndsknum=entries;

   if(ndsknum>=MAXSEL)
      LIMSEL=MAXSEL;
   else
      LIMSEL=ndsknum;

   if(padcountdown)
      padcountdown--;

   i = update_input_gui();

   switch(i)
   {
      case -1 : //UP
         if(padcountdown==0)
         {
            select--;
            if(select<0)
               select=0;

            if(fselect>0)
               fselect--;

            if(pad_held_down++<4)
               padcountdown=10;
            else
               padcountdown = 2;

            if(select<pselect)pselect--;
         }
         break;
      case  1 : /* DOWN */
         if(padcountdown==0)
         {
            select++;
            if(select>ndsknum-1)
               select=ndsknum-1;
            if(fselect<LIMSEL-1)
               fselect++;

            if(pad_held_down++<4)
               padcountdown=10;
            else
               padcountdown=2;

            if(select> (pselect+LIMSEL-1) )pselect++;
         }

         break;
      case -10: //LEFT
         //to do page UP
         break;
      case  10: //RIGHT
         //to do page DOWN
         break;
      case  2 : //OK
         {

            char *tempstr = malloc(FILENAME_MAX);
            sprintf(tempstr,"%s%s",path,files[select]->d_name);

            if ( File_DirExists(tempstr) == 1 || \
                  strcmp(files[select]->d_name,".")==0 || \
                  strcmp(files[select]->d_name,"..")==0 ) {


               if(strcmp(files[select]->d_name,".")==0 || strcmp(files[select]->d_name,"..")==0 )
                  File_HandleDotDirs(tempstr);
               else strcat(tempstr, "/"/*PATHSEP*/);

               sprintf(path, "%s",tempstr);				
               reloaddir = true;

               free(tempstr);

               select= fselect = pselect = 0;

               return "NO CHOICE";
            }

            free(tempstr);

            sprintf(seldsk,"%s%s",path,files[select]->d_name);

            //FIXME

            if( HandleExtension(files[select]->d_name,"DSK") ||\
                  HandleExtension(files[select]->d_name,"dsk") ||\
                  HandleExtension(files[select]->d_name,"SNA") ||\
                  HandleExtension(files[select]->d_name,"sna") )
               return (char*)seldsk;
            return "NO CHOICE";
         }
         break;
      case  3 : //CANCEL
         return "EMPTY";
         break;
      default : 
         break;
   }

   if(i!=-1 && i!=1)
      pad_held_down = 0;

   DrawFBoxBmp(bmp,CROP_WIDTH/4,CROP_HEIGHT/10,CROP_WIDTH/2,CROP_HEIGHT-20,RGB565(3,3,3));
   DrawFBoxBmp(bmp,2+CROP_WIDTH/4,1+CROP_HEIGHT/10,-4+CROP_WIDTH/2,-2+CROP_HEIGHT-20,RGB565(22,23,26));

   for(i = pselect; i < (pselect + LIMSEL); i++)
   {
      Draw_text(bmp,
            20 + CROP_WIDTH / 4,
            30 + (i-pselect) * 16,
            RGB565((fselect == (i-pselect)) ? 25 : 5, 5, 5),
            0x0, 1, 1, 40,
            "%s",
            files[i]->d_name);
   }

   return "NO CHOICE";
}


