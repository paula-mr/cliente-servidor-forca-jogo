# Forca
Jogo da forca desenvolvido em rede para a disciplina de Redes de Computadores da UFMG

# Funcionamento
Para compilar o projeto basta executar o comando make. O sistema foi desenvolvido para ambiente Linux e foi testado no Ubuntu.

## Servidor
O servidor é responsável por receber as conexões com os clientes e realizar o controle do jogo. O servidor atende apenas a um cliente por vez.

Ele define duas constantes: a *VERSION*, que define se será utilizado IPV4 ou IPV6; e a *WORD*, que será a palavra do jogo.

O sistema recebe um parâmetro referente à porta em que o servidor será executado.

## Cliente
O cliente é responsável pela interação com o cliente, exibir a interface e enviar e receber os dados do servidor.

O sistema recebe dois parâmetros referentes ao endereço e â porta do servidor.

# Documentação
## Server
O servidor possui um main, que controla o fluxo de execução e valida se os argumentos foram recebidos de forma correta e a partir destes é criado um socket para o servidor.

Depois, o servidor espera a conexão de algum cliente, quando isso acontece é enviada uma mensagem de confirmação (tipo 1) contendo também o tamanho da palavra e o servidor espera pelos palpites do cliente até completar a palavra. Ao receber um palpite, a letra é validada e é enviado a resposta (tipo 3), a quantidade de ocorrências da letra na palavra e depois as posições da letra na palavra. Ao completar a palavra, o servidor envia a finalização (tipo 4), fecha a conexão com esse cliente e espera uma nova conexão.

Essa função utiliza a função *close* da biblioteca *sys/socket.h* para finalizar a conexão com o socket do cliente.

### int createSocket(char **argv)
Função responsável por criar o socket do servidor e retornar o valor do socket criado. 

O socket é criado, é configurado de forma que permita o reuso da porta/endereço, é realizado o bind do socket ao endereço e depois coloca o socket a um estado passivo para esperar pelas conexões dos clientes, configurando-o para permitir apenas uma conexão por vez. Se tudo correr bem, é exibido na tela o endereço e utilizado.

Qualquer erro de conexão leva a um print do erro identificado e interrupção do programa.

Essa função utiliza a função definida na *common.h* *initializeSocketAddress* para realizar o controle entre versões de endereçamento IPV4 e IPV6 e o *printAddress* para printar o endereço. Além disso, ele utiliza as funções *socket*, *setsockopt*, *bind* e *listen* da biblioteca *sys/socket.h*.

### int connectToClientSocket(int sock)
Função responsável por receber e controlar as conexões do cliente ao servidor, retornando o valor do socket do cliente.

Retira a primeira conexão da fila de conexões pendentes e estabelece a conexão. Se tudo correr bem, exibe na tela o endereço do cliente conectado.

Qualquer erro de conexão leva a um print do erro identificado e interrupção do programa.

Essa função utiliza a função definida na *common.h* *printAddress* para printar o endereço. Além disso, ele utiliza a função *accept* da biblioteca *sys/socket.h*.

### void sendAcknowledgmentMessage(int clientSocket)
Função responsável por enviar a mensagem de confirmação ao estabelecer uma conexão.

Envia uma mensagem de 2 bytes, sendo o primeiro o tipo da mensagem, 1, e o segundo o tamanho da palavra a ser adivinhada.

Caso o número de bytes enviado seja diferente de dois é gerado um print do erro e a interrupção do programa.

Essa função utiliza a função *send* da biblioteca *sys/socket.h*.

### char receiveLetter(int clientSocket)
Função responsável por receber a letra de palpite enviada pelo cliente e retorná-la.

Recebe uma mensagem de 2 bytes, sendo o primeiro o tipo da mensagem, 2, e o segundo a letra de palpite.

Caso o número de bytes recebido seja diferente de dois ou o tipo da mensagem recebida seja diferente de 2 é gerado um print do erro e a interrupção do programa.

Essa função utiliza a função *recv* da biblioteca *sys/socket.h*.

### int sendGuessAnswer(int clientSocket, char letter, char *filledWord)
Função responsável por realizar o controle do jogo a partir da letra de palpite e enviar a resposta para o cliente, retornando se a palavra está completa ou não.

Envia uma mensagem de 2 + n bytes, sendo o primeiro o tipo da mensagem, 3, o segundo a quantidade n de ocorrências da letra na palavra e os n bytes seguintes as posições de ocorrência da letra na palavra. Caso a letra enviada complete a palavra e finalize o jogo, é enviada no lugar uma mensagem de 1 byte contendo o tipo 4 pela função *sendFinalMessage*.

Caso o número de bytes recebido seja diferente de 2 + n bytes é gerado um print do erro e a interrupção do programa.

Essa função utiliza a função *send* da biblioteca *sys/socket.h*.

### void sendFinalMessage(int clientSocket)
Função responsável por enviar a mensagem de finalização do jogo para o cliente.

