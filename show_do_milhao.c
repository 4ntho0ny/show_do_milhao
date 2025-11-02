#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define QTD_PERGUNTAS_TOTAL 70
#define QTD_PERGUNTAS_NIVEIS_1_2_3 20
#define QTD_PERGUNTAS_NIVEL_4 10

typedef enum {
    INPUT_A = 'a',
    INPUT_B = 'b',
    INPUT_C = 'c',
    INPUT_D = 'd',
    INPUT_1 = '1',
    INPUT_2 = '2',
    INPUT_3 = '3',
    INPUT_4 = '4',
    INPUT_5 = '5',
} INPUT_USUARIO;

typedef struct {
    char nivel;
    char descricao[200];
    char alt[4][30];
    char alt_correta;
} pergunta;

void carregar_perguntas(pergunta *perguntas);
void carregar_perguntas_por_nivel(pergunta *todas_perguntas, pergunta *perguntas_por_nivel, int nvl_atual);
int sortear_pergunta(int qtd_perguntas);
int verificar_posicao_existe(pergunta *perguntas, int posicao_pergunta, int qtd_perguntas);
void imprimir_pergunta(pergunta pergunta);
// int entrada_invalida(char entrada);
void eliminar_pergunta(pergunta *perguntas, int posicao_pergunta, int qtd_perguntas);
void msg_derrota();
void msg_vitoria();
void imprimir_ajuda(int *ajudas);
void diminuir_quantidade_ajuda(int *ajudas, int id_ajuda);
int utilizar_ajuda(int *ajudas, int id_ajuda);

int main() {
    FILE *file;
    
    pergunta todas_perguntas[QTD_PERGUNTAS_TOTAL];
    pergunta pergunta_atual;
    pergunta *perguntas_por_nivel;

    int nvl_atual = 1; 
    int running = 1; 
    int qtd_perguntas_repondidas = 0;
    int qtd_perguntas_nvl = QTD_PERGUNTAS_NIVEIS_1_2_3;
    int qtd_respostas_nvl = 5;
    int pergunta_sorteada;
    int nao_sortear_pergunta = 0;
    int ajudas[4] = {3,3,3,3};
    int ajuda_utilizada = 0;

    float val_pergunta = 1000.0;
    float val_acumulado = 0.0;

    char entrada;

    carregar_perguntas(todas_perguntas);

    while (running)
    {
        // Verificar o nível do jogador para alocar a quantidade de memória correta para armazenar as perguntas
        if (nvl_atual == 4) {
            qtd_perguntas_nvl = QTD_PERGUNTAS_NIVEL_4;
            qtd_respostas_nvl = 1;
            
            pergunta *temp_ptr = (pergunta *)realloc(perguntas_por_nivel, qtd_perguntas_nvl * sizeof(pergunta));

            if (temp_ptr == NULL)
                break;

            perguntas_por_nivel = temp_ptr;
            free(temp_ptr);
        }
        else {
            qtd_perguntas_nvl = QTD_PERGUNTAS_NIVEIS_1_2_3;
            perguntas_por_nivel = (pergunta *)malloc(qtd_perguntas_nvl * sizeof(pergunta));
        }
        
        // Carregar somentes as perguntas do nível atual do jogador
        carregar_perguntas_por_nivel(todas_perguntas, perguntas_por_nivel, nvl_atual);

        while (qtd_perguntas_repondidas < qtd_respostas_nvl)
        {
            if (nao_sortear_pergunta == 0) {
                pergunta_sorteada = sortear_pergunta(qtd_perguntas_nvl);
                pergunta_atual = perguntas_por_nivel[pergunta_sorteada];
            }

            // =========== Vizualição da pergunta e opções ===========
            printf("== ------------\n");
            printf("== Pergunta %d - Nivel %d\n\n", qtd_perguntas_repondidas + 1, nvl_atual);            
            printf("== Voce possui um total acumulado de R$ %.2f ==\n\n", val_acumulado);
            imprimir_pergunta(pergunta_atual);
            imprimir_ajuda(ajudas);
            printf("~~~~~~~~~~~~~~~\n\n");
            // =======================================================

            printf("Digite sua resposta: ");
            scanf(" %c", &entrada);
            
            if (entrada == INPUT_5) {
                running = 0;
                break;
            }
            else if (entrada == INPUT_1 || entrada == INPUT_2 || entrada == INPUT_3 || entrada == INPUT_4) {

                ajuda_utilizada = utilizar_ajuda(ajudas, (int)(entrada - '0') - 1);
                if (ajuda_utilizada == 0) {
                    nao_sortear_pergunta = 1;
                    continue;
                }

            }
            else if (entrada == INPUT_A || entrada == INPUT_B || entrada == INPUT_C || entrada == INPUT_D)  {

                if (toupper(entrada) != toupper(pergunta_atual.alt_correta)) {
                    msg_derrota();
                    running = 0;
                    break;
                }

                printf("\n********** Certa Resposta! **********\n\n");
            }
            else {
                printf("\nXXXXXXXXX Entrada Invalida XXXXXXXXX\n\n");   
                nao_sortear_pergunta = 1;
                continue;
            }
            
            eliminar_pergunta(perguntas_por_nivel, pergunta_sorteada, qtd_perguntas_nvl);
 
            val_acumulado += val_pergunta;
            qtd_perguntas_repondidas ++;
            qtd_perguntas_nvl --;
        }

        if (running == 0)
            break;

        if (nvl_atual == 1) {
            val_acumulado = 10000.00;
            val_pergunta = 10000.00;
        }
        else if (nvl_atual == 2) {
            val_acumulado = 100000.00;
            val_pergunta = 100000.00;
        }
        else if (nvl_atual == 4) {
            msg_vitoria();
            break;
        }

        nvl_atual ++; 
        val_pergunta *= 10.00;
        qtd_perguntas_repondidas = 0;
        free(perguntas_por_nivel);
        perguntas_por_nivel = NULL;
    }

    return 0;
}

