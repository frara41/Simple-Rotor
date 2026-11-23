

/* RotorSerialPointPIDOled4P2 v  0.4- código apontamento de rotor via serial com feedback de posição por pot via OLED. Velocidade calculada por PID




Este código está calibrado para  o Prótótipo 02 (PVC) > em desenvolvimento em 26/08/2025
> precisa de testes para refinar valores de  PID
> precisa de ajustes para novo oled > and
> tentativa de correção para inclusão de limites de curso no software > and
> Testar retorno de comunicação de posição via serial


Controla motor DC com ponte H  via comando serial
Controla direção e velocidade (usando PWM)


A porta de leitura analógica do pot2 do arduino retorna um valor entre 0 e 1024, mapeando  a posição angular do rotor
A velocidade é proporcional ao erro da posição angular do pot2 em relação ao angulo enviado via serial, calculada via PID


O resultado do apontamento do rotor é apresentado no visor oled, nos formatos -180> 0 > 180 e 0-360


lembrar que no arduino A4= SDA, e  A5= SCL, e o display funcionou com 3.3v...
*/
 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PID_v1.h>




int IN1 = 10; // Pino IN1 da ponte H
int IN2 = 11; // Pino IN2 da ponte H
int ENA = 9;  // Pino ENA da ponte H
int pin1 = A0;
int pin2 = A1;
int t1 = 10;


long pot2 =0;
long Pot180n = 359; // valor do pot2 equivalente a - 180 graus >> rotor2
long Pot180p = 665; // valor do pot2 equivalente a + 180 graus >> rotor2



long AZmin = -270; // Valor mínimo  de curso do rotor, em graus
long AZmax = 270;  // Valor máximo  de curso do rotor, em graus
//long Potmin = 282; // Valor mínimo  de curso do pot2, equivalente a -270
//long Potmax = 742; // valor máximo  de curso do pot2, equivalente a +270


long ctp = 0;
long delt = 0;
long vel = 0;  
int  vmin = 120; // define velocidade mínima
long Azr = 0; // Azimute do rotor
long Azr2 = 0; // Azimute do rotor outro formato
long Azs1 = 0; // azimute alvo
int tol = 2; // tolerância máxima no delta entre azimute do rotor e azimute alvo




// variaveis de leitura serial:


int AZgp = 0;
int ELgp = 0;
String TextToSend ="";
String AZgp1 ="";
String ELgp1 ="";
String AZgp2 ="";
String ELgp2 ="";








// PID Gain variables
// Fine tune as required
double Kp = 1;  // Proportional Gain
double Ki = 0.1;  // Integral Gain
double Kd = 0.1;  // Derivative Gain
 
// PID Parameter variables
double Setpoint;
double Input;
double Output;








//Create display Object
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);








// Create PID Object
PID myPID(&Input, &Output, &Setpoint, Kp, Ki , Kd, DIRECT);








void setup() {            
 display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
 display.display();
 delay(1000);
 // Clear the buffer.
 display.clearDisplay();
 display.display();
 // text display tests
 display.setTextSize(2);
 display.setTextColor(WHITE);
 
 pinMode(IN1, OUTPUT);
 pinMode(IN2, OUTPUT);
 pinMode(pin1, INPUT);
 pinMode(pin2, INPUT);
 Serial.begin(9600);
// Serial.setTimeout(10); // Testar se precisa usar ou não...


 //Serial.print("RotorserialPointPIMOled v 0.2 by Fabio Franca  em 13/09/23");
// Serial.println();
 digitalWrite(IN1,LOW);
 digitalWrite(IN2,LOW);
 
 // definições do PID
 myPID.SetMode(AUTOMATIC);
 myPID.SetOutputLimits(-255, 255);
 myPID.SetSampleTime(20);








}




