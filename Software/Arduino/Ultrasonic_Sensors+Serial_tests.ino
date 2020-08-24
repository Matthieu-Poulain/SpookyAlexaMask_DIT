#include <Servo.h>

Servo mouth;  // create servo object to control a servo
int pos = 90;    // variable to store the servo position



/* Constantes pour les broches */
const byte TRIGGER_1 = 2; // Broche TRIGGER 1
const byte ECHO_1 = 3;    // Broche ECHO 1

const byte TRIGGER_2 = 4; // Broche TRIGGER 2
const byte ECHO_2 = 5;    // Broche ECHO 2

const byte TRIGGER_3 = 6; // Broche TRIGGER 3
const byte ECHO_3 = 7;    // Broche ECHO 3

const byte TRIGGER_4 = 8; // Broche TRIGGER 4
const byte ECHO_4 = 9;    // Broche ECHO 4

//const byte TRIGGER_5 = 10; // Broche TRIGGER 5
//const byte ECHO_5 = 11;    // Broche ECHO 5


/* Constantes pour le timeout */
const unsigned long MEASURE_TIMEOUT = 25000UL; // 25ms = ~8m à 340m/s

/* Vitesse du son dans l'air en mm/us */
const float SOUND_SPEED = 340.0 / 1000;
int currentPos = 50;

float initDist[4] = {0, 0, 0, 0};
//float initDist_5=0;

//Replaced by:
float currentDist[4];

float initArrayForMean_1[5];
float initArrayForMean_2[5];
float initArrayForMean_3[5];
float initArrayForMean_4[5];
float initArrayForMean_5[5];

float distArrayForMean_1[5];
float distArrayForMean_2[5];
float distArrayForMean_3[5];
float distArrayForMean_4[5];
float distArrayForMean_5[5];


//float valueArray[6]={0,0,0,0,0};
float tampon_DistMin = 3000;
int positionOeil = 0;
static int us_val;


float Distance_mesurement(const byte Trigger, const byte Echo) {

  /* 1. Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER */
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);

  /* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe) */
  long measure = pulseIn(Echo, HIGH, MEASURE_TIMEOUT);

  /* 3. Calcul la distance à partir du temps mesuré */
  float distance_cm = measure / 0.2 * SOUND_SPEED;
  // float test = 3000 * (1 / (1 / 2.0 * SOUND_SPEED));
  /* Affiche les résultats en mm, cm et m */
  //Serial.print(F("mesure: "));
  //Serial.print(distance_mm / 10.0, 2);
  //Serial.println(F("cm "));

  //return measure;
  return distance_cm;
}

void swap(float *p, float *q) {
  float t;

  t = *p;
  *p = *q;
  *q = t;
}

void sort(float a[], int n) {
  int i, j;

  for (i = 0; i < n - 1; i++) {
    for (j = 0; j < n - i - 1; j++) {
      if (a[j] > a[j + 1])
        swap(&a[j], &a[j + 1]);
    }
  }
}

void setup() {
  /* Initialise le port série */
  Serial.begin(115200);
  mouth.attach(3);  // attaches the servo on pin 3 to the servo object
  /* Initialise les broches */
  pinMode(TRIGGER_1, OUTPUT);
  digitalWrite(TRIGGER_1, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_1, INPUT);

  pinMode(TRIGGER_2, OUTPUT);
  digitalWrite(TRIGGER_2, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_2, INPUT);

  pinMode(TRIGGER_3, OUTPUT);
  digitalWrite(TRIGGER_3, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_3, INPUT);

  pinMode(TRIGGER_4, OUTPUT);
  digitalWrite(TRIGGER_4, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_4, INPUT);

  //pinMode(TRIGGER_5, OUTPUT);
  //digitalWrite(TRIGGER_5, LOW); // La broche TRIGGER doit être à LOW au repos
  //pinMode(ECHO_5, INPUT);
  delay(7000);






  for (int i = 0; i < 5; i++) {

    initArrayForMean_1[i] = Distance_mesurement(TRIGGER_1, ECHO_1);
    if (initArrayForMean_1[i] == 0) {
      initArrayForMean_1[i] = 17700;

    }
    delay(31);
    initArrayForMean_2[i] = Distance_mesurement(TRIGGER_2, ECHO_2);
    if (initArrayForMean_2[i] == 0) {
      initArrayForMean_2[i] = 17700;

    }
    delay(31);
    initArrayForMean_3[i] = Distance_mesurement(TRIGGER_3, ECHO_3);
    if (initArrayForMean_3[i] == 0) {
      initArrayForMean_3[i] = 17700;

    }
    delay(31);
    initArrayForMean_4[i] = Distance_mesurement(TRIGGER_4, ECHO_4);
    if (initArrayForMean_4[i] == 0) {
      initArrayForMean_4[i] = 17700;

    }
    delay(31);


  }
  sort(initArrayForMean_1, 5);
  initDist[0] = initArrayForMean_1[2];

  sort(initArrayForMean_2, 5);
  initDist[1] = initArrayForMean_2[2];

  sort(initArrayForMean_3, 5);
  initDist[2] = initArrayForMean_3[2];

  sort(initArrayForMean_4, 5);
  initDist[3] = initArrayForMean_4[2];

  //sort(initArrayForMean_5,5);
  //initDist[4]=initArrayForMean_5[2];

  Serial.println(" . ");
  for (int i = 0; i < 3; i++) {
    Serial.println(initDist[i]);
  }
  Serial.println(" . ");

}



