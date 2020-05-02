/************************************************************************************************************************************
* AUTOR: Mateus Vilanova dos Passos
* ALTERADO EM: 08/02/2020
* 
* DESCRIÇÃO DO QUE FAZ: inicializa dfplayer mini, e toca um som para sinalizar que foi inicializado, le botao e espera ser 
* apertado para tocar som de ignição do sabre de luz ligando um led, quando botao é pressionado por 1,5 seg toca som de desativação 
* do sabre de luz e desliga led.
* 
* TO DO: Fazer led diminuir e aumentar intensidade; Fazer uma função para controlar o som ligado do sabre; Começar modo menu;
* Adicionar som de desvio de blaster;
*
************************************************************************************************************************************/


//BIBLIOTECAS
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Adafruit_NeoPixel.h>
#include <MPU6050_tockn.h>
#include <Wire.h>

// DEFINIÇÕES DE PINOS
#define pinRx 10
#define pinTx 11
#define BOT 2
#define PINLED 9
#define NUMLEDS 61
/*pinos cristal*/
#define RED 6
#define GREEN 5
#define BLUE 3

// DEFINIÇÕES  
#define TempoDeBounce 100
#define TEMPO_DE_BOUNCE 100
#define TEMPO_DESLIGA 1500

// INSTANCIANDO OBJETOS
SoftwareSerial playerMP3Serial(pinRx, pinTx);
Adafruit_NeoPixel pixels(NUMLEDS, PINLED, NEO_GRB + NEO_KHZ800);
MPU6050 mpu6050(Wire);

DFRobotDFPlayerMini playerMP3;

// DECLARAÇÃO FUNÇOES
int modoBotao(int pin);
void setCorCristal(int r, int g, int b);
void mostraCor(int r, int g, int b);
void menu();
void cores();
void volume();

//void executa();
void turnOn();
void turnOf();
void on(int botao);
//void of();
void menu();
void cores();

// DECLARAÇAO DE VARIAVEIS
int modo = 0; //
int volumeMP3 = 28; //30 maximo
long timer;
/* valor rgb da lamina */
int r = 0;
int g = 0;
int b = 255; //azul como valor defult
/* valor rgb cristal */
int rc = 255;
int gc = 255;
int bc = 0; //azul como defult

void setup() 
{
  pinMode(BOT, INPUT_PULLUP); //seta botao para modo pull up

  pinMode(RED, OUTPUT); //define pinos do led do  cristal como saida
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  setCorCristal(255, 255, 255); //desliga led

  pixels.begin(); //inicializa fita de leds
  pixels.setBrightness(250); //define brilho da fita 
  pixels.clear(); // Set all pixel colors to 'off'
  
  Serial.begin(9600); //inicia monitor serial
  
  Wire.begin();//inicia mp6050
  mpu6050.begin();
  mpu6050.calcGyroOffsets(false); //calibra giroscopio
  
  playerMP3Serial.begin(9600); //inicia dfplayer

  Serial.println();
  Serial.println(F("Iniciando DFPlayer ... (Espere 3~5 segundos)"));

  if (!playerMP3.begin(playerMP3Serial))  // COMUNICAÇÃO REALIZADA VIA SOFTWARE SERIAL
  {  
    Serial.println(F("Falha ao iniciar:"));
    Serial.println(F("1.Confira as conexões!"));
    Serial.println(F("2.Confira o cartão SD!"));
    while(true)
    {
      delay(0);
    }
  }

  Serial.println(F("DFPlayer ligado!"));

  playerMP3.volume(volumeMP3); //define volume do player

  playerMP3.playFolder(1, 1); //toca som de boot
  delay(1000);

}

void loop() 
{
  int estado_bot = modoBotao(BOT); //recebe o estado do botao 1 apertado 2 pressionado por 1,5 segundos

  if(modo == 0) //desligado
  {
    if(estado_bot == 1)
    {
      turnOn();
      modo = 1; //muda modo para ligado
    }
    if(estado_bot == 2)
    {
      menu();
    }
  }

  if(modo == 1) //ligado
  {
    on(estado_bot);
  }

}

/*=========================================================================================================================================================*/
/*========================================================== FUNCOES ======================================================================================*/
/*=========================================================================================================================================================*/

