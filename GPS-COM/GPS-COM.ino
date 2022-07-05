#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <QMC5883LCompass.h>
#include <Stepper.h>

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
static const double HOME_LAT = 15.48331, HOME_LON = 73.80937;
int delta=0;
const int stepsPerRevolution = 2038;

// Creates an instance of stepper class
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

// The TinyGPS++ object
TinyGPSPlus gps;

//compass Object
QMC5883LCompass compass;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  compass.init();
  myStepper.setSpeed(5);
}

void loop()
{
  updateCompass(homeHeading(), heading());
  smartDelay(1000);
}

double homeHeading()
{
  double courseToHome =
    TinyGPSPlus::courseTo(
      gps.location.lat(),
      gps.location.lng(),
      HOME_LAT, 
      HOME_LON);
  return courseToHome;
}
void updateCompass(double courseH,int H)
{
  //code to move stepper motor by (courseH-H)-delta
  double angle=(courseH-H)-delta;
  //myStepper.step(angle*5.66);
  //delta = angle;
  Serial.print(angle);
  Serial.print("    ");
  Serial.print(courseH);
  Serial.print("    ");
  Serial.println(H);
}
int heading()
{
  int a;
  
  // Read compass values
  compass.read();

  // Return Azimuth reading
  a = compass.getAzimuth();
  return a;
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