void loop() {



  for (int i = 0; i < 5; i++) {

    distArrayForMean_1[i] = Distance_mesurement(TRIGGER_1, ECHO_1);
    if (distArrayForMean_1[i] == 0) {
      distArrayForMean_1[i] = 17700;

    }
    delay(31);
    distArrayForMean_2[i] = Distance_mesurement(TRIGGER_2, ECHO_2);
    if (distArrayForMean_2[i] == 0) {
      distArrayForMean_2[i] = 17700;

    }
    delay(31);
    distArrayForMean_3[i] = Distance_mesurement(TRIGGER_3, ECHO_3);
    if (distArrayForMean_3[i] == 0) {
      distArrayForMean_3[i] = 17700;

    }
    delay(31);
    distArrayForMean_4[i] = Distance_mesurement(TRIGGER_4, ECHO_4);
    if (distArrayForMean_4[i] == 0) {
      distArrayForMean_4[i] = 17700;

    }
    delay(31);

    /*
      distArrayForMean_5[i]=Distance_mesurement(TRIGGER_5,ECHO_5);
      if (distArrayForMean_5[i] ==0){
      distArrayForMean_5[i] = 17700;
      }
      delay(31);
    */
  }

  float distance_mm;



  sort(distArrayForMean_1, 3);
  currentDist[0] = distArrayForMean_1[2]/initDist[0];

  sort(distArrayForMean_2, 3);
  currentDist[1]  = distArrayForMean_2[2]/initDist[1];

  sort(distArrayForMean_3, 3);
  currentDist[2] = distArrayForMean_3[2]/initDist[2];

  sort(distArrayForMean_4, 3);
  currentDist[3] = distArrayForMean_4[2]/initDist[3];

  //sort(distArrayForMean_5,3);
  //currentDist[4] =distArrayForMean_5[2]/initDist[4];

  /*
     for(int j=0;j<=3;j++){
    distance_mm= currentDist[j] / 2.0 * SOUND_SPEED;
    Serial.print(j);
    Serial.print(" : ");
    Serial.println(distance_mm / 10.0, 2);
    }
  */



  Serial.println(" ");
  Serial.println(currentDist[0]);
  Serial.println(currentDist[1]);
  Serial.println(currentDist[2]);
  Serial.println(currentDist[3]);
  Serial.println(" currentdistmin: ");
  sort(currentDist, 4);

  Serial.println(currentDist[0]);
  if (currentDist[0] == distArrayForMean_1[2]/initDist[0] && currentDist[0]<=0.9)
    positionOeil = 1;
  else if (currentDist[0] == distArrayForMean_2[2]/initDist[1] && currentDist[0]<=0.9)
    positionOeil = 2;
  else if (currentDist[0] == distArrayForMean_3[2]/initDist[2] && currentDist[0]<=0.9)
    positionOeil = 3;
  else if (currentDist[0] == distArrayForMean_4[2]/initDist[3] && currentDist[0]<=0.9)
    positionOeil = 4;
  // Include position 5
  else
    positionOeil = 0;




  //positionOeil = 1;
Serial.print("posoeil: ");
Serial.println(positionOeil);

  switch (positionOeil) {
    case 1:
        currentPos=0;
        break;
    case 2:
      currentPos = 20;
      break;
    case 3:
      currentPos = 45;
      break;
    case 4:
      currentPos = 65;
      break;
    case 5:
      currentPos = 90;
      break;
    default:
      //Insert a reset counter
      break;
  }

  us_val = map(currentPos, 0, 100, 544, 1500);
  mouth.writeMicroseconds(us_val);

  //delay(100);



}


