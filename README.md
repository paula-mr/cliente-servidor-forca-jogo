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
## Servidor
O servidor possui um main, que controla o fluxo de execução e valida se os argumentos foram recebidos de forma correta. Depois