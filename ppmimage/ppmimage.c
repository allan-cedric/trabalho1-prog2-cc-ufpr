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
    if(parse_header(imgfile, ppmimg))
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
        if(imgfile != stdin)
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
        if(imgfile != stdin)
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
            if(imgfile != stdin)
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
        if(imgfile != stdin)
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
            if(imgfile != stdin)
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
    }

    /* === Leitura dos pixels === */
    ppmimg->mean_red = 0;
    ppmimg->mean_green = 0;
    ppmimg->mean_blue = 0;

    if (!strcmp(ppmimg->type, "P3"))
        parse_pixels_P3(imgfile, ppmimg);
    else
        parse_pixels_P6(imgfile, ppmimg);

    /* === Cálculo da cor predominante === */
    int num_pixels = (ppmimg->width * ppmimg->height);
    ppmimg->mean_red = sqrtf(ppmimg->mean_red / num_pixels);
    ppmimg->mean_green = sqrtf(ppmimg->mean_green / num_pixels);
    ppmimg->mean_blue = sqrtf(ppmimg->mean_blue / num_pixels);

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
                if(imgfile != stdin)
                    fclose(imgfile);
                free_ppmimage(ppmimg);
                fprintf(stderr, "Error reading a pixel\n");
                return 1;
            }
            ppmimg->img[line][col].red = red;
            ppmimg->img[line][col].green = green;
            ppmimg->img[line][col].blue = blue;

            ppmimg->mean_red += (red * red);
            ppmimg->mean_green += (green * green);
            ppmimg->mean_blue += (blue * blue);

            ret = fscanf(imgfile, "%i %i %i", &red, &green, &blue);
        }
    }
    return 0;
}

int parse_pixels_P6(FILE *imgfile, ppmimage_t *ppmimg)
{
    int line, col, ret;
    byte *channel_rgb = (byte *)malloc(sizeof(byte) * 3);
    ret = fread(channel_rgb, sizeof(byte), 3, imgfile);
    for (line = 0; line < ppmimg->height && !feof(imgfile); line++)
    {
        for (col = 0; col < ppmimg->width && !feof(imgfile); col++)
        {
            if (ret < 3)
            {
                if(imgfile != stdin)
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

            ppmimg->mean_red += (channel_rgb[0] * channel_rgb[0]);
            ppmimg->mean_green += (channel_rgb[1] * channel_rgb[1]);
            ppmimg->mean_blue += (channel_rgb[2] * channel_rgb[2]);

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