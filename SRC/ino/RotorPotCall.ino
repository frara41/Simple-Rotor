/* RotorPotCall - código de calibração de pot de posição > verifica o ponto central e extremos de valores do POT para posição... > ok


Controla motor DC com ponte H  com um potenciômetro (pot1) ligado à porta A0 do arduino
Controla direção e velocidade (usando PWM)
A porta de leitura analógica A0 do arduino retorna um valor entre 0 e 1024, conforme a posição do pot. quando o pot1 está no centro do percurso, a porta retorna em torno de 512. Assim:
pot em torno de 512 > motor parado
Pot > 512 > motor gira para um lado
Pot < 512, > motor gira para o  outro lado.


Calibrando o rotor:


com o uso de pot1, o rotor é colocado na posição 0, e   feita a leitura  do pot2 (A1) para identificar a posição central (pot 2 =512...). Marcado o ponto 0, o rotor é levado respectivamente  às posições -180 e + 180, sendo feitas as leituras de pot2 nestes pontos. deve-se realizar algumas leituras, e se obter a média dos valores encontrados, que serão posteriormente utilizados nos códigos de comando do rotor. 
 */
 
int IN1 = 10; // Pino IN1 da ponte H
int IN2 = 11; // Pino IN2 da ponte H
int ENA = 9;  // Pino ENA da ponte H
int pin1 = A0;
int pin2 = A1;
int t1 = 500;
long pot1 = 0;
long pot2 =0;
long ctp = 512;
long delt = 0;
long vel = 0;
int tol = 50;




void setup() {            
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  Serial.begin(9600);
  Serial.print("RotorPotCall,  by Fabio Franca  em 09/08/23");
  Serial.println();
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
}


void loop() {
  pot1 = analogRead(pin1);
  pot2 = analogRead(pin2);
  delt = pot1 - 512;
  ctp = abs(delt);
  vel = int((ctp-2)/2);
  analogWrite(ENA,vel);
  Serial.print(pot1);
  Serial.print(" ; ");
  Serial.print(ctp);
  Serial.print(" ; ");
  Serial.print(delt);
  Serial.print(" ; ");
  Serial.print(vel);
  Serial.print(" ; ");
  Serial.println(pot2);
 
  if (ctp <= tol) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
   
  }
 
  if ((ctp > tol) and (delt)  >  0) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  }
 
   if ((ctp > tol) and (delt)  <  0) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  }
 
  delay(t1);            // espera t1
 


}
