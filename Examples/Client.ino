
#include <Arduino.h>
#include "NightMareTCPClient.h"
//Create a client with debug enabled.
NightMareTCPClient client(true);

#define HOST "host"
#define PORT 100

#define WIFISSID "mywifi"
#define WIFIPASSWD "********"

uint32_t lastTimeAMessageWasSent = 0;
int counter = 0;

String newMessage(String message)
{
    Serial.printf("server: %s\n", message.c_str());
    //for now return is thrown away
    //the flow is aways clients asks
    //server responds, however the typing
    //is in place for easy modding
    return "ack";
}

void setup()
{
    Serial.begin(115200);
    
    //Start WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFISSID, WIFIPASSWD);

    uint8_t counter = 0;
    bool notconnected = false;
    while (WiFi.status() != WL_CONNECTED && !notconnected)
    {

        if (millis() % 100 == 0)
        {
            counter++;
            Serial.print(".");
        }

        if (counter == 10)
        {
            Serial.print(WiFi.status());
            Serial.println();
            counter = 0;
        }
        yield();
        if (millis() > 15000)
        {
            Serial.print("Reseting at: ");
            Serial.print(millis());
            ESP.restart();
        }
    }
    
    //Set the handler for messages that the client recieves
    client.setMessageHandler(newMessage);
    //Start the client
    Serial.printf("Connecting to: %s at port: %d\n",HOST,PORT);
    client.connect(HOST, PORT);
    if (client.connected())
        Serial.println("Connected!");
    else
        Serial.println("Failed to connect.");
}

void loop()
{   //must be called frequently
    client.handleClient();

    if (millis() - lastTimeAMessageWasSent > 10000)
    {
        Serial.printf("Sending message {%d}\n",counter);
        counter ++;
        client.send("Hello from the other side!");
        lastTimeAMessageWasSent = millis();
    }
    
}