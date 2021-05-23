 //delacracion de librerias

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
 #include <Ultrasonic.h>
//Declaremos los pines CE y el CSN

//float Kp=16,Ki=4.99,Kd=7.5; //kp correcion proporcional del error k1 contador de errores acumulaTICO KD corrigen oscilacion

float Kp=27,Ki=0.000000,Kd=2.5;

//float Kp=33,Ki=0.00000,Kd=7;
//float Kp=18,Ki=0.0001,Kd=2.5;
float error=0, P=0, I=0, D=0, PID_value=0;
float previous_error=0, previous_I=0;
int velocidad=135;
int initial_motor_right=velocidad;
int initial_motor_left=velocidad;

 //declaracion de pines de motor
int motor1_1=42;//El pin 4 a In1 del L298N
int motor1_2=40;//El pin 4 a In1 del L298N
int motor2_1=5;//El pin 3 a In2 del L298N
int motor2_2=6;//El pin 3 a In2 del L298N
int pwmizq=7;//El pin 7 a In3 del L298N
int pwmder=4;//El pin 8 a In4 del L298N
//declaracion de pines sensor ultrasonico
int echo=26;  //El pin 5 a EnA del L298N
int trig=22;//El pin 6 aEnB del L298N
//declaracionde pines sensor opticos
int sensorm2=24; 
int sensorm1=28;
int sensor0=30;
int sensor1=32;
int sensor2=34;
long tiempo = 0;

long distance=0;
Ultrasonic ultrasonic(trig,echo); // (Trig PIN,Echo PIN)

//Variable con la dirección del canal que se va a leer
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";
//creamos el objeto radio (NRF24L01)

//vector para los datos recibidos
char dato[2];

void read_sensor_values(void);
void calculate_pid(void);
void motor_control(void);
void atras(char mov);
void adelante(char mov);
void giraDerecha(char mov);
void giraIzquierda(char mov);
void parar(void);
void write_leds(char led, int brightness);
void distancia (void);
void validar(char mov);

void setup()
{
 //inicializamos el NRF24L01 
  
  //inicializamos el puerto serie
  Serial.begin(115200); 
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

 
 //salidad de pines
 pinMode(motor1_1, OUTPUT);
pinMode(motor1_2, OUTPUT);
pinMode(motor2_1, OUTPUT);
  pinMode(motor2_2, OUTPUT);
  pinMode(pwmizq, OUTPUT); 
  pinMode(pwmder, OUTPUT);
 //ultrasonico 
  pinMode(echo, INPUT); //ECHO
  pinMode(trig, OUTPUT); //trig
  //senosres
 pinMode( sensor0,INPUT_PULLUP);
 pinMode(sensor1,INPUT_PULLUP);
 pinMode(sensor2,INPUT_PULLUP);
pinMode( sensorm1,INPUT_PULLUP);
pinMode(sensorm2,INPUT_PULLUP);

}
 
 void atras(char mov){ 
  // Serial.println("el motor va pa atras");
  digitalWrite(motor1_1,LOW);
  digitalWrite(motor1_2,HIGH);
  digitalWrite(motor2_1,LOW);
  digitalWrite(motor2_2,HIGH);
  analogWrite(pwmizq,mov);
  analogWrite(pwmder,mov);
 // analogWrite(derecho,200);//Velocidad motor
 // analogWrite(izquierdo,200);
}
void adelante(char mov){ 
  // Serial.println("el motor va pa delante");
  digitalWrite(motor1_1,HIGH);
  digitalWrite(motor1_2,LOW);
  digitalWrite(motor2_1,HIGH);
  digitalWrite(motor2_2,LOW);
  analogWrite(pwmizq,mov);
  analogWrite(pwmder,mov);
 // analogWrite(derecho,200);
 // analogWrite(izquierdo,200);
}
void giraDerecha(char mov){ 
  // Serial.println("el motor va pa la derecha");
  digitalWrite(motor1_1,HIGH);
  digitalWrite(motor1_2,LOW);
  digitalWrite(motor2_1,LOW);
  digitalWrite(motor2_2,LOW);
  analogWrite(pwmizq,mov);
  analogWrite(pwmder,mov/2);
}
void giraIzquierda(char mov){ 
  // Serial.println("el motor va pa la izquierda");
  digitalWrite(motor1_1,LOW);
  digitalWrite(motor1_2,LOW);
  digitalWrite(motor2_1,HIGH);
  digitalWrite(motor2_2,LOW);
  analogWrite(pwmizq,mov/2);
  analogWrite(pwmder,mov);
}
void parar(){ 
  // Serial.println("apaga motores");
  digitalWrite(motor1_1,LOW);
  digitalWrite(motor2_1,LOW);
  digitalWrite(motor1_1,LOW);
  digitalWrite(motor2_2,LOW);
   analogWrite(pwmizq,0);
  analogWrite(pwmder,0);
}

