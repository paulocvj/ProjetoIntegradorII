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

/* Variaveis globais */
volatile bool autonomia = false;

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
      else if (dado_recebido == 'v' || dado_recebido == 'V')
          autonomia = (autonomia == true ? false : true);
          //autonomia = toggle_autonomia(autonomia);
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
    if (autonomia) {
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

const unsigned long periodo_caso_esq = 100;
unsigned long tempo_caso_esq = millis();

void caso_esquerda(){
  unsigned long tempo_atual = millis();

  motores.parar();
  if(tempo_atual - tempo_caso_esq > periodo_caso_esq)
  {
    if(seguidor_linha.obter_esquerda() == 1)
      motores.esquerda(200);
    else
      motores.tras(70);
  }
}

const unsigned long periodo_caso_dir = 100;
unsigned long tempo_caso_dir = millis();

void caso_direita(){
  unsigned long tempo_atual = millis();

  motores.parar();
  if(tempo_atual - tempo_caso_dir > periodo_caso_dir)
  {
    if(seguidor_linha.obter_direita() == 1)
      motores.direita(200);
    else
      motores.tras(70);
  }
}

bool toggle_autonomia(bool autonomia)
{
  if (autonomia)
    return false;
  return true;
}