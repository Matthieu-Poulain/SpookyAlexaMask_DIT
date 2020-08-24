#include <Servo.h>
Servo eyes;  // create servo object to control a servo
Servo mouth;  // create servo object to control a servo
int pos = 90;    // variable to store the servo position




const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data

boolean newData = false;




int leftXPosMax = 20;
int rightXposMax = 50;
int leftAngle = 70;
int rightAngle = 110;

int noFaceIncrement =0;





void setup() {
    
    Serial.begin(115200); // opens serial port, sets data rate to 115200 baud
    eyes.attach(3);
    mouth.attach(4);
    eyes.write(pos);
    mouth.write(pos);
}



void loop() {

    recvWithEndMarker();
    ProcessNewData();
    adc_to_mouth(analogRead(A0));
    
}


void recvWithEndMarker() {

//Reveive Function. Aquires a string from Serial until it gets the endMarker

    static byte ndx = 0;
    char endMarker = ']';
    char startMarker = '[';
    char rc;
    
    // if (Serial.available() > 0) {
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        
        if (rc != endMarker && rc != startMarker ) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else if (rc != startMarker) {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}







void showNewDataDebug() {
//Debug Function
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}







void sendBackNewData() {
//Debug function sending back the processed data
    if (newData == true) {
        String incomingString = String(receivedChars);
        String outgoingString;
        if (incomingString.substring(0, 6).equals("srvCMD")){
            outgoingString = String(incomingString.substring(6,(incomingString.length()+1)));
        }
        else{
            outgoingString = incomingString;
        }
        Serial.print(outgoingString);
        newData = false;
    }
}



void ProcessNewData() {
// Full function processing the command and controlling the eyes' servo.
    if (newData == true) {
        String incomingString = String(receivedChars);
        String outgoingString;
        if (incomingString.substring(0, 6).equals("srvCMD")){
            outgoingString = incomingString.substring(6,(incomingString.length()+1));
            convertAndSendSrvPos(outgoingString);
            if (noFaceIncrement!=0){
              noFaceIncrement = 0;
            }
              
            
            
        }
        else if(incomingString.substring(0, 6).equals("NoFace")){
            noFaceIncrement += 1;
            if (noFaceIncrement==5){
                eyes.write(90); // reset eyes position
                noFaceIncrement=0;
            }
            outgoingString = incomingString;

        }
        
        Serial.print(outgoingString);
        newData = false;
    }
}


void convertAndSendSrvPos(String strPos){
  //int pixReducedPos =  int(strPos.toInt()/10); // Less precise but more stable if the processing CPU is too fast.
  int pixReducedPos =  int(strPos.toInt());
  eyes.write(map(pixReducedPos, leftXPosMax*10, rightXposMax*10, leftAngle, rightAngle));
}


void adc_to_mouth(int adc_val){  //maps the ADC val from alexa audio line out to the mouth position

  static int mouth_val;
  mouth_val = map(adc_val, 0, 500, 0, 100);
  set_mouth_position(constrain(mouth_val,0,100));
  //Serial.println(adc_val);
  
}


void set_mouth_position(int percent_open){

  static int us_val;
  us_val = map(percent_open, 0, 100, 1500, 1300);
  mouth.writeMicroseconds(us_val);
  
}