void loop() {
 uint8_t numero_canal;
 //if ( radio.available(&numero_canal) )
 if ( radio.available() )
 {    
    
     // Serial.println("recibi info");
     //Leemos los datos y los guardamos en la variable datos[]
     radio.read(dato,sizeof(dato));
     
  //   Serial.flush();
     //reportamos por el puerto serial los datos recibidos
     write_leds(dato[0], dato[1]);
     tiempo = 0;
 }
 else
 {
  //  Serial.println("No hay datos de radio disponibles");
 tiempo ++;
 // Serial.println(tiempo);
 }
 
 if(dato[0]=='p'){
 
 validar(250);
 
 }
 
 
if ( (dato[0] == 'f') || (dato[0] == 'l')   || (dato[0] == 'z') ) {
    
  distancia();
  // Serial.println("mido distancia"); 
  if (distance <= 15)  {
   parar();
  // Serial.println("me voy a matar");
   dato[0]= 's';
  }
 // Serial.println("tiempesito");
  else if (tiempo == 1500){
   parar();
   tiempo = 0;
//  Serial.println("me pare por voluntada propia ");
   dato[0]= 's';
  }
 }
 
 
  if  (dato[0] == 'r'  &&  tiempo == 1500) {
  
   parar();
   tiempo = 0;
  // Serial.println("ya me canse");
   dato[0]= 's';
  }
   
 
 
 if (dato[0] == 'a') {
    
 distancia();
 // Serial.println("cheque la distancia"); 
 if (distance <= 15)  {
   
  // Serial.println("parate");
   
  parar();
   
  // Serial.println("parate");
 //dato[0]= 's';
   
   
  }
  else
  {
 // Serial.println("trabajo muy duro soy un esclavo");
read_sensor_values();
    calculate_pid();
    motor_control();
   

}
  //MODO AUTOMATICO 
 }




}


void write_leds(char led, int brightness)
{
   Serial.println(led);         
    Serial.println(brightness);
    //  int time;
//  Serial.println(led);
  distancia();
  if (led == 'r'){
     atras(255);
     
      return;
  }
     
  else if (led == 's'){
   parar();
  return;
  }
    else if(distance > 15){
      if (led == 'f'){
   // Serial.println("1");
   adelante(255);    
    return;
  }
  if (led == 'l'){
    giraIzquierda(255);
      return;
     
  }
  //Derecha
  if (led == 'z'){
    giraDerecha(255);
    return;
  }
  
  }
  else{
  parar();
  return;
  }    
      
    return;
  }
  
void distancia (){
  distance=ultrasonic.Ranging(CM);
  /*long duration = 0;
  distance= 0;
  digitalWrite(trig, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trig, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = (duration/2) / 29.1;  
// Serial.println(distance);
 */ 
}

