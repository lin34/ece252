
/**
 * @file: find_png.c
 * @brief: Finds valid png pictures recursively within a directory
 * @return The path of all the png pictures in no particular order
 *
 */
#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>   /* for printf().  man 3 printf */
#include <stdlib.h>  /* for exit().    man 3 exit   */
#include <unistd.h>
#include <dirent.h>
#include <string.h>  /* for strcat().  man strcat   */

#include "lab_png.h"
#include <errno.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
        exit(1);
    }
    int count = 0;
    count = listPngInDir(argv[1]);
    printf("%d", count);
    if(count == 0)
    {
        printf("findpng: No PNG file found");
    }
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

    while ((p_dirent = readdir(p_dir)) != NULL) {
        char *str_path = p_dirent->d_name;  /* relative path name! */
        char path[255];
        if (str_path == NULL) {
            fprintf(stderr,"Null pointer found!"); 
            exit(3);
        } else {
            // printf("%s\n", str_path);
            struct stat buf;
            strcpy(path, dir);
            strcat(path, "/");
            strcat(path, str_path);
            printf("%s\n", path);
            int statNum = lstat(path, &buf);
            if      (statNum == 0 && S_ISREG(buf.st_mode))
            {
                if(is_png(path)){
                    count++;
                    printf("%s\n", path);
                }
            }
            else if (S_ISDIR(buf.st_mode))
            {
                if (strcmp(str_path, ".") != 0 && strcmp(str_path, "..") != 0) {

                    count += listPngInDir(path);
                }
            }
        }
    }

    if ( closedir(p_dir) != 0 ) {
        perror("closedir");
        exit(3);
    }

    return 0;
}
