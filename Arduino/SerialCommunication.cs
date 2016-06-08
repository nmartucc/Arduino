using UnityEngine;
using System.Collections;
using System.IO.Ports;

public class SerialCommunication : MonoBehaviour {

	// Use this for initialization
	SerialPort sPort = new SerialPort("COM3", 9600);

	void Start () {
		sPort.Open ();	//opens serial port connection
	}	
	
	// Update is called once per frame
	void Update () {
		string rawData = sPort.ReadLine ();	//waits until newline char is transmitted and reads the line
		char delimiter = ',';
		string[] distancestrings = rawData.Split (delimiter);	//splits string into strings delimited by ','
		double[] parseddistances = new double[distancestrings.Length];
		for(int i = 0; i < distancestrings.Length; ++i){	//converts each string in the array to a double
			parseddistances[i] = int.Parse (distancestrings[i]);
		}
	}
}
