#include "BluetoothSerial.h"
#include <WiFi.h>

// Pin tanımlamaları
#define led_1 19
#define led_2 18
#define led_3 5
#define led_4 17
#define led_5 16
#define led_6 4
#define led_7 0
#define led_8 2
#define led_9 15
#define led_10 32
#define led_11 33
#define led_12 25
#define led_13 26
#define led_14 27
#define led_15 14
#define led_16 13


// Wi-Fi ayarları
String ssid = "cokgizlinet";      // Bağlanılacak Wi-Fi ağının adı
String password = "gizlisifrebu"; // Wi-Fi ağının parolası
String prev_ssid = "cokgizlinet";         // Son başarılı SSID'yi kaydet
String prev_password = "gizlisifrebu"; // Son başarılı parolayı kaydet
WiFiServer server(80); // HTTP sunucusunu port 80'de başlat

BluetoothSerial SerialBT; // Bluetooth nesnesi

void setup() {
  Serial.begin(115200);

  // Wi-Fi bağlantısını başlat
  connectWiFi(); // connectWiFi fonksiyonu çağrılıyor

  server.begin(); // HTTP sunucusunu başlat

  // Bluetooth bağlantısını başlat
  SerialBT.begin("ESP32_BT"); // Bluetooth cihaz adı
  Serial.println("Bluetooth başlatıldı. Eşleştirme bekleniyor...");
  // Bağlantı durumu kontrolü
if (SerialBT.hasClient()) {
  Serial.println("Bir cihaz bağlandı.");
} else {
  Serial.println("Bluetooth bağlantısı bekleniyor...");
}

  // LED pinlerini çıkış olarak ayarla
pinMode(led_1, OUTPUT);
pinMode(led_2, OUTPUT);
pinMode(led_3, OUTPUT);
pinMode(led_4, OUTPUT);
pinMode(led_5, OUTPUT);
pinMode(led_6, OUTPUT);
pinMode(led_7, OUTPUT);
pinMode(led_8, OUTPUT);
pinMode(led_9, OUTPUT);
pinMode(led_10, OUTPUT);
pinMode(led_11, OUTPUT);
pinMode(led_12, OUTPUT);
pinMode(led_13, OUTPUT);
pinMode(led_14, OUTPUT);
pinMode(led_15, OUTPUT);
pinMode(led_16, OUTPUT);

// Tüm LED'leri kapalı duruma getir
digitalWrite(led_1, HIGH);
digitalWrite(led_2, HIGH);
digitalWrite(led_3, HIGH);
digitalWrite(led_4, HIGH);
digitalWrite(led_5, HIGH);
digitalWrite(led_6, HIGH);
digitalWrite(led_7, HIGH);
digitalWrite(led_8, HIGH);
digitalWrite(led_9, HIGH);
digitalWrite(led_10, HIGH);
digitalWrite(led_11, HIGH);
digitalWrite(led_12, HIGH);
digitalWrite(led_13, HIGH);
digitalWrite(led_14, HIGH);
digitalWrite(led_15, HIGH);
digitalWrite(led_16, HIGH);

}

