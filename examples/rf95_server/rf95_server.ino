// rf95_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_client
// Tested with Anarduino MiniWirelessLoRa, ESP32, Arduino Uno R4



#include <RH_RF95.h>

#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
    // Arduino Uno R4 (Minima or WiFi): Use Serial1 hardware UART
    // Serial1 uses pins D0 (RX) and D1 (TX)
    #define COMSerial Serial1
    #define ShowSerial Serial

    RH_RF95<HardwareSerial> rf95(COMSerial);
#elif defined(__AVR__)
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(10, 11); // RX, TX
    #define COMSerial SSerial
    #define ShowSerial Serial

    RH_RF95<SoftwareSerial> rf95(COMSerial);
#endif

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define COMSerial Serial1
    #define ShowSerial SerialUSB

    RH_RF95<Uart> rf95(COMSerial);
#endif

#ifdef ARDUINO_ARCH_STM32F4
    #define COMSerial Serial
    #define ShowSerial SerialUSB

    RH_RF95<HardwareSerial> rf95(COMSerial);
#endif

#ifdef ESP32
    // ESP32: Use Serial1 for LoRa module (default pins: RX=9, TX=10)
    // You can reassign pins with Serial1.begin(baud, SERIAL_8N1, RX_PIN, TX_PIN)
    #define COMSerial Serial1
    #define ShowSerial Serial

    RH_RF95<HardwareSerial> rf95(COMSerial);
#endif


int led = 13;


void setup() {
    ShowSerial.begin(115200);

    #if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
        // Arduino Uno R4: Initialize Serial1 for LoRa module communication
        // Serial1 uses pins D0 (RX) and D1 (TX)
        COMSerial.begin(57600);
        delay(100); // Give serial time to initialize

        // Arduino Uno R4 uses built-in LED on pin 13 (default already set)
    #elif defined(ESP32)
        // ESP32: Initialize Serial1 for LoRa module communication
        // Using GPIO16 (RX) and GPIO17 (TX) - more commonly available on ESP32 boards
        // GPIO9/GPIO10 are often connected to flash and may not work
        COMSerial.begin(57600, SERIAL_8N1, 16, 17); // RX=GPIO16, TX=GPIO17
        delay(100); // Give serial time to initialize

        ShowSerial.println("ESP32 Serial1 initialized on GPIO16(RX)/GPIO17(TX)");

        // ESP32 commonly uses built-in LED on pin 2
        led = 2;
    #endif

    ShowSerial.println("RF95 server test.");
    ShowSerial.println("Initializing LoRa module...");

    pinMode(led, OUTPUT);

    if (!rf95.init()) {
        ShowSerial.println("init failed");
        ShowSerial.println("Check: 1) Grove module powered? 2) RX/TX pins connected? 3) Grove firmware loaded?");
        while (1);
    }

    ShowSerial.println("LoRa module initialized successfully!");
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
    // you can set transmitter powers from 5 to 23 dBm:
    //rf95.setTxPower(13, false);

    rf95.setFrequency(434.0);
}

void loop() {
    if (rf95.available()) {
        // Should be a message for us now
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (rf95.recv(buf, &len)) {
            digitalWrite(led, HIGH);

            ShowSerial.print("got request: ");
            ShowSerial.println((char*)buf);

            // Send a reply
            uint8_t data[] = "And hello back to you";
            rf95.send(data, sizeof(data));
            rf95.waitPacketSent();
            ShowSerial.println("Sent a reply");

            digitalWrite(led, LOW);
        } else {
            ShowSerial.println("recv failed");
        }
    }
}


