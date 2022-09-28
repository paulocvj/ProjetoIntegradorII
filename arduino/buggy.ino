/*
  buggy.ino - Aplicação exemplo para o buggy 4x4
  
  - Exibe os dados dos sensores pelo terminal UART 9600 bps.
  - Recebe comandos da serial para movimentação dos motores.
  
  Created by Renan Augusto Starke, July 26, 2021.  
  Released into the public domain.
*/

#include "Motores.h"
#include "Linha.h"

/* Tarefas básicas do exemplo */
void tarefa_1();
void tarefa_2();
void go_right();
void go_left();
void go_back();
volatile bool exibir_estado = true;

/* Variavel pra armazenar ultima letra lida */
char lastRead;

/* Instâncias das classe dos sensores/atuadores */
Motores motores;

/* Configuração e mensagem inicial */
void setup() {
  
  Serial.begin(9600);
  while (!Serial); 

  Serial.println("Teste buggy 4x4");
  Serial.println("f: mover para frente");
  Serial.println("r: mover para trás");
  Serial.println("e: mover para esquerda");
  Serial.println("d: mover para direita");  
  Serial.println("s: exibir estado dos sensores a cada 1s");
  Serial.println("i: exibir descrição das dos estados");

  delay(5000);  

}

/* Loop de atualização do sensores e das tarefas */
void loop() {
  if (lastRead != 'H') tarefa_1();
  else tarefa_2();

}

/* Tarefa 2: comandos da serial */
void tarefa_1(){

  /* Caso tenha recebido algum dado do PC */
  if (Serial.available()) {
     char dado_recebido = Serial.read();
     lastRead = dado_recebido;
      if (dado_recebido == 'H') return;
      if (dado_recebido == 'S')
          motores.parar();          
      else if (dado_recebido == 'F')
          motores.frente(200);      
      else if (dado_recebido == 'B')
          motores.tras(200);      
      else if (dado_recebido == 'L')
          motores.esquerda(150);          
      else if (dado_recebido == 'R')
          motores.direita(150);
      else if (dado_recebido == 's') {
        if (exibir_estado == true)
          exibir_estado = false;
        else
          exibir_estado = true;        
      }
      
      if (dado_recebido == 'i') {
        Serial.println("Bat0\tBat1\tVelocidade\tDistância");
      }

  }
}

void tarefa_2()
{
  if (obter_esquerda()) go_right();
  if (obter_direita()) go_left();
  if (obter_esquerda() && obter_direita()) go_back();
  if (!obter_esquerda() && !obter_direita()) motores.frente(200);
}

void go_right()
{
  motores.parar();
  motores.direita(50);
}

void go_left()
{
  motores.parar();
  motores.esquerda();
}

void go_back()
{
  motores.parar();
  motores.tras(200);
}