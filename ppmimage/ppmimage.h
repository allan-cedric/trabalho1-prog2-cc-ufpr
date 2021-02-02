/*
    Header file: 'ppmimage.h'
    Autor: Allan Cedric G. B. Alves da Silva
    Profile: Estudante de Ciência da Computação - UFPR

    Biblioteca que serve para realizar operações em uma imagem no formato PPM(Portable PixMap).
*/

#ifndef __PPMIMAGE_H__
#define __PPMIMAGE_H__

/* === Bibliotecas === */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* === Macros === */
#define MAX_SIZE_STR 100
#define MAX_SIZE_PPM_TYPE_STR 2


/*
    Nomenclatura para facilitar.
*/
typedef unsigned char byte;

/*
    Estrutra que imita um pixel no padrão RGB.
*/
typedef struct pixel_t
{
    byte red, green, blue;
} pixel_t;

/* 
    Estrutura que armazena uma imagem no formato PPM.
    Além disso, armazena os metadados da imagem, como
    altura, largura e tipo da imagem (P3 ou P6).
*/
typedef struct ppmimage_t
{
    char *type;
    int width, height, channel_max_value;
    float mean_red, mean_green, mean_blue;
    pixel_t **img;
} ppmimage_t;

/*
    Função que realiza o parsing de uma imagem do tipo PPM.
*/
void parse_ppmimage(FILE *imgfile, ppmimage_t *ppmimg);

/*
    Função que realiza o parsing do cabeçalho de uma imagem do tipo PPM.
*/
void parse_header(FILE* imgfile, ppmimage_t *ppmimg);

/*
    Função que realiza a alocação de memória dos pixels de uma imagem do tipo PPM.
*/
void alloc_pixels(FILE* imgfile, ppmimage_t *ppmimg);

/*
    Função que realiza o parsing dos pixels de uma imagem do tipo PPM P3.
*/
void parse_pixels_P3(FILE* imgfile, ppmimage_t *ppmimg);

/*
    Função que realiza o parsing dos pixels de uma imagem do tipo PPM P6.
*/
void parse_pixels_P6(FILE* imgfile, ppmimage_t *ppmimg);

/*
    Função que desaloca a memória reservada para uma imagem do tipo PPM.
*/
void free_ppmimage(ppmimage_t *ppmimg);

#endif