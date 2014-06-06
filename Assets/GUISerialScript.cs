using UnityEngine;
using System;
using System.Collections;
using System.IO.Ports;


public class GUISerialScript : MonoBehaviour {
	public static string COMPort = "/dev/tty.FireFly-E79B-SPP";
	
	public static bool portopen = false; //if port is open or not
	public static SerialPort _SerialPort = new SerialPort(COMPort, 9600); //COM port and baudrate
	
	public static bool Errorhandling = false;

	public static byte[] send_high = {0xff};
	public static byte[] send_low = {0x00};


	// Use this for initialization
	void Start () {

	}
	
	// Update is called once per frame
	void Update () {
	
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
	
	public void vibrate(int i) {
		//sp.Close();
		//sp.Open();
		if (i == 1) {
			_SerialPort.Write (send_high, 0, send_high.Length);
		} else {
			_SerialPort.Write (send_low, 0, send_low.Length);
		}
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
