% Projeto Final - Veículo Autoguiado
% Sistemas Embarcados - UTFPR
% Francisco Miamoto - Agosto de 2021

# Introdução 

O presente documento tem como objetivo apresentar os diagramas detalhados do funcionamento
do sistema de controle do veículo autoguiado.

Como primeiro passo, veremos a arquitetura proposta.

## Arquitetura

![](./img/architecture.png)

## Diagramas de atividade

Conhecendo as tarefas envolvidas, a seguir são apresentados os diagramas de atividades
para cada tarefa.

![Atividades da tarefa `Track Manager`](./img/track.png)

![Atividades da tarefa `Obstacle Watcher`](./img/obstaclewatcher.png)

![Atividades da tarefa `Speed Controller`](./img/speed.png)

![Atividades da tarefa `UART Writer`](./img/uartwriter.png)

![Atividades da tarefa `UART Reader`](./img/uartreader.png)
