// gcc router.c -o router -lpthread
// ./router

#include "router.h"

Roteador        roteador;
Tabela          roteamento_tabela[MAX_ROT];
Vizinho         vizinho_tabela[MAX_ROT];
Distancia_vetor dv_tabela[MAX_ROT];
Pacote          msg_in[TAM_FILA], msg_out; //filas do roteador

int sckt, id, dv_alterado = 1, msg_flag, msg_control_in = 0, msg_id_control = 0;
int alive_flag[MAX_ROT];

struct sockaddr_in socket_in_me, socket_other;

//threads
pthread_t       recebe, envia_msg, envia_vetor, still_alive;
pthread_mutex_t envia_mutex = PTHREAD_MUTEX_INITIALIZER;

//coverter char pra int copiei da internet
int toint(char *str) {

  int ans;
  ans = 0;

  for(int i = strlen(str) - 1, pot = 1; i >= 0; i--, pot *= 10)
    ans += pot * (str[i] - '0');
    
  return ans;   

}

void die(char *s){

  perror(s);
  exit(1);

}

void ler_msg() {

    system("clear");
    printf("___________~<>~___________\n");

    for (int i = 0; i <= msg_control_in; i++) {
        if (i < msg_control_in && msg_in[i].source != 0) {
        
            printf("De: %d\n", msg_in[i].source);
            printf("%s\n\n", msg_in[i].conteudo);
        
        }        
    }

    printf("___________~<>~___________\n");
    printf("Pressione ENTER para voltar\n");
    getchar(); 
    getchar();

}

Pacote criar_msg (int type, int destino) {

    Pacote msg;

    msg.source      = id;
    msg.destination = destino;
    msg.type        = type;

    if (type == MSG) {
        msg.msg_id = msg_control_in++;
        
        printf("Digite sua mensagem... \n");
        getchar();

        fgets(msg.conteudo, TAM_MSG, stdin);
        return msg;

    } else if (type == DISTANCIA_VETOR) {

        msg.dv[id] = dv_tabela[id];
        return msg;

    }

    return msg;

}

void enviar_msg() {

    int destino = -1;

    system("clear");

    printf("___________~<>~___________\n");
    printf("Informe o roteador de destino: ");
    scanf("%d", &destino);

    if (destino < 0 || destino >= MAX_ROT) {
        printf("O roteador não existe. Pressione ENTER para voltar.");
        getchar();
    }

    if (roteamento_tabela[destino].next == -1) {
        printf("Impossível chegar ao roteador. Pressione ENTER para voltar.");
        getchar();
    }

    if (destino == id) {
        printf("Você não pode enviar mensagens a si mesmo. Pressione ENTER para voltar.");
        getchar();
    }

    msg_out = criar_msg(MSG, destino);
    msg_flag = 1;

    printf("___________~<>~___________\nMensagem enviada. Pressione ENTER para voltar");
    getchar();

}

