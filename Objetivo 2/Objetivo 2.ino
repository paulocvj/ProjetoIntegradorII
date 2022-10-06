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

volatile bool exibir_estado = true;
char dado_controle;

/* Instâncias das classe dos sensores/atuadores */
Motores motores;
Linha seguidor_linha;

/* Configuração e mensagem inicial */
void setup() {
  
  Serial.begin(9600);
  while (!Serial); 

  Serial.println("Teste buggy 4x4");
  Serial.println("F: mover para frente");
  Serial.println("B: mover para trás");
  Serial.println("L: mover para esquerda");
  Serial.println("R: mover para direita");  
  Serial.println("s: exibir estado dos sensores a cada 1s");

  delay(5000);  

}

/* Loop de atualização do sensores e das tarefas */
void loop() {

  tarefa_1();

  tarefa_2();

  tarefa_exibir_estado();

}

/* Tarefa 1 - controlar o carrinho via bluetooth */
void tarefa_1() {

  /* Caso tenha recebido algum dado do PC */
  if (Serial.available()) {
     char dado_recebido = Serial.read();
     dado_controle = dado_recebido;
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

const unsigned long periodo_tarefa_2 = 2;
unsigned long tempo_tarefa_2 = millis();

/* Tarefa 2 - Fazer o carrinho seguir a linha */
void tarefa_2(){
  unsigned long tempo_atual = millis();

  if(tempo_atual - tempo_tarefa_2 > periodo_tarefa_2) {
    tempo_tarefa_2 = tempo_atual;
    if ((dado_controle == 'v' || dado_controle == 'V') && seguidor_linha.obter_esquerda() != 1 && seguidor_linha.obter_direita() != 1)
        motores.frente(65);
    if ((dado_controle == 'v' || dado_controle == 'V') && seguidor_linha.obter_esquerda() == 1) {
        motores.esquerda(200);
    }
    if ((dado_controle == 'v' || dado_controle == 'V') && seguidor_linha.obter_direita() == 1) {
        motores.direita(200);
    }
  }
}

/* Exibe valor dos sensores IR */
void tarefa_exibir_estado(){
  static unsigned long tempo_atual = 0;

  /* Escalonamento a cada 1s. Se ativado a exibição */
  if ((exibir_estado) && (millis() - tempo_atual >= 1000)){
    
    Serial.print(seguidor_linha.obter_esquerda());
    Serial.print("\t");
    Serial.print(seguidor_linha.obter_direita());
    
    Serial.println("");
    
    tempo_atual = millis();    
  }  
}

