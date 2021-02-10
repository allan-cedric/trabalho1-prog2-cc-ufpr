/*
    ===
    Header file: 'ppmimage.h'
    Autor: Allan Cedric G. B. Alves da Silva
    Profile: Estudante de Ciência da Computação - UFPR

    Biblioteca que serve para realizar operações em uma imagem no formato PPM(Portable PixMap)
    ===
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

#define RED 0
#define GREEN 1
#define BLUE 2

/* === Nomenclatura para facilitar === */
typedef unsigned char byte;

/* === Estrutura que imita um pixel no padrão RGB === */
typedef struct pixel_t
{
    byte red, green, blue;
} pixel_t;

/*
    ===
    Estrutura que armazena uma imagem no formato PPM.
    Além disso, armazena os metadados da imagem, como
    altura, largura e tipo da imagem (P3 ou P6)
    ===
*/
typedef struct ppmimage_t
{
    char *type;
    int width, height, channel_max_value;
    float *dominant_color_rgb;
    pixel_t **img;
} ppmimage_t;

/* === Realiza a leitura e a alocação de memória de uma imagem do tipo PPM === */
/* Retorna 0 se terminou com sucesso, senão 1 */
int read_ppmimage(FILE *imgfile, ppmimage_t *ppmimg);

/* === Realiza o parsing do cabeçalho de uma imagem do tipo PPM === */
/* Retorna 0 se terminou com sucesso, senão 1 */
int parse_header(FILE *imgfile, ppmimage_t *ppmimg);

/* === Realiza a alocação de memória dos pixels de uma imagem do tipo PPM === */
/* Retorna 0 se terminou com sucesso, senão 1 */
int alloc_pixels(FILE *imgfile, ppmimage_t *ppmimg);

/* === Realiza o parsing dos pixels de uma imagem do tipo PPM P3 === */
/* Retorna 0 se terminou com sucesso, senão 1 */
int parse_pixels_P3(FILE *imgfile, ppmimage_t *ppmimg);

/* === Realiza o parsing dos pixels de uma imagem do tipo PPM P6 === */
/* Retorna 0 se terminou com sucesso, senão 1 */
int parse_pixels_P6(FILE *imgfile, ppmimage_t *ppmimg);

/* === Desaloca toda a memória reservada para uma imagem do tipo PPM === */
void free_ppmimage(ppmimage_t *ppmimg);

/* === Cálculo da cor predominante de uma imagem PPM. (Utiliza o método da raiz quadrada das médias) === */
/* Retorna as cores no vetor 'dominant_color_rgb', o qual deve ser previamente alocado com tamanho 3 */
void dominant_color_ppmimage(ppmimage_t *ppmimg, int init_lin, int init_col, int offset_lin, int offset_col, float *dominant_color_rgb);

/* === Retorna a distância entre duas cores formatadas no padrão RGB. (Utiliza a aproximação redmean) === */
float approx_redmean(float *rgb_1, float *rgb_2);

/* === Altera uma certa matriz de pixels de uma imagem PPM por uma outra imagem PPM === */
void change_submatrix_ppmimage(ppmimage_t *main_ppmimg, int init_lin, int init_col, ppmimage_t *ppmimg);

/* === Escreve um novo arquivo PPM a partir de uma imagem PPM alocada em memória === */
/* Retorna 0 se terminou com sucesso, senão 1 */
int write_ppmimage(ppmimage_t *ppmimg, FILE *imgfile);

#endif