// ============================================================================
//PROJETO WAR ESTRUTURADO - DESAFIOS DE CÓDIGO
// ============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ============================================================================
// --- 1. CONSTANTES ---
// ============================================================================
#define MAX_NOME 30
#define MAX_COR 10

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
void liberarMemoria(Territorio* mapa);

// Lógica de Jogo
void atacar(Territorio* atacante, Territorio* defensor);
int rolarDado();

// Interface
void exibirMapa(const Territorio* mapa, int tamanho_mapa);
void limparBufferEntrada();

// ============================================================================
// --- 4. FUNÇÃO PRINCIPAL (MAIN) ---
// ============================================================================
int main() {
    // CABECALHO
    printf("===================== WAR =====================\n\n\n"); 
    
    // Inicializacao
    srand(time(NULL));
    int num_territorios = 0;
    
    // Alocacao
    Territorio* mapa = alocarMapa(&num_territorios);

    if (mapa == NULL || num_territorios == 0) {
        fprintf(stderr, "Erro na alocacao de memoria ou numero de territorios invalido. Encerrando.\n");
        return 1;
    }

    // Cadastro e Exibicao Inicial
    cadastrarMapa(mapa, num_territorios);
    printf("\n--- CONFIGURACAO INICIAL CONCLUIDA ---\n");
    exibirMapa(mapa, num_territorios);

    // Loop de Jogo
    int escolha_atacante, escolha_defensor, continuar = 1;

    while (continuar) {
        printf("\n==============================================\n");
        printf("--- FASE DE ATAQUE ---\n");
        printf("Digite o ID do Territorio Atacante (1 a %d) ou 0 para sair: ", num_territorios);
        if (scanf("%d", &escolha_atacante) != 1 || escolha_atacante == 0) {
            continuar = 0;
            break;
        }
        limparBufferEntrada();
        
        printf("Digite o ID do Territorio Defensor (1 a %d): ", num_territorios);
        if (scanf("%d", &escolha_defensor) != 1) {
            limparBufferEntrada();
            printf("ID invalido. Tente novamente.\n");
            continue;
        }
        limparBufferEntrada();

        int idx_atacante = escolha_atacante - 1;
        int idx_defensor = escolha_defensor - 1;

        // Validacao de Limites
        if (idx_atacante < 0 || idx_atacante >= num_territorios || 
            idx_defensor < 0 || idx_defensor >= num_territorios || 
            idx_atacante == idx_defensor) {
            printf("\nOpcao invalida: IDs fora do limite ou atacando o proprio territorio.\n");
            continue;
        }

        Territorio *atacante = &mapa[idx_atacante];
        Territorio *defensor = &mapa[idx_defensor];
        
        // Validacao de Regras
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

        exibirMapa(mapa, num_territorios);
    }

    // Liberacao de Memoria
    printf("\nEncerrando o programa. Liberando memoria alocada...\n");
    liberarMemoria(mapa);
    printf("Memoria liberada com sucesso.\n");

    return 0; 
} 

// ============================================================================
// --- 5. IMPLEMENTAÇÃO DAS FUNÇÕES ---
// ============================================================================

// Funções de Utilidade
int rolarDado() {
    return rand() % 6 + 1;
}

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Funções de Memória e Setup
Territorio* alocarMapa(int *tamanho_mapa) {
    int n;
    printf("Quantos territorios voce deseja no mapa (max 26)? ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > 26) {
        printf("Numero invalido. Definindo 5 territorios por padrao.\n");
        n = 5;
    }
    limparBufferEntrada();
    *tamanho_mapa = n;
    return (Territorio*) calloc(n, sizeof(Territorio));
}

void liberarMemoria(Territorio* mapa) {
    if (mapa != NULL) {
        free(mapa);
    }
}

void cadastrarMapa(Territorio* mapa, int tamanho_mapa) {
    printf("\n--- CADASTRO MANUAL DE TERRITORIOS (%d Total) ---\n", tamanho_mapa);
    for (int i = 0; i < tamanho_mapa; i++) {
        Territorio* t = &mapa[i];

        printf("\nTerritorio %d:\n", i + 1);
        printf("Nome: ");
        scanf(" %29s", t->nome);
        limparBufferEntrada();

        printf("Cor do Exercito (Ex: Azul, Vermelho): ");
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

// Função de Lógica Principal
void atacar(Territorio* atacante, Territorio* defensor) {
    
    int dado_ataque = rolarDado(); 
    int dado_defesa = rolarDado(); 

    // Saida formatada conforme requisito
    printf("\n--- RESULTADO DA BATALHA ---\n");
    printf("O atacante %s rolou um dado e tirou: %d\n", atacante->nome, dado_ataque);
    printf("O defensor %s rolou um dado e tirou: %d\n", defensor->nome, dado_defesa);
    
    if (dado_ataque > dado_defesa) {
        // Atacante vence
        defensor->tropas--;
        printf("VITORIA DO ATAQUE! O defensor perdeu 1 tropa.\n"); 
        
        if (defensor->tropas == 0) {
            // Conquista do Territorio
            printf("\nCONQUISTA! Territorio %s foi dominado por %s.\n", defensor->nome, atacante->cor);
            
            // 1. Mudanca de Dono
            strcpy(defensor->cor, atacante->cor);
            
            // 2. Transferencia de Tropas (Metade das tropas atacantes)
            int tropas_movidas = atacante->tropas / 2;
            if (tropas_movidas == 0) tropas_movidas = 1; 

            atacante->tropas -= tropas_movidas;
            defensor->tropas = tropas_movidas;
            
            printf("MOVIMENTO DE TROPAS: %d tropas movidas de %s para %s.\n", 
                   tropas_movidas, atacante->nome, defensor->nome);
        }

    } else {
        // Defensor vence ou empate
        atacante->tropas--;
        printf("DERROTA! O defensor venceu ou empatou. O atacante perdeu 1 tropa.\n");
    }
    
    // Garantir minimo de 1 tropa no atacante
    if (atacante->tropas < 1) {
        atacante->tropas = 1;
    }
}

// Funções de Interface
void exibirMapa(const Territorio* mapa, int tamanho_mapa) {
    printf("\n------------------- ESTADO ATUAL DO MAPA -------------------\n");
    printf("| ID | %-28s | %-10s | %-6s |\n", "NOME DO TERRITORIO", "DONO", "TROPAS");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < tamanho_mapa; i++) {
        // Usa const para garantir que os dados sao apenas lidos.
        printf("| %-2d | %-28s | %-10s | %-6d |\n", 
               i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("------------------------------------------------------------\n");
}