Envia uma mensagem de 1 byte, sendo ele o tipo da mensagem, 4.

Caso o número de bytes recebido seja diferente de 1 byte é gerado um print do erro e a interrupção do programa.

Essa função utiliza a função *send* da biblioteca *sys/socket.h*.



## Client
O cliente possui um main, que controla o fluxo de execução e valida se os argumentos foram recebidos de forma correta e a partir destes se conecta com o servidor.

Depois, o cliente espera uma mensagem de confirmação de conexão com o servidor (tipo 1) contendo também o tamanho da palavra. A partir disso, ele printa em comando uma interface da palavra a ser adivinhada e recebe via terminal as letras a serem enviadas como palpite até receber uma mensagem de tipo 4, que indica a finalização do jogo. Assim, ele finaliza o socket criado e finaliza o programa.

Essa função utiliza a função *close* da biblioteca *sys/socket.h* para finalizar  o socket criado.

### int createSocket(char **argv)
Função responsável por criar e retornar o socket do cliente criado para se conectar com o servidor.

O socket é criado e conectado ao endereço recebido do servidor. Se tudo correr bem, é exibido na tela o endereço e utilizado.

Qualquer erro de conexão leva a um print do erro identificado e interrupção do programa.

Essa função utiliza a função definida na *common.h* *parseAddress* para realizar o controle entre versões de endereçamento IPV4 e IPV6 e o *printAddress* para printar o endereço. Além disso, ele utiliza as funções *socket* e *connect* da biblioteca *sys/socket.h*.

### int receiveAcknowledgmentMessage(int sock)
Função responsável por receber a mensagem de confirmação ao estabelecer uma conexão e retornar o tamanho da palavra.

Recebe uma mensagem de 2 bytes, sendo o primeiro o tipo da mensagem, 1, e o segundo o tamanho da palavra a ser adivinhada.

Caso o número de bytes recebido seja diferente de dois ou o tipo da mensagem recebida seja diferente de 1 é gerado um print do erro e a interrupção do programa.

Essa função utiliza a função *recv* da biblioteca *sys/socket.h*.

### char guessLetter(int sock)
Função responsável por receber a letra de palpite do teclado e enviá-la para o servidor.

Envia uma mensagem de 2 bytes, sendo o primeiro o tipo da mensagem, 2, e o segundo a letra de palpite.

Caso o número de bytes enviado seja diferente de dois é gerado um print do erro e a interrupção do programa.

Essa função utiliza a função *send* da biblioteca *sys/socket.h*.

### int receiveAnswer(int sock, char letter, char *word)
Função responsável por receber a resposta do servidor para a letra enviada e manipular a palavra contendo o estado atual do jogo.

Recebe uma mensagem de 2 + n bytes, sendo o primeiro o tipo da mensagem, 3, o segundo a quantidade n de ocorrências da letra na palavra e os n bytes seguintes as posições de ocorrência da letra na palavra, assim o sistema preenche essas posições na palavra de controle com a letra enviada. Caso não tenha ocorrências, é printado na tela que a letra não está presente na palavra. Caso a letra enviada complete a palavra e finalize o jogo, é recebida no lugar uma mensagem de 1 byte contendo o tipo 4, assim a palavra guardada tem todos os índices incompletos preenchidos com a letra enviada, finalizando o jogo.

Caso o número de bytes recebido seja igual a 0 bytes e o tipo de mensagem diferente de 3 ou 4 é gerado um print do erro e a interrupção do programa.

Essa função utiliza a função *recv* da biblioteca *sys/socket.h*.


## Common
Possui funções não específicas utilizadas pelo sistema.

### int initializeSocketAddress(const char *proto, const char *portstr, struct sockaddr_storage *storage)
Função responsável por criar a estrutura de endereço de forma genérica para versões de IPV4 e IPV6.

A função recebe a versão de endereçamento, a porta ser criada e a estrutura a ser utilizada. São realizadas conversões na porta e criadas estruturas específicas para IPV4 ou IPV6, dependendo da versão recebida.

Caso aconteça algum erro, é retornado -1. Em caso de sucesso, é retornado 0.

### int parseAddress(const char *addrstr, const char *portstr, struct sockaddr_storage *storage)
Função responsável por realizar parse do endereço de forma genérica para versões IPV4 e IPV6.

A função recebe o endereço e a porta a se conectar, além da estrtutura a ser preenchida. São realizadas conversões na porta, verifica-se se o endereço é de tipo IPV4, se for retorna a estrutura para IPV4, se não realiza o mesmo para IPV6.

Caso aconteça algum erro, é retornado -1. Em caso de sucesso, é retornado 0.

### void printAddress(const struct sockaddr *addr)
Função responsável por imprimir o endereço enviado.

A função recebe o endereço, que tem seus dados convertidos para string e printados.

Qualquer erro de conexão leva a um print do erro identificado e interrupção do programa.