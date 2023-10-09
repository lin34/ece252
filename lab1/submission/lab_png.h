/**
 * @brief  micros and structures for a simple PNG file 
 *
 * Copyright 2018-2020 Yiqing Huang
 *
 * This software may be freely redistributed under the terms of MIT License
 */
#pragma once

/******************************************************************************
 * INCLUDE HEADER FILES
 *****************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>

/******************************************************************************
 * DEFINED MACROS 
 *****************************************************************************/

#define PNG_SIG_SIZE    8 /* number of bytes of png image signature data */
#define CHUNK_LEN_SIZE  4 /* chunk length field size in bytes */          
#define CHUNK_TYPE_SIZE 4 /* chunk type field size in bytes */
#define CHUNK_CRC_SIZE  4 /* chunk CRC field size in bytes */
#define DATA_IHDR_SIZE 13 /* IHDR chunk data field size */

/******************************************************************************
 * STRUCTURES and TYPEDEFS 
 *****************************************************************************/
typedef unsigned char U8;
typedef unsigned int  U32;

typedef struct chunk {
    U32 length;  /* length of data in the chunk, host byte order */
    U8  type[4]; /* chunk type */
    U8  *p_data; /* pointer to location where the actual data are */
    U32 crc;     /* CRC field  */
} *chunk_p;

/* note that there are 13 Bytes valid data, compiler will padd 3 bytes to make
   the structure 16 Bytes due to alignment. So do not use the size of this
   structure as the actual data size, use 13 Bytes (i.e DATA_IHDR_SIZE macro).
 */
typedef struct data_IHDR {// IHDR chunk data 
    U32 width;        /* width in pixels, big endian   */
    U32 height;       /* height in pixels, big endian  */
    U8  bit_depth;    /* num of bits per sample or per palette index.
                         valid values are: 1, 2, 4, 8, 16 */
    U8  color_type;   /* =0: Grayscale; =2: Truecolor; =3 Indexed-color
                         =4: Greyscale with alpha; =6: Truecolor with alpha */
    U8  compression;  /* only method 0 is defined for now */
    U8  filter;       /* only method 0 is defined for now */
    U8  interlace;    /* =0: no interlace; =1: Adam7 interlace */
} *data_IHDR_p;

/* A simple PNG file format, three chunks only*/
typedef struct simple_PNG {
    struct chunk *p_IHDR;
    struct chunk *p_IDAT;  /* only handles one IDAT chunk */  
    struct chunk *p_IEND;
} *simple_PNG_p;

/******************************************************************************
 * FUNCTION PROTOTYPES 
 *****************************************************************************/
/*int is_png(U8 *buf, size_t n); Takes 8 bytes and checks whether it matches the PNG image file signature*/
int get_png_height(struct data_IHDR *buf);
int get_png_width(struct data_IHDR *buf);
int get_png_data_IHDR(struct data_IHDR *out, FILE *fp, long offset, int whence);

/* declare your own functions prototypes here */

int is_png(char *filePath) {
    FILE *fp = fopen(filePath, "rb");
    if (fp == NULL){
        perror("fopen");
        return errno;
    }
    U8 *buf = malloc(8);
    fread(buf, sizeof(buf), 1, fp);
    if (
        buf[0] == 0x89 &&
        buf[1] == 0x50 &&
        buf[2] == 0x4e &&
        buf[3] == 0x47 &&
        buf[4] == 0x0d &&
        buf[5] == 0x0a &&
        buf[6] == 0x1a &&
        buf[7] == 0x0a
    ) {
        free(buf);
        return 1;
    }
    free(buf);
    return 0;
}

int get_chunk(struct chunk *out, FILE *fp, long offset){
    U8 *buf = malloc(CHUNK_LEN_SIZE);
    fseek(fp, offset, SEEK_SET);
    fread(buf, sizeof(buf), 1, fp);
    //convert length from big endian to little endian
    int len = 0;
    for (int i = 0; i < 4; i++) {
        len = len * 256 + buf[i];
    }
    out->length = len;
    //printf("Length of chunk %d", len);

    fseek(fp, offset + CHUNK_LEN_SIZE, SEEK_SET);
    fread(buf, sizeof(buf), 1, fp);
    for (int i = 0; i < sizeof(buf); i++)
    {
        out->type[i] = buf[i];
    }
    printf("Out type: %s Length: %d \n", out->type, len);

    U8 *stream = malloc(len);
    fseek(fp, offset + CHUNK_LEN_SIZE + CHUNK_TYPE_SIZE, SEEK_SET);
    fread(stream, len, 1, fp);
    out->p_data = stream;

    fseek(fp, offset + 8 + len, SEEK_SET);
    fread(buf, sizeof(buf), 1, fp);
    int crc;
    for (int i = 0; i < 4; i++) {
        crc = crc * 256 + buf[i];
    }
    out->crc = crc;

    free(buf);
    return 1;
}

int get_chunks(struct simple_PNG *out, FILE *fp)
{
    printf("chunk 1");
    get_chunk(out->p_IHDR, fp, 8);
    printf("chunk 2");
    get_chunk(out->p_IDAT, fp, out->p_IHDR->length + 20);
    get_chunk(out->p_IEND, fp, out->p_IHDR->length + out->p_IDAT->length + 32);
    return 1;
}

int get_IHDR_data(U8 *ihdr_data, struct data_IHDR *out)
{
    int w, h;
    u_int32_t *buf = malloc(4);
    for(int i = 0; i < 4; i++)
    {
        w = w * 256 + ihdr_data[i];
    }

    for (int i = 4; i < 8; i++) {
        h = h * 256 + ihdr_data[i];
    }

    out->width = w;
    out->height = h;
    out->bit_depth = ihdr_data[8];
    out->color_type = ihdr_data[9];
    out->compression = ihdr_data[10];
    out->filter = ihdr_data[11];
    out->interlace = ihdr_data[12];
    return 1;
}

int validate_CRC(struct chunk *chunk)
{
    U8 *stream = malloc(CHUNK_TYPE_SIZE + chunk->length);
    for (int i = 0; i < CHUNK_TYPE_SIZE; i++)
    {
        stream[i] = chunk->type[i];
    }
    for (int j = 0; j < chunk->length; j++)
    {
        int i = j + CHUNK_TYPE_SIZE;
        stream[i] = chunk->p_data[i];
    }
    unsigned long x = crc(stream, CHUNK_TYPE_SIZE + chunk->length);
    if(x != chunk->crc)
    {
        printf("%.4s chunk CRC error: computed %04lx, expected %04x\n", chunk->type, x, chunk->crc);
        free(stream);
        return 0;
    }
    free(stream);
    return 1;
}