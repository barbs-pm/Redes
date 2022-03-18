//coverter char pra int copiei da internet
int toint(char *str) {

  int ans;
  ans = 0;

  for(int i = strlen(str) - 1, pot = 1; i >= 0; i--, pot *= 10)
    ans += pot * (str[i] - '0');
    
  return ans;   

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


void cabecalho_menus() {

    system("clear");
    printf("___________~<>~___________\n");
    printf("      ROTEADOR %d\n\n", id);

}

void menu() {

    int op;

    do {

		cabecalho_menus();
		printf("|0| Sair\n");
		printf("|1| Enviar Mensagem\n");
		printf("|2| Visualizar Mensagem\n");
        printf("|3| Mostrar vetor distância\n");
        printf("|4| Mostrar tabela de roteamento\n");
        printf("|5| Mostrar log vetor distância\n");
		printf("___________~<>~___________\n");
		printf("Escolha sua opção: ");
		scanf("%d", &op);

		switch(op){ 
			case 1: enviar_msg(); break;
            case 2: ler_msg(); break;
            case 3: ver_dv(); break;
            case 4: ver_tabela_roteamento(); break;
            case 5: log_dv(); break;
		}
	
	} while(op != 0);

}