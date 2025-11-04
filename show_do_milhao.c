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

#pragma region FUNCTIONS
void carregar_perguntas(pergunta *perguntas);
void carregar_perguntas_por_nivel(pergunta *todas_perguntas, pergunta *perguntas_por_nivel, int nvl_atual);
int sortear(int qtd);
int verificar_posicao_existe(pergunta *perguntas, int posicao_pergunta, int qtd_perguntas);
void imprimir_pergunta(pergunta pergunta);
// int entrada_invalida(char entrada);
void eliminar_pergunta(pergunta *perguntas, int posicao_pergunta, int qtd_perguntas);
void msg_derrota();
void msg_vitoria();
void msg_resposta_certa();
void msg_resposta_invalida();
void imprimir_ajuda(int *ajudas);
int diminuir_quantidade_ajuda(int *ajudas, int id_ajuda);
int utilizar_ajuda(int *ajudas, int id_ajuda, pergunta pergunta_atual);
int ajuda_cartas(pergunta pergunta_atual);
pergunta retirar_resposta_incorreta(int quantidade_resposta_incorretas, pergunta pergunta_atual);
char ler_entrada();
int pergunta_atual_ajuda(pergunta pergunta_atual);
void gerar_votos_alternativas(int qtd_votos, int percentual_correta, char alt_correta);
#pragma endregion

