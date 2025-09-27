#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME 30
#define MAX_COR  10

typedef struct {
    char nome[MAX_NOME];
    char cor[MAX_COR];
    int tropas;
} Territorio;

/* Estrutura simples para jogador */
typedef struct {
    char nome[MAX_NOME];
    char cor[MAX_COR];
    char *missao; /* alocada dinamicamente com malloc */
} Jogador;

/* --- Prototipos das funcoes --- */
Territorio* criarMapa(int n);
void exibirMapa(Territorio* mapa, int n);
void atacar(Territorio* atacante, Territorio* defensor);
void liberarMapa(Territorio* mapa);
void liberarJogadores(Jogador* jogadores, int n);
void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
void exibirMissao(const char* missao); 
int verificarMissao(char* missao, Territorio* mapa, int tamanho, const char* corJogador);

/* --- Função principal --- */
int main() {
    srand((unsigned)time(NULL)); // inicializa RNG

    printf("=== WAR Estruturado - Missões Estratégicas (versão iniciante) ===\n");

    int t, numJog;
    printf("Quantos territorios? ");
    if (scanf("%d", &t) != 1 || t <= 0) {
        printf("Quantidade invalida. Saindo.\n");
        return 1;
    }
    getchar(); // limpar \n

    Territorio* mapa = criarMapa(t);
    if (!mapa) {
        printf("Erro de memoria.\n");
        return 1;
    }

    /* cadastra jogadores */
    printf("\nQuantos jogadores? ");
    if (scanf("%d", &numJog) != 1 || numJog <= 0) {
        printf("Numero de jogadores invalido. Encerrando.\n");
        liberarMapa(mapa);
        return 1;
    }
    getchar();

    Jogador* jogadores = (Jogador*) calloc(numJog, sizeof(Jogador));
    if (!jogadores) {
        printf("Erro ao alocar memoria para jogadores.\n");
        liberarMapa(mapa);
        return 1;
    }

    for (int i = 0; i < numJog; ++i) {
        printf("\nNome do jogador %d: ", i);
        fgets(jogadores[i].nome, MAX_NOME, stdin);
        jogadores[i].nome[strcspn(jogadores[i].nome, "\n")] = 0;

        printf("Cor/dono do jogador %d (ex: Vermelho): ", i);
        fgets(jogadores[i].cor, MAX_COR, stdin);
        jogadores[i].cor[strcspn(jogadores[i].cor, "\n")] = 0;
    }

    /* --- vetor de missoes (pelo enunciado, pelo menos 5) --- */
    char *missoesTexto[] = {
        "Conquistar 3 territorios (ter 3 territorios no total).",
        "Eliminar todas as tropas da cor Vermelho.",
        "Ter pelo menos 10 tropas no total (somando seus territorios).",
        "Controlar 50% dos territorios do mapa.",
        "Controlar dois territorios adjacentes (na lista de cadastro)."
    };
    int totalMissoes = sizeof(missoesTexto) / sizeof(missoesTexto[0]);

    /* atribuir missao dinamicamente para cada jogador (malloc) */
    for (int i = 0; i < numJog; ++i) {
        /* aloca um espaço para guardar a string da missão do jogador */
        jogadores[i].missao = (char*) malloc(256); // 256 é suficiente pra nossa string
        if (!jogadores[i].missao) {
            printf("Erro ao alocar missao pro jogador %d\n", i);
            // limpar o que já foi alocado e sair
            liberarJogadores(jogadores, numJog);
            liberarMapa(mapa);
            return 1;
        }
        atribuirMissao(jogadores[i].missao, missoesTexto, totalMissoes);
        /* mostrar a missão apenas uma vez (instrução do enunciado) */
        printf("\n%s, sua missao sorteada: \n", jogadores[i].nome);
        exibirMissao(jogadores[i].missao);
    }

    /* LOOP SIMPLIFICADO DO JOGO: cada jogador faz um turno com opcao de atacar ou pular.
       Ao final de cada turno verificamos se missão foi cumprida. */
    int jogoAtivo = 1;
    while (jogoAtivo) {
        for (int p = 0; p < numJog; ++p) {
            printf("\n--- Turno de %s (%s) ---\n", jogadores[p].nome, jogadores[p].cor);
            printf("Mapa atual:\n");
            exibirMapa(mapa, t);

            printf("\nEscolha: 1-Atacar  2-Pular  0-Sair do jogo\nOpcao: ");
            int opc;
            if (scanf("%d", &opc) != 1) { opc = 2; }
            getchar();

            if (opc == 1) {
                /* realizar ataque simples: escolher indices */
                int ia, id;
                printf("Indice atacante: ");
                scanf("%d", &ia);
                printf("Indice defensor: ");
                scanf("%d", &id);
                getchar();

                if (ia < 0 || ia >= t || id < 0 || id >= t) {
                    printf("Indice invalido!\n");
                } else if (strcmp(mapa[ia].cor, jogadores[p].cor) != 0) {
                    printf("Esse territorio nao e seu (tem que atacar com territorio seu)!\n");
                } else if (strcmp(mapa[ia].cor, mapa[id].cor) == 0) {
                    printf("Nao pode atacar territorio da mesma cor!\n");
                } else if (mapa[ia].tropas < 2) {
                    printf("O territorio atacante precisa ter pelo menos 2 tropas.\n");
                } else {
                    atacar(&mapa[ia], &mapa[id]);
                }
            } else if (opc == 0) {
                jogoAtivo = 0;
                break;
            } else {
                printf("%s escolheu pular o turno.\n", jogadores[p].nome);
            }

            /* Ao final do turno, verificar se o jogador cumpriu a missão.
               Passamos a missão por referencia (string) e tambem a cor do jogador
               para que a funcao saiba quais territorios pertencem ao jogador. */
            int cumpriu = verificarMissao(jogadores[p].missao, mapa, t, jogadores[p].cor);
            if (cumpriu) {
                printf("\n***** PARABENS %s! Voce cumpriu sua missao e venceu o jogo! *****\n", jogadores[p].nome);
                jogoAtivo = 0;
                break;
            } else {
                printf("Missao nao cumprida ainda. Continua o jogo.\n");
            }
        } // fim for jogadores
    } // fim while jogoAtivo

    /* libera memoria: mapa e missoes alocadas */
    liberarJogadores(jogadores, numJog);
    liberarMapa(mapa);

    printf("Jogo encerrado. Memoria liberada. Tchau!\n");
    return 0;
}

