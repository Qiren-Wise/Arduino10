#include <Wire.h> //Biblioteca de Comunicacao I2C
#include <MechaQMC5883.h> //Biblioteca do Sensor QMC5883 - Modulo GY-273
MechaQMC5883 bussola; //Criacao do objeto para o sensor
int x = 0, y = 0, z = 0;
int angulo = 0;
void setup()
{
    Wire.begin(); //Inicializacao da Comunicacao I2C
    Serial.begin(9600); //Inicializacao da comunicacao Serial
    bussola.init(); //Inicializando o Sensor QMC5883
    for(byte pino = 2; pino <= 9; pino++)
    {
     pinMode(pino, OUTPUT);
    }
}
void loop()
{
  bussola.read(&x,&y,&z); //Obter o valor dos eixos X, Y e Z do Sensor
  angulo = atan2(x, y)/0.0174532925; //Calculo do angulo usando os eixos X e Y atraves     da formula
//Ajuste do angulo entre 0 e 360 graus
if(angulo < 0)
angulo+=360;
angulo = 360-angulo;
Serial.println(angulo); //Imprime o valor do angulo na Serial do Arduino
//Apresentando o sentido com base no angulo encontrado
if (angulo > 338 || angulo < 22)
{
digitalWrite(2, HIGH);
digitalWrite(3, LOW);
digitalWrite(4, LOW);
digitalWrite(5, LOW);
digitalWrite(6, LOW);
digitalWrite(7, LOW);
digitalWrite(8, LOW);
digitalWrite(9, LOW);
Serial.println("Norte");
}
if (angulo > 22 && angulo < 68)
{
digitalWrite(2, LOW);
digitalWrite(3, HIGH);
digitalWrite(4, LOW);
digitalWrite(5, LOW);
digitalWrite(6, LOW);
digitalWrite(7, LOW);
digitalWrite(8, LOW);
digitalWrite(9, LOW);
Serial.println("Nordeste");
}
if (angulo > 68 && angulo < 113)
{
digitalWrite(2, LOW);
digitalWrite(3, LOW);
digitalWrite(4, HIGH);
digitalWrite(5, LOW);
digitalWrite(6, LOW);
digitalWrite(7, LOW);
digitalWrite(8, LOW);
digitalWrite(9, LOW);
Serial.println("Leste");
}
if (angulo > 113 && angulo < 158)
{
digitalWrite(2, LOW);
digitalWrite(3, LOW);
digitalWrite(4, LOW);
digitalWrite(5, HIGH);
digitalWrite(6, LOW);
digitalWrite(7, LOW);
digitalWrite(8, LOW);
digitalWrite(9, LOW);
Serial.println("Suldeste");
}
if (angulo > 158 && angulo < 203)
{
digitalWrite(2, LOW);
digitalWrite(3, LOW);
digitalWrite(4, LOW);
digitalWrite(5, LOW);
digitalWrite(6, HIGH);
digitalWrite(7, LOW);
digitalWrite(8, LOW);
digitalWrite(9, LOW);
Serial.println("Sul");
}
if (angulo > 203 && angulo < 248)
{
digitalWrite(2, LOW);
digitalWrite(3, LOW);
digitalWrite(4, LOW);
digitalWrite(5, LOW);
digitalWrite(6, LOW);
digitalWrite(7, HIGH);
digitalWrite(8, LOW);
digitalWrite(9, LOW);
Serial.println("Suldoste");
}
if (angulo > 248 && angulo < 293)
{
digitalWrite(2, LOW);
digitalWrite(3, LOW);
digitalWrite(4, LOW);
digitalWrite(5, LOW);
digitalWrite(6, LOW);
digitalWrite(7, LOW);
digitalWrite(8, HIGH);
digitalWrite(9, LOW);
Serial.println("Oeste");
}
if (angulo > 293 && angulo < 338)
{
digitalWrite(2, LOW);
digitalWrite(3, LOW);
digitalWrite(4, LOW);
digitalWrite(5, LOW);
digitalWrite(6, LOW);
digitalWrite(7, LOW);
digitalWrite(8, LOW);
digitalWrite(9, HIGH);
Serial.println("Noroeste");
}
delay(500); //Delay de 500 ms entre novas leituras
}
