#include "src/router.h"
#include "src/funcoesMsg.c"
#include "src/funcoesDv.c"
#include "src/uteis.c"

//configurar o roteador com base no arquivo router.config
void config_roteador(int id_logado) {

    FILE *config = fopen("roteador.config", "r");

    if (config) {

        int id_origem, port;
        char ip[30];

        for (int i = 0; fscanf(config, "%d %d %s", &id_origem, &port, ip) != EOF; i++) {
            if (id_origem == id_logado) {
                roteador.id     = id;
                roteador.port   = port;
                strcpy(roteador.ip, ip);
                break;
            } 
        }

    } else printf("Erro ao configurar o roteador. Pressione ENTER para voltar.");

    fclose(config);

    if((sckt = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("Erro ao criar socket\n");

    memset((char *) &socket_in_me, 0, sizeof(socket_in_me));

    socket_in_me.sin_family        = AF_INET;
    socket_in_me.sin_port          = htons(roteador.port);
    socket_in_me.sin_addr.s_addr   = htonl(INADDR_ANY);

    if (bind(sckt, (struct sockaddr*)&socket_in_me, sizeof(socket_in_me)) == -1)
        die("Erro ao dar bind\n");
}

void topologia_rede (int id_logado) {

    FILE *links  = fopen("enlaces.config" , "r");
    FILE *config = fopen("roteador.config", "r");

    int id_roteador, id_destino, distancia;
    int id_origem, porta_socket;
    char ip[30];

    if (!(links && config)) {
        printf("Falha ao encontrar arquivos de enlace.config ou roteador.config\n");
        return;
    }

    for(int i = 0; fscanf(links, "%d %d %d", &id_roteador, &id_destino, &distancia) != EOF; i++) {
        for(int j = 0; fscanf(config, "%d %d %s", &id_origem, &porta_socket, ip) != EOF; j++) {
            
            if(id_logado == id_roteador && id_origem == id_destino) {
                vizinho_tabela[id_destino].port = porta_socket;

                strcpy(vizinho_tabela[id_destino].ip, ip);

                roteamento_tabela[id_destino].next    = id_destino;
                roteamento_tabela[id_destino].cost    = distancia;
                vizinho_tabela[id_destino].cost       = distancia;
                dv_tabela[id_logado].cost[id_destino] = distancia;
                
                continue;
            }

            if(id_logado == id_destino && id_origem == id_roteador) {

                vizinho_tabela[id_roteador].port = porta_socket;

                strcpy(vizinho_tabela[id_roteador].ip, ip);

                roteamento_tabela[id_roteador].next     = id_roteador;
                roteamento_tabela[id_roteador].cost     = distancia;
                vizinho_tabela[id_roteador].cost        = distancia;
                dv_tabela[id_logado].cost[id_roteador]  = distancia;

            }
        }

        rewind(config);
    }
}

void *recebe_pacote(void *n) {

    int slen = sizeof(socket_other);

    while(1){

        Pacote pkg_in;

        if ((recvfrom(sckt, &pkg_in, sizeof(pkg_in), 0, (struct sockaddr*) &socket_in_me, &slen)) == -1) 
            printf("Erro ao receber a mensagem\n");
        
        if (pkg_in.type == MSG) {
            if (pkg_in.destination == id) {

                printf("\n\nMensagem recebida do roteador %d.\n\n", pkg_in.source);
                msg_in[msg_control_in] = pkg_in;
                msg_control_in++;

            } else {

                msg_out = pkg_in;
                printf("\nRetransmitindo de %d para %d\n", id, roteamento_tabela[msg_out.destination].next);
                msg_flag = 1;

            }

        } else if (pkg_in.type == DISTANCIA_VETOR) {

            flag_estado[pkg_in.source] = 1;

            for (int i = 0; i < MAX_ROT; i++) {
                if (pkg_in.dv[pkg_in.source].cost[i] == INF && roteamento_tabela[i].next == pkg_in.source) {

                    dv_tabela[id].cost[i]       = INF;
                    dv_tabela[i].cost[id]       = INF;
                    roteamento_tabela[i].next   = -1;
                    dv_alterado                 = TRUE;

                }       

                dv_tabela[pkg_in.source].cost[i] = pkg_in.dv[pkg_in.source].cost[i];

                if (dv_tabela[id].cost[i] > dv_tabela[pkg_in.source].cost[i] + dv_tabela[id].cost[pkg_in.source] && 
                    dv_tabela[pkg_in.source].cost[i] + dv_tabela[id].cost[pkg_in.source] <= INF) {
                
                    dv_tabela[id].cost[i]       = dv_tabela[pkg_in.source].cost[i] + dv_tabela[id].cost[pkg_in.source];
                    roteamento_tabela[i].cost   = dv_tabela[pkg_in.source].cost[i] + dv_tabela[id].cost[pkg_in.source];
                    roteamento_tabela[i].next   = pkg_in.source;
                    dv_alterado = TRUE;
                
                }
            }           
        }
    }
}

void inicializa_tabelas() {

    for (int i = 0; i < MAX_ROT; i++) {

        vizinho_tabela[i].port = -1;
        vizinho_tabela[i].cost = INF;
        
        roteamento_tabela[i].cost = INF;
        roteamento_tabela[i].next = -1;

        for (int j = 0; j < MAX_ROT; j++) {

            dv_tabela[i].cost[j] = INF;  

            if (roteamento_tabela[j].next == i) 
                roteamento_tabela[j].next = -1;

        }
    }

    vizinho_tabela[id].cost    = 0;
    dv_tabela[id].cost[id]     = 0;
    roteamento_tabela[id].cost = 0;
    roteamento_tabela[id].next = id;

}

void *checa_estado(void *n) {
<<<<<<< HEAD
    time_t timer;
    timer = time(0);
=======
>>>>>>> 39e918e2ef77089f72f0c1cb6b896f2de31684dc

    while (1) {
        if (difftime(time(0), timer) >= 3 * TEMPO_ENVIO) {
            pthread_mutex_lock(&envia_mutex);
            
            for (int i = 0; i < MAX_ROT; i++) {
                if (flag_estado[i] != 1 && i != id && vizinho_tabela[i].cost != INF) {
                    for (int j = 0; j < MAX_ROT; j++)
                        dv_tabela[id].cost[j] = vizinho_tabela[j].cost;

                    dv_tabela[id].cost[i]     = INF;
                    dv_tabela[i].cost[id]     = INF;
                    vizinho_tabela[i].cost    = INF;
                    roteamento_tabela[i].next = -1;
                    roteamento_tabela[i].cost = INF;
                    dv_alterado               = TRUE;

                }
            }
          
            memset(flag_estado, 0, sizeof(flag_estado));
            pthread_mutex_unlock(&envia_mutex);
            timer = time(0);

        }
    }
}

void inicializa_threads() {

    pthread_create(&recebe         , NULL, recebe_pacote  , NULL);
    pthread_create(&envia_vetor    , NULL, envia_dv       , NULL);
    pthread_create(&envia_msg      , NULL, envia_mensagem , NULL);
    pthread_create(&estado_roteador, NULL, checa_estado   , NULL);

}

int main() {

    id = login();

    memset((char *) &socket_other, 0, sizeof(socket_other));

    socket_other.sin_family      = AF_INET;
    socket_other.sin_addr.s_addr = htonl(INADDR_ANY);

    config_roteador(id);
    inicializa_tabelas();

    memset(flag_estado, 0, sizeof(flag_estado));

    topologia_rede(id);
    inicializa_threads();

    menu();

}