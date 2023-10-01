
/**
 * @file: find_png.c
 * @brief: Finds valid png pictures recursively within a directory
 * @return The path of all the png pictures in no particular order
 * 
  */

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>   /* for printf().  man 3 printf */
#include <stdlib.h>  /* for exit().    man 3 exit   */
#include <string.h>  /* for strcat().  man strcat   */
#include "lab_png.h"
#include <errno.h>



int main(int argc, char *argv[]) 
{
   if (argc == 1) {
        fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
        exit(1);
    }

    listPngInDir(argv[1]);
    return 0;
}

int listPngInDir(const char *dir)
{
    int count = 0;
    DIR *p_dir;
    struct dirent *p_dirent;
    char str[64];
    if ((p_dir = opendir(dir)) == NULL) {
        sprintf(str, "opendir(%s)", dir);
        perror(str);
        exit(2);
    }
    char path[256];
    while ((p_dirent = readdir(p_dir)) != NULL) {
        strcpy(path,dir);
        strcat(path, "/");
        strcat(path, p_dirent->d_name);
        char *str_path = p_dirent->d_name;  /* relative path name! */
        struct stat buf; /*String buffer*/
        if (str_path == NULL) {
            fprintf(stderr,"Null pointer found!"); 
            exit(3);
        } else {
            if (fstat(path, &buf) < 0) {
                perror("lstat error");
                continue;
            }  
            if      (S_ISREG(buf.st_mode))
            {
                /*regular*/
                if (is_png(path))
                {
                    /*TODO: ALSO check if it is crc safe*/
                    printf("%s\n", path);
                    count++;
                }
            }
            else if (S_ISDIR(buf.st_mode)){
                /*directory*/
                /*Run this function recursively*/
                count += listPngInDir(path);
            }
            
        }
    }

    if ( closedir(p_dir) != 0 ) {
        perror("closedir");
        exit(3);
    }
    return count;
}
