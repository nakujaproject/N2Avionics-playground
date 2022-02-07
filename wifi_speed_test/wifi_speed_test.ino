
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <Servo.h>

// Create wifi, networking and servo interface objects
ESP8266WiFiMulti WiFiMulti;
WiFiUDP Udp;
Servo myservo;

// Create a buffer for UDP communications
int const incomingBufferSize = 128;
char incomingPacket[incomingBufferSize];

// Specify the request packets
// Kudos to http://www.rane.com/note161.html and http://www.net-snmp.org/ for helping understand the SNMP packet

int const requestSize = 42;

byte uploadRequestPacekt[requestSize] = { 0x30, // Sequence Complex Data Type
                                         0x28, // Request Size (less this 2 byte header)
                                         0x02, // Version Type Integer
                                         0x01, // Version Length 1
                                         0x00, // Version Number 0
                                         0x04, // Type Octet String
                                         0x06, // Length 7
                                         0x70, // p
                                         0x75, // u
                                         0x62, // b
                                         0x6C, // l
                                         0x69, // i
                                         0x63, // c
                                         0xA0, // SNMP PDU Type GetRequest
                                         0x1B, // PDU Length
                                         0x02, // Request ID Type Integer
                                         0x01, // Request ID Length
                                         0x0A, // Reuest ID Value LSB (using 10 to id upload)
                                         0x02, // Error Tyne Integer
                                         0x01, // Error Length 1
                                         0x00, // Error Value 0
                                         0x02, // Error Index Type Integer
                                         0x01, // Error Index Type Length 1
                                         0x00, // Error Index Type Value 0
                                         0x30, // Varbind List Type Sequence
                                         0x10, // Varbind List Type Length
                                         0x30, // Varbind Type Sequence
                                         0x0E, // Varbind Type Length
                                         0x06, // Object Identifier Type Object Identifier
                                         0x0A, // Object Identifier Length
                                         0x2B, // ISO.3
                                         0x06, // 6
                                         0x01, // 1
                                         0x02, // 2
                                         0x01, // 1
                                         0x02, // 2
                                         0x02, // 2
                                         0x01, // 1
                                         0x0A, // 10 (Upload)
                                         0x07, // 7 (eth0/lan on RV024G)
                                         0x05, // Value Type Null
                                         0x00 }; // Value Length 0

byte downloadRequestPacekt[requestSize] = { 0x30, // Sequence Complex Data Type
                                         0x28, // Request Size (less this 2 byte header)
                                         0x02, // Version Type Integer
                                         0x01, // Version Length 1
                                         0x00, // Version Number 0
                                         0x04, // Type Octet String
                                         0x06, // Length 7
                                         0x70, // p
                                         0x75, // u
                                         0x62, // b
                                         0x6C, // l
                                         0x69, // i
                                         0x63, // c
                                         0xA0, // SNMP PDU Type GetRequest
                                         0x1B, // PDU Length
                                         0x02, // Request ID Type Integer
                                         0x01, // Request ID Length
                                         0x10, // Reuest ID Value LSB (using 16 to id download)
                                         0x02, // Error Tyne Integer
                                         0x01, // Error Length 1
                                         0x00, // Error Value 0
                                         0x02, // Error Index Type Integer
                                         0x01, // Error Index Type Length 1
                                         0x00, // Error Index Type Value 0
                                         0x30, // Varbind List Type Sequence
                                         0x10, // Varbind List Type Length
                                         0x30, // Varbind Type Sequence
                                         0x0E, // Varbind Type Length
                                         0x06, // Object Identifier Type Object Identifier
                                         0x0A, // Object Identifier Length
                                         0x2B, // ISO.3
                                         0x06, // 6
                                         0x01, // 1
                                         0x02, // 2
                                         0x01, // 1
                                         0x02, // 2
                                         0x02, // 2
                                         0x01, // 1
                                         0x10, // 16 (Upload)
                                         0x07, // 7 (eth0/lan on RV024G)
                                         0x05, // Value Type Null
                                         0x00 }; // Value Length 0

// Variables to store the last read values and times
unsigned long uploadValueLast = 0;
unsigned long downloadValueLast = 0;
unsigned long uploadTimeLast = 0;
unsigned long downloadTimeLast = 0;

// Variables for storing the derived values
double uploadRate = 0;    // Result in Mbps
double downloadRate = 0;  // Result in Mbps

int currentServoPos = 0;
int newServoPos = 0;

// Procedure to request upload data from the gateway
void sendUploadRequest() {
//  Udp.beginPacket(IPAddress(192, 168, 0, 1), 161);
  Udp.beginPacket(WiFi.gatewayIP(), 161);
  Udp.write(uploadRequestPacekt, requestSize);
  Udp.endPacket();
}

// Procedure to request download data from the gateway
void sendDownloadRequest() {
//  Udp.beginPacket(IPAddress(192, 168, 0, 1), 161);
  Udp.beginPacket(WiFi.gatewayIP(), 161);
  Udp.write(downloadRequestPacekt, requestSize);
  Udp.endPacket();
}

// Calculate the servo's new position and set it
void updateServo() {
  double pos = _max(uploadRate, downloadRate);
  pos = _max(0, _min({top_connection_speed}, pos));
  newServoPos = pos/{top_connection_speed}*180;
}

