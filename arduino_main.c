#include <WiFi.h>
#include <HTTPClient.h> // Genialna biblioteka dostępna tylko na ESP32!

// 1. WPISZ DANE SWOJEJ SIECI
const char* ssid     = "WEiTI";
const char* password = "WD@eB7+Fj";

// 2. WPISZ PEŁNY LINK DO TWOJEGO MOCKAPI (ten sam, co w kodzie C)
const char* apiUrl   = "https://6a0c3d595aa893e1015b462b.mockapi.io/live-tracker/status/1";

const int buttonPin = 9; // na razie guzik boot
int lastButtonState = HIGH;

// Ta funkcja robi dokładnie to samo, co Twój gigantyczny CURL w C!
void sendDataToCloud(bool statusValue) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Otwieramy połączenie z naszym API
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");

    // Tworzymy paczkę JSON
    String jsonBody = "{\"id\":\"1\",\"is_green\":" + String(statusValue ? "true" : "false") + "}";

    // Wysyłamy zapytanie PUT (tak jak robiliśmy to wcześniej dla rekordu /1)
    int httpResponseCode = http.PUT(jsonBody);

    if (httpResponseCode > 0) {
      Serial.print("Sukces chmury! Kod HTTP: ");
      Serial.println(httpResponseCode); // Kod 200 oznacza, że MockAPI zapisało dane
    } else {
      Serial.print("Błąd wysyłania: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    http.end(); // Zamykamy bezpiecznie połączenie
  } else {
    Serial.println("Brak połączenia z WiFi. Nie mogę wysłać danych.");
  }
}

void setup() {
  Serial.begin(115200); // Szybki terminal dla ESP32
  
  pinMode(buttonPin, INPUT_PULLUP); // Aktywujemy wewnętrzny opornik

  // Łączenie z siecią WiFi
  Serial.print("Łączenie z WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nPołączono z siecią WiFi!");
}

void loop() {
  int currentButtonState = digitalRead(buttonPin);

  // Wykrycie wciśnięcia (przejście z HIGH do LOW)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.println("Guzik wciśnięty! Zmieniam status na URL...");
    sendDataToCloud(true);
    delay(250); // Anty-drganie styków
  } 
  // Wykrycie puszczenia (przejście z LOW do HIGH)
  else if (lastButtonState == LOW && currentButtonState == HIGH) {
    Serial.println("Guzik puszczony! Zmieniam status na URL...");
    sendDataToCloud(false);
    delay(250); 
  }

  lastButtonState = currentButtonState;
}

