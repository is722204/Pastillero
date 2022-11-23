#include <Adafruit_LiquidCrystal.h>

//=======================================
// PASTILLERO ELECTRÓNICO
// Asignatura: IoT
// Titulo: Funcionamiento del reloj
//=====================================

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);             // LCD a usar
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
 #include <Adafruit_GFX.h> // Inclusión de Libreria Grafica
 #include <Adafruit_SSD1306.h> // Inclusión de Libreria de la pantalla

#include "Ubidots.h"
#include <WiFi.h> 
#include <WiFiClientSecure.h>
#include <PubSubClient.h> 
#include <HTTPClient.h>
#include <DHT.h>
#include <DHT_U.h>
//#include"logo.h"
//SDA 21
//SCL 22
//VCC 3.3

// *************************
// Definiciones para la Pantalla
 #define ANCHOPANTALLA 128 //  Ancho de la Pantalla en Pixeles
 #define ALTOPANTALLA 64 // Alto de la Pantalla en Pixeles
 #define OLED_RESET     4 // ASignación del Pin de Reset para la pantalla
 #define SCREEN_ADDRESS 0x3C //Segun el datasheet de la pantalla la direccion es
                            //0x3D para 128x64, 0x3C para 128x32
const char* UBIDOTS_TOKEN = "BBFF-Q6ZM02UgdW2ia2Wn8LUUBDXAorwd1e";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = "Honor50"; // Put here your Wi-Fi SSID
const char* WIFI_PASS = "1235678"; // Put here your Wi-Fi password
#define PIN_LED 33
#define PIN_TTP223B 4		// SIG de sensor en pin digital 4
#define PIN_BUZZER 32
Ubidots ubidots(UBIDOTS_TOKEN, UBI_TCP);

Adafruit_SSD1306 display(ANCHOPANTALLA, ALTOPANTALLA, &Wire, OLED_RESET);
//*************************

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};

int ahorahora;           // Guarda la hora del reloj RTC
int ahoraminuto;         // Guarda los minutos del reloj RTC
int ahorasegundo;
int ahorames;
int ahorayear;
char dia_semana;
int dia;                 // Guarda el dia del reloj RTC
int setfecha = 2; // para cargar la hora en el reloj
int bandera = 0;

//Horarios 
 int h1hora = 21;
 int h1minuto = 58;
 int h2hora = 21;
 int h2minuto = 48;
 int h3hora = 21;
 int h3minuto = 48; 

int LED1 = 25;
int LED2 = 26;
int LED3= 27;
//int LED4 = 25;
//int LED5 = 26;
//int LED6= 27;

int EMUX1 = 5;
int EMUX2 = 19;
int EMUX3= 23;

void setup() {
  
  lcd.init();            // Inicializa lcd
  lcd.backlight();  
  pinMode(setfecha, INPUT);                  // Declaracion de pin de salida

	Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);		// pin 3 como salida
  pinMode(PIN_TTP223B, INPUT);	// pin 4 como entrada
  pinMode(PIN_BUZZER, OUTPUT);
 
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
	Serial.println("Serial OK");

	#ifdef ARDUINO_ARCH_ESP8266
		Wire.begin(0, 2); // D3 and D4 on ESP8266
	#else
		Wire.begin();
	#endif

if (! rtc.begin()) {   //En caso de que no se encuentre el RTC
Serial.println("Couldn't find RTC");
while (1);
}

 //Inicio de Pantalla
   Wire.begin();               // Inicialización del Protocolo I2C para el manejo de pantalla
   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
     Serial.println(F("SSD1306 Fallo la Localización de Pantalla"));
     for(;;);                  // Si no la encuentra se bloquea en un ciclo infinito 
   }
   delay(500);
   display.clearDisplay();     // Función de Borrado de Pantalla
   display.display();

