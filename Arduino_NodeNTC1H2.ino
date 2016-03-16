#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 4  //pinnissä d4 on anturin
   
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;

#include <SPI.h>
#include "RF24.h"
#include "thingspeakrequest.h"
#include <math.h>         //loads the more advanced math functions
#include <avr/io.h>
#include <avr/interrupt.h>

#define SERIAL_DEBUG  1   //1 = On, 0 = Off

#define RND_MIN  -50
#define RND_MAX  50

//#define INTERVAL_MSEC 900000 //15 Minutes
#define INTERVAL_MSEC 16000 //16 sec

#define TS_W_API_KEY  "1BLSSRQMMNLXNIRA"  

#define TS_R_API_KEY  "EIS2QHC6USTCNRA2" //lukemista varten
#define TS_W_CH_NUM  "86934"  // 84035 oma kanava

//pinMode(2, INPUT);
volatile int sensorValue = 0;

int m_iBlockSize = 32;
char message[512];
float temp;
float Temp;
int val;
long int ovi= 0;
int sensorPin = A1; // select the input pin for ldr
//int sensorArvo = 0; // variable to store the value coming from the 



/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
                     
byte addresses[][6] = {"1Sepp","5Sepp"}; 
//byte addresses[][6] = {"1Node","4Node"};
void sendMessage(const char *msg);
float Thermister(int RawADC);
int valonMitt();

volatile int ovi1MuutosMaara;

ThingSpeakUploadRequest uploadRequest;

void setup()
{
    if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");  
      // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);
    ovi1MuutosMaara = 0;
    sei(); 
    pinMode(2, INPUT);  //INT0 keskeytys
    attachInterrupt(0, INT0_handler, CHANGE);
    radio.begin();  
    radio.setPALevel(3);
    
    radio.setAutoAck(1);                    // Ensure autoACK is enabled
    radio.enableAckPayload();               // Allow optional ack payloads
    radio.setRetries(0,15);                 // Smallest time between retries, max no. of retries
    radio.setPayloadSize(32);
    
    radio.openWritingPipe(addresses[1]);        // Both radios listen on the same pipes by default, but opposite addresses
    radio.openReadingPipe(1,addresses[0]);      // Open a reading pipe on address 0, pipe 1
    
    if(SERIAL_DEBUG) Serial.begin(9600);         
    
    if(SERIAL_DEBUG)Serial.print("\n"); 
}
void loop()
{ 

  sensors.requestTemperatures(); // Send the command to get temperatures
  
  float tempC = sensors.getTempC(insideThermometer);

   
   //int val;                //Create an integer variable
 // double temp;            //Variable to hold a temperature value
  val=analogRead(0);      //Read the analog port 0 and store the value in val
  temp=Thermister(val);   //Runs the fancy math on the raw analog value
  Serial.println(temp);   //Print the value to the serial port


   uploadRequest.SetApiKey(TS_W_API_KEY);
  uploadRequest.SetFieldData(1,temp);
  uploadRequest.SetFieldData(2,tempC);  //Huone 2 lampo
  uploadRequest.SetFieldData(3,valonMitt());  //Huone valotpaalla/poissa
  uploadRequest.SetFieldData(4,ovi);
 
  uploadRequest.ToString(message); 
  sendMessage(message);
  ovi1MuutosMaara = 0;
  int teho = radio.getPALevel();
  Serial.println(teho);
  Serial.print("Timon anturi   ");
  Serial.print(tempC); 
  Serial.print("Celssius");
  
  
  
  
  
   delay(INTERVAL_MSEC);



    
}

void sendMessage(const char *msg)
{   
     unsigned int msgLen = strlen(msg);
     unsigned int fullBlocks = msgLen / m_iBlockSize;
     char lastBlock[m_iBlockSize];
     memset(lastBlock,0,sizeof(lastBlock));

     unsigned int block;

     for(block = 0; block < fullBlocks; block++)
     {
         radio.write(&msg[block*m_iBlockSize],m_iBlockSize);
     }

     strcpy(lastBlock,&msg[block*m_iBlockSize]);
     radio.write(&msg[block*m_iBlockSize],m_iBlockSize,0);
}


 

 
float Thermister(int RawADC) {  //Function to perform the fancy math of the Steinhart-Hart equation
 //double Temp;
 Temp = log(((10240000/RawADC) - 10000));
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15 ;              // Convert Kelvin to Celsius
 //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit - comment out this line if you need Celsius
 return Temp;
}
 
void INT0_handler()
{
  cli();
  
  ovi1MuutosMaara++;
  
  //Serial.println("Ovi auku");
  sensorValue = digitalRead(2);
  if (sensorValue){
     ovi = 1;
     Serial.println("Ovi auki"); 
  }

  else {
  ovi = 0;
  Serial.println("Ovi kiinni");
  }

  if(ovi1MuutosMaara > 1)
  {
         ovi = 1;
  }
  
  sei();
}




int valonMitt() {
int sensorArvo = analogRead(sensorPin); // read the value from the sensor
Serial.println(sensorArvo); //prints the values coming from the sensor on the screen
return sensorArvo;
}

























 
