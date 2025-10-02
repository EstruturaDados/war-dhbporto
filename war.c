#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ============================================================================
// --- 1. CONSTANTES ---
// ============================================================================
#define MAX_NOME 30
#define MAX_COR 10
#define MAX_MISSAO 150
#define TOTAL_MISSOES 5

// Vetor de Strings para Missões Estratégicas
const char* MISSOES_ESTRATEGICAS[TOTAL_MISSOES] = {
    "Dominar 5 territorios com pelo menos 3 tropas em cada um (Objetivo 1).",
    "Eliminar todas as tropas do exercito de cor 'Vermelho' (Objetivo 2).",
    "Dominar mais de 75%% dos territorios do mapa (Objetivo 3).",
    "Dominar todos os territorios que comecem com a letra 'A' ou 'B' (Objetivo 4).",
    "Possuir pelo menos 15 tropas distribuidas em seus territorios (Objetivo 5)."
};

// ============================================================================
// --- 2. ESTRUTURA DE DADOS (STRUCT) ---
// ============================================================================
typedef struct {
    char nome[MAX_NOME];
    char cor[MAX_COR]; // Cor do exercito que domina
    int tropas;
} Territorio;

// ============================================================================
// --- 3. PROTÓTIPOS DAS FUNÇÕES ---
// ============================================================================

// Setup e Memória
Territorio* alocarMapa(int *tamanho_mapa);
void cadastrarMapa(Territorio* mapa, int tamanho_mapa);
void liberarMemoria(Territorio* mapa, char* missao);
void limparBufferEntrada();

// Lógica de Missão
void atribuirMissao(char** destino, const char* missoes[], int totalMissoes);
int verificarMissao(const char* missao, const Territorio* mapa, int tamanho_mapa, const char* cor_jogador);
void exibirMissao(const char* missao, const char* cor_jogador);

// Lógica de Jogo
void atacar(Territorio* atacante, Territorio* defensor);
int rolarDado();
void ordenar_dados_desc(int dados[], int n);

// Interface
void exibirMapa(const Territorio* mapa, int tamanho_mapa);

