/*
    ===
    Header file: 'parsing.h'
    Autor: Allan Cedric G. B. Alves da Silva
    Profile: Estudante de Ciência da Computação - UFPR - GRR20190351

    Biblioteca que serve para realizar o parsing de opções/argumentos da linha de comando
    ===
*/

#ifndef __PARSING_H__
#define __PARSING_H__

/* === Macro que permite a utilização de funções non-standard === */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* === Bibliotecas === */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

/* === Estrutura que armazena os argumentos das opções da linha de comando === */
typedef struct arguments_t
{
    char *input_filename;
    char *output_filename;
    char *directory_filename;
} arguments_t;

/* === Realiza o parsing da linha de comando === */
/* A estrutura de argumentos 'args' irá apontar para os argumentos de argv */
void parse_opt(int argc, char **argv, arguments_t *args);

/* 
    ===
    Envia na saída padrão de erros (stderr)
    uma mensagem de ajuda sobre o programa
    ===
*/
void help_display();

/*
   ===
   Envia na saída padrão de erros (stderr)
   uma mensagem de erro referente a falta de argumentos na linha de comando
   ===
*/
void missing_arg_display(char opt);

/* 
    ===
    Envia na saída padrão de erros (stderr)
    uma mensagem de erro referente a uma opção inválida
    ===
*/
void invalid_opt_display(char opt);

/*
    ===
    Envia na saída padrão de erros (stderr)
    uma mensagem de erro referente a argumentos desnecessários
    ===
*/
void many_arg_display();

#endif