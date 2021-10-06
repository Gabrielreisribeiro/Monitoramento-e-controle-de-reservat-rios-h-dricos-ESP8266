#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <NewPingESP8266.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#else
#error "Board not found"
#endif
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

#include <ArduinoJson.h>
#include <Ticker.h>

#define LED1 13
#define LED2 12

//----------------------SENSOR ULTRASSONICO -----------------------------
#define TRIGGER_PIN  0  
#define ECHO_PIN     4  

long duracao = 0;
float distancia = 0;
float leitura, conversao1, conversao2, conversao3;
int cont = 0;

NewPingESP8266 sonar(TRIGGER_PIN, ECHO_PIN); // NewPing setup of pins and maximum distance.

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
//----------------------------------------------------------------------------------

//----------------------SENSOR DE VAZÃO -----------------------------

 
float vazao; //Variável para armazenar o valor em L/min
float media = 0; //Variável para fazer a média
int contaPulso; //Variável para a quantidade de pulsos
int i = 0; //Variável para segundos
int Min = 00; //Variável para minutos
float Litros = 0; //Variável para Quantidade de agua
float MiliLitros = 0; //Variavel para Conversão
 

void ICACHE_RAM_ATTR incpulso(){
 contaPulso++; //Incrementa a variável de pulsos
}
 
//----------------------------------------------------------------------------------
void send_sensor();
Ticker timer;

char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<script>
var connection = new WebSocket('ws://'+location.hostname+':81/');

var button_1_status = 0;
var button_2_status = 0;
var sens_data = 0;
var vaz_data = 0;
var med_data = 0;
var dis_data = 0;
connection.onmessage = function(event){

  var full_data = event.data;
  console.log(full_data);
  var data = JSON.parse(full_data);
  sens_data = data.sens;
  vaz_data = data.vaz;
  med_data = data.med;
  dis_data = data.dis;
  document.getElementById("sensor_meter").value = sens_data;
  document.getElementById("sensor_value").innerHTML = sens_data;
  document.getElementById("vazao_meter").value = vaz_data;
  document.getElementById("vazao_value").innerHTML = vaz_data;
  document.getElementById("media_meter").value = med_data;
  document.getElementById("media_value").innerHTML = med_data;
  document.getElementById("distancia_meter").value = dis_data;
  document.getElementById("distancia_value").innerHTML = dis_data;
}

function button_1_on()
{
  console.log("LED 1 is ON");
  connection.send("LED 1 is ON");
}

function button_1_off()
{
console.log("LED 1 is OFF");
connection.send("LED 1 is OFF");
}
</script>

<style>
  *{
    padding: 0;
    margin: 0;
    box-sizing: border-box;
    align-items: center;
  }
  html,body{
    width: 100%;
  }
  header{
    padding-bottom: 30px;
    background: #4A6FFC;
    padding-top: 30px;
    text-align: center;
    border-bottom-left-radius: 7px;
    border-bottom-right-radius: 7px;
  }
  h1, article{
    color: antiquewhite;
  }
  .meter-container{
    height: 100%;
    display: flex;
    flex-direction: row;
    padding-top: 4%;
    padding-bottom: 4%;
  }
  .Container-agua{
    width:50%;
    height: 50%;
    display: flex;
    border-left: 2px #74C4F1;
    align-items: center;
    justify-content: center;
    border-right: 1px solid #647DDB;

    
  }
  .Container-vazao{
    width: 50%;
    height: 50%;
    display: flex;
    flex-direction: row;
    align-items: center;
    justify-content: center;
  }
  
  #vazao_meter{
    width: 50%;
    height: 60px;
    border-radius: 50px;
    margin-left: 5%
        
  }
  #sensor_meter{
    width: 40%;
    height: 60px;
    border-radius: 50px;
    margin-left: 5%
    

  
  }
    #media_meter{
    width: 50%;
    height: 60px;
    border-radius: 50px;
    margin-left: 5%
        
  }
  #distancia_meter{
    width: 40%;
    height: 60px;
    border-radius: 50px;
    margin-left: 5%
    

  
  }
  h3{
    color: #00044F;
    margin-left: 2%;
  }
  
  .control{
    width: 100%;
    background-color: rgba(251,251,251,0.27);;
    height: 70px;
    display: flex;
    flex-direction: row;
    align-items: center;
    justify-content:flex-end;
      box-shadow: 0.5px 0.5px 5px #000;
  }
  
  button{
    width: 16%;
    height: 70%;
    border-radius: 10px;
    border-style: none;
    margin: 10px;
    background: rgba(189,189,189,0.93);
    animation-delay: 2s;
    position: relative;
    transition: all 0.5s ease-in;
    cursor: pointer;
  }
    button:hover{
background-color:rgba(54,54,54,0.73);
    color: aliceblue;
    animation-delay: 1.5s;
  }
  .control h3{
  margin-right: 55%;
    font-family:Cambria, "Hoefler Text", "Liberation Serif", Times, "Times New Roman", "serif";
  }
  </style>
  <header>
    <h1>DASHBOARD</h1>
    <article>Controle de Sisterna</article>
  </header>
<body>
<div class="control">
  <h3> Acionar Bomba </h3>
<button onclick= "button_1_on()" >On</button><button onclick="button_1_off()" >Off</button>
  </div>