/*
=================================================================
=========================== FUNCTIONS ===========================
=================================================================
*/

void carregar_perguntas(pergunta *perguntas) {

    FILE *file = fopen("perguntas.dat", "rb");

    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }
    
    int count = 0;
    while (fread(&perguntas[count], sizeof(pergunta), 1, file) == 1) {        
        count ++;
    }

    fclose(file);

    return;
}

void carregar_perguntas_por_nivel(pergunta *todas_perguntas, pergunta *perguntas_por_nivel, int nvl_atual) {

    int contador = 0;
    for (int i = 0; i < QTD_PERGUNTAS_TOTAL; i++)
    {
        if (todas_perguntas[i].nivel == nvl_atual) {
            perguntas_por_nivel[contador] = todas_perguntas[i];
            contador ++;
        }
    }
}

int sortear_pergunta(int qtd_perguntas) {
    srand(time(0));

    return (rand() % qtd_perguntas);
}

void imprimir_pergunta(pergunta pergunta) {

        printf("%s\n", pergunta.descricao);
        for (int j = 0; j < 4; j++) {
            printf(" %c) %s\n", 'a' + j, pergunta.alt[j]);
        }

        printf("\n");
}

void eliminar_pergunta(pergunta *perguntas, int posicao_pergunta, int qtd_perguntas) {
    for (int i = posicao_pergunta; i < qtd_perguntas; i++)
    {
        perguntas[i] = perguntas[i + 1];
    }
}

void msg_derrota() {
    printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    printf("\nXXXXXXXXX VOCE PERDEU O JOGO :( XXXXXXXXX\n");
    printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n\n");
}

void msg_vitoria() {
    printf("\n*****************************************\n");
    printf("\n********** PARABENS, VOCE VENCEU! *******\n");
    printf("\n*****************************************\n");
    printf("\n********** VOCE É UM MILIONARIO! ********\n");
    printf("\n*****************************************\n");
    printf("\n********** VOCE GANHOU R$ 1.000.000! ****\n");
    printf("\n*****************************************\n\n");
}

void imprimir_ajuda(int *ajudas) {
    printf("-- Ajuda --\n");
    printf("[1] Pular pergunta (%d)\n", ajudas[0]);
    printf("[2] Pedir ajuda a plateia (%d)\n", ajudas[1]);
    printf("[3] Pedir ajuda aos universitarios (%d)\n", ajudas[2]);
    printf("[4] Pedir ajuda as cartas (%d)\n", ajudas[3]);
    printf("[5] Parar\n");
}

void diminuir_quantidade_ajuda(int *ajudas, int id_ajuda) {
    
    if (id_ajuda < 0 || id_ajuda > 3) {
        printf("Ajuda inválida.\n");
    }
    else if (ajudas[id_ajuda] > 0) {
        ajudas[id_ajuda]--;
        printf("\nAjuda %d utilizada. Restam %d.\n", id_ajuda + 1, ajudas[id_ajuda]);
    } else {
        printf("\nXXXXXXXXX Voce ja utilizou todas as ajudas da opcao %d. XXXXXXXXX\n", id_ajuda + 1);
    }

    printf("\n");
}

int utilizar_ajuda(int *ajudas, int id_ajuda) {
    int ajuda_utilizada = 0;

    if (id_ajuda == 0 && ajudas[id_ajuda] > 0) {
        printf("\n********** Pergunta Pulada! **********\n\n");
        ajuda_utilizada = 1;
    } else if (id_ajuda == 2 && ajudas[id_ajuda] > 0) {
        ajuda_utilizada = 1;
    } else if (id_ajuda == 3 && ajudas[id_ajuda] > 0) {
        ajuda_utilizada = 1;
    } else if (id_ajuda == 4 && ajudas[id_ajuda] > 0) {
        ajuda_utilizada = 1;
    }

    diminuir_quantidade_ajuda(ajudas, id_ajuda);

    return ajuda_utilizada;
}