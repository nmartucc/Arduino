/**
NOTE: This program only works for getting distance values from ONE sensor at a time - it continually reads lines from the serial monitor and 
parses it as an integer (or you can change to float, double, etc.)  Also, make sure in the Arduino code that there is no extra output, only numbers, (in other words, no letters or words)
otherwise this script may not work.  The distances are stored in the distance1 variable so you can do whatever you want to it.  
As a final note, the ReadLine() command reads one line from when you do Serial.println() in the Arduino code, so make sure in that code you pass information in the form of Serial.println().
**/

using UnityEngine;
using UnityEngine.Networking;
using System.Collections;
using System.IO.Ports;

public class TestSensor : NetworkBehaviour {

	[SyncVar]
	int distance1, distance2 = 0;

	SerialPort sPort = new SerialPort("COM3", 9600);
	// Use this for initialization
	void Start () {
		sPort.Open();
	}
	
	// Update is called once per frame
	void Update () {
		if(isServer){
			string datafromArduino = sPort.ReadLine ();
			distance1 = int.Parse (datafromArduino);
			print ("Ultrasound sensor 1 data: ");
			print (distance1);
		}
		if (isLocalPlayer) {
			if (distance1 < 300) { //assuming cm, if the sensor measures meters then 3 instead of 300
				print ("Warning: wall up ahead in less than 3m(~10ft)");
			}
			//return;
		}
	}
}