<div class="meter-container">
  <div class = "Container-agua">
    <h3>Volume da Agua</h3><meter value="0" min="0" max="4" id="sensor_meter"> </meter><h3 id="sensor_value"> 2 Litros </h3>
  </div>
  
  <div class = "Container-vazao" >
    <h3>Vazao da Agua</h3><meter value="0" min="0" max="100" id="vazao_meter"> </meter><h3 id="vazao_value"> 2 L/</h3>
  </div>
  
</div>
  <div class="meter-container">
  <div class = "Container-agua">
    <h3>Distancia</h3><meter value="0" min="0" max="20" id="distancia_meter"> </meter><h3 id="distancia_value"> 2 cm</h3>
  </div>
  
  <div class = "Container-vazao" >
    <h3>Media Vazao</h3><meter value="0" min="0" max="10" id="media_meter"> </meter><h3 id="media_value"> 2 </h3>
  </div>
  
</div>


</body>
</html>
)=====";

// ipaddress/led1/on
//ipaddress/led1/off

// ipaddress/led2/on
//ipaddress/led2/off


AsyncWebServer server(80); // server port 80
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not found");
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      String message = String((char*)( payload));
      Serial.println(message);

      if(message == "LED 1 is OFF"){
        digitalWrite(2,LOW);
      }

      if(message == "LED 1 is ON"){
        digitalWrite(2,HIGH);
      }

  }
}

void setup(void)
{
  
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  WiFi.softAP("techiesms", "");
  Serial.println("softap");
  Serial.println("");
  Serial.println(WiFi.softAPIP());


  if (MDNS.begin("ESP")) { //esp.local/
    Serial.println("MDNS responder started");
  }



  server.on("/", [](AsyncWebServerRequest * request)
  { 
   
  request->send_P(200, "text/html", webpage);
  });

   server.on("/led1/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    digitalWrite(2,HIGH); 
  request->send_P(200, "text/html", webpage);
  });

  server.onNotFound(notFound);

  server.begin();  // it will start webserver
  websockets.begin();
  websockets.onEvent(webSocketEvent);
  timer.attach(0.5,send_sensor);
  pinMode(D5,INPUT);

//-----------------------SENSOR DE VAZAO-------------------------------------------
   pinMode(14, INPUT);
// attachInterrupt(0, incpulso, RISING); //Configura o pino 2(Interrupção 0) interrupção
attachInterrupt(digitalPinToInterrupt(5), incpulso, RISING);
   pinMode(2, OUTPUT); // RELÈ
}
//--------------------------------------------------------------------------------

void loop()
{
 websockets.loop();
//----------------------SENSOR ULTRASSONICO -----------------------------
  delay(1000);
  digitalWrite(TRIGGER_PIN, LOW);    // Limpa TRIGGER_PIN
  delayMicroseconds(2);          // aguarda 2 microsegundos
 
  digitalWrite(TRIGGER_PIN, HIGH);   // Seta TRIGGER_PIN HIGH 
  delayMicroseconds(10);         // aguada 10 microsegundos
  digitalWrite(TRIGGER_PIN, LOW);    // Seta TRIGGER_PIN LOW 
   
  // Leitura do echoPin, retorna a onda de som em microsegundos
  duracao = pulseIn(ECHO_PIN, HIGH);
  distancia= duracao*0.034/2;
  //distancia = distancia*10; // converter cm para mm
  
  //  leitura = sonar.ping_cm();
  conversao1 = distancia*227,17; // volume(de um cone cortado) é dado em cm cúbico
  conversao2 = (conversao1/1000);// converte de cm cubicos para Litro
  conversao3 = (mapFloat(conversao2, 0, 3.6, 3.6, 0));// converte 0 ate 3.6L

  Serial.print("Litros: ");
  Serial.println(conversao3); // imprime os litros
  Serial.print("leitura: ");
  Serial.println(distancia); // imprime a distancia medida pelo sensor ultrassonico


//----------------------SENSOR DE VAZAO-------------------------- -----------------------------
 contaPulso = 0;//Zera a variável
 sei(); //Habilita interrupção
 delay (1000); //Aguarda 1 segundo
 cli(); //Desabilita interrupção
 
 vazao = contaPulso / 7.5; //Converte para L/min
 media = media + vazao; //Soma a vazão para o calculo da media
 i++;
 
 MiliLitros = vazao / 60;
 Litros = Litros + MiliLitros;

 media = media / 60; //faz a média
 Serial.print("nMedia por minuto = "); //Imprime a frase Media por minuto =
 Serial.print(media); //Imprime o valor da media
 Serial.println(" L/min - "); //Imprime L/min
 i = 0; //Zera a variável i para uma nova contagem

//------------------------------------------------------------ -----------------------------

}
void send_sensor()
{
   float vaz = vazao;
   float sens = conversao3;
   float dis = distancia;
   float med = media;
  String JSON_Data = "{\"sens\":";
         JSON_Data += sens;
         JSON_Data += ",\"vaz\":";
         JSON_Data += vaz;
         JSON_Data += ",\"dis\":";
         JSON_Data += dis;
         JSON_Data += ",\"med\":";
         JSON_Data += med;
         JSON_Data += "}";
   Serial.println(JSON_Data);     
  websockets.broadcastTXT(JSON_Data);
}
