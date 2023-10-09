/*
@parameters a list of valid png pictures of the same width, but varying height
@brief: Concatenates png images vertically based on the number at the end
        Uses decompressed (zlib) png images, adds them, then compresses them all in one go.
        There is a check bit involved.
@return: a composed png image, all.png
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>   /* for printf().  man 3 printf */
#include <stdlib.h>  /* for exit().    man 3 exit   */
#include <unistd.h>
#include <dirent.h>
#include <string.h>  /* for strcat().  man strcat   */
#include "crc.h"      /* for crc()                   */
#include "zutil.h"    /* for mem_def() and mem_inf() */
#include "lab_png.h"
#include <errno.h>
int main(int argc, char **argv)
{
    //Step 1: obtain the full length of the data section
    int length = 0; //length of image in pixels
    U8 *width = malloc(4);
    int height = 0;
    
    for(int i = 1; i < argc; i++)
    {
        char* str_file = argv[i];
        FILE *fp = fopen(str_file, "rb");
        //printf("Step2\n");
        struct simple_PNG png;
        struct simple_PNG *ptr_png = &png;
        struct chunk ihdr;
        struct chunk idat;
        struct chunk iend;
        ptr_png->p_IHDR = &ihdr;
        ptr_png->p_IDAT = &idat;
        ptr_png->p_IEND = &iend;
        printf("Make chunks\n");
        get_chunks(ptr_png, fp);
        struct data_IHDR ihdr_data;
        get_IHDR_data(ihdr.p_data, &ihdr_data);
        width = png.p_IHDR->p_data;
        height += ihdr_data.height;
        printf("Chunk %d: %s is height %d", i, str_file, height);
        length += ptr_png->p_IDAT->length;
    }

    //Step 2: allocate the full length array and get pixel data
    U8 all[length * 32];
    int all_len = 0;
    for(int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "rb");
        struct simple_PNG png;
        struct chunk ihdr;
        struct chunk idat;
        struct chunk iend;
        png.p_IHDR = &ihdr;
        png.p_IDAT = &idat;
        png.p_IEND = &iend;
        get_chunks(&png, fp);
        U64 len_inf = 0;
        U8 inflated[png.p_IDAT->length * 512]; //dunno why x 512
        mem_inf(inflated, &len_inf , png.p_IDAT->p_data, png.p_IDAT->length);
        memcpy(all + all_len, inflated, len_inf);
        all_len += len_inf;
    }
    U64 len_def;
    U8 *deflated = malloc(length);
    mem_def(deflated, &len_def, all, all_len, Z_DEFAULT_COMPRESSION);

    FILE *fa = fopen("all.png","wb");

    U64 *sig = 0x0a1a0a0d474e5089;
    U32 *ihdr_len = 0x0d000000;

    U8 *ihdr_head = malloc(4);
    ihdr_head[0] = 0x49; //I
    ihdr_head[1] = 0x48; //H
    ihdr_head[2] = 0x44; //D
    ihdr_head[3] = 0x52; //R

    U8 *ihdr_info = malloc(4);
    ihdr_info[0] = 0x08; //Bit depth
    ihdr_info[1] = 0x06;
    ihdr_info[2] = 0x0;
    ihdr_info[3] = 0x0;
    ihdr_info[4] = 0x0;

    U8 *ihdr_height = malloc(4);
    ihdr_height[0] = (height >> 24) & 0xFF;
    ihdr_height[1] = (height >> 16) & 0xFF;
    ihdr_height[2] = (height >> 8) & 0xFF;
    ihdr_height[3] = height & 0xFF;

    U8 *buf = malloc(17);
    memcpy(buf, ihdr_head, 4);
    memcpy(buf + 4, width, 4);
    memcpy(buf + 8, ihdr_height, 4);
    memcpy(buf + 12, ihdr_info, 5);

    U32 c = crc(buf, 17);
    U32 *c_crc = ntohl(c);

    fwrite(&sig, 8, 1, fa);
    fwrite(&ihdr_len, 4, 1, fa);
    fwrite(buf, 17, 1, fa);
    fwrite(&c_crc, 4, 1, fa);

    U32 *idat_type = 0x54414449;
    U8 *idat_type_2 = malloc(4);
    idat_type_2[0] = 0x49;
    idat_type_2[1] = 0x44;
    idat_type_2[2] = 0x41;
    idat_type_2[3] = 0x54;
    U8 *idat_buf = malloc(length + 4);
    memcpy(idat_buf, idat_type_2, 4);
    memcpy(idat_buf + 4, deflated, length);
    U32 *c_def_len = ntohl(len_def);
    c = crc(idat_buf, len_def + 4);
    c_crc = ntohl(c);

    fwrite(&c_def_len, 4, 1, fa);
    fwrite(&idat_type, 4, 1, fa);
    fwrite(deflated, len_def, 1, fa);
    fwrite(&c_crc, 4, 1, fa);

    U32 *iend_len = 0x0;
    U64 *iend_data = 0x826042ae444e4549;
    fwrite(&iend_len, 4, 1, fa);
    fwrite(&iend_data, 8, 1, fa);

    fclose(fa);

    return 1;

}