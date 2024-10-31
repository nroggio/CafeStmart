
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>


// Variables globales
int seconds = 0;

// Modo
int modo = 0;
// Pines botones
const int okButtonPin = 10;
const int upButtonPin = 9;
const int downButtonPin = 8;
//Variables para boton
int lastOkButtonState = HIGH;
int lastUpButtonState = HIGH;
int lastDownButtonState = HIGH;

// Pin para el relé
const int relayPin = 13; // Relé conectado al pin 13

// ON / OFF Cafetera
bool coffeeMakerOn = false;

// Menu
const String menuItems[3] = {"Empezar", "Programar", "Configurar"};
int currentMenuIndex = 0; // Índice del menú actual
const int menuSize = 3;

// Variables para la hora
int horasDecenas = 0;
int horasUnidades = 0;
int minutosDecenas = 0;
int minutosUnidades = 0;

// Declaracion de LCD I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Temporizador
unsigned long lastInteractionTime = 0;
const unsigned long timeoutDuration = 5000; // 5 segundos
bool isMenuActive = false;


RTC_DS3231 rtc;
// Funciones
void ProgramarHora();
void seleccionarOpcion();

void setup() {
  lcd.init();
  rtc.begin();
  lcd.backlight();
  lcd.setCursor (0,5);
  lcd.print("Bienvenido");
  lcd.setCursor (2,4);
  lcd.print("*Cafe Smart*");
  delay(2000);

  rtc.adjust(DateTime(2024, 10, 30, 17, 00, 00));

    // Pines de botones como entradas
  pinMode(okButtonPin, INPUT_PULLUP);
  pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);

  // Configuracion pin de rele
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); 

  //Leer hora de RTC
  DateTime now = rtc.now();
  horasDecenas = now.hour() / 10;
  horasUnidades = now.hour() % 10;
  minutosDecenas = now.minute() / 10;
  minutosUnidades = now.minute() % 10;
    
// Iniciar el temporizador
  lastInteractionTime = millis();
}

void loop() {
  if (!isMenuActive) {
    menuPrincipal(); 
    } else {
      mostrarMenu();
    }
    
  manejarBotones();

    // Verificar si ha pasado el tiempo de espera
  if (millis() - lastInteractionTime > timeoutDuration) {
    isMenuActive = false;
  }

  delay(100);
}

void menuPrincipal() {
  
 DateTime now = rtc.now();
lcd.clear();

lcd.setCursor(0, 0);
lcd.print("Cafetera: ");
lcd.print(coffeeMakerOn ? "ON " : "OFF");
  
lcd.setCursor (0,1);
lcd.print("Cantidad agua");
  
lcd.setCursor(0, 2);
lcd.print("Hora: ");
lcd.print(now.hour());
lcd.print(":");
lcd.print(now.minute());
lcd.print(":");
lcd.println(now.second());
    
lcd.setCursor(0, 3);
lcd.print("Tiene agua?");
}

void mostrarMenu() {
  lcd.clear();
  for (int i = 0; i < menuSize; i++) {
    lcd.setCursor(0, i);
    lcd.print((i == currentMenuIndex) ? ">" : " ");
    lcd.print(menuItems[i]);
    }
}

void manejarBotones() {
  // Leer el estado de los botones
  int okButtonState = digitalRead(okButtonPin);
  int upButtonState = digitalRead(upButtonPin);
  int downButtonState = digitalRead(downButtonPin);

    // Actualizar el tiempo de interacción
  if (okButtonState == LOW || upButtonState == LOW || downButtonState == LOW) {
    lastInteractionTime = millis(); // Reiniciar temporizador
    }

    // Manejar el boton OK
  if (lastOkButtonState == HIGH && okButtonState == LOW) {
    if (!isMenuActive) {
      isMenuActive = true; // Activar el menu secundario
      } else {
      seleccionarOpcion(); // Seleccionar opción del menu del menu secundario
      }
    }

    // Manejar el botón UP
  if (lastUpButtonState == HIGH && upButtonState == LOW) {
    currentMenuIndex = (currentMenuIndex - 1 + menuSize) % menuSize; // Mover hacia arriba
    }

    // Manejar el botón DOWN
    if (lastDownButtonState == HIGH && downButtonState == LOW) {
        currentMenuIndex = (currentMenuIndex + 1) % menuSize; // Mover hacia abajo
    }

    // Actualizar el estado anterior de los botones
    lastOkButtonState = okButtonState;
    lastUpButtonState = upButtonState;
    lastDownButtonState = downButtonState;
}

void ProgramarHora(){
      // Leer el estado de los botones
  int okButtonState = digitalRead(okButtonPin);
  int upButtonState = digitalRead(upButtonPin);
  int downButtonState = digitalRead(downButtonPin);

  // Variable para el parpadeo
  boolean parpadear = false;
  unsigned long tiempoParpadeo = 0;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Programar hora: ");

  // Ajusta la hora el modo y los botones presionados
  if (lastOkButtonState == HIGH && okButtonState == LOW) {
  modo = (modo + 1) % 4;
  } else if (lastUpButtonState == HIGH && upButtonState == LOW) {
    switch (modo) {
      case 0: horasDecenas = (horasDecenas + 1) % 3; break;
      case 1: horasUnidades = (horasUnidades + 1) % 10; break;
      case 2: minutosDecenas = (minutosDecenas + 1) % 6; break;
      case 3: minutosUnidades = (minutosUnidades + 1) % 10; break;
    }
  } else if (lastDownButtonState == HIGH && downButtonState == LOW) {
    switch (modo) {
      case 0: horasDecenas = (horasDecenas + 2) % 3; break; // Resta 1
      case 1: horasUnidades = (horasUnidades + 9) % 10; break;
      case 2: minutosDecenas = (minutosDecenas + 5) % 6; break;
      case 3: minutosUnidades = (minutosUnidades + 9) % 10; break;
    }
  }

    // Parpadea el dígito que se está editando
  if (millis() - tiempoParpadeo >= 500) { // Cada 500ms cambiamos el estado del parpadeo
    tiempoParpadeo = millis();
    parpadear = !parpadear;
  }

  lcd.setCursor(1,0);
  lcd.print(horasDecenas);
  if (modo == 0 && parpadear){ 
    lcd.print('_');} 
    else {
      lcd.setCursor(1,1);
      lcd.print(horasUnidades);
    }  
  lcd.setCursor(1,2);
  lcd.print(":");
  
  lcd.setCursor(1,3);
  lcd.print(minutosDecenas);
  if (modo == 2 && parpadear) {
    lcd.print('_');}
    else{
      lcd.setCursor(1,4);
      lcd.print(minutosUnidades);
    }

  lcd.println();

    // Actualizar el estado anterior de los botones
  lastOkButtonState = okButtonState;
  lastUpButtonState = upButtonState;
  lastDownButtonState = downButtonState;
};

  
void seleccionarOpcion() {
    lcd.clear();

    if (currentMenuIndex == 0) {
        lcd.print("Iniciando...");
        digitalWrite(relayPin, HIGH);  // Activar el rele
        coffeeMakerOn = true;  // Indicar que la cafetera está encendida

    } else if (currentMenuIndex == 1) {
        ProgramarHora();
    
    } else if (currentMenuIndex == 2) {
        lcd.print("Configurando...");
    }
    delay(1000); 

}
