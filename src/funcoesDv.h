void transfer_dv(char why) {
    Pacote msg;

    for (int n_id = 0; n_id < MAX_ROT; n_id++) {
        if (vizinho_tabela[n_id].cost != INF && vizinho_tabela[n_id].port != -1) {
            
            msg = criar_msg(DISTANCIA_VETOR, n_id);
            socket_other.sin_port = htons(vizinho_tabela[n_id].port);

            if (inet_aton(vizinho_tabela[n_id].ip, &socket_other.sin_addr) == 0)
                die("Erro na obtenção do IP do destino (Vetores Distância)\n");
            else if (sendto(sckt, &msg, sizeof(msg), 0, (struct sockaddr *)&socket_other, sizeof(socket_other)) == -1)
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

void ver_dv() {

    system("clear");
    printf("___________~<>~___________\n");

    for (int i = 0; i < MAX_ROT; i++) {
        
        printf("\n");
        
        for (int j = 0; i < MAX_ROT; j++) {
        
            if (vizinho_tabela[i].cost != INF || i == id) 
                if (dv_tabela[i].cost != INF)
                    printf("Origem: %d para destino: %d, com custo = %d\n", i, j, dv_tabela[i].cost[j]); 
                else
                    printf("Origem: %d para destino: %d, com custo = INF\n", i, j);
            else {
                printf("Não possui os vetores de %d\n", i)
                break
            }
        }
    }
    
    printf("___________~<>~___________\n");
    printf("Pressione ENTER para voltar ao menu");
    getchar();
    getchar();

}