/* ===================== FUNCOES ===================== */

/* Cria e cadastra o mapa dinamicamente (calloc) */
Territorio* criarMapa(int n) {
    Territorio* mapa = (Territorio*) calloc((size_t)n, sizeof(Territorio));
    if (!mapa) return NULL;

    for (int i = 0; i < n; ++i) {
        printf("\nCadastro Territorio %d:\n", i);
        printf("Nome: ");
        fgets(mapa[i].nome, MAX_NOME, stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = 0;

        printf("Cor/dono: ");
        fgets(mapa[i].cor, MAX_COR, stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = 0;

        printf("Tropas (>=1): ");
        while (scanf("%d", &mapa[i].tropas) != 1 || mapa[i].tropas < 1) {
            printf("Valor invalido. Informe um inteiro >= 1: ");
            getchar(); // limpa entrada
        }
        getchar(); // limpa \n remanescente
    }
    return mapa;
}

/* Mostrar mapa (acesso via ponteiro) */
void exibirMapa(Territorio* mapa, int n) {
    for (int i = 0; i < n; ++i) {
        printf("[%d] %s | Cor: %s | Tropas: %d\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

/* Função de ataque (igual ao enunciado, simples) */
void atacar(Territorio* atacante, Territorio* defensor) {
    int dadoA = (rand() % 6) + 1;
    int dadoD = (rand() % 6) + 1;

    printf("\nAtaque: %s (%s, %d) -> %s (%s, %d)\n",
           atacante->nome, atacante->cor, atacante->tropas,
           defensor->nome, defensor->cor, defensor->tropas);

    printf("Rolagem: atacante %d x defensor %d\n", dadoA, dadoD);

    if (dadoA > dadoD) {
        int transf = atacante->tropas / 2;
        if (transf < 1) transf = 1;
        printf("Atacante venceu! Transferindo %d tropas e mudando dono para %s\n", transf, atacante->cor);

        /* atualiza defensor */
        strncpy(defensor->cor, atacante->cor, MAX_COR-1);
        defensor->cor[MAX_COR-1] = '\0';
        defensor->tropas = transf;

        /* reduz tropas do atacante */
        atacante->tropas -= transf;
        if (atacante->tropas < 1) atacante->tropas = 1;
    } else {
        /* empate = derrota do atacante aqui (simplificação) */
        printf("Atacante perdeu a disputa. Perde 1 tropa.\n");
        atacante->tropas -= 1;
        if (atacante->tropas < 0) atacante->tropas = 0;
    }
}

/* Libera mapa (free) */
void liberarMapa(Territorio* mapa) {
    if (mapa) free(mapa);
}

/* Libera jogadores e suas missoes alocadas */
void liberarJogadores(Jogador* jogadores, int n) {
    if (!jogadores) return;
    for (int i = 0; i < n; ++i) {
        if (jogadores[i].missao) {
            free(jogadores[i].missao);
            jogadores[i].missao = NULL;
        }
    }
    free(jogadores);
}

/* Atribui uma missão aleatória (usa strcpy para copiar para destino).
   Aqui destino deve ter espaço suficiente (malloc feito pelo chamador). */
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int idx = rand() % totalMissoes;
    /* copiar texto da missão para o destino (passagem por referencia) */
    strcpy(destino, missoes[idx]);
    /* eu (o aluno) deixei simples: copiei direto, sem checagem extra */
}

/* Mostra a missão (passagem por valor: const char*). Só printa uma vez. */
void exibirMissao(const char* missao) {
    printf(" -> %s\n", missao);
    printf("(Guarde bem, ela sera verificada durante o jogo.)\n");
}

int verificarMissao(char* missao, Territorio* mapa, int tamanho, const char* corJogador) {
    if (!missao || !mapa || !corJogador) return 0;

    // 1) Conquistar 3 territorios (ter 3 territorios no total)
    if (strncmp(missao, "Conquistar 3 territorios", 24) == 0) {
        int contador = 0;
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].cor, corJogador) == 0) contador++;
        }
        if (contador >= 3) return 1;
        return 0;
    }

    // 2) Eliminar todas as tropas da cor Vermelho
    if (strncmp(missao, "Eliminar todas as tropas da cor Vermelho", 40) == 0) {
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].cor, "Vermelho") == 0 && mapa[i].tropas > 0) return 0;
        }
        return 1; // nao encontrou tropas da cor Vermelho
    }

    // 3) Ter pelo menos 10 tropas no total
    if (strncmp(missao, "Ter pelo menos 10 tropas", 24) == 0) {
        int soma = 0;
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].cor, corJogador) == 0) soma += mapa[i].tropas;
        }
        if (soma >= 10) return 1;
        return 0;
    }

    // 4) Controlar 50% dos territorios do mapa
    if (strncmp(missao, "Controlar 50% dos territorios", 30) == 0) {
        int owns = 0;
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].cor, corJogador) == 0) owns++;
        }
        if (owns * 100 >= 50 * tamanho) return 1; // owns/tamanho >= 0.5
        return 0;
    }

    // 5) Controlar dois territorios adjacentes (na lista)
    if (strncmp(missao, "Controlar dois territorios adjacentes", 37) == 0) {
        for (int i = 0; i < tamanho - 1; ++i) {
            if (strcmp(mapa[i].cor, corJogador) == 0 && strcmp(mapa[i+1].cor, corJogador) == 0) {
                return 1;
            }
        }
        return 0;
    }
    return 0;
}
