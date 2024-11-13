#define photopin 34
#define interval 1 // in milli seconds
// #define interval 3000 // in micro seconds
#define threshold 100
#define buffersize 16000



String binData = "";

char binaryToChar(String binary) {
  int value = 0;
  for (int i = 0; i < 8; i++) {
    value <<= 1;  // Shift left to make space for the next bit
    if (binary.charAt(i) == '1') {
      value = value | 1;  // Set the least significant bit if it's '1'
    }
  }
  return (char)value;  // Return the resulting character
}

void processData() {
  int indx = binData.indexOf("00000010");
  // Serial.println(indx);
  binData.remove(0, indx+8);
  while(binData.length()) {
    String subData = binData.substring(0, 8);
    binData.remove(0, 8);
    if(subData == "00000011") break;
    char ch = binaryToChar(subData);
    // processedData += ch;
    Serial.print(ch);
    vTaskDelay(interval * 8 / portTICK_PERIOD_MS); //in milliseconds
    // vTaskDelay(interval * 8 ); // in microseconds
  }
  binData.clear();
  Serial.println("");
}

void signalRecieving(void *parameter) {
  for(;;) {
    int value = analogRead(photopin);
    if(value > threshold) binData += '0';
    else binData += '1';

    vTaskDelay(interval / portTICK_PERIOD_MS); // in milli seconds
    // vTaskDelay(interval); // in micro seconds
  }
}

void signalProcessing(void *parameter) {

  for(;;) {
    if(binData.length() > buffersize) {
    binData.remove(0, 15900);
    }
    else if(binData.indexOf("00000010") > -1) {
      binData.remove(0, binData.indexOf("00000010"));
      vTaskDelay(interval * 10 / portTICK_PERIOD_MS); // in milli seconds
      // vTaskDelay(interval * 10); // in micro seconds

      processData();
    }

    vTaskDelay(500 / portTICK_PERIOD_MS); // in milli seconds
    // vTaskDelay(500 * 1000); // in micro seconds
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(photopin, INPUT);

  pinMode(2, OUTPUT);

  // digitalWrite(2, HIGH);
  // delay(100);
  // digitalWrite(2, LOW);

  // Create a task on Core 0
  xTaskCreatePinnedToCore(
    signalRecieving,         // Function to implement the task
    "Signal Recieving",       // Name of the task
    10000,           // Stack size in words
    NULL,            // Task input parameter
    1,               // Priority of the task
    NULL,            // Task handle
    0                // Core where the task should run (Core 0)
  );

  // Create a task on Core 1
  xTaskCreatePinnedToCore(
    signalProcessing,         // Function to implement the task
    "Signal Processing",       // Name of the task
    10000,           // Stack size in words
    NULL,            // Task input parameter
    1,               // Priority of the task
    NULL,            // Task handle
    1                // Core where the task should run (Core 1)
  );
}

void loop() {

}