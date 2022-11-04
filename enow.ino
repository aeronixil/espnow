#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
// configure the below two line 

String devIdentifier = "bravo";  //name of this device
//mac address of peer device

//uint8_t peerAddress[] = {0x3C, 0x61, 0x05, 0x17, 0x1B, 0x1C}; 
uint8_t peerAddress[] = {0x24, 0x0A, 0xC4, 0xEF, 0x66, 0x10};  //mac address of peer device
//
//
//


// Define variables to store incoming readings
String receivedMsg;
String receivedFrom;
int receivedNumber;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct data_Packet {
    String msg;
    String deviceName;
    int number = 0;             
    
} struct_message;

int temp = 0;
String str;


// data packet object to store data to be sent
data_Packet goingOut;


// data packet object to store receving data
data_Packet comingIn;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    //success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&comingIn, incomingData, sizeof(comingIn));
  Serial.print("Bytes received: ");
  Serial.println(len);
  receivedMsg = comingIn.msg;
  receivedNumber = comingIn.number;
  receivedFrom = comingIn.deviceName;

  Serial.print("message from: ");
  Serial.println(receivedFrom);
  Serial.print("message: ");
  Serial.println(receivedMsg);
  Serial.print("transmission unit: ");
  Serial.println(receivedNumber);
  Serial.println("\n\n\n\n\n\n");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  int a= esp_wifi_set_protocol( ESP_IF_WIFI_STA, WIFI_PROTOCOL_LR );
    Serial.println(a);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 8;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
 

  if(Serial.available())
  {
    str = Serial.readString();
    goingOut.msg=str;
    goingOut.number = temp++;
    goingOut.deviceName = devIdentifier;


    esp_err_t result = esp_now_send(peerAddress, (uint8_t *) &goingOut, sizeof(goingOut));
   
  if (result != ESP_OK) {
    Serial.println("Error sending the data");
  }

  }//end serial if check statement


  //endloop
}
