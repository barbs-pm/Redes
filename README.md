# Protocolo de Roteamento de Redes

**Objetivo:** Implementar um protocolo de roteamento de redes via sockets UDP executando o algoritmo de Bellman-Ford distribuído

## Funcionamento 
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

## Como compilar
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

## Saída do programa
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

## Implementação e estrutura
## Implementação

Funções desenvolvidas foram:
1. Geração do login e menu
2. Configura informações relevantes para o roteador inicializado (ip, socket, id)
3. Cria as tabelas que irão receber os dados da topologia da rede
4. Inicializada a função de topologia que irá salvar as informações dos vizinhos
5. A inicialização das threads recebe, envia_vetor, envia_msg, estado_roteador
      - recebe: o roteador recebe o pacote, identifica se é uma mensagem ou se é informação do vetor distância. Se for uma mensagem pra ele, é tratada para ser lida. Se for para outro roteador, a mensagem é retransmitida para um vizinho. Se for um vetor distância, ele executa o algoritmo de Bellman-Ford e atualiza a tabela de roteamento.
      - envia_vetor: cria a mensagem do tipo vetor distância, calcula o tempo em que foi feito isso para futuramente conferir se o roteador permanece ativo desde o último envio. Chama uma função para salvar os logs de alteração da tabela de roteamento.
      - envia_mensagem: tenta obter as informações do roteador para enviar, e se der tudo certo, tenta fazer o envio da mensagem.
      - estado_roteador: a cada quantidade de tempo ele verifica se teve alteração e atualiza o custo.

**Src:** a pasta src funciona para separar os códigos conforme sua utilidade. No arquivo uteis.c você irá encontrar funções para controle do menu, impressão de cabeçalhos, conversão de str para int. No arquivo router.h irá encontrar as structs e variáveis globais presentes no programa. No arquivo funcoesDv.c estarão os arquivos que em sua maioria alteram o vetor distância, enviam ele bem como a tabela de roteamento. No arquivo funcoesMsg.c é a mesma coisa, porém relacionado as mensagens

## Como Contribuir

Para contribuir e deixar a comunidade open source um lugar incrivel para aprender, projetar, criar e inspirar outras pessoas. Basta seguir as instruções logo abaixo:

1. Realize um Fork do projeto
2. Crie um branch com a nova feature (`git checkout -b feature/featureCompilador`)
3. Realize o Commit (`git commit -m 'Add some featureCompilador'`)
4. Realize o Push no Branch (`git push origin feature/featureCompilador`)
5. Abra um Pull Request

<br>

## Autora

- **[Bárbara Pegoraro Markus](https://github.com/barbs-pm)** - _Acadêmica do Curso de Ciência da Computação - UFFS_. 