// ============================================================================
// --- 4. FUNÇÃO PRINCIPAL (MAIN) ---
// ============================================================================
int main() {
    // CABECALHO
    printf("===================== WAR SIMULATOR =====================\n"); 
    
    // Inicializacao
    srand(time(NULL));
    int num_territorios = 0;
    
    // Alocacao
    Territorio* mapa = alocarMapa(&num_territorios);
    char* missao_jogador = NULL; 
    char cor_jogador_buffer[MAX_COR] = ""; 
    const char* COR_JOGADOR = cor_jogador_buffer;

    if (mapa == NULL || num_territorios == 0) {
        fprintf(stderr, "Erro na alocacao de memoria ou numero de territorios invalido. Encerrando.\n");
        return 1;
    }

    // Atribuicao da Missão
    atribuirMissao(&missao_jogador, MISSOES_ESTRATEGICAS, TOTAL_MISSOES);

    // Cadastro
    cadastrarMapa(mapa, num_territorios);
    
    // Determinação da Cor do Jogador (Baseada no primeiro território)
    strncpy(cor_jogador_buffer, mapa[0].cor, MAX_COR - 1);
    cor_jogador_buffer[MAX_COR - 1] = '\0';
    
    printf("\n--- CONFIGURACAO INICIAL CONCLUIDA ---\n");
    printf("A cor detectada para o JOGADOR PRINCIPAL é: %s\n", COR_JOGADOR);

    // Loop de Jogo com Menu Interativo
    int escolha_atacante, escolha_defensor, acao = 0;
    int jogador_venceu = 0;

    do {
        // Verifica a missão no início de cada "turno" lógico
        if (verificarMissao(missao_jogador, mapa, num_territorios, COR_JOGADOR)) {
            jogador_venceu = 1;
            acao = 0; // Sai do loop para declarar vitória
            break;
        }

        // Exibe a Missão
        exibirMissao(missao_jogador, COR_JOGADOR);
        
        // Exibe o Mapa (Única exibição por ciclo de jogo)
        exibirMapa(mapa, num_territorios); 
        
        printf("\n--- MENU DE ACOES ---\n");
        printf("1 - Atacar\n");
        printf("2 - Verificar Missao\n");
        printf("0 - Sair\n");
        printf("Escolha sua acao: ");
        
        if (scanf("%d", &acao) != 1) {
            limparBufferEntrada();
            acao = -1; // Opção inválida
        }
        limparBufferEntrada();

        switch (acao) {
            case 1: // ATACAR
                printf("\n--- FASE DE ATAQUE ---\n");
                printf("Escolha o territorio atacante (1 a %d): ", num_territorios);
                if (scanf("%d", &escolha_atacante) != 1) {
                    limparBufferEntrada();
                    printf("ID de ataque invalido. Tente novamente.\n");
                    continue;
                }
                limparBufferEntrada();
                
                printf("Escolha o territorio defensor (1 a %d): ", num_territorios);
                if (scanf("%d", &escolha_defensor) != 1) {
                    limparBufferEntrada();
                    printf("ID de defesa invalido. Tente novamente.\n");
                    continue;
                }
                limparBufferEntrada();

                int idx_atacante = escolha_atacante - 1;
                int idx_defensor = escolha_defensor - 1;

                // Validações
                if (idx_atacante < 0 || idx_atacante >= num_territorios || 
                    idx_defensor < 0 || idx_defensor >= num_territorios || 
                    idx_atacante == idx_defensor) {
                    printf("\nOpcao invalida: IDs fora do limite ou atacando o proprio territorio.\n");
                    continue;
                }

                Territorio *atacante = &mapa[idx_atacante];
                Territorio *defensor = &mapa[idx_defensor];
                
                // Validação de Cor
                if (strcmp(atacante->cor, COR_JOGADOR) != 0) {
                    printf("\nNAO PODE ATACAR: Territorio atacante nao pertence ao seu exercito (%s).\n", COR_JOGADOR);
                    continue;
                }

                if (strcmp(atacante->cor, defensor->cor) == 0) {
                    printf("\nNAO PODE ATACAR: Voce nao pode atacar um territorio da sua propria cor (%s).\n", atacante->cor);
                    continue;
                }
                
                if (atacante->tropas <= 1) {
                    printf("\nREQUISITO DE TROPAS: Territorio atacante (%s) precisa de no minimo 2 tropas para atacar (tem %d).\n", 
                           atacante->nome, atacante->tropas);
                    continue;
                }

                // Execucao da Batalha
                atacar(atacante, defensor);
                break;

            case 2: // VERIFICAR MISSÃO
                if (verificarMissao(missao_jogador, mapa, num_territorios, COR_JOGADOR)) {
                    printf("\n--- RESULTADO DA VERIFICACAO ---\n");
                    printf("MISSAO CUMPRIDA! Parabens!\n");
                    jogador_venceu = 1;
                    acao = 0; // Força a saída do loop
                } else {
                    printf("\n--- RESULTADO DA VERIFICACAO ---\n");
                    printf("Missao ainda NAO CUMPRIDA. Continue atacando!\n");
                }
                break;

            case 0: // SAIR
                printf("\nSaindo do jogo...\n");
                break;

            default:
                printf("\nOpcao invalida. Digite 1, 2 ou 0.\n");
                break;
        }

    } while (acao != 0);
    
    // Declaração do Vencedor
    printf("\n==============================================\n");
    if (jogador_venceu) {
        printf("PARABENS! O JOGADOR DE COR %s CUMPRIU SUA MISSAO ESTRATEGICA!\n", COR_JOGADOR);
        printf("MISSAO CONCLUIDA: %s\n", missao_jogador);
        printf("FIM DE JOGO: VITORIA POR OBJETIVO!\n");
    } else {
        printf("Jogo encerrado pelo jogador.\n");
    }

    // Liberacao de Memoria
    printf("\nEncerrando o programa. Liberando memoria alocada...\n");
    liberarMemoria(mapa, missao_jogador);
    printf("Memoria liberada com sucesso.\n");

    return 0; 
} 


// ============================================================================
// --- 5. IMPLEMENTAÇÃO DAS FUNÇÕES DE SETUP E UTILIDADE ---
// ============================================================================

int rolarDado() {
    return rand() % 6 + 1;
}

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ordenar_dados_desc(int dados[], int n) {
    int i, j, temp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (dados[j] < dados[j+1]) {
                temp = dados[j];
                dados[j] = dados[j+1];
                dados[j+1] = temp;
            }
        }
    }
}

