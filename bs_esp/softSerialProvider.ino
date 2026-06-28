#if RELAY_PROVIDER == RELAY_PROVIDER_SOFTWARE_SERIAL
#include <SoftwareSerial.h>

SoftwareSerial mySerial(SOFTWARE_SERIAL_RX, SOFTWARE_SERIAL_TX); // RX, TX

static bool got_feedback = true;
static bool got_status = true;
static uint8_t tRelayId;
static uint8_t tRelayStatus;

void softwareSerialProviderSetup() {
    mySerial.begin(9600); // Set the baud rate for SoftwareSerial
}

void sendUartCommandSetRelayStatus(uint8_t relayId, uint8_t status) {
    if((relayId>relayCount()) || (relayStatus(relayId) == status)) return;
    tRelayId = relayId;
    tRelayStatus = status;
    mySerial.printf("REL SET %d %d\n", relayId, status);
    got_feedback = false;
}

void sendUartCommandGetRelayStatus() {
    mySerial.printf("REL GET\n");
    got_status = false;
}

void softwareSerialProviderLoop() {
    if (mySerial.available()) {
        String receivedData = mySerial.readStringUntil('\n');
        if (receivedData.startsWith("relay")) {
            int relayId, status;
            sscanf(receivedData.c_str(), "relay %d %d", &relayId, &status);
            // Extract relayId and relayStatus from the received data
            // int relayId = receivedData.charAt(6) - '0';
            // int relayStatus = receivedData.charAt(8) - '0';
            if(relayId > relayCount()) return;
            if(relayStatus(relayId) == status) return;

            // Set the relay status
            relayUpdate((unsigned char)relayId, (unsigned char)status);
            // relayMQTT((unsigned char)relayId);
            //[todo] need to report
            
    got_status = true;
        }

        if (receivedData.startsWith("factoryset")) {
            settingFactoryReset();
        }

        if (receivedData.startsWith("OK")) {
            got_feedback = true;
        }
    }

    static int _check_feedback_last_millis = 0;
    if(millis() - _check_feedback_last_millis > 500){
        _check_feedback_last_millis = millis();

        if(!got_feedback)
            sendUartCommandSetRelayStatus(tRelayId, tRelayStatus);

        if(!got_status)
            sendUartCommandGetRelayStatus();
    }

}


#endif
