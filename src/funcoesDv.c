void transfer_dv(char why) {
    Pacote msg;
    time_t rawtime;
    struct tm * timeinfo;

    for (int n_id = 0; n_id < MAX_ROT; n_id++) {
        if (vizinho_tabela[n_id].cost != INF && vizinho_tabela[n_id].port != -1) {
            
            msg = criar_msg(DISTANCIA_VETOR, n_id);
            socket_other.sin_port = htons(vizinho_tabela[n_id].port);

            if (inet_aton(vizinho_tabela[n_id].ip, &socket_other.sin_addr) == 0)
                die("Erro na obtenção do IP do destino (Vetores Distância)\n");
            else if (sendto(sckt, &msg, sizeof(msg), 0, (struct sockaddr *)&socket_other, sizeof(socket_other)) == -1)
                die("Erro ao enviar vetores distância\n");
            else if(why == 'C') { //caso a tabela de roteamento for alterada, salvar as informações dos vetores distancia
                time ( &rawtime );
                timeinfo = localtime ( &rawtime );

                dv_logs[count_log].id_origem  = id;
                dv_logs[count_log].id_destino = n_id;
                dv_logs[count_log].hora       = asctime (timeinfo);

                count_log++;
            }
        }
    }
}

void *envia_dv(void *n) {
    time_t timer;
    timer = time(0);
    sleep(1);

    while (1) {

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

    cabecalho_menus();
    printf("Vetor distância:\n\n");

    for (int i = 0; i < MAX_ROT; i++) {
        
        printf("\n");
        
        for (int j = 0; j < MAX_ROT; j++) {
        
            if (vizinho_tabela[i].cost != INF || i == id) 
                if (dv_tabela[i].cost[j] != INF)
                    printf("Origem: %d para destino: %d, com custo = %d\n", i, j, dv_tabela[i].cost[j]); 
                else if (i == j)
                    continue;
                else
                    printf("Origem: %d para destino: %d, com custo = INF\n", i, j);
            else {
                printf("O roteador %d está desligado ou é inalcançavel.", i);
                break;
            }
        }
    }
    
    printf("\n\n___________~<>~___________\n");
    printf("Pressione ENTER para voltar ao menu");
    getchar();
    getchar();

}

void ver_tabela_roteamento() {

    cabecalho_menus();
    printf("Tabela de roteamento do roteador %d para:\n\n", id);

    for (int i = 0; i < MAX_ROT; i++) {

        if(id == i) 
            continue;

        if (roteamento_tabela[i].cost != INF)
            printf("- roteador %d, com custo: %d\n", i, roteamento_tabela[i].cost);
        else
            printf("- roteador %d, com custo: INF\n", i);

    } 

    printf("\n\n___________~<>~___________\n");
    printf("Pressione ENTER para voltar ao menu");
    getchar();
    getchar();
}

void log_dv() {

    cabecalho_menus();
    printf("Log vetor distância\n\n");

    for (int i = 0; i < count_log; i++)
        printf(" - Roteadores: %d -> %d, Hora: %s", dv_logs[i].id_origem, dv_logs[i].id_destino, dv_logs[i].hora);

    printf("___________~<>~___________\n");
    printf("Pressione ENTER para voltar ao menu");
    getchar();
    getchar();
}