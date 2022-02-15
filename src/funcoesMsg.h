
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