// Function to check for and read new SNMP data
int checkForReply(unsigned long inNow) {

  if (Udp.parsePacket()>0) {
    int lengthRead = Udp.read(incomingPacket, incomingBufferSize);

    lengthRead++; // A quick fix for when Udp.read returns [wrongly] one short
    if (lengthRead>incomingBufferSize) { return false; } // Range check for quick fix

    int pointer = 0;

    // Check the expected message type
    if (pointer>=lengthRead) { return false; } // Range check
    if (incomingPacket[pointer]!=0x30) { return false; } // Chech value is Sequence
    pointer += 1;
    
    // Check the message is the right size
    if (pointer>=lengthRead) { return false; } // Range check
    if ((incomingPacket[pointer]+2)>lengthRead) { return false; } // Chech value is Sequence
    pointer += 1;

    // Skip the version
    pointer += 1;
    if (pointer>=lengthRead) { return false; } // Range check
    pointer += incomingPacket[pointer];
    pointer += 1;

    // Skip the community string
    pointer += 1;
    if (pointer>=lengthRead) { return false; } // Range check
    pointer += incomingPacket[pointer];
    pointer += 1;
    
    // Check the DPU header is for a GetResponse DPU
    if (pointer>=lengthRead) { return false; } // Range check
    if (incomingPacket[pointer]!=0xA2) { return false; } // Chech value is an integer
    pointer += 1;
    pointer += 1;
    
    // Read the request ID
    if (pointer>=lengthRead) { return false; } // Range check
    if (incomingPacket[pointer]!=0x02) { return false; } // Chech value is an integer
    pointer += 1;
    if (pointer>=lengthRead) { return false; } // Range check
    int counterLength = incomingPacket[pointer];
    pointer += 1;
    if (pointer+counterLength>=lengthRead) { return false; } // Range check
    unsigned long requestID = 0;
    for (int offset=0; offset<counterLength; offset++) {
      requestID = (requestID<<8) | incomingPacket[pointer+offset];
    }
    pointer += counterLength;
    
    // Skip the error
    pointer += 1;
    if (pointer>=lengthRead) { return false; } // Range check
    pointer += incomingPacket[pointer];
    pointer += 1;
    
    // Skip the error index
    pointer += 1;
    if (pointer>=lengthRead) { return false; } // Range check
    pointer += incomingPacket[pointer];
    pointer += 1;
    
    // Skip the varbind list header
    pointer += 2;
    
    // Skip the varbind header
    pointer += 2;
    
    // Skip the identifier
    pointer += 1;
    if (pointer>=lengthRead) { return false; } // Range check
    pointer += incomingPacket[pointer];
    pointer += 1;
    
    // Read the data
    if (pointer>=lengthRead) { return false; } // Range check
    if (incomingPacket[pointer]!=0x41) { return false; } // Chech value is a counter Counter
    pointer +=1;
    if (pointer>=lengthRead) { return false; } // Range check
    int dataLength = incomingPacket[pointer];
    pointer +=1;
    if (pointer+dataLength>=lengthRead) { return false; } // Range check
    unsigned long value = 0;
    for (int offset=0; offset<dataLength; offset++) {
      value = (value<<8) | incomingPacket[pointer+offset];
    }

    // Check what to do with the data we recieved based on the ID
    if (requestID==0x0A) { // Upload
      uploadRate = (double) 8/1000 * (value-uploadValueLast) / (inNow-uploadTimeLast); // Result in Mbps
      uploadValueLast = value;
      uploadTimeLast = inNow;
      updateServo();
      return true;
    }
    else if (requestID==0x10) { // Download
      downloadRate = (double) 8/1000 * (value-downloadValueLast) / (inNow-downloadTimeLast); // Result in Mbps
      downloadValueLast = value;
      downloadTimeLast = inNow;
      updateServo();
      return true;
    }
  }

  // We get this far but had nothing to do with the data we recieved
  return false;
  
}

// Setup routine
void setup() {

  // Connect to the WiFi
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("{wifi_ssid}", "{wifi_password}");
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
  }

  // Start listening for SNMP messges
  Udp.begin(162);

  // Switch the TX pin for use as a GPIO pin
  pinMode(1, FUNCTION_3);

  // Connect the servo
  myservo.attach(1);  // attaches the servo on TX pin to the servo object

}

// Keep trach of when we last request data
unsigned long millisLast = 0;

// The main loop
void loop() {

  // Read the time so we only need to digit once
  unsigned long millisNow = millis();

  // Check for a new SNMP message
  checkForReply(millisNow);

  // Move the servo slowly
  if (newServoPos>currentServoPos) {
    currentServoPos++;
    myservo.write(180-currentServoPos);
  }
  else if (newServoPos<currentServoPos) {
    currentServoPos--;
    myservo.write(180-currentServoPos);
  }

  // Check if we need to send another SNMP request
  if ((millisNow-millisLast)>5000) {
    // Send the requests
    sendDownloadRequest();
    sendUploadRequest();
    // Set value to zero if a few updates have been missed
    if ((millisNow-uploadTimeLast)>15000) {
      uploadRate = 0;
      updateServo();
    }
    if ((millisNow-downloadTimeLast)>15000) {
      downloadRate = 0;
      updateServo();
    }
    // Rememebr when we did this to wait another 5 seconds
    millisLast = millisNow;
  }

  // Save a little power
  delay(40);

}