int modoBotao(int pin) //verifica se o botao foi apertado ou se foi segurado por tempo determinado
{
  static unsigned long delay_botao = 0;
  static unsigned long botao_apertado;
  static bool estado_anterior_botao;
  static int fase;
  
  bool estado_botao;
  int retorno = 0;

  if((millis() - delay_botao) > TEMPO_DE_BOUNCE)
  {
    estado_botao = digitalRead(BOT);

    if((!estado_botao) && (estado_botao != estado_anterior_botao)) //verifica se o botao foi apertado
    {
      delay_botao = millis();
      botao_apertado = millis();
      fase = 1; 
    }

    if( (fase == 1) && ((millis() - botao_apertado) > TEMPO_DESLIGA) ) //verifica se botao foi pressionado pelo tempo determinado
    {
      fase = 0;
      retorno = 2;
    }

    if( estado_botao && (estado_botao != estado_anterior_botao)) //verifica se botao foi apertado e solto
    {
      if(fase == 1)
      {
        retorno = 1;
      }

      fase = 0;
    }

    estado_anterior_botao = estado_botao;
    
  }

  return(retorno);
}

void setCorCristal(int r, int g, int b)
{
  digitalWrite(RED, r);
  digitalWrite(GREEN, g);
  digitalWrite(BLUE, b);
}

void turnOn()
{
  playerMP3.playFolder(1, 2);

  setCorCristal(rc, gc, bc); //liga led em azul
    
  for(int i = 0; i < NUMLEDS; i++) 
  { 
    pixels.setPixelColor(i, pixels.Color(r, g, b)); //muda cor de cada led para azul

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(10); // Pause before next pass through loop
  }
  delay(500);
  timer = millis();
  playerMP3.playFolder(1, 8);
}

void turnOf()
{
  playerMP3.playFolder(1, 6);

  setCorCristal(255, 255, 255); //desliga led

  for(int i = NUMLEDS ; i > -1; i--) 
  { 
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); //desliga cada led

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(10); // Pause before next pass through loop
  }
}

void on(int botao)
{
  if(millis() - timer > 20000)
  {
    timer = millis();
    playerMP3.playFolder(1, 8);
  }
  
  //le giroscopio()

  if(botao == 2)
  {
    turnOf();
    modo = 0; //muda modo para desligado
  }
  //if(botao == 1)
}

void mostraCor(int r, int g, int b)
{
  for(int i = 0; i < NUMLEDS; i++) 
  { 
    pixels.setPixelColor(i, pixels.Color(r, g, b)); //muda cor de cada led

    pixels.show();   // Send the updated pixel colors to the hardware.
  }
  
  setCorCristal(255 - r, 255 - g, 255 - b); //muda cor cristal
}

void menu()
{
  if(volumeMP3 == 0) //define volume para 50% se entrar no menu em modo mudo
  {
    volumeMP3 = 20;
    playerMP3.volume(volumeMP3); //define volume do player
    delay(50);
  } 
  
  bool continua = true;
  float angulo;
  int botao;
  int modo = 0; //1 cor, 2 sons, 3 volume

  Serial.println("entrou menu");
  playerMP3.playFolder(2, 25);
  delay(8500);
   
  while(continua)
  {
    botao = modoBotao(BOT);
    mpu6050.update();
    angulo = mpu6050.getAngleX();
    
    if((angulo < 10 && angulo > 0) && modo != 1)
    {
      Serial.print(mpu6050.getAngleX());
      Serial.println("cores");
      playerMP3.playFolder(2, 26);
      modo = 1;
    }
    else if((angulo < 20 && angulo > 10) && modo != 2)
    {
      Serial.print(mpu6050.getAngleX());
      Serial.println("sons");
      playerMP3.playFolder(2, 28);
      modo = 2;

    }
    else if((angulo < 0 && angulo > -10) && modo != 3)
    {
      Serial.print(mpu6050.getAngleX());
      Serial.println("volume");
      playerMP3.playFolder(2, 27);
      modo = 3;
    }

   // Serial.println(botao);

    if(modo == 1 && botao == 1)
    {
      cores();
    }
    else if(modo == 2 && botao == 1)
    {
      Serial.println("entrou em sons");
    }
    else if (modo == 3 && botao == 1)
    {
      volume();
    }
    else if(botao == 2)
    {
      Serial.println("saiu menu");
      playerMP3.playFolder(2, 30);
      continua = false;
    }
    
  }
}

