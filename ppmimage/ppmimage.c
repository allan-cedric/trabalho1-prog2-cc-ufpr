/*
    Source file: 'ppmimage.c'
    Autor: Allan Cedric G. B. Alves da Silva
    Profile: Estudante de Ciência da Computação - UFPR

    Biblioteca que serve para realizar operações em uma imagem no formato PPM(Portable PixMap).
*/

#include "ppmimage.h"

int read_ppmimage(FILE *imgfile, ppmimage_t *ppmimg)
{
    /* === Alocação em memória e parsing do cabeçalho === */
    if (parse_header(imgfile, ppmimg))
        return 1;

    /* === Alocação em memória e parsing dos pixels === */
    if (alloc_pixels(imgfile, ppmimg))
        return 1;

    return 0;
}

int parse_header(FILE *imgfile, ppmimage_t *ppmimg)
{
    char *str = (char *)malloc(sizeof(char) * (MAX_SIZE_STR + 1));
    if (!str)
    {
        fprintf(stderr, "Memory allocation error! - 'char *str' -> parse_ppmimage\n");
        return 1;
    }

    /* Lê e verifica o tipo da imagem PPM. */
    int ret = fscanf(imgfile, "%s", str);
    if (ret < 1 || (strcmp(str, "P3") && strcmp(str, "P6")))
    {
        if (imgfile != stdin)
            fclose(imgfile);
        free(str);
        str = NULL;
        fprintf(stderr, "Error: this is not a PPM image type\n");
        return 1;
    }
    /* Consome um 'whitespace' - separador genérico. */
    fgetc(imgfile);

    /* Armazena o tipo da imagem */
    ppmimg->type = (char *)malloc(sizeof(char) * (MAX_SIZE_PPM_TYPE_STR + 1));
    if (!ppmimg->type)
    {
        if (imgfile != stdin)
            fclose(imgfile);
        free(str);
        str = NULL;
        fprintf(stderr, "Memory allocation error! - 'ppmimg->type' -> parse_ppmimage\n");
        return 1;
    }
    strcpy(ppmimg->type, str);

    /* Libera memória do buffer que armazenava o tipo de imagem. */
    free(str);
    str = NULL;

    /* Parsing realizado para obter a largura, altura e o valor máximo de um canal RGB. */
    int search_value;
    for (search_value = 0; search_value < 3; search_value++)
    {
        /* Ignora os comentários. */
        while (fgetc(imgfile) == '#')
            while (fgetc(imgfile) != '\n');
        fseek(imgfile, -1, SEEK_CUR);

        /* Chavea qual dado vai ser armazenado, seguindo a ordem feita no formarto PPM. */
        switch (search_value)
        {
        case 0:
            ret = fscanf(imgfile, "%i", &ppmimg->width);
            break;
        case 1:
            ret = fscanf(imgfile, "%i", &ppmimg->height);
            break;
        case 2:
            ret = fscanf(imgfile, "%i", &ppmimg->channel_max_value);
            break;
        default:
            break;
        }
        if (!ret)
        {
            if (imgfile != stdin)
                fclose(imgfile);
            free(ppmimg->type);
            ppmimg->type = NULL;
            fprintf(stderr, "Error reading header\n");
            return 1;
        }
        /* Consome um 'whitespace' - separador genérico. */
        fgetc(imgfile);
    }

    /* Ignora os comentários. */
    while (fgetc(imgfile) == '#')
        while (fgetc(imgfile) != '\n');
    fseek(imgfile, -1, SEEK_CUR);

    return 0;
}