int main() {
    srand(time(0));

    #pragma region VARIABLES
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
    int id_ajuda;

    float val_pergunta = 1000.0;
    float val_acumulado = 0.0;

    char entrada;
    #pragma endregion

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
            if (!nao_sortear_pergunta) {
                pergunta_sorteada = sortear(qtd_perguntas_nvl);
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

            entrada = ler_entrada();

            if (entrada == INPUT_5) {
                running = 0;
            }
            else if (entrada == INPUT_1 || entrada == INPUT_2 || entrada == INPUT_3 || entrada == INPUT_4) {
                id_ajuda = (int)(entrada - '0') - 1;                

                if (!diminuir_quantidade_ajuda(ajudas, id_ajuda)) {
                    nao_sortear_pergunta = 1;
                    continue;
                }

                if (id_ajuda == 0) {
                    printf("\n********** Pergunta Pulada! **********\n\n");
                    nao_sortear_pergunta = 0;
                    continue;
                } else if (id_ajuda == 1) {
                    gerar_votos_alternativas(30, 40, pergunta_atual.alt_correta);
                    nao_sortear_pergunta = 1;
                    continue;
                } else if (id_ajuda == 2) {
                    gerar_votos_alternativas(3, 70, pergunta_atual.alt_correta);
                    nao_sortear_pergunta = 1;
                    continue;
                } else if (id_ajuda == 3) {                    
                    int carta_escolhida = ajuda_cartas(pergunta_atual);
                    pergunta_atual = retirar_resposta_incorreta(carta_escolhida, pergunta_atual);

                    running = pergunta_atual_ajuda(pergunta_atual);
                }
            }
            else if (entrada == INPUT_A || entrada == INPUT_B || entrada == INPUT_C || entrada == INPUT_D)  {

                if (toupper(entrada) != toupper(pergunta_atual.alt_correta)) {
                    msg_derrota();
                    running = 0;
                }
                else {
                    msg_resposta_certa();
                    nao_sortear_pergunta = 0;
                }
            }
            else {
                msg_resposta_invalida();
                nao_sortear_pergunta = 1;
                continue;
            }
            
            if (running == 0)
                break;

            eliminar_pergunta(perguntas_por_nivel, pergunta_sorteada, qtd_perguntas_nvl);
 
            val_acumulado += val_pergunta;
            qtd_perguntas_repondidas ++;
            qtd_perguntas_nvl --;
        }

        if (running == 0)
            break;

        if (nvl_atual == 1) {
            val_acumulado = 10000.0;
            val_pergunta = 10000.0;
        }
        else if (nvl_atual == 2) {
            val_acumulado = 100000.0;
            val_pergunta = 100000.0;
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

#pragma region FUNCTIONS
void msg_resposta_invalida() {
    printf("\nXXXXXXXXX Resposta Invalida XXXXXXXXX\n\n");
}

void msg_resposta_certa() {
    printf("\n********** Certa Resposta! **********\n\n");
}

char ler_entrada() {
    char entrada;
    printf("Digite a entrada: ");
    scanf(" %c", &entrada);
    return entrada;
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

int sortear(int qtd) {
    return (rand() % qtd);
}

void imprimir_pergunta(pergunta pergunta) {

    printf("%s\n", pergunta.descricao);
    for (int i = 0; i < 4; i++) {
        if (pergunta.alt[i][0] != '\0') {
            printf(" %c) %s\n", 'a' + i, pergunta.alt[i]);
        }
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

int diminuir_quantidade_ajuda(int *ajudas, int id_ajuda) {
    int ajuda_utilizada = 0;

    if (id_ajuda < 0 || id_ajuda > 3) {
        printf("Ajuda inválida.\n");
    }
    else if (ajudas[id_ajuda] > 0) {
        ajudas[id_ajuda]--;
        printf("\nAjuda %d utilizada. Restam %d.\n", id_ajuda + 1, ajudas[id_ajuda]);
        ajuda_utilizada = 1;
    } else {
        printf("\nXXXXXXXXX Voce ja utilizou todas as ajudas da opcao %d. XXXXXXXXX\n", id_ajuda + 1);
    }

    printf("\n");

    return ajuda_utilizada;
}

void desenhar_cartas() {
    printf("\n********** Ajuda das Cartas **********\n\n");
    printf(" _______   _______   _______   _______\n");
    printf("|       | |       | |       | |       |\n");
    printf("|       | |       | |       | |       |\n");
    printf("|       | |       | |       | |       |\n");
    printf("|_______| |_______| |_______| |_______|\n");
    printf("    1         2         3         4   \n");
}

void sortear_cartas(int *cartas) {
    int numero_respostas_tiradas;
    for (int i = 0; i < 4; i++)
    {
        numero_respostas_tiradas = sortear(4);
        cartas[i] = numero_respostas_tiradas;
        // printf("Carta %d: %d respostas incorretas retiradas\n", i + 1, numero_respostas_tiradas);
    }
    
};

int ajuda_cartas(pergunta pergunta_atual) {
    int *cartas = (int *) malloc(4 * sizeof(int));
    int carta_escolhida;

    sortear_cartas(cartas);

    while (1)
    {
        desenhar_cartas();
        printf("Escolha uma carta (1-4): ");
        scanf("%d", &carta_escolhida);
        
        if (carta_escolhida < 1 || carta_escolhida > 4) {
            msg_resposta_invalida();
            continue;
        }

        break;
    }
    
    return cartas[carta_escolhida - 1];
}

pergunta retirar_resposta_incorreta(int quantidade_resposta_incorretas, pergunta pergunta_atual) {
    int respostas_removidas = 0;

    while (respostas_removidas < quantidade_resposta_incorretas) {
        int indice = sortear(4);

        if ('a' + indice != pergunta_atual.alt_correta && pergunta_atual.alt[indice][0] != '\0') {
            pergunta_atual.alt[indice][0] = '\0';
            respostas_removidas++;
        }
    }

    printf("\n%d respostas incorretas removidas!\n\n", quantidade_resposta_incorretas);
    
    return pergunta_atual;
}

int pergunta_atual_ajuda(pergunta pergunta_atual) {
    char entrada;
    int running = 1;

    while (1)
    {                        
        imprimir_pergunta(pergunta_atual);        
        
        entrada = ler_entrada();

        if (entrada == INPUT_5) {
            running = 0;
        }        
        else if (entrada != INPUT_A && entrada != INPUT_B && entrada != INPUT_C && entrada != INPUT_D) {
            msg_resposta_invalida();
            continue;
        }
        else if (toupper(entrada) != toupper(pergunta_atual.alt_correta)) {
            msg_derrota();
            running = 0;
        }
        else {
            msg_resposta_certa();
        }

        break;
    }

    return running;
}

void imprimir_votos_alternativas(int *votos_alternativas) {    
    printf("\n-- Votos da Plateia --\n");
    printf("A: %d votos\n", votos_alternativas[0]);
    printf("B: %d votos\n", votos_alternativas[1]);
    printf("C: %d votos\n", votos_alternativas[2]);
    printf("D: %d votos\n", votos_alternativas[3]);
    printf("~~~~~~~~~~~~~~~\n\n");
}

// void gerar_votos_alternativas(int qtd_votos, int percentual_correta, char alt_correta) {
//     int sorteio;
//     int votos_alternativas[4] = {0,0,0,0};
//     int percentual_incorreta = (100 - percentual_correta) / 3;
//     int contador = 1;

//     for (int i = 0; i < qtd_votos; i++)
//     {
//         sorteio = sortear(100) + 1;

//         if (sorteio <= percentual_correta) {
//             votos_alternativas[0]++;
//         }
//         else if (sorteio <= percentual_correta + percentual_incorreta) {
//             votos_alternativas[1]++;
//         }
//         else if (sorteio <= percentual_correta + (percentual_incorreta * 2)) {
//             votos_alternativas[2]++;
//         } 
//         else {
//             votos_alternativas[3]++;
//         }
//     }
    
//     imprimir_votos_alternativas(votos_alternativas);
// }

void gerar_votos_alternativas(int qtd_votos, int percentual_correta, char alt_correta) {
    int i, sorteio;
    int votos_alternativas[4] = {0, 0, 0, 0};
    int alternativas[4] = { 'a', 'b', 'c', 'd' };
        
    int votos_corretos = 0;
    int votos_incorretos[3] = {0,0,0};
    
    int restante = 100 - percentual_correta;
    int percentual_base_incorreta = restante / 3;

    int faixa_correta = percentual_correta;
    int faixa_incorreta_1 = faixa_correta + percentual_base_incorreta;
    int faixa_incorreta_2 = faixa_incorreta_1 + percentual_base_incorreta;
    int faixa_incorreta_3 = faixa_incorreta_2 + percentual_base_incorreta;

    for (i = 0; i < qtd_votos; i++) {
        sorteio = sortear(100) + 1;

        if (sorteio <= faixa_correta) {
            votos_corretos++;
        }
        else if (sorteio <= faixa_incorreta_1) {
            votos_incorretos[0]++;
        }
        else if (sorteio <= faixa_incorreta_2) {
            votos_incorretos[1]++;
        } 
        else if (sorteio <= faixa_incorreta_3) {
            votos_incorretos[2]++;
        }
    }
        
    int indice_correto = alt_correta;
    
    int indice_incorreta = 0;

    for (i = 0; i < 4; i++) {
        if (alternativas[i] == indice_correto) {
            votos_alternativas[i] = votos_corretos;
        } else {            
            votos_alternativas[i] = votos_incorretos[indice_incorreta];
            indice_incorreta++;
        }
    }

    imprimir_votos_alternativas(votos_alternativas); 
}
#pragma endregion