#include <SerialGSM.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

SerialGSM cell(2,3);
SoftwareSerial uart_gps(2,3);

TinyGPS gps;

int x, y, z, x1, y1, z1;
float fLat, fLon;

void getgps(TinyGPS &gps);


void setup()
{ 
	Serial.begin(115200); //115200
      

 	cell.begin(9600);	// Initialises gsm module, begins to setup
 	delay(30000);		// wait for GSM module to be fully registered on network

 	getInitialValues();
}


void getInitialValues()
{
	//store the initial readings from acelerometer when it is in situe
	x = analogRead(2);       // read analog input pin 0 - the x value
  	y = analogRead(1);       // read analog input pin 1 - the y value
  	z = analogRead(0);       // read analog input pin 1 - the z value
}

void loop()
{

	if(bikeDisturbed() == true)
  	{
    	alarmSequence();
  	}
  
}

boolean  bikeDisturbed()
{	
	//check if the accelerometer readings have changed from the initial values
  	x1 = analogRead(2);
  	y1 = analogRead(1);
  	z1 = analogRead(0);
  
  	if(x1 > (x+20) || x1 < (x-20))
  	{
    	return true;
  	}
  
  	else if (y1 > (y+20) || y1 < (y-20))
  	{
    	return true;
  	}
  
  	else if (z1 > (z+20) || z1 < (z-20))
  	{
    	return true;
  	}
  
  	else
  	{
    	return false;
  	}
}

void alarmSequence()
{

 	warningCall();	//call mobile phone for 20 seconds then hang up
        cell.end();

 	do
 	{
 		getGPSInfo();	//get updated gps info continuously
 	}
 	while(1>0);

}

void warningCall()
{
 	cell.println("ATDXXXXXXXXX"); // dial the phone number. replace XXXXX with number including country code.
	delay(20000); // wait 20 seconds.
 	cell.println("ATH"); // end call
}


void getGPSinfo()
{
    uart_gps.begin(4800);
        delay(30000);

  while(uart_gps.available())     // While there is data on the RX pin...
  {
      int c = uart_gps.read();    // load the data into a variable...
      if(gps.encode(c))      // if there is a new valid sentence...
        {
           getgps(gps); //grab the data
           sendSMS(fLat, fLon);
	   delay(30000);
        }
  }

}

void getgps(TinyGPS &gps)
{
        gps.f_get_position(&fLat, &fLon);
        uart_gps.end();
}

void sendSMS(float latitude, float longitude)
{
        
        cell.begin(9600);
        delay(30000);
        
	char strLat [12];
	char strLon [12];

	dtostrf(latitude, 8, 6, strLat);
	dtostrf(longitude, 8, 6, strLon);

	String message = "http://maps.google.com/maps?q=";
	message += strLat; //adds latitude to the string
	message += ",";
	message += strLon; //adds longitude to string
 
 	char charMessage[160];
 	message.toCharArray(charMessage, 160);
 
 	cell.Verbose(true); // used for debugging
 	cell.Boot(); 
 	cell.FwdSMS2Serial();
 	cell.Rcpt("XXXXXXXXXX"); // number to send the sms to
 	cell.Message(charMessage);
 	cell.SendSMS();
 
        cell.end();
        
        uart_gps.begin(4800);  
        delay(30000);
        
       // uart_gps.listen();
}
