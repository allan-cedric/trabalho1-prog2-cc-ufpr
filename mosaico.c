#include "parsing/parsing.h"
#include "ppmimage/ppmimage.h"
#include <sys/types.h>
#include <dirent.h>

#define STD_DIR "./tiles"

/* Filtro de imagens do tipo PPM. */
int filter_entries(const struct dirent *entry);

int main(int argc, char **argv)
{
    /* Inicialização padrão. */
    arguments_t opt_args;
    opt_args.input_filename = NULL;
    opt_args.output_filename = NULL;
    opt_args.directory_filename = NULL;

    /* Armazena o nome do diretório atual */
    char *main_dir;
    main_dir = getcwd(NULL, 0);
    if (!main_dir)
    {
        fprintf(stderr, "Error determining current directory\n");
        exit(1);
    }

    /* 
        Parsing da linha de comando. 
        Os argumentos das opções da linha de comando são armazenados 'opt_args'.
    */
    parse_opt(argc, argv, &opt_args);

    /* Parsing do diretório de pastilhas. */
    int num_dir_files;
    struct dirent **dir_files;
    if (opt_args.directory_filename)
        num_dir_files = scandir(opt_args.directory_filename, &dir_files, filter_entries, alphasort);
    else
        num_dir_files = scandir(STD_DIR, &dir_files, filter_entries, alphasort);

    if (num_dir_files < 0)
    {
        free(main_dir);
        main_dir = NULL;
        fprintf(stderr, "Error reading tiles directory\n");
        exit(1);
    }

    /* Muda para o diretório de pastilhas. */
    int ret;
    if (opt_args.directory_filename)
        ret = chdir(opt_args.directory_filename);
    else
        ret = chdir(STD_DIR);
    if (ret == -1)
    {
        free(main_dir);
        main_dir = NULL;
        while (num_dir_files--)
        {
            free(dir_files[num_dir_files]);
            dir_files[num_dir_files] = NULL;
        }
        free(dir_files);
        dir_files = NULL;
        fprintf(stderr, "Error accessing directory\n");
        exit(1);
    }

    /* Vetor de imagens do tipo PPM. */
    ppmimage_t *tiles = (ppmimage_t *)malloc(sizeof(ppmimage_t) * num_dir_files);
    if (!tiles)
    {
        free(main_dir);
        main_dir = NULL;
        while (num_dir_files--)
        {
            free(dir_files[num_dir_files]);
            dir_files[num_dir_files] = NULL;
        }
        free(dir_files);
        dir_files = NULL;
        fprintf(stderr, "Memory allocation error!\n");
        exit(1);
    }

    /* Alocação de memória das pastilhas */
    if (opt_args.directory_filename)
        fprintf(stderr, "Reading tiles from %s and calculating tiles' average colors...\n", opt_args.directory_filename);
    else
        fprintf(stderr, "Reading tiles from %s and calculating tiles' average colors...\n", STD_DIR);
    int i;
    for (i = 0; i < num_dir_files; i++)
    {
        FILE *imgfile = fopen(dir_files[i]->d_name, "r");
        if (!imgfile)
        {
            free(main_dir);
            main_dir = NULL;
            while (num_dir_files--)
            {
                if (dir_files[num_dir_files])
                {
                    free(dir_files[num_dir_files]);
                    dir_files[num_dir_files] = NULL;
                }
            }
            free(dir_files);
            dir_files = NULL;
            while (i--)
                free_ppmimage(&(tiles[i]));
            free(tiles);
            tiles = NULL;
            fprintf(stderr, "Error opening a PPM image\n");
            exit(1);
        }
        parse_ppmimage(imgfile, &(tiles[i]));
        fclose(imgfile);
        free(dir_files[i]);
        dir_files[i] = NULL;
    }
    free(dir_files);
    dir_files = NULL;

    fprintf(stderr, "%i tiles read\n", num_dir_files);
    fprintf(stderr, "Tile size is %ix%i\n", tiles[0].width, tiles[0].height);

    ret = chdir(opt_args.directory_filename);
    if (ret == -1)
    {
        free(main_dir);
        main_dir = NULL;
        while (num_dir_files--)
            free_ppmimage(&tiles[num_dir_files]);
        free(tiles);
        tiles = NULL;
        fprintf(stderr, "Error accessing directory\n");
        exit(1);
    }

    /* Desalocação de memória das últimas estruturas. */
    free(main_dir);
    main_dir = NULL;
    while (num_dir_files--)
        free_ppmimage(&tiles[num_dir_files]);
    free(tiles);
    tiles = NULL;

    /*FILE *ppm_img = fopen(opt_args.directory_filename, "r");
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
    ppm_test = NULL;*/

    return 0;
}

int filter_entries(const struct dirent *entry)
{
    char *extension = strrchr(entry->d_name, '.');
    return (extension && !strcmp(extension, ".ppm"));
}