void loop() {
  // Bluetooth üzerinden gelen komutları kontrol et
  if (SerialBT.available()) {
    String command = ""; // Gelen komutları biriktirmek için kullanılacak değişken
    while (SerialBT.available()) {
      char c = SerialBT.read();
      command += c; // Gelen veriyi biriktir
      delay(10); // Komutun tamamlanmasını bekle
    }

    // Komutu temizle
    command.trim();
    Serial.println("Bluetooth'tan Gelen Komut: " + command);

    // Wi-Fi SSID ve parola güncelleme komutlarını kontrol et
    if (command.startsWith("SSID:")) {
      ssid = command.substring(5);  // SSID değerini güncelle
      Serial.println("Yeni SSID: " + ssid);
      disconnectWifi();
    } else if (command.startsWith("PASS:")) {
      password = command.substring(5);  // Parolayı güncelle
      Serial.println("Yeni Parola: " + password);
      connectWiFi();  // Yeni parola ile Wi-Fi'ye yeniden bağlan
    } else {
      // LED kontrol komutlarını çalıştır
      controlLEDs(command);
    }
  }

  // Wi-Fi üzerinden gelen HTTP isteklerini kontrol et
  WiFiClient client = server.available(); // Bağlantı bekleyen bir istemci olup olmadığını kontrol et

  if (client) { // Eğer bir istemci bağlandıysa
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\r') { // İstek sonuna ulaşıldığında
          Serial.println("HTTP İsteği: " + request);

          // İstekten komutu çıkar
          int start = request.indexOf("GET /") + 5;
          int end = request.indexOf("HTTP/");
          String command = request.substring(start, end);

          // Komutu temizle
          command.replace("\n", "");
          command.replace("\r", "");
          command.replace(" ", "");
          command.replace("\t", "");
          command.trim();

          Serial.println("HTTP'den Gelen Komut: " + command);

          // Komuta göre LED'leri kontrol et
          controlLEDs(command);

          // HTTP yanıtını gönder
          if (client.peek() == '\n') {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            String response = "<html><body>" + command + " is received</body></html>";
            client.println(response);
            break;
          }
        }
      }
    }
    client.stop(); // İstemci bağlantısını sonlandır
  }
}

void disconnectWifi() {
    WiFi.disconnect(true);
    Serial.println("Wi-Fi bağlantısı kesildi!");
}

// Yeni SSID ve şifre ile Wi-Fi'ye bağlanan fonksiyon
void connectWiFi() {
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Wi-Fi'ye bağlanılıyor");
  unsigned long startAttemptTime = millis();
  bool connectionSuccess = false;
  // Wi-Fi'ye bağlanmaya çalış, 5 saniye zaman aşımı
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 5000) {
    delay(1000);
    Serial.print(".");
  }
// Eğer bağlantı sağlandıysa
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi Bağlantısı Sağlandı!");
    Serial.print("IP Adresi: ");
    Serial.println(WiFi.localIP());
    if (SerialBT.hasClient()) {
      String ipAddress = "IP:" + WiFi.localIP().toString(); 
        SerialBT.print(ipAddress);
        Serial.println("Bluetooth üzerinden IP adresi gönderildi.");
    } else {
      Serial.println("Bluetooth bağlantısı yok, IP adresi gönderilemedi.");
    }
    prev_ssid = ssid;         
    prev_password = password; 
  } else {
    Serial.println("\nBağlantı başarısız! Eski Wi-Fi'ye dönülüyor...");
    ssid = prev_ssid;         
    password = prev_password; 
    connectWiFi();            
  }
}

// LED'leri kontrol eden fonksiyon
void controlLEDs(String command) {
  int cmd = command.toInt();   
  int ledNumber = cmd / 10;    // Bölüm 
  int ledState = cmd % 10;     // Kalan 
  int ledPin = -1;            

  if (ledNumber <= 8) {  
    switch (ledNumber) {
      case 1: ledPin = led_1; break;
      case 2: ledPin = led_2; break;
      case 3: ledPin = led_3; break;
      case 4: ledPin = led_4; break;
      case 5: ledPin = led_5; break;
      case 6: ledPin = led_6; break;
      case 7: ledPin = led_7; break;
      case 8: ledPin = led_8; break;
    }
  } else if (ledNumber > 8 && ledNumber <= 16) { 
    switch (ledNumber) {
      case 9: ledPin = led_9; break;
      case 10: ledPin = led_10; break;
      case 11: ledPin = led_11; break;
      case 12: ledPin = led_12; break;
      case 13: ledPin = led_13; break;
      case 14: ledPin = led_14; break;
      case 15: ledPin = led_15; break;
      case 16: ledPin = led_16; break;
    }
  } else {
    return; 
  }

  if (ledState == 1) {
    digitalWrite(ledPin, HIGH);
  } else if (ledState == 0) {
    digitalWrite(ledPin, LOW);
  } else {
    return;
  }
}




