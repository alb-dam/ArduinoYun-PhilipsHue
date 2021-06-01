#include <Process.h>

static int light_sensor = A0; //analogic pin connected to the light sensor
static int motion_sensor = 3; //digital pin connected to the motion sensor
static int await_minutes = 4; //specify the minutes before the light shutdown automatically
static int treshold = 150; //modify this value to change the treshold the light turns on

unsigned long await; //this variable hold the minutes expressed in milliseconds
int brightness = 0; //this variable hold the brightness index of the light sensor
boolean movement;  //this variable hold the state of the motion sensor
boolean light_state; //this variable hold the state of the light, true = 1 = on, false = 0 = off
boolean last_light_state; //this variable hold the last state of the light

//change IPADRESS, Philips Hue Bridge's USERNAME and the NUMBER of the light you want to control (e.g .../lights/6/state)
String turn_on = "curl -X PUT -d '{\"on\":false}' http://IPADRESS/api/USERNAME/lights/NUMBER/state";
String turn_off = "curl -X PUT -d '{\"on\":true}' http://IPADRESS/api/USERNAME/lights/NUMBER/state";

void setup() {
  Bridge.begin();  //initialize the bridge to send request from linux shell
  Serial.begin(9600); //initialize the serial port to show info
  //define pin mode (input/output)
  pinMode(light_sensor, INPUT);
  pinMode(motion_sensor, INPUT);
  await = ((await_minutes * 60UL ) * 1000UL); //change await minutes to milliseconds so we can use them in delay function
  light_state = false;   //default light state = off
  last_light_state = true;  //default last light state = on
}

void loop() {
  Process p;

  brightness = analogRead(light_sensor);   //read brightness index from pin A0 (light_sensor)
  movement = digitalRead(motion_sensor);   //read motion sensor state from pin 3 (motion_sensor)

  if (treshold < brightness && movement == HIGH) { //if the brightness index is higher than trashold and movement is detected
    light_state = true;                             //set the light state on
  }
  else {                                            //in any other case
    light_state = false;                            //set the light state off
  }
  //enter the loop only if a light state change happend
  if (light_state != last_light_state) {
    //than if the condition is true turn on light
    if (light_state == true) {
      Serial.print(" - Turning on light "); //send info to serial monitor
      p.runShellCommand(turn_on); //turn on light sending the request to Philips Hue Hub
      delay(await);
      //if the condition is false turn off light
    } else if (light_state == false) {
      Serial.print(" - Turning off light "); //send info to serial monitor
      p.runShellCommand(turn_off); //turn off light sending the request to Philips Hue Hub
    }
    delay(100);
  }
  last_light_state = light_state;
  //send info to serial monitor
  Serial.print("Brightness perceived : ");
  Serial.print(brightness);
  Serial.print(" Movement detected : ");
  Serial.print(movement);
  Serial.print(" - Light state : ");
  Serial.println(light_state);
  delay(50);
}
