#include "parsing/parsing.h"

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

    

    return 0;
}