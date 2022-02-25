#include <logger_spiffs.h>
#include <Ticker.h>

const String filepath = "/log.log";

LoggerSPIFFS myLog(filepath);

Ticker counterLoop;

int counter;
float interval = 1;

Ticker counterLoop(){
  counter++;

  if(counter%2 == 0){
    myLog.append(String("val:") + counter);
    Serial.println("Log file size: " +  myLog.getActualSize());
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  myLog.begin();

  Serial.println("Logging started...");

  counterLoop.attach(interval, counterLoop);
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
