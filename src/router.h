#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>

#define TAM_FILA 93
#define TAM_MSG 100
#define MAX_ROT 4
#define INF 100
#define TEMPO_ENVIO 3

#define MSG 0
#define DISTANCIA_VETOR 1

#define TRUE 1
#define FALSE 2

// estrutura dos roteadores
typedef struct {
    int id, port;
    char ip[30];
} Roteador;

typedef struct {
    int cost[MAX_ROT];
} Distancia_vetor;

//estrutura dos pacotes
typedef struct {
    int msg_id;
    int source, destination, type; //header (origem, destino e tipo de msg)
    char conteudo[TAM_MSG];        //mensagem
    Distancia_vetor dv[MAX_ROT];
} Pacote;

//estrutura tabelas de roteamento
typedef struct {
    int cost, next;
} Tabela;

typedef struct {
    int port, cost;
    char ip[30];
} Vizinho;

typedef struct {
    int id_origem, id_destino;
    char* hora;
} DV_log;


Roteador        roteador;
Tabela          roteamento_tabela[MAX_ROT];
Vizinho         vizinho_tabela[MAX_ROT];
Distancia_vetor dv_tabela[MAX_ROT];
Pacote          msg_in[TAM_FILA], msg_out; //filas do roteador
DV_log          dv_logs[100];

int sckt, id, dv_alterado = 1, msg_flag, msg_control_in = 0, msg_id_control = 0, count_log = 0;
int flag_estado[MAX_ROT];
time_t timer;

struct sockaddr_in socket_in_me, socket_other;

//threads
pthread_t       recebe, envia_msg, envia_vetor, estado_roteador;
pthread_mutex_t envia_mutex = PTHREAD_MUTEX_INITIALIZER;

void cabecalho_menus();