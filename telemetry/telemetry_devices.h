/**
 * @file telemetry_devices.h
 * @author Nathaniel Asia
 * @brief 
 * @version 0.1
 * @date 2024-06-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef TELEMETRY_DEVICES_
#define TELEMETRY_DEVICES_

#include "led.h"
#include <stdint.h>


// Parent Class
class TelemetryDevice{
    private:
        // Radio
        LED statusLight;
    public:
        TelemetryDevice(){}
        void initStatusLight(uint8_t);
        void initRadio();
};

// Subclass (Remote Device)
class TelemetryRemoteDevice : public TelemetryDevice
{
private:
    
    //bluetooth object
public:
    TelemetryRemoteDevice() {}

};

// Subclass (OnboardDevice)
class TelemetryOnboardDevice : public TelemetryDevice
{
private:
    // can bus object
public:
    TelemetryOnboardDevice() {}
    void listenForCANMessage();
    void transmitCANMessage(/*RADIO CAN MESSAGE STRUCT*/); 
};



#endif