void cores()
{
  Serial.println("entrou em cores");
  playerMP3.playFolder(2, 26);
  
  bool continua = true;
  float angulo;
  int botao;
  int cor = 0; //1 azul, 2 verde, 3 vermelho, 4 roxo
   
  while(continua)
  {
    botao = modoBotao(BOT);
    mpu6050.update();
    angulo = mpu6050.getAngleX();
    
    if((angulo < 10 && angulo > 0) && cor != 1)
    {
      Serial.println("azul");
      playerMP3.playFolder(3, 31);
      mostraCor(0, 0, 255);
      cor = 1;
    }
    else if((angulo < 20 && angulo > 10) && cor != 2)
    {
      Serial.println("verde");
      playerMP3.playFolder(3, 32);
      mostraCor(0, 255, 0);
      cor = 2;

    }
    else if((angulo < 0 && angulo > -10) && cor != 3)
    {
      Serial.println("vermelho");
      playerMP3.playFolder(3, 33);
      mostraCor(255, 0, 0);
      cor = 3;
    }
    else if((angulo < -10 && angulo > -20) && cor != 4)
    {
      Serial.println("roxo");
      playerMP3.playFolder(3, 34);
      mostraCor(109, 33, 119);
      cor = 4;
    }

    if(cor == 1 && botao == 1)
    {
      Serial.println("selecionou azul");
      playerMP3.playFolder(3, 35);
      r = 0;
      g = 0;
      b = 255;
      rc = 255;
      gc = 255;
      bc = 0;
    }
    else if(cor == 2 && botao == 1)
    {
      Serial.println("selecionou verde");
      playerMP3.playFolder(3, 35);
      r = 0;
      g = 255;
      b = 0;
      rc = 255;
      gc = 0;
      bc = 255;
    }
    else if (cor == 3 && botao == 1)
    {
      Serial.println("selecionou vermelho");
      playerMP3.playFolder(3, 35);
      r = 255;
      g = 0;
      b = 0;
      rc = 0;
      gc = 255;
      bc = 255;
    }
    else if (cor == 4 && botao == 1)
    {
      Serial.println("selecionou roxo");
      playerMP3.playFolder(3, 35);
      r = 109;
      g = 33;
      b = 119;
      rc = 146;
      gc = 222;
      bc = 136;
    }
    else if(botao == 2)
    {
      Serial.println("saiu cores");
      continua = false;
      mostraCor(0, 0, 0);
      delay(50);
      playerMP3.playFolder(2, 29);
      delay(100);
    }
    
  }
}

void volume()
{
  Serial.println("entrou em volume");
  playerMP3.playFolder(2, 27);
  
  bool continua = true;
  float angulo;
  int botao;
  int volume = 0; //1 100%, 2 50%, 3 mudo
   
  while(continua)
  {
    botao = modoBotao(BOT);
    mpu6050.update();
    angulo = mpu6050.getAngleX();
    
    if((angulo < 10 && angulo > 0) && volume != 1)
    {
      Serial.println("50%");
      playerMP3.playFolder(4, 37);
      volume = 1;
    }
    else if((angulo < 20 && angulo > 10) && volume != 2)
    {
      Serial.println("100%");
      playerMP3.playFolder(4, 36);
      volume = 2;

    }
    else if((angulo < 0 && angulo > -10) && volume != 3)
    {
      Serial.println("mudo");
      playerMP3.playFolder(4, 38);
      volume = 3;
    }

    if(volume == 1 && botao == 1)
    {
      Serial.println("selecionou 50%");
      playerMP3.playFolder(4, 39);
     volumeMP3 = 20;
    }
    else if(volume == 2 && botao == 1)
    {
      Serial.println("selecionou 100%");
      playerMP3.playFolder(4, 39);
      volumeMP3 = 30;
    }
    else if (volume == 3 && botao == 1)
    {
      Serial.println("selecionou mudo");
      playerMP3.playFolder(4, 39);
      volumeMP3 = 0;
    }
    else if(botao == 2)
    {
      Serial.println("saiu volume");
      continua = false;
      playerMP3.volume(volumeMP3); //define volume do player
      delay(50);
      playerMP3.playFolder(2, 29);
    }
    
  }
}