Territorio* alocarMapa(int *tamanho_mapa) {
    int n;
    printf("Quantos territorios voce deseja no mapa (max 26)? ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > 26) {
        printf("Numero invalido. Definido 5 territorios por padrao.\n");
        n = 5;
    }
    limparBufferEntrada();
    *tamanho_mapa = n;
    return (Territorio*) calloc(n, sizeof(Territorio)); 
}

void liberarMemoria(Territorio* mapa, char* missao) {
    if (mapa != NULL) {
        free(mapa);
    }
    if (missao != NULL) {
        free(missao); 
    }
}

void cadastrarMapa(Territorio* mapa, int tamanho_mapa) {
    printf("\n--- CADASTRO MANUAL DE TERRITORIOS (%d Total) ---\n", tamanho_mapa);
    for (int i = 0; i < tamanho_mapa; i++) {
        Territorio* t = &mapa[i];

        printf("\nTerritorio %d:\n", i + 1);
        printf("Nome (max %d): ", MAX_NOME - 1);
        scanf(" %29s", t->nome); 
        limparBufferEntrada();

        printf("Cor do Exercito (Ex: Azul, Vermelho, max %d): ", MAX_COR - 1);
        scanf(" %9s", t->cor); 
        limparBufferEntrada();

        printf("Numero de Tropas (Min 1): ");
        if (scanf("%d", &t->tropas) != 1 || t->tropas < 1) {
            t->tropas = 1;
            printf("Valor invalido ou menor que 1. Definido 1 tropa.\n");
        }
        limparBufferEntrada();
    }
}

// ============================================================================
// --- 6. IMPLEMENTAÇÃO DAS FUNÇÕES DE MISSÃO ---
// ============================================================================

void atribuirMissao(char** destino, const char* missoes[], int totalMissoes) {
    int indice_sorteado = rand() % totalMissoes;
    const char* missao_selecionada = missoes[indice_sorteado];
    
    *destino = (char*) malloc(MAX_MISSAO * sizeof(char));

    if (*destino == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para a missao.\n");
        exit(1);
    }

    strncpy(*destino, missao_selecionada, MAX_MISSAO - 1);
    (*destino)[MAX_MISSAO - 1] = '\0'; 
}

void exibirMissao(const char* missao, const char* cor_jogador) {
    printf("\n--- SUA MISSAO (Exercito %s) ---\n", cor_jogador);
    if (missao != NULL) {
        printf("%s\n", missao);
    } else {
        printf("ERRO: Nenhuma missao atribuida.\n");
    }
}

int verificarMissao(const char* missao, const Territorio* mapa, int tamanho_mapa, const char* cor_jogador) {
    
    int territorios_jogador = 0;
    int tropas_jogador = 0;
    int territorios_vermelhos = 0;
    int territorios_A_B_total = 0;
    int territorios_A_B_dominados = 0;
    
    for (int i = 0; i < tamanho_mapa; i++) {
        if (strcmp(mapa[i].cor, cor_jogador) == 0) {
            territorios_jogador++;
            tropas_jogador += mapa[i].tropas;
        }

        if (strcmp(mapa[i].cor, "Vermelho") == 0) {
            territorios_vermelhos++;
        }
        
        if (mapa[i].nome[0] == 'A' || mapa[i].nome[0] == 'B') {
            territorios_A_B_total++;
            if (strcmp(mapa[i].cor, cor_jogador) == 0) {
                territorios_A_B_dominados++;
            }
        }
    }
    
    // Objetivo 1: Dominar 5 territórios com pelo menos 3 tropas.
    if (strstr(missao, "Objetivo 1") != NULL) {
        int cont_territorios_com_3_tropas = 0;
        for (int i = 0; i < tamanho_mapa; i++) {
            if (strcmp(mapa[i].cor, cor_jogador) == 0 && mapa[i].tropas >= 3) {
                cont_territorios_com_3_tropas++;
            }
        }
        if (cont_territorios_com_3_tropas >= 5) return 1;
    }

    // Objetivo 2: Eliminar todas as tropas do exercito de cor 'Vermelho'.
    if (strstr(missao, "Objetivo 2") != NULL) {
        if (territorios_vermelhos == 0) return 1;
    }
    
    // Objetivo 3: Dominar mais de 75% dos territórios do mapa.
    if (strstr(missao, "Objetivo 3") != NULL) {
        if ((float)territorios_jogador / tamanho_mapa > 0.75) return 1;
    }

    // Objetivo 4: Dominar todos os territórios que começam com 'A' ou 'B'.
    if (strstr(missao, "Objetivo 4") != NULL) {
        if (territorios_A_B_total > 0 && territorios_A_B_dominados == territorios_A_B_total) return 1;
    }

    // Objetivo 5: Possuir pelo menos 15 tropas.
    if (strstr(missao, "Objetivo 5") != NULL) {
        if (tropas_jogador >= 15) return 1;
    }
    
    return 0;
}

