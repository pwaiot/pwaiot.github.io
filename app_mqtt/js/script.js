/**
 * config here
 */
//make your account to use "cloudmqtt.com"
//or use eclipse.org - public
var topicDsBrd = 'Vfc/Accellog/Lab/Developer/PWAIoT/Test01/Dashboard';
var topicCmd   = 'Vfc/Accellog/Lab/Developer/PWAIoT/Test01/Commands';

var mqttURI  = '';  //eclipse - ws://iot.eclipse.org:443/ws' - if config, ignore host/port/path;
var mqttHost = (localStorage.getItem('vfcmqtt-mqttHost') || 'iot.eclipse.org');
var mqttPort = Number(localStorage.getItem('vfcmqtt-mqttPort') || 443);
var mqttPath = '/ws';  //eclipse - '/ws'
var mqttUser = (localStorage.getItem('vfcmqtt-mqttUser') || ''); //eclipse - '' (empty)
var mqttPwd  = (localStorage.getItem('vfcmqtt-mqttPwd' ) || ''); //eclipse - '' (empty)
var mqttUseSSL = true; //SSL
var mqttVersion = 4;
var mqttTimeout = 5;
/** */

//console.log('port:', mqttPort);

$('#configHost').val(mqttHost);
$('#configPort').val(mqttPort);
$('#configUser').val(mqttUser);
$('#configPwd' ).val(mqttPwd);

//Connect Options
var options = {
  //Gets Called if the connection has sucessfully been established
  onSuccess: mqtt_OnConnect,
  //Gets Called if the connection could not be established
  onFailure: mqtt_OnFailure,

  useSSL: mqttUseSSL,
  mqttVersion: mqttVersion,
  timeout: mqttTimeout,
  userName: mqttUser,
  password: mqttPwd
};

// message command format:
// {"cmd": "", "value": ""}
var jsonMsg = {
  cmd: null,
  value: null
};

var jsonRetMsg = {
  cmd: null,
  value: null,
  datetime: null
};

$( document ).ready(function() {
  mqtt_Connect(); //ativar para conectar automaticamente
});

function myclientid() {
  return "myclientid_" + parseInt(Math.random() * 100, 10);
}

var client;

//Using the HiveMQ public Broker, with a random client Id
if (mqttURI != '') {
  client = new Paho.MQTT.Client(mqttURI, 'myclientid_' + myclientid());
} else {
  client = new Paho.MQTT.Client(mqttHost, mqttPort, mqttPath, myclientid());
}

//Gets  called if the websocket/mqtt connection gets disconnected for any reason
client.onConnectionLost = function (responseObject) {
  //Depending on your scenario you could implement a reconnect logic here
  mqtt_PrintAndShowLogMsg('connection lost: ' + responseObject.errorMessage);
};

client._disconnected = function () {
  mqtt_PrintAndShowLogMsg('disconnected');
};

//Gets called whenever you receive a message for your subscriptions
client.onMessageArrived = function (message) {
  try {
    //convert to JSON object
    //console.log('mqtt message received: ', message.payloadString);
    var obj = jsonRetMsg;
    obj = JSON.parse(message.payloadString);

    //console.log('objeto recebido: ');
    //console.log(obj);
    mqtt_PrintAndShowLogMsg('Tópico: ' + message.destinationName);
    mqtt_PrintAndShowLogMsg('objeto recebido: ');
    mqtt_PrintAndShowLogMsg(JSON.stringify(obj));
    //mqtt_PrintAndShowLogMsg(obj.cmd);
    //mqtt_PrintAndShowLogMsg(obj.value);
  }
  catch(err) {
    console.log("Error: " + err + ".");
  }
  finally {
    //recebendo = false;
  }
};

//Creates a new Messaging.Message Object and sends it to the HiveMQ MQTT Broker
var publish = function (payload, topic, qos) {
  //Send your message (also possible to serialize it as JSON or protobuf or just use a string, no limitations)
  payload = JSON.stringify(payload); //convert message to JSON

  var message = new Paho.MQTT.Message(payload); //using String message
  message.destinationName = topic;
  message.qos = qos;
  client.send(message);
};

/*function mqtt_Reconnect() {
  //reconnect not working
  location.reload(false); //false - Default. Reloads the current page from the cache
}*/

function mqtt_Connect() {
  mqtt_PrintAndShowLogMsg('Conectando...');
  client.connect(options);
}

function mqtt_OnConnect() {
  mqtt_PrintAndShowLogMsg('Conectado');
  mqtt_Subscribe();
}

function mqtt_OnFailure(message) {
  mqtt_PrintAndShowLogMsg('Falha conexão: ' + message.errorMessage);
}

function mqtt_Subscribe() {
  mqtt_PrintAndShowLogMsg('Se inscrevendo em: ' + topicDsBrd);
  client.subscribe(topicDsBrd,
    {
      qos: 2,
      onSuccess: callBack_OnSubscribe
    }
  );

  mqtt_PrintAndShowLogMsg('Se inscrevendo em: ' + topicCmd);
  client.subscribe(topicCmd,
    {
      qos: 2,
      onSuccess: callBack_OnSubscribeCmd
    }
  );
}

function callBack_OnSubscribe(invocationContext, grantedQos) {
  mqtt_PrintAndShowLogMsg('Inscrito em: ' + topicDsBrd);
}

function callBack_OnSubscribeCmd(invocationContext, grantedQos) {
  mqtt_PrintAndShowLogMsg('Inscrito em: ' + topicCmd);
}

function mqtt_Publish_Cmd(cmd) {
  publish(cmd,topicCmd,2);
}

function mqtt_Disconnect() {
  try {
    client.disconnect();
  } catch (error) {
    //nothing
  }
}

function mqtt_PrintAndShowLogMsg(msg) {
  console.log(msg);
  $('#divlogs').prepend('<span>' + msg + '</span><br/>');
  $("#edt_server_msg").val(msg);
}
