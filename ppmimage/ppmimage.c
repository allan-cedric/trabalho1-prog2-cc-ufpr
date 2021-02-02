/*
    Source file: 'ppmimage.c'
    Autor: Allan Cedric G. B. Alves da Silva
    Profile: Estudante de Ciência da Computação - UFPR

    Biblioteca que serve para realizar operações em uma imagem no formato PPM(Portable PixMap).
*/

#include "ppmimage.h"

void parse_ppmimage(FILE *imgfile, ppmimage_t *ppmimg)
{
    char *str = (char *)malloc(sizeof(char) * (MAX_SIZE_STR + 1));
    if (!str)
    {
        fprintf(stderr, "Memory allocation error! - 'char *str' -> parse_ppmimage\n");
        exit(1);
    }

    /* Lê e verifica o tipo da imagem PPM. */
    int ret = fscanf(imgfile, "%s", str);
    if (ret < 1 || (strcmp(str, "P3") && strcmp(str, "P6")))
    {
        fclose(imgfile);
        free(str);
        str = NULL;
        fprintf(stderr, "Error: this is not a PPM image type\n");
        exit(1);
    }
    /* Consome um 'whitespace' - separador genérico. */
    fgetc(imgfile);

    /* Armazena o tipo da imagem */
    ppmimg->type = (char *)malloc(sizeof(char) * (MAX_SIZE_PPM_TYPE_STR + 1));
    if (!ppmimg->type)
    {
        fprintf(stderr, "Memory allocation error! - 'ppmimg->type' -> parse_ppmimage\n");
        exit(1);
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

        /* Chavea com dado vai ser armazenado, seguindo a ordem feita no formarto PPM. */
        switch (search_value)
        {
            case 0:
                ret = fscanf(imgfile, "%i", &ppmimg->width);
                if (!ret)
                {
                    fclose(imgfile);
                    free(ppmimg->type);
                    ppmimg->type = NULL;
                    fprintf(stderr, "Error reading image width\n");
                    exit(1);
                }
                break;
            case 1:
                ret = fscanf(imgfile, "%i", &ppmimg->height);
                if (!ret)
                {
                    fclose(imgfile);
                    free(ppmimg->type);
                    ppmimg->type = NULL;
                    fprintf(stderr, "Error reading image height\n");
                    exit(1);
                }
                break;
            case 2:
                ret = fscanf(imgfile, "%i", &ppmimg->channel_max_value);
                if (!ret)
                {
                    fclose(imgfile);
                    free(ppmimg->type);
                    ppmimg->type = NULL;
                    fprintf(stderr, "Error reading image channel max value\n");
                    exit(1);
                }
                break;
            default:
                break;
        }
        /* Consome um 'whitespace' - separador genérico. */
        fgetc(imgfile);
    }
    
    /* Ignora os comentários. */
    while (fgetc(imgfile) == '#')
        while (fgetc(imgfile) != '\n');
    fseek(imgfile, -1, SEEK_CUR);

    parse_pixels(imgfile, ppmimg);
}

void parse_pixels(FILE *imgfile, ppmimage_t *ppmimg)
{
    /* Tratamento rigoroso para alocar memória para a imagem. */
    ppmimg->img = (pixel_t **)malloc(sizeof(pixel_t *) * ppmimg->height);
    if (!ppmimg->img)
    {
        fclose(imgfile);
        fprintf(stderr, "Memory allocation error! - 'ppmimg->img' -> parse_pixels\n");
        free(ppmimg->type);
        ppmimg->type = NULL;
        exit(1);
    }
    
    int line;
    for (line = 0; line < ppmimg->height; line++)
    {
        ppmimg->img[line] = (pixel_t *)malloc(sizeof(pixel_t) * ppmimg->width);
        if (!ppmimg->img[line])
        {
            fclose(imgfile);
            fprintf(stderr, "Memory allocation error! - 'ppmimg->img[pixel]' -> parse_line\n");
            free(ppmimg->type);
            ppmimg->type = NULL;
            while (line--)
            {
                free(ppmimg->img[line]);
                ppmimg->img[line] = NULL;
            }
            free(ppmimg->img);
            ppmimg->img = NULL;
            exit(1);
        }
    }

    /* Leitura dos pixels de uma imagem PPM tipo P3. */
    int ret, col;
    if (!strcmp(ppmimg->type, "P3"))
    {
        int red, green, blue;
        ret = fscanf(imgfile, "%i %i %i", &red, &green, &blue);
        for (line = 0; line < ppmimg->height && ret != EOF; line++)
        {
            for (col = 0; col < ppmimg->width && ret != EOF; col++)
            {
                if (ret < 3)
                {
                    fclose(imgfile);
                    fprintf(stderr, "Error reading a pixel\n");
                    free(ppmimg->type);
                    ppmimg->type = NULL;
                    while (ppmimg->height--)
                    {
                        free(ppmimg->img[ppmimg->height]);
                        ppmimg->img[ppmimg->height] = NULL;
                    }
                    free(ppmimg->img);
                    ppmimg->img = NULL;
                    exit(1);
                }
                ppmimg->img[line][col].red = red;
                ppmimg->img[line][col].green = green;
                ppmimg->img[line][col].blue = blue;
                ret = fscanf(imgfile, "%i %i %i", &red, &green, &blue);
            }
        }
    }
    /* Leitura dos pixels de uma imagem PPM tipo P6. */
    else
    {
        unsigned char *channel_rgb = (unsigned char *)malloc(sizeof(unsigned char) * 3);
        ret = fread(channel_rgb, sizeof(unsigned char), 3, imgfile);
        for (line = 0; line < ppmimg->height && !feof(imgfile); line++)
        {
            for (col = 0; col < ppmimg->width && !feof(imgfile); col++)
            {
                if (ret < 3)
                {
                    fclose(imgfile);
                    fprintf(stderr, "Error reading a pixel\n");
                    free(ppmimg->type);
                    ppmimg->type = NULL;
                    while (ppmimg->height--)
                    {
                        free(ppmimg->img[ppmimg->height]);
                        ppmimg->img[ppmimg->height] = NULL;
                    }
                    free(ppmimg->img);
                    ppmimg->img = NULL;
                    free(channel_rgb);
                    channel_rgb = NULL;
                    exit(1);
                }
                ppmimg->img[line][col].red = channel_rgb[0];
                ppmimg->img[line][col].green = channel_rgb[1];
                ppmimg->img[line][col].blue = channel_rgb[2];
                ret = fread(channel_rgb, sizeof(unsigned char), 3, imgfile);
            }
        }
        free(channel_rgb);
        channel_rgb = NULL;
    }
}