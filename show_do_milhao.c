#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define QTD_PERGUNTAS_TOTAL 70
#define QTD_PERGUNTAS_NIVEIS_1_2_3 20
#define QTD_PERGUNTAS_NIVEL_4 10

typedef enum {
    ALT_A = 'A',
    ALT_B = 'B',
    ALT_C = 'C',
    ALT_D = 'D',
    ALT_1 = '1',
    ALT_2 = '2',
    ALT_3 = '3',
    ALT_4 = '4',
    ALT_5 = '5',
} Alternativas; // PAREI AQUI

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

int main() {
    FILE *file;
    
    pergunta todas_perguntas[QTD_PERGUNTAS_TOTAL],
    *perguntas_por_nivel;

    int nvl_atual = 1, 
    running = 1, 
    qtd_perguntas_repondidas = 0,
    qtd_perguntas_nvl = QTD_PERGUNTAS_NIVEIS_1_2_3,
    pergunta_sorteada;

    float val_inicial = 0.0, 
    val_pergunta = 0.0, 
    val_acumulado = 0.0;

    char entrada;

    carregar_perguntas(todas_perguntas);

    // for (int i = 0; i < QTD_PERGUNTAS; i++) {
    //     printf("\n[%d] Pergunta\n", i + 1);
    //     printf("Nivel: %c\n", perguntas[i].nivel);
    //     printf("Descrição: %s\n", perguntas[i].descricao);
    //     printf("Alternativas:\n");   
    //     for (int j = 0; j < 4; j++) {
    //         // Imprime A), B), C) ou D)
    //         printf(" %c) %s\n", 'A' + j, perguntas[i].alt[j]);
    //     }  
    //     printf("Alternativa correta: %c\n", perguntas[i].alt_correta);
    //     printf("------------------------------------------\n");
    // }

    while (running)
    {

        // Verificar o nível do jogador para alocar a quantidade de memória correta para armazenar as perguntas
        if (nvl_atual == 4) {
            qtd_perguntas_nvl = QTD_PERGUNTAS_NIVEL_4;
        }
        perguntas_por_nivel = malloc(qtd_perguntas_nvl * sizeof(pergunta));
        
        // Carregar somentes as perguntas do nível atual do jogador
        carregar_perguntas_por_nivel(todas_perguntas, perguntas_por_nivel, nvl_atual);

        while (1)
        {
            pergunta_sorteada = sortear_pergunta(qtd_perguntas_nvl);
            
            printf("==------------\n");
            printf("== Pergunta %d", qtd_perguntas_repondidas + 1);            
            printf("== Voce possui um total acumulado de R$ %f ==", val_acumulado);

            imprimir_pergunta(perguntas_por_nivel[pergunta_sorteada]);

            // imprimir opções de ajuda

            printf("~~~~~~~~~~~~~~~");

            break;
        }


        // for (int i = 0; i < 20; i++) {
        //     printf("\n[%d] Pergunta\n", i + 1);
        //     printf("Nivel: %c\n", perguntas_por_nivel[i].nivel);
        //     printf("Descrição: %s\n", perguntas_por_nivel[i].descricao);
        //     printf("Alternativas:\n");            
        //     for (int j = 0; j < 4; j++) {
        //         // Imprime A), B), C) ou D)
        //         printf(" %c) %s\n", 'A' + j, perguntas_por_nivel[i].alt[j]);
        //     }            
        //     printf("Alternativa correta: %c\n", perguntas_por_nivel[i].alt_correta);
        //     printf("------------------------------------------\n");
        // }
        running = 0;
    }

    
    return 0;
}

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
}