void loop() {
 // Leitura de dados seriais:
 if (Serial.available()> 0 ){
  String datain = Serial.readString();
  delay(10);
  datain.trim();
 
  if (datain == "PNN") {
      TextToSend = "Parking N: ";
      AZgp = 0;
      ELgp = 0;
      AZgp2 = String(AZgp, DEC);
      ELgp2 = String(ELgp, DEC);
      datain = "0";
      delay(100);
    }
  if (datain == "PSE") {
      TextToSend = "Parking S, esq.: ";
      AZgp = -180;
      ELgp = 0;
      AZgp2 = String(AZgp, DEC);
      ELgp2 = String(ELgp, DEC);
      datain = "0";
      delay(100);
    }
  if (datain == "PSD") {
      TextToSend = "Parking S, dir.: ";
      AZgp = 180;
      ELgp = 0;
      AZgp2 = String(AZgp, DEC);
      ELgp2 = String(ELgp, DEC);
      datain = "0";
      delay(100);
  
  if (datain != "0")
  {
   char *ptr = NULL;
    char buff[8];
    char *strings[3];
    byte index = 0;
    datain.toCharArray(buff,8);
    ptr = strtok(buff,":");
    while (ptr != NULL)
    {
      strings[index] = ptr;
      index ++;
      ptr = strtok(NULL,":");
    }
  AZgp1= strings[0];
  ELgp1= strings[1];
  AZgp = AZgp1.toInt();
  ELgp = ELgp1.toInt();
    AZgp2 = String(AZgp, DEC);
    ELgp2 = String(ELgp, DEC);
    datain = "0";
    delay(100);
   
  }
 }


 // Leitura da posição do rotor:
 pot2 = analogRead(pin2);
 

 Azr = int(360*(pot2 - Pot180n)/(Pot180p-Pot180n) -180); //  Calcula o valor do azimute do rotor, no formato -180 <> 180
 
 delt = AZgp - Azr; // AZgp era azs1 antes...
 ctp = abs(delt);


  // Calcula parâmetros PID e comanda o motor
  Setpoint = Azr;
  Input = AZgp; // era azs1.
  myPID.Compute();
  vel = abs(Output);
    if (vel < vmin){
    vel = vmin;
  }
  delt = AZgp - Azr; // AZgp era azs1 antes...
  ctp = abs(delt);
  analogWrite(ENA,vel);


 // opera o motor segundo a função de erro...


// immplementa limites de movimentação do rotor, para evitar excesso de curso
 if (Azr <= AZmin); {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  }
 if (Azr >= AZmax); {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  }


// Implementa tolerâncias na medida do Angulo
  if (ctp <= tol) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  }
 // movimenta o rotor
 
  if ((Azr < AZmax) && (ctp > tol) && (delt)  >=  0) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  }

  if ((Azr > AZmin) && (ctp > tol) && (delt)  <  0) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  }






 // Escreve azimute rotor em formato 0-360
  if (Azr >=0){
    Azr2 =  Azr;
  }
  else {
  Azr2 = 360 + Azr;
  }




  // Oled print
  String Azrd = String(Azr, DEC);
  String Azr2d = String(Azr2, DEC);
  String AZgpd = String(AZgp, DEC);
  // String Oleds = String(AZgpd + "<>" + Azrd +" : "+ Azr2d);
 // String Oleds = String(AZgpd + "<>" + Azrd);
 String Oleds = String(Azrd);
  display.clearDisplay();
  display.setCursor(0, 0);  
  display.println("PT2ATC-RTR");
 // display.setCursor(0, 16);  
  display.print("SAT> ");
  display.println(AZgp);
  display.print("RTR> "); 
  display.println(Azr);
 // display.print(Oleds);
  display.display();
 
  // Serial print
  String deltd = String(delt, DEC);
  String veld = String(vel, DEC);
  TextToSend = String(AZgpd + "<>"+ Azrd +" : "+ Azr2d +" : "+ deltd +" : "+ veld);
  //Serial.println(TextToSend);
 
 // Releitura da posição do rotor:
  pot2 = analogRead(pin2);
  Azr = int(360*(pot2 - Pot180n)/(Pot180p-Pot180n) -180); //  Calcula o valor do azimute do rotor
 
  delay(t1);            // espera t1




 }





 
}
