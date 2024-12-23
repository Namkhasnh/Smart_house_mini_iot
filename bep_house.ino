#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>  // Thêm thư viện DHT
#include <ESP32Servo.h>// Thư viện Servo

const char* ssid = "hihi";       // Tên WiFi của Khánh
const char* password = "22222222"; // Mật khẩu WiFi

const String BOT_TOKEN = "7224249343:AAGnwC7ePEiawnyAxgasVT71rG4_2ROjejc"; // Token bot
const String CHAT_ID = "-4506696436"; // Chat ID của Khánh
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
int botRequestDelay = 1000;
unsigned long lastTimeBotRan = 0;
unsigned long doorOpenMillis = 0;  // Lưu thời gian khi cửa mở
const long doorOpenTimeout = 15000; // Thời gian tối đa (5 giây) trước khi thông báo
bool doorAlertSent = false;
unsigned long lastAlertMillis = 0; // Thời gian gửi cảnh báo gần nhất
unsigned long previousMillis = 0;  // Biến dùng để đo thời gian
const long interval = 1500;       // Khoảng cách thời gian giữa các lần kiểm tra
const int reedSwitch = 14;        // Chân GPIO kết nối với cảm biến cửa
bool changeState = false;         // Cờ báo hiệu trạng thái thay đổi
bool state;                       // Trạng thái hiện tại của cửa (mở/đóng)
String doorState;
Servo servoRainCover; 
const int servoRainPin = 12;
const int relay = 26;                     // chân relay
const int buzzer = 21;                    // chân còi
const int ledPin = 2;                     // Chân kết nối LED ngoài
bool ledState = LOW;
bool isWatering = false;
bool hasSentWateredMessage = false;
const int gasPin = 34;                     // Chân cảm biến khí gas
const int flamePin = 35;                    // chân cảm biến lửa
const int rainPin = 33;                       //chân cảm biến mưa
const int moisturePin = 32;                //độ ẩm đất    
const int THRESHOLD_GAS = 2000;            // Ngưỡng cho cảm biến khí gas
const int THRESHOLD_FLAME = 500;
const int THRESHOLD_RAIN = 2000;
const int DHTPIN = 4;                       // Chân kết nối cảm biến DHT
const int DHTTYPE = DHT11;                   // Kiểu cảm biến (DHT11 hoặc DHT22)
DHT dht(DHTPIN, DHTTYPE);  // Khởi tạo cảm biến DHT

ICACHE_RAM_ATTR void changeDoorStatus() {
  changeState = true;
}

Servo servoMotor;                         // Khởi tạo động cơ servo
const int servoPin = 13;                  // Chân kết nối động cơ servo
bool isRaining = false;                  // Trạng thái mưa

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    String text = bot.messages[i].text;
    if (text == "/batdenbep") {
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
      bot.sendMessage(chat_id, "💡LED state set to ON", "");
    }
    
    if (text == "/tatdenbep") {
      ledState = LOW;
      digitalWrite(ledPin, ledState);
      bot.sendMessage(chat_id, "LED state set to OFF", "");
    }
    
    if (text == "/trangthaidenbep") {
      bot.sendMessage(chat_id, digitalRead(ledPin) ? "LED is ON" : "LED is OFF", "");
    }
    if (text == "/dongcua") {
      servoMotor.write(100);
      
    }
    if (text == "/mocua") {
      servoMotor.write(0);
    }
    
    if (text == "/dongmai") {
      servoRainCover.write(90);
      bot.sendMessage(CHAT_ID, "Mái đã Đóng");
    }
    if (text == "/momai") {
      servoRainCover.write(0);
      bot.sendMessage(CHAT_ID, "Mái đã Mở");
    }

    if (text == "/doamdat") {
      int moistureLevel = analogRead(moisturePin);
      int moisture = (100-((moistureLevel / 4095.00)* 100));
      bot.sendMessage(chat_id, "Độ ẩm đất: " + String(moisture), "");
     
      }
    if (text == "/temp") {
      float h = dht.readHumidity();    // Đọc độ ẩm
      float t = dht.readTemperature(); // Đọc nhiệt độ

      if (isnan(h) || isnan(t)) {
        bot.sendMessage(chat_id, "Failed to read from DHT sensor!", "");
      } else {
        String tempMessage = "🌡️ Nhiệt độ: " + String(t) + "°C\n" + "💧 Độ ẩm: " + String(h) + "%";
        bot.sendMessage(chat_id, tempMessage, "");
      }
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(rainPin, INPUT);
  pinMode(moisturePin, INPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(relay,OUTPUT);
  pinMode(reedSwitch, INPUT_PULLUP);
  state = digitalRead(reedSwitch);
  attachInterrupt(digitalPinToInterrupt(reedSwitch), changeDoorStatus, CHANGE);

  digitalWrite(relay,HIGH);
  digitalWrite(ledPin, ledState);
  WiFi.begin(ssid, password);
  client.setInsecure();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Kết nối WiFi thành công!");
  Serial.println(WiFi.localIP());
  dht.begin(); // Khởi tạo cảm biến 
  servoMotor.attach(servoPin); // Khởi động động cơ servo
  servoMotor.write(90);
  servoRainCover.attach(servoRainPin); // Gán động cơ servo mới
  servoRainCover.write(0);  // Đặt góc ban đầu cho servo che mưa 
  bot.sendMessage(CHAT_ID, "Bot Started", "");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (millis() > lastTimeBotRan + botRequestDelay) {
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      while (numNewMessages) {
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      }
      lastTimeBotRan = millis();
    }
    int rainStatus = analogRead(rainPin);
    int gasLevel = analogRead(gasPin);
    int flameLevel = analogRead(flamePin);
    int moistureLevel = analogRead(moisturePin);
    int moisture = (100-((moistureLevel / 4095.00)* 100));

    Serial.print("Mức khí gas: ");
    Serial.println(gasLevel);
    Serial.print("Mức lửa: ");
    Serial.println(flameLevel);
    Serial.print("Trạng thái cảm biến mưa: ");
    Serial.println(rainStatus);
    Serial.print("Độ ẩm đất: ");
    Serial.println(moisture);

    if (changeState) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Cập nhật trạng thái cửa
    state = !state;
    if (state) {
      doorState = "đóng";
      doorAlertSent = false;       // Reset cảnh báo khi cửa đóng
    } else {
      doorState = "mở";
      doorOpenMillis = millis();  // Ghi lại thời gian cửa được mở
    }

    // Gửi thông báo trạng thái qua Telegram
    bot.sendMessage(CHAT_ID, "🚪 Cửa đang " + doorState, "");
    changeState = false;  // Reset cờ thay đổi trạng thái
  }
}