// Inicio del reloj
// valores a introducir para configurar el RTC
// ===========================
int ano = 2022; // 0-2099
int mes = 11;   // 0-12
int dia = 16;   // 0-31
int hora= 20;   // 0-23
int minu = 20;   // 0-59
int seg = 00;   // 0-59
// ===========================

  display.display();
  delay(2);
  display.clearDisplay();
 
 display.clearDisplay();
 display.setTextColor(WHITE);
 display.startscrollright(0x00, 0x0F);
 display.setTextSize(2);
 display.setCursor(0,5);
 display.print("  Reloj");
 display.display();
 delay(3000);


}
char* getDay1(int dia){
  switch(dia){
    case 0:
    return "domingo1";
    
    case 1:
    return "lunes1";

    case 2:
    return "martes1";

    case 3:
    return "miercoles1";

    case 4:
    return "jueves1";

    case 5:
    return "viernes1";

    case 6:
    return "sabado1";
  }
 
}
char* getDay2(int dia){
    switch(dia){
    case 0:
    return "domingo2";
    
    case 1:
    return "lunes2";

    case 2:
    return "martes2";

    case 3:
    return "miercoles2";

    case 4:
    return "jueves2";

    case 5:
    return "viernes2";

    case 6:
    return "sabado2";
  }
}
//FUNCIONES DE LA ALARMA
void sonarAlarma(){
    //digitalWrite(PIN_TTP223B, HIGH);	// si se presiono el sensor
    digitalWrite(PIN_LED, HIGH);		// enciende LED
    digitalWrite(PIN_BUZZER, HIGH);
}

void apagarAlarma(){
    
    digitalWrite(PIN_TTP223B,LOW);
    digitalWrite(PIN_LED, LOW);		// sino apaga LED
    digitalWrite(PIN_BUZZER, LOW); // sino apaga LED
    delay(60000);
}