int alloc_pixels(FILE *imgfile, ppmimage_t *ppmimg)
{
    /* === Tratamento rigoroso para alocar memória para a imagem === */
    ppmimg->img = (pixel_t **)malloc(sizeof(pixel_t *) * ppmimg->height);
    if (!ppmimg->img)
    {
        if (imgfile != stdin)
            fclose(imgfile);
        free(ppmimg->type);
        ppmimg->type = NULL;
        fprintf(stderr, "Memory allocation error! - 'ppmimg->img' -> parse_pixels\n");
        return 1;
    }

    int line;
    for (line = 0; line < ppmimg->height; line++)
    {
        ppmimg->img[line] = (pixel_t *)malloc(sizeof(pixel_t) * ppmimg->width);
        if (!ppmimg->img[line])
        {
            if (imgfile != stdin)
                fclose(imgfile);
            free(ppmimg->type);
            ppmimg->type = NULL;
            while (line--)
            {
                free(ppmimg->img[line]);
                ppmimg->img[line] = NULL;
            }
            free(ppmimg->img);
            ppmimg->img = NULL;
            fprintf(stderr, "Memory allocation error! - 'ppmimg->img[pixel]' -> parse_line\n");
            return 1;
        }
        int i;
        for(i = 0; i < ppmimg->width; i++)
        {
            ppmimg->img[line][i].red = 0;
            ppmimg->img[line][i].green = 0;
            ppmimg->img[line][i].blue = 0;
        }
    }

    /* === Leitura dos pixels === */
    ppmimg->dominant_color_rgb = (float *)malloc(sizeof(float) * 3);
    if (!ppmimg->dominant_color_rgb)
    {
        if (imgfile != stdin)
            fclose(imgfile);
        free_ppmimage(ppmimg);
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }

    ppmimg->dominant_color_rgb[0] = 0;
    ppmimg->dominant_color_rgb[1] = 0;
    ppmimg->dominant_color_rgb[2] = 0;

    if (!strcmp(ppmimg->type, "P3"))
        parse_pixels_P3(imgfile, ppmimg);
    else
        parse_pixels_P6(imgfile, ppmimg);

    /* === Cálculo da cor predominante === */
    int num_pixels = (ppmimg->width * ppmimg->height);
    ppmimg->dominant_color_rgb[0] = sqrtf(ppmimg->dominant_color_rgb[0] / num_pixels);
    ppmimg->dominant_color_rgb[1] = sqrtf(ppmimg->dominant_color_rgb[1] / num_pixels);
    ppmimg->dominant_color_rgb[2] = sqrtf(ppmimg->dominant_color_rgb[2] / num_pixels);

    return 0;
}

int parse_pixels_P3(FILE *imgfile, ppmimage_t *ppmimg)
{
    int red, green, blue, line, col, ret;
    ret = fscanf(imgfile, "%i %i %i", &red, &green, &blue);
    for (line = 0; line < ppmimg->height && ret != EOF; line++)
    {
        for (col = 0; col < ppmimg->width && ret != EOF; col++)
        {
            if (ret < 3)
            {
                if (imgfile != stdin)
                    fclose(imgfile);
                free_ppmimage(ppmimg);
                fprintf(stderr, "Error reading a pixel\n");
                return 1;
            }
            ppmimg->img[line][col].red = red;
            ppmimg->img[line][col].green = green;
            ppmimg->img[line][col].blue = blue;

            ppmimg->dominant_color_rgb[0] += (red * red);
            ppmimg->dominant_color_rgb[1] += (green * green);
            ppmimg->dominant_color_rgb[2] += (blue * blue);

            ret = fscanf(imgfile, "%i %i %i", &red, &green, &blue);
        }
    }
    return 0;
}

int parse_pixels_P6(FILE *imgfile, ppmimage_t *ppmimg)
{
    int line, col, ret;
    byte *channel_rgb = (byte *)malloc(sizeof(byte) * 3);
    if (!channel_rgb)
    {
        if (imgfile != stdin)
            fclose(imgfile);
        free_ppmimage(ppmimg);
        fprintf(stderr, "Memory allocation error!\n");
        return 1;
    }
    ret = fread(channel_rgb, sizeof(byte), 3, imgfile);
    for (line = 0; line < ppmimg->height && !feof(imgfile); line++)
    {
        for (col = 0; col < ppmimg->width && !feof(imgfile); col++)
        {
            if (ret < 3)
            {
                if (imgfile != stdin)
                    fclose(imgfile);
                free_ppmimage(ppmimg);
                free(channel_rgb);
                channel_rgb = NULL;
                fprintf(stderr, "Error reading a pixel\n");
                return 1;
            }
            ppmimg->img[line][col].red = channel_rgb[0];
            ppmimg->img[line][col].green = channel_rgb[1];
            ppmimg->img[line][col].blue = channel_rgb[2];

            ppmimg->dominant_color_rgb[0] += (channel_rgb[0] * channel_rgb[0]);
            ppmimg->dominant_color_rgb[1] += (channel_rgb[1] * channel_rgb[1]);
            ppmimg->dominant_color_rgb[2] += (channel_rgb[2] * channel_rgb[2]);

            ret = fread(channel_rgb, sizeof(byte), 3, imgfile);
        }
    }
    free(channel_rgb);
    channel_rgb = NULL;

    return 0;
}

void free_ppmimage(ppmimage_t *ppmimg)
{
    if (ppmimg->type)
    {
        free(ppmimg->type);
        ppmimg->type = NULL;
    }
    if (ppmimg->dominant_color_rgb)
    {
        free(ppmimg->dominant_color_rgb);
        ppmimg->dominant_color_rgb = NULL;
    }
    if (ppmimg->img)
    {
        while (ppmimg->height--)
        {
            if (ppmimg->img[ppmimg->height])
            {
                free(ppmimg->img[ppmimg->height]);
                ppmimg->img[ppmimg->height] = NULL;
            }
        }
        free(ppmimg->img);
        ppmimg->img = NULL;
    }
}