// ============================================================================
// --- 7. IMPLEMENTAÇÃO DAS FUNÇÕES DE LÓGICA DE JOGO ---
// ============================================================================

void atacar(Territorio* atacante, Territorio* defensor) {
    
    int n_dados_ataque = (atacante->tropas > 3) ? 3 : (atacante->tropas - 1);
    int n_dados_defesa = (defensor->tropas > 2) ? 2 : defensor->tropas;

    int dados_ataque[3] = {0, 0, 0};
    int dados_defesa[2] = {0, 0};

    // Rolar e ordenar dados
    for (int i = 0; i < n_dados_ataque; i++) dados_ataque[i] = rolarDado();
    for (int i = 0; i < n_dados_defesa; i++) dados_defesa[i] = rolarDado();

    ordenar_dados_desc(dados_ataque, n_dados_ataque);
    ordenar_dados_desc(dados_defesa, n_dados_defesa);

    // Comparar dados e contar perdas
    int num_comparacoes = (n_dados_ataque < n_dados_defesa) ? n_dados_ataque : n_dados_defesa;
    int perdas_atacante = 0;
    int perdas_defensor = 0;

    for (int i = 0; i < num_comparacoes; i++) {
        if (dados_ataque[i] > dados_defesa[i]) {
            perdas_defensor++; 
        } else {
            perdas_atacante++; 
        }
    }
    
    // --- RESULTADO DA BATALHA (Formato da Imagem) ---
    printf("\n--- RESULTADO DA BATALHA ---\n");
    // Mostra o MAIOR dado de cada lado.
    printf("Ataque (%s): %d | Defesa (%s): %d\n", 
           atacante->nome, dados_ataque[0], defensor->nome, dados_defesa[0]);

    if (perdas_defensor > 0) {
        printf("VITORIA DO ATAQUE! O defensor perdeu %d tropa(s).\n", perdas_defensor); 
    } else {
        printf("DERROTA DO ATAQUE! O atacante perdeu %d tropa(s).\n", perdas_atacante);
    }
    // ------------------------------------------------

    // Aplicar as perdas
    atacante->tropas -= perdas_atacante;
    defensor->tropas -= perdas_defensor;

    if (defensor->tropas <= 0) {
        // Conquista do Territorio
        printf("\nCONQUISTA! Territorio %s foi dominado por %s.\n", defensor->nome, atacante->cor);
        
        // 1. Mudanca de Dono
        strncpy(defensor->cor, atacante->cor, MAX_COR - 1);
        defensor->cor[MAX_COR - 1] = '\0'; 
        
        // 2. Transferencia de Tropas
        int max_tropas_moviveis = atacante->tropas - 1; 
        int tropas_movidas = (max_tropas_moviveis < n_dados_ataque) ? max_tropas_moviveis : n_dados_ataque;

        if (tropas_movidas < 1) tropas_movidas = 1; 

        atacante->tropas -= tropas_movidas;
        defensor->tropas = tropas_movidas;
        
        printf("MOVIMENTO DE TROPAS: %d tropas movidas de %s para %s.\n", 
               tropas_movidas, atacante->nome, defensor->nome);
    }

    // Garantir que o atacante tem pelo menos 1 tropa
    if (atacante->tropas < 1) {
        atacante->tropas = 1;
    }
}

// Funções de Interface
void exibirMapa(const Territorio* mapa, int tamanho_mapa) {
    // TEXTO ALTERADO AQUI
    printf("\n----------------------- MAPA DO MUNDO -----------------------\n");
    printf("| ID | %-28s | %-10s | %-6s |\n", "NOME DO TERRITORIO", "DONO", "TROPAS");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < tamanho_mapa; i++) {
        printf("| %-2d | %-28s | %-10s | %-6d |\n", 
               i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("------------------------------------------------------------\n");
}