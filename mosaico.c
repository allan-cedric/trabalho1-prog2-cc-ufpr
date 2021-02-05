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
    fprintf(stderr, "Building mosaic image\n");
    int lin, col, offset_width, offset_height, lim_point_width, lim_point_height;
    offset_width = tiles[0].width;
    offset_height = tiles[0].height;

    lim_point_width = input_ppmimg.width + (offset_width - (input_ppmimg.width % offset_width));
    lim_point_height = input_ppmimg.height + (offset_height - (input_ppmimg.height % offset_height));

    float *dominant_color_rgb = (float *)malloc(sizeof(float) * 3);
    if (!dominant_color_rgb)
    {
        while (num_dir_files--)
            free_ppmimage(&tiles[num_dir_files]);
        free(tiles);
        tiles = NULL;
        free_ppmimage(&input_ppmimg);
        fprintf(stderr, "Memory allocation error!\n");
        exit(1);
    }

    for (lin = 0; lin < lim_point_height; lin += offset_height)
    {
        for (col = 0; col < lim_point_width; col += offset_width)
        {
            dominant_color_rgb[0] = 0;
            dominant_color_rgb[1] = 0;
            dominant_color_rgb[2] = 0;
            dominant_color_ppmimage(&input_ppmimg, lin, col, offset_height, offset_width, dominant_color_rgb);
            int index = 0;
            float small_distance = approx_redmean(dominant_color_rgb, tiles[index].dominant_color_rgb);
            for (i = 1; i < num_dir_files; i++)
            {
                float distance = approx_redmean(dominant_color_rgb, tiles[i].dominant_color_rgb);
                if (distance < small_distance)
                {
                    small_distance = distance;
                    index = i;
                }
            }
            change_submatrix_ppmimage(&input_ppmimg, lin, col, &tiles[index]);
        }
    }
    free(dominant_color_rgb);
    dominant_color_rgb = NULL;

    /* Desalocação de memória das pastilhas. */
    while (num_dir_files--)
        free_ppmimage(&tiles[num_dir_files]);
    free(tiles);
    tiles = NULL;

    /* Escrita do fotomosaico em um arquivo PPM */
    FILE *output_ppmmosaic;
    if (opt_args.output_filename)
        output_ppmmosaic = fopen(opt_args.output_filename, "w");
    else
        output_ppmmosaic = stdout;

    if (!output_ppmmosaic)
    {
        free_ppmimage(&input_ppmimg);
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    fprintf(stderr, "Writing output file\n");
    write_ppmimage(&input_ppmimg, output_ppmmosaic);
    if (opt_args.output_filename)
        fclose(output_ppmmosaic);

    /* Desalocação de memória da imagem de entrada. */
    free_ppmimage(&input_ppmimg);

    return 0;
}

int filter_entries(const struct dirent *entry)
{
    char *extension = strrchr(entry->d_name, '.');
    return (extension && !strcmp(extension, ".ppm"));
}