void dominant_color_ppmimage(ppmimage_t *ppmimg, int init_lin, int init_col, int offset_lin, int offset_col, float *dominant_color_rgb)
{
    /* === Cálculo da cor predominante === */
    int lin, col;
    for (lin = init_lin; lin < init_lin + offset_lin; lin++)
    {
        for (col = init_col; col < init_col + offset_col; col++)
        {
            if (lin < ppmimg->height && col < ppmimg->width)
            {
                dominant_color_rgb[0] += (ppmimg->img[lin][col].red * ppmimg->img[lin][col].red);
                dominant_color_rgb[1] += (ppmimg->img[lin][col].green * ppmimg->img[lin][col].green);
                dominant_color_rgb[2] += (ppmimg->img[lin][col].blue * ppmimg->img[lin][col].blue);
            }
        }
    }

    int num_pixels = (offset_lin * offset_col);
    dominant_color_rgb[0] = sqrtf(dominant_color_rgb[0] / num_pixels);
    dominant_color_rgb[1] = sqrtf(dominant_color_rgb[1] / num_pixels);
    dominant_color_rgb[2] = sqrtf(dominant_color_rgb[2] / num_pixels);
}

float approx_redmean(float *rgb_1, float *rgb_2)
{
    float redmean = (rgb_2[0] + rgb_1[0]) / 2;
    float delta_red = (rgb_2[0] - rgb_1[0]), delta_green = (rgb_2[1] - rgb_1[1]), delta_blue = (rgb_2[2] - rgb_1[2]);

    float first_term = ((2 + (redmean / 256)) * powf(delta_red, 2));
    float second_term = (4 * powf(delta_green, 2));
    float third_term = ((2 + ((255 - redmean) / 256)) * powf(delta_blue, 2));

    return sqrtf(first_term + second_term + third_term);
}

void change_submatrix_ppmimage(ppmimage_t *main_ppmimg, int init_lin, int init_col, ppmimage_t *ppmimg)
{
    /* Mudança de uma submatriz de ordem ppmimg->width da imagem principal. */
    int main_lin, main_col, lin, col;
    for (main_lin = init_lin, lin = 0; main_lin < init_lin + ppmimg->height; main_lin++, lin++)
    {
        for (main_col = init_col, col = 0; main_col < init_col + ppmimg->width; main_col++, col++)
        {
            if (main_lin < main_ppmimg->height && main_col < main_ppmimg->width)
            {
                main_ppmimg->img[main_lin][main_col].red = ppmimg->img[lin][col].red;
                main_ppmimg->img[main_lin][main_col].green = ppmimg->img[lin][col].green;
                main_ppmimg->img[main_lin][main_col].blue = ppmimg->img[lin][col].blue;
            }
        }
    }
}

int write_ppmimage(ppmimage_t *ppmimg, FILE *imgfile)
{
    /* Tipo da imagem PPM. */
    fprintf(imgfile, "%s\n", ppmimg->type);

    /* Comentário básico. */
    fprintf(imgfile, "# Created by mosaico program\n");

    /* Largura, comprimento e valor máximo de um canal RGB. */
    fprintf(imgfile, "%i %i\n%i\n", ppmimg->width, ppmimg->height, ppmimg->channel_max_value);

    int i, j;
    /* Escrevendo os pixels */
    if (!strcmp(ppmimg->type, "P3"))
    {
        for (i = 0; i < ppmimg->height; i++)
        {
            for (j = 0; j < ppmimg->width; j++)
                fprintf(imgfile, "%i\n%i\n%i\n", ppmimg->img[i][j].red, ppmimg->img[i][j].green, ppmimg->img[i][j].blue);
        }
    }
    else
    {
        int ret;
        for (i = 0; i < ppmimg->height; i++)
        {
            for (j = 0; j < ppmimg->width; j++)
            {
                ret = 0;
                ret = fwrite(&(ppmimg->img[i][j].red), sizeof(byte), 1, imgfile);
                ret += fwrite(&(ppmimg->img[i][j].green), sizeof(byte), 1, imgfile);
                ret += fwrite(&(ppmimg->img[i][j].blue), sizeof(byte), 1, imgfile);
                if(ret != 3)
                {
                    fprintf(stderr, "Error writing file\n");
                    return 1;
                }
            }
        }
    }
    return 0;
}