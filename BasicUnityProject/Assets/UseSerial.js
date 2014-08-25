//
// Setting Variables to be used globally within the JavaScript
//

var InstanceObject : Transform;

var PositionContainer : GameObject;
var Cconnect;


function Start () {
	//set 'Cconnect' to be used as connector to the C# script.
	Cconnect = GetComponent("GUISerialScript");
}

function Update () {

}