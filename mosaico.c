#include "parsing/parsing.h"
#include "ppmimage/ppmimage.h"
#include <sys/types.h>
#include <dirent.h>

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

    ppmimage_t* ppm_test = (ppmimage_t *)malloc(sizeof(ppmimage_t) * 10);
    int i;
    for(i = 0; i < 10; i++)
    {
        ppm_test[i].type = NULL;
        ppm_test[i].img = NULL;
    }

    parse_ppmimage(ppm_img, &ppm_test[0]);
    fclose(ppm_img);

    printf("Type: %s\n", ppm_test[0].type);
    printf("Width: %i\n", ppm_test[0].width);
    printf("Height: %i\n", ppm_test[0].height);
    printf("Channel max value: %i\n", ppm_test[0].channel_max_value);
    printf("Mean red: %.2f\n", ppm_test[0].mean_red);
    printf("Mean green: %.2f\n", ppm_test[0].mean_green); 
    printf("Mean blue: %.2f\n", ppm_test[0].mean_blue);  

    for(i = 0; i < 10; i++)
        free_ppmimage(&ppm_test[i]);
    free(ppm_test);
    ppm_test = NULL;

    return 0;
}