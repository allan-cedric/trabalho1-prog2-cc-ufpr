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

    /* 
        Parsing da linha de comando. 
        Os argumentos das opções da linha de comando vão ser apontados por 'opt_args'.
    */
    parse_opt(argc, argv, &opt_args);

    /* Armazena o nome do diretório atual. */
    char *main_dir;
    main_dir = getcwd(NULL, 0);
    if (!main_dir)
    {
        fprintf(stderr, "Error determining current directory\n");
        exit(1);
    }

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
        fprintf(stderr, "Error parsing tiles directory\n");
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
        fprintf(stderr, "Error accessing tiles directory\n");
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

    /* Leitura e alocação de memória das pastilhas. */
    if (opt_args.directory_filename)
        fprintf(stderr, "Reading tiles from %s and calculating tiles' average colors...\n", opt_args.directory_filename);
    else
        fprintf(stderr, "Reading tiles from %s and calculating tiles' average colors...\n", STD_DIR);
    int i;
    FILE *imgfile;
    for (i = 0; i < num_dir_files; i++)
    {
        imgfile = fopen(dir_files[i]->d_name, "r");
        if (!imgfile || read_ppmimage(imgfile, &(tiles[i])))
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
            fprintf(stderr, "Error opening a PPM tile\n");
            exit(1);
        }
        fclose(imgfile);
        free(dir_files[i]);
        dir_files[i] = NULL;
    }
    free(dir_files);
    dir_files = NULL;

    /* Metadados das pastilhas */
    fprintf(stderr, "%i tiles read\n", num_dir_files);
    fprintf(stderr, "Tile size is %ix%i\n", tiles[0].width, tiles[0].height);

    /* Volta para o diretório principal. */
    ret = chdir(main_dir);
    if (ret == -1)
    {
        free(main_dir);
        main_dir = NULL;
        while (num_dir_files--)
            free_ppmimage(&tiles[num_dir_files]);
        free(tiles);
        tiles = NULL;
        fprintf(stderr, "Error accessing main directory\n");
        exit(1);
    }
    free(main_dir);
    main_dir = NULL;

    /* Imagem PPM de entrada */
    ppmimage_t input_ppmimg;
    if (opt_args.input_filename)
        imgfile = fopen(opt_args.input_filename, "r");
    else
        imgfile = stdin;

    fprintf(stderr, "Reading input image...\n");
    if (!imgfile || read_ppmimage(imgfile, &input_ppmimg))
    {
        while (num_dir_files--)
            free_ppmimage(&tiles[num_dir_files]);
        free(tiles);
        tiles = NULL;
        fprintf(stderr, "Error opening input image\n");
        exit(1);
    }
    fprintf(stderr, "Input image is PPM %s, %ix%i pixels\n", input_ppmimg.type, input_ppmimg.width, input_ppmimg.height);

    if (opt_args.input_filename)
        fclose(imgfile);

    /* Geração do fotomosaico */

    /* Desalocação de memória das últimas estruturas. */
    while (num_dir_files--)
        free_ppmimage(&tiles[num_dir_files]);
    free(tiles);
    tiles = NULL;
    free_ppmimage(&input_ppmimg);

    return 0;
}

int filter_entries(const struct dirent *entry)
{
    char *extension = strrchr(entry->d_name, '.');
    return (extension && !strcmp(extension, ".ppm"));
}