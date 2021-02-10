/*
    ===
    Source file: 'parsing.c'
    Autor: Allan Cedric G. B. Alves da Silva
    Profile: Estudante de Ciência da Computação - UFPR

    Biblioteca que serve para realizar o parsing de opções/argumentos da linha de comando
    ===
*/

#include "parsing.h"

void parse_opt(int argc, char **argv, arguments_t *args)
{
    char opt;

    /* === Permite que o próprio programa trate das mensagens de erro durante o parsing === */
    opterr = 0;

    /* === Parsing da linha de comando === */
    while ((opt = getopt(argc, argv, "i:o:p:h")) != -1)
    {
        switch (opt)
        {
            case 'i':
                args->input_filename = optarg;
                break;
            case 'o':
                args->output_filename = optarg;
                break;
            case 'p':
                args->directory_filename = optarg;
                break;
            case 'h':
                help_display();
                break;
            case '?':
                if (optopt == 'i' || optopt == 'o' || optopt == 'p')
                    missing_arg_display(optopt);
                else
                    invalid_opt_display(optopt);
                break;
            default:
                abort();
        }
    }
    if (optind < argc)
        many_arg_display();
}

void help_display()
{
    fprintf(stderr,
    "Usage: mosaico [ -p directory ] [ -i ppmimage ] [ -o ppmimage ] [ -h ]\n"
    "Generate a PPM format photographic mosaic from a PPM image.\n\n"
    " Mosaico Options:\n"
    "\t-i\tInput PPM image (P3 or P6).\n"
    "\t-o\tOutput PPM image (same type as input image).\n"
    "\t-p\tTiles directory.\n\n"
    " Informational Options:\n"
    "\t-h\tGive this help list.\n\n"
    "Report bugs to acgbas19@inf.ufpr.br.\n");
    exit(1);
}

void missing_arg_display(char opt)
{
    fprintf(stderr,
    "./mosaico: option requires an argument -- '%c'\n"
    "Try './mosaico -h' for more information.\n", opt);
    exit(1);
}

void invalid_opt_display(char opt)
{
    if (isprint(opt))
        fprintf(stderr,
        "./mosaico: invalid option -- '%c'\n"
        "Try './mosaico -h' for more information.\n", opt);
    else
        fprintf(stderr,
        "./mosaico: invalid option -- '0x%x'\n"
        "Try './mosaico -h' for more information.\n", opt);
    exit(1);
}

void many_arg_display()
{
    fprintf(stderr,
    "./mosaico: too many arguments\n"
    "Try './mosaico -h' for more information.\n");
    exit(1);
}