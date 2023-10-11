
#include <Arduino.h>
#include "NightMareTCPServer.h"
//  Creates a server in port 100 with debug enabled.
NightMareTCPServer server(100, true);
uint32_t lastTimeAMessageWasSent = 0;

#define WIFISSID "mywifi"
#define WIFIPASSWD "********"


String serverNewMessage(String message, int index)
{
    Serial.printf("[%s:%d]: %s\n", server.clients[index].client->remoteIP().toString().c_str(), server.clients[index].client->remotePort(), message.c_str());
    // Whatever is return is sent to sender.
    return "ack";
}

void setup()
{
    Serial.begin(115200);
    // Start WiFi
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

    //  Set the handler for messages to the server.
    server.setMessageHandler(serverNewMessage);
    //  Start the server
    server.begin();
}

void loop()
{ // must be called frequently
    // must be called frequently
    server.handleServer();

    if (millis() - lastTimeAMessageWasSent > 10000)
    {
        Serial.println("Client connected: ");
        String clientString;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (NULL != server.clients[i].client)
            {
                clientString += "Client [";
                clientString += i;
                clientString += "], IP: ";
                clientString += server.clients[i].client->remoteIP().toString();
                clientString += ", (self) ID: ";
                clientString += server.clients[i].clientsID;
                clientString += ", INFO: ";
                clientString += server.clients[i].clientsStatus[i];
            }
            Serial.print("  -");
            Serial.println(clientString);
        }
    }
}