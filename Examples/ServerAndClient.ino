
#include <Arduino.h>
#include "NightMareTCP.h"
// Create a client with debug enabled.
NightMareTCPClient client(true);
//  Creates a server in port 100 with debug enabled.
NightMareTCPServer server(100, true);

//overrides the default 10 max clients.
#define MAX_CLIENTS 40

#define HOST "host"
#define PORT 100

#define WIFISSID "mywifi"
#define WIFIPASSWD "********"

uint32_t lastTimeAMessageWasSent = 0;
int counter = 0;

String clientNewMessage(String message)
{
    Serial.printf("server: %s\n", message.c_str());
    // for now return is thrown away
    // the flow is aways clients asks
    // server responds, however the typing
    // is in place for easy modding
    return "ack";
}

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

    // Set the handler for messages that the client recieves
    client.setMessageHandler(clientNewMessage);
    // Start the client
    Serial.printf("Connecting to: %s at port: %d\n", HOST, PORT);
    client.connect(HOST, PORT);
    if (client.connected())
        Serial.println("Connected!");
    else
        Serial.println("Failed to connect.");
    // Disables keepAlive messages
    client.setKeepAlive(-1);
    //  Set the handler for messages to the server.
    server.setMessageHandler(serverNewMessage);
    //  Start the server
    server.begin();
    //Changes the default timeout for inactive clients
    //to 30 s.
    server.setTimeout(30000);
    //Disables timeout for inactive clients.
    server.setTimeout(-1);
}

void loop()
{ // must be called frequently
    client.handleClient();
    // must be called frequently
    server.handleServer();

    if (millis() - lastTimeAMessageWasSent > 10000)
    {
        Serial.printf("Sending message {%d}\n", counter);
        counter++;
        client.send("Hello from the other side!");
        lastTimeAMessageWasSent = millis();

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