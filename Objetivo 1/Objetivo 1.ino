/*
  buggy.ino - Aplicação exemplo para o buggy 4x4
  
  - Exibe os dados dos sensores pelo terminal UART 9600 bps.
  - Recebe comandos da serial para movimentação dos motores.
  
  Created by Renan Augusto Starke, July 26, 2021.  
  Released into the public domain.
*/

#include "Motores.h"

/* Tarefas a serem executadas */
void tarefa_1();

volatile bool exibir_estado = true;

/* Instâncias das classe dos sensores/atuadores */
Motores motores;

/* Configuração e mensagem inicial */
void setup() {
  
  Serial.begin(9600);
  while (!Serial); 

  Serial.println("Teste buggy 4x4");
  Serial.println("F: mover para frente");
  Serial.println("B: mover para trás");
  Serial.println("L: mover para esquerda");
  Serial.println("R: mover para direita");

  delay(5000);  

}

/* Loop de atualização do sensores e das tarefas */
void loop() {

  tarefa_1();

}

/* Tarefa 1 - Controlar o carrinho via bluetooth */
void tarefa_1() {

  /* Caso tenha recebido algum dado do PC */
  if (Serial.available()) {
     char dado_recebido = Serial.read();
     Serial.print(dado_recebido);
      if (dado_recebido == 'S')
          motores.parar();          
      else if (dado_recebido == 'F')
          motores.frente(200);      
      else if (dado_recebido == 'B')
          motores.tras(200);      
      else if (dado_recebido == 'L')
          motores.esquerda(150);   // testar       
      else if (dado_recebido == 'R')
          motores.direita(150);
      else if (dado_recebido == 'G')
          motores.esquerda_suave(150);
      else if (dado_recebido == 'I')
          motores.direita_suave(150);
  }
}