// Kiểm tra nếu cửa đang mở quá lâu
if (!state) { // Nếu cửa đang mở
  unsigned long currentMillis = millis();
  if (currentMillis - doorOpenMillis >= doorOpenTimeout) { // Nếu đã quá 5 giây từ khi cửa mở
    if (currentMillis - lastAlertMillis >= doorOpenTimeout) { // Gửi cảnh báo mỗi 5 giây
      bot.sendMessage(CHAT_ID, "⚠️ Cảnh báo! Cửa đã mở quá lâu !!!", "");
      lastAlertMillis = currentMillis; // Cập nhật thời gian gửi cảnh báo gần nhất
    }
  }
}
  


    if (gasLevel > THRESHOLD_GAS) {
      String message = "⚠️ Cảnh báo: Phát hiện mức khí gas cao! Mức đo: " + String(gasLevel);
      bot.sendMessage(CHAT_ID, message, "");
      for (int i= 0 ;i < 4 ; i++)
      {
        digitalWrite(buzzer , 1);
        delay(2000);
        digitalWrite(buzzer , 0);
        delay(2000);
        }
      delay(30000);
    }

    if (flameLevel < THRESHOLD_FLAME) {
      String message = "🔥 Cảnh báo: Phát hiện có lửa! Mức đo: " + String(flameLevel);
      bot.sendMessage(CHAT_ID, message, "");
      for (int i= 0 ;i < 4 ; i++)
      {
        digitalWrite(buzzer , 1);
        delay(2000);
        digitalWrite(buzzer , 0);
        delay(2000);
        }
        delay(30000);
    }
    
    if (moisture < 30 && !isWatering) { // Nếu độ ẩm đất quá thấp và chưa tưới
        String message = "🌱 Cảnh báo: độ ẩm đất đang ở mức: " + String(moisture) + "\nĐất quá khô, đang tưới nước!";
        bot.sendMessage(CHAT_ID, message, "");
        digitalWrite(relay, LOW); // Bật máy bơm nước
        isWatering = true; // Đánh dấu là đang tưới nước
        hasSentWateredMessage = false; // Đặt lại trạng thái đã gửi thông báo tưới cây
    }
    else if (moisture >= 30) {
        if (isWatering && !hasSentWateredMessage) { // Nếu đang tưới và chưa gửi thông báo
            digitalWrite(relay, HIGH); // Tắt máy bơm nước
            int newMoisture = (100-((moistureLevel / 4095.00)* 100));
            String successMessage = "💦🌱 Đã tưới cây!\nĐộ ẩm sau khi tưới: " + String(newMoisture);
            bot.sendMessage(CHAT_ID, successMessage, "");
            hasSentWateredMessage = true; // Đánh dấu là đã gửi thông báo tưới cây
        }
        isWatering = false; // Đặt lại trạng thái tưới nước nếu độ ẩm đủ
    }



    if (rainStatus < THRESHOLD_RAIN) {  // Khi phát hiện có nước, trạng thái sẽ là LOW
      if (!isRaining) {  // Chỉ thực hiện nếu trạng thái thay đổi
        isRaining = true;
        String message = "🌧️ Cảnh báo: Phát hiện có mưa!";
        bot.sendMessage(CHAT_ID, message, "");

        servoRainCover.write(90); // Đóng nắp che (đặt góc tùy ý)
        delay(500); // Đợi một chút để động cơ hoàn thành chuyển động
       // Có thể kết hợp điều khiển động cơ khác (nếu cần)
        bot.sendMessage(CHAT_ID, "Mái đã Đóng");
      }
    } else if (rainStatus >= THRESHOLD_RAIN && isRaining) {  // Khi trời không còn mưa
      isRaining = false;
      bot.sendMessage(CHAT_ID, "🌞 Trời đã hết mưa.", "");

      servoRainCover.write(0); // Mở nắp che (đặt góc tùy ý)
       // Đặt lại động cơ khác (nếu cần)
      bot.sendMessage(CHAT_ID, "Mái đã Mở");
    }
    }
  delay(1000);
}