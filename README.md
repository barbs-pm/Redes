## Protocolo de Roteamento de Redes

**Objetivo:** Implementar um protocolo de roteamento de redes via sockets UDP executando o algoritmo de Bellman-Ford distribuído<br>

**Funcionamento:** 
Cada roteador executa como um processo (multithread) individual. O programa obtém as informações de configuração via arquivos. Cada roteador é capaz de se comunicar com outros roteadores (i.e., mesmo programa instanciado múltiplas vezes) através de sockets UDP. 
O arquivo roteador.config é utilizado para especificar os dados dos roteadores, no padrão: ID Porta Socket IP. No exemplo abaixo, existem três roteadores disponíveis (que podem ser alterados no arquivo conforme necessidade):
```
1 25000 127.0.0.1
2 25001 127.0.0.1
3 25002 127.0.0.1
```
O arquivo enlaces.config descreve o custo dos enlaces entre os roteadores, no padrão: id_roteador_remetente id_roteador_destino custo_enlace. Exemplo:
```
1 2 1
2 3 5
1 3 3
```

**Como compilar:**
Caso possua o make instalado em sua máquina, abra o terminal e execute o seguinte comando
```
make run
```
Caso contrário, abra o terminal e execute os seguintes comandos:
```
gcc router.c -o router -lpthread
./router
```
Ao entrar no programa, será exibido uma mensagem pedindo pelo ID do roteador a ser logado, insira um ID que exista no arquivo roteador.config e escolha uma das opções disponíveis.
Abra um novo terminal pra cada roteador inicializado.

**Saída do programa:**
<br>
Ao executar o programa, o mesmo exibe uma mensagem exigindo o ID a ser logado. Após inserir o dado, exibe um menu com as opções disponíveis:

```
___________~<>~___________
      ROTEADOR X

|0| Sair
|1| Enviar Mensagem
|2| Visualizar Mensagem
|3| Mostrar vetor distância
|4| Mostar tabela de roteamento
|5| Mostar log vetor distância
__________~<>~___________
Escolha sua opção:       
```
<br>

## Implementação

Funções desenvolvidas foram:
1. Geração do login e menu
2. Configura informações relevantes para o roteador inicializado (ip, socket, id)
3. Cria as tabelas que irão receber os dados da topologia da rede
4. Inicializada a função de topologia que irá salvar as informações dos vizinhos
5. A inicialização das threads recebe, envia_vetor, envia_msg, estado_roteador
6. aqui complica akakka --TODO

## Como Contribuir

Para contribuir e deixar a comunidade open source um lugar incrivel para aprender, projetar, criar e inspirar outras pessoas. Basta seguir as instruções logo abaixo:

1. Realize um Fork do projeto
2. Crie um branch com a nova feature (`git checkout -b feature/featureRedes`)
3. Realize o Commit (`git commit -m 'Add some featureRedes'`)
4. Realize o Push no Branch (`git push origin feature/featureRedes`)
5. Abra um Pull Request

<br>

## Autora

- **[Bárbara Pegoraro Markus](https://github.com/barbs-pm)** - _Acadêmica do Curso de Ciência da Computação -UFFS_. 
