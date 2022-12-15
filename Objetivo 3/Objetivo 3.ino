/*
  buggy.ino - Aplicação exemplo para o buggy 4x4
  
  - Exibe os dados dos sensores pelo terminal UART 9600 bps.
  - Recebe comandos da serial para movimentação dos motores.
  
  Created by Renan Augusto Starke, July 26, 2021.  
  Released into the public domain.
*/

#include "Motores.h"
#include "Linha.h"
#include "Ultrassom.h"

/* Tarefas básicas do exemplo */
void tarefa_1();
void tarefa_2();
void tarefa_3();

volatile bool exibir_estado = true;
char dado_controle;

/* Instâncias das classe dos sensores/atuadores */
Motores motores;
Linha seguidor_linha;
Ultrassom ultrassom(Ultrassom::ECHO1, Ultrassom::TRIG1);

/* Variaveis globais */
volatile bool autonomiaTarefa2 = false;
volatile bool autonomiaTarefa3 = false;

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
  Serial.println("v ou V: Ligar/desligar modo de autonomia (seguidor de linha)");
  Serial.println("w ou W: Ligar/desligar modo de autonomia (sensor ultrassônico)");

  delay(5000);  

}

/* Loop de atualização do sensores e das tarefas */
void loop() {
  ultrassom.atualizar();

  tarefa_1();
  tarefa_2();
  tarefa_3();
 // tarefa_exibir_estado();

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
      else if (dado_recebido == 'v' || dado_recebido == 'V')
          autonomiaTarefa2 = (autonomiaTarefa2 == true ? false : true);
      else if (dado_recebido == 'w' || dado_recebido == 'W')
          autonomiaTarefa3 = (autonomiaTarefa3 == true ? false : true);
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

const unsigned long ciclo_timeout = 400; // 400*5 = 2 segundos
int contagem_ciclos = 0;
const unsigned long periodo_tarefa_2 = 5;
bool ligar_re = false;
unsigned long tempo_tarefa_2 = millis();

/* Tarefa 2 - Fazer o carrinho seguir a linha */
void tarefa_2(){
  unsigned long tempo_atual = millis();

  int esquerda = seguidor_linha.obter_esquerda();
  int direita = seguidor_linha.obter_direita();

  if(tempo_atual - tempo_tarefa_2 > periodo_tarefa_2) {
    tempo_tarefa_2 = tempo_atual;
    if (autonomiaTarefa2) {
      if (esquerda == 0 && direita == 0 && ligar_re == false) {
          motores.frente(70);
          contagem_ciclos++;
          if (contagem_ciclos >= 400) {
            contagem_ciclos = 0;
            ligar_re = true;
          }
      }
      else if (esquerda == 0 && direita == 0 && ligar_re == true)
          motores.tras(100);
      else if (esquerda == 1){
          motores.esquerda(100);
          ligar_re = false;
          contagem_ciclos = 0;
      }
      else if (direita == 1){
          motores.direita(100);
          ligar_re = false;
          contagem_ciclos = 0;
      }
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

const unsigned long periodo_tarefa_3 = 500;
unsigned long tempo_tarefa_3 = millis();

bool vira_esquerda = false;
int cont_linha_reta = 0;

void tarefa_3()
{
  unsigned long tempo_atual = millis();

  unsigned long dist = ultrassom.obter_distancia();

  if(tempo_atual - tempo_tarefa_3 > periodo_tarefa_3) {
    tempo_tarefa_3 = tempo_atual;
    if (autonomiaTarefa3) {
      if (dist > 1500) {
        cont_linha_reta++;
        if (cont_linha_reta > 5) {
          cont_linha_reta = 0;
          vira_esquerda = false;
        }
        motores.frente(70);
      }
      else if (vira_esquerda) { 
        motores.direita(250);
        vira_esquerda = false;
      }
      else {
        motores.esquerda(150);
        vira_esquerda = true;
      }
      Serial.print('\n');
      Serial.print(dist);
    }
  }
}