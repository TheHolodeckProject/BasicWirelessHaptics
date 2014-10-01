using UnityEngine;
using System;
using System.Collections;
using System.IO.Ports;

public class BluetoothScript : MonoBehaviour {

	//public static string COMPort = "/dev/tty.FireFly-E79B-SPP";
	public static string COMPort = "COM5";

	public static bool portopen = false; //if port is open or not
	public static SerialPort _SerialPort = new SerialPort(COMPort, 9600); //COM port and baudrate
	
	public static bool Errorhandling = false;

	public static byte[] handState = {0x00,0x00};
	public static byte[] thumbFinger = {0x10};
	public static byte[] indexFinger = {0x08};
	public static byte[] middleFinger = {0x04};
	public static byte[] ringFinger = {0x02};
	public static byte[] pinkyFinger = {0x01};
	public static byte[] high = {0xff};
	public static int intensity = 255;

	// Use this for initialization
	void Start () {

	}
	
	// Update is called once per frame
	void Update () {
		if (Input.GetKeyDown (KeyCode.F) && portopen)
		{
			if(intensity == 255)
			{
				print("Can't go any higher (Current Intensity: " + intensity + ") :(");
			}
			else
			{
				print("Current Intensity: " + intensity + ".");
				intensity += 10;
			}
			vibrate("rightHand", "rightThumb",intensity);
		}
		if (Input.GetKeyDown (KeyCode.E) && portopen)
		{
			if((intensity-10)< 0)
			{
				print("Can't go any lower (Current Intensity: " + intensity + ") :(");
			}
			else
			{
				print("Current Intensity: " + intensity + ".");
				intensity -= 10;
			}
			vibrate("rightHand", "rightThumb",intensity);
		}


		if (Input.GetKeyDown (KeyCode.N) && portopen)
		{
			print ("RightThumbDown");
			vibrate("rightHand", "rightThumb",intensity);
		}
		if (Input.GetKeyUp (KeyCode.N) && portopen)
		{
			print ("RightThumbUp");
			vibrate("rightHand", "rightThumb",0);
		}
		if (Input.GetKeyDown (KeyCode.U) && portopen)
		{
			print ("RightIndexDown");
			vibrate("rightHand", "rightIndex",intensity);
		}
		if (Input.GetKeyUp (KeyCode.U) && portopen)
		{
			print ("RightIndexUp");
			vibrate("rightHand", "rightIndex",0);
		}
		if (Input.GetKeyDown (KeyCode.I) && portopen)
		{
			print ("RightMiddleDown");
			vibrate("rightHand", "rightMiddle",intensity);
		}
		if (Input.GetKeyUp (KeyCode.I) && portopen)
		{
			print ("RightMiddleUp");
			vibrate("rightHand", "rightMiddle",0);
		}
		if (Input.GetKeyDown (KeyCode.O) && portopen)
		{
			print ("RightRingDown");
			vibrate("rightHand", "rightRing",intensity);
		}
		if (Input.GetKeyUp (KeyCode.O) && portopen)
		{
			print ("RightRingUp");
			vibrate("rightHand", "rightRing",0);
		}
		if (Input.GetKeyDown (KeyCode.P) && portopen)
		{
			print ("RightPinkyDown");
			vibrate("rightHand", "rightPinky",intensity);
		}
		if (Input.GetKeyUp (KeyCode.P) && portopen)
		{
			print ("RightPinkyUp");
			vibrate("rightHand", "rightPinky",0);
		}
	}

	//the GUI
	void OnGUI()
	{
		if(portopen) {
			if(GUILayout.Button("Disconnect from Bluetooth")) {
				_SerialPort.Close();
				portopen = false;
			}
		} else {
			if (GUILayout.Button ("Connect to Bluetooth")) {
				portopen = OpenConnection();
			}
		}
	}

	//Start the connection!
	public static bool OpenConnection()
	{
		if (_SerialPort != null)
		{
			if (_SerialPort.IsOpen)
			{
				_SerialPort.Close();
				print("Closing port, because it was already open!");
				return false;
			}else{
				try {
					_SerialPort.Open();
					if (Errorhandling){
						print("Port open = "+_SerialPort.IsOpen);
					}
				} catch(System.IO.IOException e) {
					if (e.Source != null)
						Debug.Log("Could not pair with BT device!");
						return false;
				}
			}
		}else
		{
			if (_SerialPort.IsOpen)
			{
				print("Port is already open");
			}else
			{
				print("Port == null");
				return false;
			}
		}
		return true;
	}// OpenConnection
	
	public void vibrate(String hand, String finger, int intensity) 
	{
		handState [0] = handState [1] = 0x00;

		if (hand == "leftHand") 
		{
			handState[0] |= 0x80;
		}

		switch(finger)
		{
		case "rightThumb":  // Index Finger
		{
			handState[0] |= thumbFinger[0];
			break;
		}
		case "rightIndex":  // Index Finger
		{
			handState[0] |= indexFinger[0];
			break;
		}
		case "rightMiddle":  // Index Finger
		{
			handState[0] |= middleFinger[0];
			break;
		}
		case "rightRing":  // Index Finger
		{
			handState[0] |= ringFinger[0];
			break;
		}
		case "rightPinky":  // Index Finger
		{
			handState[0] |= pinkyFinger[0];
			break;
		}
		default:
			break;
		}

		handState[1] |= (byte) intensity;	//255
		_SerialPort.Write (handState, 0, handState.Length);
	}

	//make sure the connection to the arduino is closed.
	void OnApplicationQuit() {
		
		if (Errorhandling){
			print("closing connection because of program exit");
		}
		_SerialPort.Close();
		portopen = false;
	}
}