void read_sensor_values()
{
  
    if(digitalRead(sensorm2)==LOW&&digitalRead(sensorm1)==LOW&&digitalRead(sensor0)==LOW&&digitalRead(sensor1)==LOW&&digitalRead(sensor2)==HIGH)
  error=4;
  else if(digitalRead(sensorm2)==LOW&&digitalRead(sensorm1)==LOW&&digitalRead(sensor0)==LOW&&digitalRead(sensor1)==HIGH&&digitalRead(sensor2)==HIGH)
  error=3;
else if(digitalRead(sensorm2)==LOW&&digitalRead(sensorm1)==LOW&&digitalRead(sensor0)==LOW&&digitalRead(sensor1)==HIGH&&digitalRead(sensor2)==LOW)
  error=2;
  else if(digitalRead(sensorm2)==LOW&&digitalRead(sensorm1)==LOW&&digitalRead(sensor0)==HIGH&&digitalRead(sensor1)==HIGH&&(digitalRead(sensor2)==LOW))
  error=1;
  else if(digitalRead(sensorm2)==LOW&&digitalRead(sensorm1)==LOW&&digitalRead(sensor0)==HIGH&&digitalRead(sensor1)==LOW&&digitalRead(sensor2)==LOW)
  error=0;
  else if(digitalRead(sensorm2)==LOW&&digitalRead(sensorm1)==HIGH&&digitalRead(sensor0)==HIGH&&digitalRead(sensor1)==LOW&&digitalRead(sensor2)==LOW)
  error=-1;
  else if(digitalRead(sensorm2)==LOW&&digitalRead(sensorm1)==HIGH&&digitalRead(sensor0)==LOW&&digitalRead(sensor1)==LOW&&digitalRead(sensor2)==LOW)
  error=-2;
  else if(digitalRead(sensorm2)==HIGH&&digitalRead(sensorm1)==HIGH&&digitalRead(sensor0)==LOW&&digitalRead(sensor1)==LOW&&digitalRead(sensor2)==LOW)
  error=-3;
  else if(digitalRead(sensorm2)==HIGH&&digitalRead(sensorm1)==LOW&&digitalRead(sensor0)==LOW&&digitalRead(sensor1)==LOW&&digitalRead(sensor2)==LOW)
  error=-4;
  else if(digitalRead(sensorm2)==LOW&&digitalRead(sensorm1)==LOW&&digitalRead(sensor0)==LOW&&digitalRead(sensor1)==LOW&&digitalRead(sensor2)==LOW)
    if(error==-4) error=-5;
    else error=5;

}

void calculate_pid()
{
  P = error;
    I = error + previous_I;
    D = error-previous_error;
    
    PID_value = (Kp*P) + (Ki*I) + (Kd*D);
    
    previous_I=I;
    previous_error=error;
  /*
  P = error;
     if(P!= 5 ||  P!=-5);
    {
    I = (P*.1) + previous_I;
    }
    I = constrain(I,-4,4);
    D = error-previous_error;
    
    PID_value = (Kp*P) + (Ki*I) + (Kd*D);
    
    previous_I=I;
    previous_error=error;*/
}

void motor_control()
{
    // Calculating the effective motor speed:
    int left_motor_speed = initial_motor_left+PID_value;
    int right_motor_speed = initial_motor_right-PID_value;
    
    // The motor speed should not exceed the max PWM value
   int left_speed = constrain(left_motor_speed,0,255);
   int right_speed = constrain(right_motor_speed,0,255);
  
  analogWrite(pwmizq,left_speed);   //Left Motor Speed
    analogWrite(pwmder,right_speed);  //Right Motor Speed
    //following lines of code are to make the bot move forward
    /*The pin numbers and high, low values might be different
    depending on your connections */
    digitalWrite(motor2_1,HIGH);
    digitalWrite(motor2_2,LOW);
    digitalWrite(motor1_2,LOW);
    digitalWrite(motor1_1,HIGH);
PID_value=0;
}

//VALIDAR
void validar(char mov){
  //ADELANTE
 digitalWrite(motor1_1,HIGH);
  digitalWrite(motor1_2,LOW);
  digitalWrite(motor2_1,HIGH);
  digitalWrite(motor2_2,LOW);
 analogWrite(pwmizq,mov);
  analogWrite(pwmder,mov);
  delay(500);
  //ATRAS
   digitalWrite(motor1_1,LOW);
  digitalWrite(motor1_2,HIGH);
  digitalWrite(motor2_1,LOW);
  digitalWrite(motor2_2,HIGH);
  analogWrite(pwmizq,mov);
  analogWrite(pwmder,mov);
   delay(500);  
dato[0]='s';
parar();
}