void loop() {
//  display.drawBitmap(0, 0,  epd_bitmap_relojLogo, 128, 64, WHITE);
//  display.display();
//  delay(3000);
while(1)
{
while (bandera == 0){
  if (digitalRead(setfecha)==HIGH){               // Si presiona el pulsador se establece la fecha que se introduzca por variables
     rtc.adjust(DateTime(2022,11,16,19,00,00)); //Año,mes,dia,hora,minutos,segundos
     rtc.adjust(DateTime(_DATE, __TIME_));
     Serial.println("Entra al setfecha");
    }
    
    DateTime now = rtc.now(); // Lee el valor actual de la hora y fecha
                              // lo coloca en una estructura now que es del tipo rtc.
     display.clearDisplay();
     display.setTextSize(2);
     display.setCursor(75,0);
     display.println(now.second(), DEC); // Lee el tiempo en segundos en formato decimal
     
     display.setTextSize(2);
     display.setCursor(25,0);
     display.println(":");
     
     display.setTextSize(2);
     display.setCursor(65,0);
     display.println(":");
     
     display.setTextSize(2);
     display.setCursor(40,0);
     display.println(now.minute(), DEC);// Lee el tiempo en Minutos en formato decimal
     
     display.setTextSize(2);
     display.setCursor(0,0);
     display.println(now.hour(), DEC);// Lee el tiempo en Horas en formato decimal
     
     display.setTextSize(2);
     display.setCursor(0,20);
     display.println(now.day(), DEC); // Lee el dia de la fecha
     
     display.setTextSize(2);
     display.setCursor(25,20);
     display.println("-");
     
     display.setTextSize(2);
     display.setCursor(40,20);
     display.println(now.month(), DEC);// Lee el mes de la fecha
     
     display.setTextSize(2);
     display.setCursor(65,20);
     display.println("-");
    
     display.setTextSize(2);
     display.setCursor(78,20);
     display.println(now.year(), DEC);// Lee el Año de la fecha
     
     display.setTextSize(2);
     display.setCursor(0,40);
     display.print(daysOfTheWeek[now.dayOfTheWeek()]); //Busca en la matriz creada
                                                       // donde está la cadena que representa el dia de la semana
                                                       // de lunes a domingo
     display.display(); 

      ahorahora = now.hour();
      ahoraminuto = now.minute();
      ahorasegundo = now.second();
      dia = now.dayOfTheWeek();
          
      Serial.println("Fecha RTC: ");
      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.println(now.day(), DEC);
    
      Serial.print("Hora: ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.println(now.second(), DEC);
    
      Serial.print("Dia: ");
      Serial.println(daysOfTheWeek[now.dayOfTheWeek()]);
      delay(1000);
  int valorManana = ubidots.get("pastillero1", getDay1(dia));
  int valorTarde = ubidots.get("pastillero1", getDay2(dia));

      if ( (int(valorManana/100)  == ahorahora) and ((valorManana%100) == ahoraminuto )){    // PRIMER HORARIO
      // enviar mensaje de que debe tomar una pastilla
      Serial.println("Es hora de tomar el medicamento 1");
      sonarAlarma();
      
       switch (dia){
          case 0:{ // domingo, LED 19
          // TERCER MUX
           digitalWrite(EMUX1, HIGH);     
           digitalWrite(EMUX2, HIGH);    
           digitalWrite(EMUX3, LOW);    
           // Enciende led 19
          digitalWrite(LED1, LOW);
          digitalWrite(LED2, LOW); 
          digitalWrite(LED3, HIGH);     
          delay(1000);
          bandera = 1;
          Serial.println("Abra el compartimiento"); 
          delay(1000);            
          }
          case 1:{ // lunes, LED 1
          // PRIMER MUX
          digitalWrite(EMUX1, LOW);
          digitalWrite(EMUX2, HIGH); 
          digitalWrite(EMUX3, HIGH); 
          // Enciende led 1
          digitalWrite(LED1, LOW);
          digitalWrite(LED2, LOW); 
          digitalWrite(LED3, LOW); 
          delay(1000);
          bandera = 1;
          Serial.println("Abra el compartimiento"); 
          delay(1000);    
          }
          case 2: {// martes, LED 4
          // PRIMER MUX
          digitalWrite(EMUX1, LOW);
          digitalWrite(EMUX2, HIGH); 
          digitalWrite(EMUX3, HIGH); 
          // Enciende led 4
          digitalWrite(LED1, HIGH);
          digitalWrite(LED2, HIGH); 
          digitalWrite(LED3, LOW);
          delay(1000);
          bandera = 1;
          Serial.println("Abra el compartimiento"); 
          delay(1000);  
          }
          case 3: {// miercoles, LED 7
          // PRIMER MUX
          digitalWrite(EMUX1, LOW);
          digitalWrite(EMUX2, HIGH); 
          digitalWrite(EMUX3, HIGH); 
          // Enciende led 7
          digitalWrite(LED1, LOW);
          digitalWrite(LED2, HIGH); 
          digitalWrite(LED3, HIGH);
          delay(1000);
          bandera = 1;
          Serial.println("Abra el compartimiento"); 
          delay(1000); 
          }
          case 4:{// jueves, LED 10
          // SEGUNDO MUX
          digitalWrite(EMUX1, HIGH);    
          digitalWrite(EMUX2, LOW);    
          digitalWrite(EMUX3, HIGH);
          // Enciende led 10
          digitalWrite(LED1, LOW);
          digitalWrite(LED2, HIGH); 
          digitalWrite(LED3, LOW);     
          delay(1000);
//          bandera = 1;
          Serial.println("Abra el compartimiento"); 
          delay(100);     
          }
          case 5:{ // viernes, LED 13
          // SEGUNDO MUX
          digitalWrite(EMUX1, HIGH);    
          digitalWrite(EMUX2, LOW);    
          digitalWrite(EMUX3, HIGH);
          // Enciende led 13
          digitalWrite(LED1, HIGH);
          digitalWrite(LED2, LOW); 
          digitalWrite(LED3, HIGH);     
          delay(1000);
          bandera = 1;
          Serial.println("Abra el compartimiento"); 
          delay(1000);
          }    
          case 6:{// sabado. LED 16
          // TERCER MUX
          digitalWrite(EMUX1, HIGH);     
          digitalWrite(EMUX2, HIGH);    
          digitalWrite(EMUX3, LOW);  
          // Enciende led 16
          digitalWrite(LED1, HIGH);
          digitalWrite(LED2, LOW); 
          digitalWrite(LED3, LOW);     
          delay(1000);
          bandera = 1;
          Serial.println("Abra el compartimiento"); 
          delay(1000);
          }
         }// cierra switch dia
      }// cierra primer horario1
      
      else if ((int(valorTarde/100) == ahorahora) and ((valorTarde%100)== ahoraminuto)){
        // SEGUNDO HORARIO
        Serial.println("Es hora de tomar el medicamento 2");
        switch (dia){
          case 0:{ // domingo, LED 20
          // TERCER MUX
            digitalWrite(EMUX1, HIGH);     
            digitalWrite(EMUX2, HIGH);    
            digitalWrite(EMUX3, LOW);    
            // Enciende led 20
            digitalWrite(LED1, HIGH);
            digitalWrite(LED2, LOW); 
            digitalWrite(LED3, HIGH);     
            delay(1000);
            bandera = 1;
            Serial.println("Abra el compartimiento"); 
            delay(1000);
          }    
          case 1:{ // lunes, LED 2
            // PRIMER MUX
            digitalWrite(EMUX1, LOW);
            digitalWrite(EMUX2, HIGH); 
            digitalWrite(EMUX3, HIGH);
            // Enciende led 2
            digitalWrite(LED1, HIGH);
            digitalWrite(LED2, LOW); 
            digitalWrite(LED3, LOW);     
            delay(1000);
            bandera = 1;
            Serial.println("Abra el compartimiento"); 
            delay(1000);
          }   
          case 2:{ // martes, LED 5
            // PRIMER MUX
            digitalWrite(EMUX1, LOW);
            digitalWrite(EMUX2, HIGH); 
            digitalWrite(EMUX3, HIGH); 
            // Enciende led 5
            digitalWrite(LED1, LOW);
            digitalWrite(LED2, LOW); 
            digitalWrite(LED3, HIGH);
            delay(1000);
            bandera = 1;
            Serial.println("Abra el compartimiento"); 
            delay(1000);
          }  
          case 3:{ // miercoles, LED 8
            // SEGUNDO MUX
            digitalWrite(EMUX1, HIGH);    
            digitalWrite(EMUX2, LOW);    
            digitalWrite(EMUX3, HIGH);    
            // Enciende led 8
            digitalWrite(LED1, LOW);
            digitalWrite(LED2, LOW); 
            digitalWrite(LED3, LOW);      
            delay(1000);
            bandera = 1;
            Serial.println("Abra el compartimiento"); 
            delay(1000);
          }  
          case 4:{ // jueves, LED 11
            // SEGUNDO MUX
            digitalWrite(EMUX1, HIGH);    
            digitalWrite(EMUX2, LOW);    
            digitalWrite(EMUX3, HIGH);   
             // Enciende led 11
            digitalWrite(LED1, HIGH);
            digitalWrite(LED2, HIGH); 
            digitalWrite(LED3, LOW);     
            delay(1000);
            bandera = 1;
            Serial.println("Abra el compartimiento"); 
            delay(1000);
          }          
          case 5:{ // viernes, LED 14
            // SEGUNDO MUX
           digitalWrite(EMUX1, HIGH);    
           digitalWrite(EMUX2, LOW);    
           digitalWrite(EMUX3, HIGH);
           // Enciende led 14
           digitalWrite(LED1, LOW);
           digitalWrite(LED2, HIGH); 
           digitalWrite(LED3, HIGH);     
           delay(1000);
            bandera = 1;
            Serial.println("Abra el compartimiento"); 
            delay(1000);     
          }         
          case 6: {// sabado. LED 17
            // TERCER MUX
            digitalWrite(EMUX1, HIGH);     
            digitalWrite(EMUX2, HIGH);    
            digitalWrite(EMUX3, LOW);  
            // Enciende led 17
           digitalWrite(LED1, LOW);
           digitalWrite(LED2, HIGH); 
           digitalWrite(LED3, LOW);    
           delay(1000);
            bandera = 1;
            Serial.println("Abra el compartimiento"); 
            delay(1000);
          }
        }// cierra switch
      }// cierra hora 2
      
      //  else if ((h3hora == ahorahora) and (h3minuto == ahoraminuto)){                   // TERCER HORARIO
      //    Serial.println("Es hora de tomar el medicamento 3");
      //    switch (dia){
      //      case 0:{ // domingo, LED 21
      //        // TERCER MUX
      //        digitalWrite(EMUX1, HIGH);     
      //        digitalWrite(EMUX2, HIGH);    
      //        digitalWrite(EMUX3, LOW);    
      // //       // Enciende led 21
      //       digitalWrite(LED1, LOW);
      //       digitalWrite(LED2, HIGH); 
      //       digitalWrite(LED3, HIGH);     
      // //      delay(1000);
      //        bandera = 1;
      //        Serial.println("Abra el compartimiento"); 
      //        delay(1000);
      // //     }       
      //      case 1:{ // lunes, LED 3
      //        // PRIMER MUX
      //        digitalWrite(EMUX1, LOW);
      //        digitalWrite(EMUX2, HIGH); 
      //        digitalWrite(EMUX3, HIGH);
      //        // Enciende led 3
      //       digitalWrite(LED1, LOW);
      //       digitalWrite(LED2, HIGH); 
      //       digitalWrite(LED3, LOW);      
      // //      delay(1000);
      //        bandera = 1;
      //        Serial.println("Abra el compartimiento"); 
      //        delay(1000);
      // //     }
      //      case 2:{ // martes, LED 6
      // //       // PRIMER MUX
      //       digitalWrite(EMUX1, LOW);
      //        digitalWrite(EMUX2, HIGH); 
      //        digitalWrite(EMUX3, HIGH); 
      //        // Enciende led 6
      //       digitalWrite(LED1, HIGH);
      //       digitalWrite(LED2, LOW); 
      //     digitalWrite(LED3, HIGH); 
      //      delay(1000);
      //       bandera = 1;
      //        Serial.println("Abra el compartimiento"); 
      //        delay(1000);
      // //     }     
      //      case 3:{ // miercoles, LED 9
      //        // SEGUNDO MUX
      //      digitalWrite(EMUX1, HIGH);    
      //      digitalWrite(EMUX2, LOW);    
      //      digitalWrite(EMUX3, HIGH);    
      //      // Enciende led 9
      //      digitalWrite(LED1, HIGH);
      //      digitalWrite(LED2, LOW); 
      //      digitalWrite(LED3, LOW);    
      //      delay(1000);
      //        bandera = 1;
      //        Serial.println("Abra el compartimiento"); 
      //        delay(1000);
      // //     }       
      //      case 4:{ // jueves, LED 12
      //        // SEGUNDO MUX
      //     digitalWrite(EMUX1, HIGH);    
      //      digitalWrite(EMUX2, LOW);    
      //      digitalWrite(EMUX3, HIGH);   
      //      // Enciende led 12
      //      digitalWrite(LED1, LOW);
      //      digitalWrite(LED2, LOW); 
      //      digitalWrite(LED3, HIGH);     
      //      delay(1000);
      //        bandera = 1;
      //        Serial.println("Abra el compartimiento"); 
      //        delay(1000);
      //      }         
      //      case 5:{ // viernes, LED 15
      //        // TERCER MUX
      //      digitalWrite(EMUX1, HIGH);     
      //      digitalWrite(EMUX2, HIGH);    
      //      digitalWrite(EMUX3, LOW);  
      //      // Enciende led 15
      //      digitalWrite(LED1, LOW);
      //     digitalWrite(LED2, LOW); 
      //      digitalWrite(LED3, LOW);      
      //      delay(1000);
      //        bandera = 1;
      //        Serial.println("Abra el compartimiento"); 
      //        delay(1000);   
      //      }   
      //      case 6: {// sabado. LED 18
      //        // TERCER MUX
      //      digitalWrite(EMUX1, HIGH);     
      //      digitalWrite(EMUX2, HIGH);    
      //      digitalWrite(EMUX3, LOW);  
      //      // Enciende led 18
      //      digitalWrite(LED1, HIGH);
      //      digitalWrite(LED2, HIGH); 
      //      digitalWrite(LED3, LOW);     
      //      delay(1000);
      //        bandera = 1;
      //        Serial.println("Abra el compartimiento"); 
      //        delay(1000);
      //      }      
      //    }// cierra switch
      //  }// cierra hora3
      bandera = 0;
  
  //CREAR CÓDIGO
  if(digitalRead(PIN_TTP223B)==HIGH){
    ubidots.add("alarma",1);
    ubidots.send();    
     apagarAlarma();
     //apagarTodosLosLeds;
   }
}// cierra bandera == 0    
}// cierra while 1
}