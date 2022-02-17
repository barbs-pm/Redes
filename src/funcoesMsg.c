void die(char *s){

  perror(s);
  exit(1);

}

void ler_msg() {

    cabecalho_menus();
    printf("Histórico de mensagens:\n\n");

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
        
        cabecalho_menus();
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
    char* msg_retorno = "";

    cabecalho_menus();
    printf("Informe o roteador de destino: ");
    scanf("%d", &destino);
    cabecalho_menus();

    if (destino == id) 
        msg_retorno = "Você não pode enviar mensagens a si mesmo.";

    else if (destino < 0 || destino >= MAX_ROT) 
        msg_retorno = "O roteador não existe.";

    else if (roteamento_tabela[destino].next == -1) 
        msg_retorno = "Impossível chegar ao roteador.";

    else {
        msg_out = criar_msg(MSG, destino);
        msg_flag = 1;
        msg_retorno = "Mensagem enviada";
    }

    printf("___________~<>~___________\n");
    printf("%s", msg_retorno);
    printf("\nPressione ENTER para voltar");
    getchar();
    getchar();

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