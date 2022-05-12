#include <WiFi.h>
#include <SocketIoClient.h>
#include <DHT.h>

#define ONBOARD_LED 2
#define DHTPIN 23
#define DHTTYPE DHT11 //DHT21 //DHT22

const char*       ssid     = "nombre_de_la_red"; 
const char*       password = "contrasena";
const char*       server   = "ip.de.ser.ver";
const uint16_t    port     = 5001; //Puerto del servidor

String    mensaje;
uint64_t  now       = 0;
uint64_t  timestamp = 0;
float     h         = 0;
float     t_c       = 0;  
float     t_f       = 0;  

SocketIoClient socketIO;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(ONBOARD_LED,OUTPUT);
  Serial.begin(115200);
  conectar_WiFiSTA();
  socketIO.begin(server, port);
  socketIO.on("desde_servidor_comando",procesar_mensaje_recibido);
}

void loop() {
  now = millis();
  if(now - timestamp > 1000)
  {
    timestamp = now;

    h = dht.readHumidity();
    t_c = dht.readTemperature();
    t_f = dht.readTemperature(true);

    if (isnan(h) || isnan(t_c) || isnan(t_f)) {
      return;
    }
    
    mensaje = "\""+String(t_c)+"\"";
    socketIO.emit("desde_esp32_temp_c",mensaje.c_str());
    mensaje = "\""+String(t_f)+"\"";
    socketIO.emit("desde_esp32_temp_f",mensaje.c_str());
    mensaje = "\""+String(h)+"\"";
    socketIO.emit("desde_esp32_hum",mensaje.c_str());
  }
  socketIO.loop();
}

void conectar_WiFiSTA()
{
   delay(10);
   Serial.println("");
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) 
   { 
     delay(100);  
     Serial.print('.'); 
   }
   Serial.println("");
   Serial.print("Conectado a STA:\t");
   Serial.println(ssid);
   Serial.print("My IP Address:\t");
   Serial.println(WiFi.localIP());
}

void procesar_mensaje_recibido(const char * payload, size_t length) {
 Serial.printf("Mensaje recibido: %s\n", payload);
 String paystring = String(payload);
 if(paystring == "ON")
 {
  digitalWrite(ONBOARD_LED,HIGH);
 }
 else if(paystring == "OFF")
 {
  digitalWrite(ONBOARD_LED,LOW);
 }
}
