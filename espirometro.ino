#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,10,9,8,7);

const int EchoPin = 5;
const int TriggerPin = 6;
volatile int NumPulsos; //variable para la cantidad de pulsos recibidos
int PinSensor = 2;    //Sensor conectado en el pin 2
float factor_conversion = 7.5; //para convertir de frecuencia a caudal
float volumen = 0;
long dt = 0; //variación de tiempo por cada bucle
long t0 = 0; //millis() del bucle anterior


void setup() {
  lcd.begin(16,2);
  
  Serial.begin(9600);
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(PinSensor, INPUT);
  attachInterrupt(0, ContarPulsos, RISING); //(Interrupcion 0(Pin2),funcion,Flanco de subida)
  Serial.println ("Envie 'r' para restablecer el volumen a 0 Litros");
  t0 = millis();
}

void loop() {
  int cm = ping(TriggerPin, EchoPin);

  if (Serial.available()) {
    if (Serial.read() == 'r')volumen = 0; //restablecemos el volumen si recibimos 'r'
  }
  float frecuencia = ObtenerFrecuencia(); //obtenemos la Frecuencia de los pulsos en Hz
  float caudal_L_m = frecuencia / factor_conversion; //calculamos el caudal en L/m
  dt=millis()-t0; //calculamos la variación de tiempo
  t0=millis();
  volumen=volumen+(caudal_L_m/60)*(dt/1000); // volumen(L)=caudal(L/s)*tiempo(s)

  //-----Enviamos por el puerto serie---------------//
  Serial.print("D: ");
  Serial.print(cm, 1);
  Serial.print ("cm\tF: ");
  Serial.print (frecuencia, 0);
  Serial.print ("Hz\tC: ");
  Serial.print (caudal_L_m, 2);
  Serial.print ("L/min\tVolumen: "); 
  Serial.print (volumen,3); 
  Serial.println (" L");
  delay(1000);

  //-----Enviamos a la LCD----//
  lcd.setCursor(0,0);
  lcd.print("F: ");
  lcd.print(caudal_L_m);
  lcd.print("L/min");
  lcd.setCursor(0,1);
  lcd.print("V: ");
  lcd.print(volumen);
  lcd.print(" L");
}

//---Función que se ejecuta en interrupción---------------//
void ContarPulsos ()
{
  NumPulsos++;  //incrementamos la variable de pulsos
}

//---Función para obtener frecuencia de los pulsos--------//
int ObtenerFrecuencia()
{
  int frecuencia;
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(1000);   //muestra de 1 segundo
  noInterrupts(); //Desabilitamos las interrupciones
  frecuencia = NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}


int ping(int TriggerPin, int EchoPin) {
  long duration, distanceCm;

  digitalWrite(TriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  duration = pulseIn(EchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos

  distanceCm = duration * 10 / 292 / 2;  //convertimos a distancia, en cm
  return distanceCm;
}
