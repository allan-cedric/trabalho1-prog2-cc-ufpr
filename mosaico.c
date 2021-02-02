#include "parsing/parsing.h"
#include "ppmimage/ppmimage.h"

#define MAX_SIZE_STR 100

int main(int argc, char **argv)
{
    /* Inicialização padrão */
    arguments_t opt_args;
    opt_args.input_filename = NULL;
    opt_args.output_filename = NULL;
    opt_args.directory_filename = NULL;

    /* 
        Parsing da linha de comando. 
        Os argumentos das opções da linha de comando são armazenados 'opt_args'.
    */
    parse_opt(argc, argv, &opt_args);

    FILE *ppm_img = fopen(opt_args.directory_filename, "r");
    if (!ppm_img)
    {
        fprintf(stderr, "Error opening PPM image\n");
        exit(1);
    }

    ppmimage_t ppm_test;

    parse_ppmimage(ppm_img, &ppm_test);

    printf("Type: %s\n", ppm_test.type);
    printf("Width: %i\n", ppm_test.width);
    printf("Height: %i\n", ppm_test.height);
    printf("Channel max value: %i\n", ppm_test.channel_max_value);
    int i, j;
    for(i = 0; i < ppm_test.height; i++)
    {
        for(j = 0; j < ppm_test.width; j++)
            printf("(%i %i %i) ", ppm_test.img[i][j].red, ppm_test.img[i][j].green, ppm_test.img[i][j].blue);
        printf("\n");
    }    

    free(ppm_test.type);
    ppm_test.type = NULL;

    for(i = 0; i < ppm_test.height; i++)
    {
        free(ppm_test.img[i]);
        ppm_test.img[i] = NULL;
    }
    free(ppm_test.img);
    ppm_test.img = NULL;

    fclose(ppm_img);

    return 0;
}