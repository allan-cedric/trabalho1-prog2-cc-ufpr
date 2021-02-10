## Trabalho da disciplina de Programação II (CI1002) - Ciência da Computação - UFPR

* Programa que gera um fotomosaico no formato **PPM(PortablePix Map)** de uma imagem **PPM**.

### Compilação

`makefile`: Possui todas as regras de compilação para o projeto em questão.

* Uso: `./make`

### Bibliotecas do projeto

`parsing`: Implementação de toda a estrutura de parsing da linha de comando.

`ppmimage`: Implementação de diversas operações com uma imagem **PPM** no formato **P3** ou **P6**.

### Executando o projeto

`mosaico.c`: Programa principal que recebe como entrada uma imagem **PPM**, e gera um fotomosaico de mesmo formato e dimensões da imagem entrada. O fotomosaico é composto por diversas imagens menores (pastilhas) que também estão no formato **PPM**.

* Uso: `./mosaico [ -p diretório ] [ -i ppmimage ] [ -o ppmimage ] [ -h ]`

* `-p`: Define o diretório de pastilhas utilizado.* 

* `-i`: Define a imagem de entrada utilizada. (Recomenda-se imagens de alta resolução para perceber melhor o efeito).

* `-o`: Define o nome da imagem de saída (fotomosaico).

* `-h`: Envia na saída padrão de erros (stderr), uma mensagem de ajuda sobre o programa.

***Caso não seja especificado a opção `-p`, será assumido o diretório de pastilhas `./tiles`.**