//configurar o roteador com base no arquivo router.config
void config_roteador(int id_logado) {

    FILE *config = fopen("roteador.config", "r");

    if (config) {

        int id_origem, port;
        char ip[30];

        for (int i = 0; fscanf(config, "%d %d %s", &id_origem, &port, ip) != EOF; i++) {
            if (id_origem != id_logado) continue;
            else {

                roteador.id     = id;
                roteador.port   = port;
                strcpy(roteador.ip, ip);

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

                roteamento_tabela[id_destino].next = id_destino;
                roteamento_tabela[id_destino].cost = distancia;
                vizinho_tabela[id_destino].cost    = distancia;
                dv_tabela[id_logado].cost[id_destino]   = distancia;
                
                continue;
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

            alive_flag[pkg_in.source] = 1;

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

void *envia_mensagem(void *n){
  Pacote msg;
  int next;

  while(1){
    if(msg_flag){
      pthread_mutex_lock(&envia_mutex);

      msg = msg_out;
      next = roteamento_tabela[msg_out.destination].next;

      socket_other.sin_port = htons(vizinho_tabela[next].port);

      if(inet_aton(vizinho_tabela[next].ip, &socket_other.sin_addr) == 0)
        die("Erro na obtenção do IP do destino (Mensagem)\n");
      else
      if(sendto(sckt, &msg, sizeof(msg), 0, (struct sockaddr*) &socket_other, sizeof(socket_other)) == -1)
        die("Erro ao enviar mensagem\n");

      msg_flag = 0;

      pthread_mutex_unlock(&envia_mutex);
    }
  }
}

void transfer_dv(char why){
  Pacote msg;

  for(int n_id = 0; n_id < MAX_ROT; n_id++){
    if(vizinho_tabela[n_id].cost != INF && vizinho_tabela[n_id].port != -1){
      msg = criar_msg(DISTANCIA_VETOR, n_id);
      socket_other.sin_port = htons(vizinho_tabela[n_id].port);

      if(inet_aton(vizinho_tabela[n_id].ip, &socket_other.sin_addr) == 0)
        die("Erro na obtenção do IP do destino (Vetores Distância)\n");
      else if(sendto(sckt, &msg, sizeof(msg), 0, (struct sockaddr*) &socket_other, sizeof(socket_other)) == -1)
        die("Erro ao enviar vetores distância\n");
      /*else if(why == 'C')
        printf("\nRoteador %d enviando vetores distância para roteador %d. Houve mudança na tabela de roteamento.\n", id, n_id);*/
    }
  }
}

void *envia_dv(void *n){

  time_t timer;

  timer = time(0);

  sleep(1);

  while(1){

    pthread_mutex_lock(&envia_mutex);

    double dv_exec_time = difftime(time(0), timer);

    if (dv_alterado == TRUE) {

      transfer_dv('C');
      dv_alterado = FALSE;
      timer = time(0);

    } else if (dv_exec_time >= TEMPO_ENVIO) {

      transfer_dv('T');
      timer = time(0);

    }

    pthread_mutex_unlock(&envia_mutex);

  }
}

int login() {

    int id;

    system("clear");
	printf("___________~<>~___________\n");
    printf("SEJA BEM-VINDO AO CHAT UOL!\n\n");

    while(1) {

        printf("Por favor, informe o login: ");
        scanf("%d", &id);

        if (id < 0 || id >= MAX_ROT) 
            printf("Login inválido.\n");
        else
            break;

    };
    
    return id; 
    
}

void inicializa_tabelas() {

    for (int i = 0; i < MAX_ROT; i++) {

        vizinho_tabela[i].port = -1;
        vizinho_tabela[i].cost = INF;
        
        roteamento_tabela[i].cost = INF;
        roteamento_tabela[i].next = -1;

        for (int j = 0; j < MAX_ROT; j++) {

            dv_tabela[i].cost[j] = INF;     
            if (roteamento_tabela[j].next == i) roteamento_tabela[j].next = -1;

        }
    }

    vizinho_tabela[id].cost    = 0;
    dv_tabela[id].cost[id]     = 0;
    roteamento_tabela[id].cost = 0;
    roteamento_tabela[id].next = id;

}

void inicializa_threads() {

    pthread_create(&recebe     , NULL, recebe_pacote  , NULL);
    pthread_create(&envia_vetor, NULL, envia_dv       , NULL);
    pthread_create(&envia_msg  , NULL, envia_mensagem , NULL);

}

void menu() {

    int op;

    do {

		system("clear");
		printf("___________~<>~___________\n");
        printf("      ROTEADOR %d\n\n", id);
		printf("|0| Sair\n");
		printf("|1| Enviar Mensagem\n");
		printf("|2| Visualizar Mensagem\n");
		printf("___________~<>~___________\n");
		printf("Escolha sua opção: ");
		scanf("%d", &op);

		switch(op){ 
			case 1: enviar_msg(); break;
            case 2: ler_msg(); break;
		}
	
	} while(op != 0);

}

int main() {

    id = login();

    memset((char *) &socket_other, 0, sizeof(socket_other));

    socket_other.sin_family      = AF_INET;
    socket_other.sin_addr.s_addr = htonl(INADDR_ANY);

    config_roteador(id);
    inicializa_tabelas();

    memset(alive_flag, 0, sizeof(alive_flag));

    topologia_rede(id);
    inicializa_threads();

    menu();

}