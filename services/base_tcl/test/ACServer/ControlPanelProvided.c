/******************************************************************************
 * Copyright (c) 2013 - 2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

/**
 * Per-module definition of the current module for debug logging.  Must be defined
 * prior to first inclusion of aj_debug.h.
 * The corresponding flag dbgAJSVCAPP is defined in the containing sample app.
 */
#define AJ_MODULE AJSVCAPP
#include <ajtcl/aj_debug.h>

#include <ajtcl/services/Definitions.h>

//HEADERS FOR GENERATING A RANDOM NUMBER
#include <time.h>
#include <stdlib.h>

#ifndef NDEBUG
extern AJ_EXPORT uint8_t dbgAJSVCAPP;
#endif

#define kForceAllSensorsON			0

#define kSensorOdorValueUpdate 			0
#define kSensorDustValueUpdate 			1
#define kSensorAllergenValueUpdate 		2
#define kSensorCleanMetalGridValueUpdate 	3
#define kSensorReplaceFilterValueUpdate 	4
#define kSensorCleanMonitorValueUpdate 		5

static uint8_t powerStatus = 0; //set 0 or OFF as default POWER status

static uint16_t flowValue = 0; 	//set AUTO as default FLOW value
static uint16_t timerValue = 0; //set 1H default TIMER value

static uint8_t powerStatusUpdate = 0;
static uint8_t resetUpdate = 0;
static uint8_t flowValueUpdate = 0;
static uint8_t timerValueUpdate = 0;

static int16_t sensorCleanMonitorValue = -1;

//set 0 or LED NOT DISPLAYED as default sensors values
static uint8_t sensorOdorValue = 0;
static uint8_t sensorDustValue = 0;
static uint8_t sensorAllergenValue = 0;
static uint8_t sensorCleanMetalGridValue = 0;
static uint8_t sensorReplaceFilterValue = 0;

static uint8_t signalsToSend = 0;
static uint8_t eventsToSend = 0;
static int isSRandInitialized = 0;

static uint8_t powerStatusUpdateForEvent = 0;    //TEMPORARY VARIABLE

//-------------ON ACTION METHODS

void AppHandle_PowerON_AirPurifier()
{
    AJ_AlwaysPrintf(("\n**************** POWER ON AIR PURIFIER ****************\n"));

    powerStatus = 1;

    //SEND A SIGNAL TO EVERY CLIENT CONNECTED TO UPDATE CLIENT UI
    powerStatusUpdate = 1;
    powerStatusUpdateForEvent = 1;
}

void AppHandle_PowerOFF_AirPurifier()
{
    AJ_AlwaysPrintf(("\n**************** POWER OFF AIR PURIFIER ****************\n"));

    //SET DEFAULT VALUES
    powerStatus = 0;
    flowValue = 4;
    timerValue = 2;
    sensorCleanMonitorValue = -1;
    sensorOdorValue = 0;
    sensorDustValue = 0;
    sensorAllergenValue = 0;
    sensorCleanMetalGridValue = 0;
    sensorReplaceFilterValue = 0;
    signalsToSend = 0;

    //SEND A SIGNAL TO EVERY CLIENT CONNECTED TO UPDATE CLIENT UI
    powerStatusUpdate = 1;
    powerStatusUpdateForEvent = 1;
}

void AppHandle_ResetAirPurifier()
{
    AJ_AlwaysPrintf(("\n**************** RESET AIR PURIFIER ****************\n"));

    sensorCleanMetalGridValue = 0;
    sensorReplaceFilterValue = 0;

    //SEND A SIGNAL TO EVERY CLIENT CONNECTED TO UPDATE CLIENT UI
    resetUpdate = 1;
}

//-------------PROPERTIES GETTERS

uint8_t getPowerStatus()
{
    return powerStatus;
}

uint8_t getPowerStatusUpdate()
{
    return powerStatusUpdate;
}

uint8_t getResetUpdate()
{
    return resetUpdate;
}

uint8_t getFanSpeedValueUpdate()
{
    return flowValueUpdate;
}

uint8_t getTimerValueUpdate()
{
    return timerValueUpdate;
}

uint16_t getFanSpeedValue()
{
    return flowValue;
}

uint16_t getTimerValue()
{
    return timerValue;
}

int16_t getSensorCleanMonitorValue()
{
    return sensorCleanMonitorValue;
}

uint8_t getSensorOdorValue()
{
    return sensorOdorValue;
}

uint8_t getSensorDustValue()
{
    return sensorDustValue;
}

uint8_t getSensorAllergenValue()
{
    return sensorAllergenValue;
}

uint8_t getSensorCleanMetalGridValue()
{
    return sensorCleanMetalGridValue;
}

uint8_t getSensorReplaceFilterValue()
{
    return sensorReplaceFilterValue;
}

//-------------PROPERTIES SETTERS

void setPowerStatus(uint8_t p)
{
    powerStatus = p;
}

void setPowerStatusUpdate(uint8_t p)
{
    powerStatusUpdate = p;
}

void setResetUpdate(uint8_t r)
{
    resetUpdate = r;
}

void setFanSpeedValueUpdate(uint8_t f)
{
    flowValueUpdate = f;
}

void setTimerValueUpdate(uint8_t t)
{
    timerValueUpdate = t;
}

void setFanSpeedValue(uint16_t f)
{
    flowValue = f;
    flowValueUpdate = 1;
}

void setTimerValue(uint16_t t)
{
    timerValue = t;
    timerValueUpdate = 1;
}

//TEMPORARY METHOD
void setPowerStatusUpdateForEvent(uint8_t p)
{
    powerStatusUpdateForEvent = p;
}

//-------------signalsToSend variable SETTERS

void setSensorOdorValueUpdate() {
    signalsToSend |= 1 << 0;
}

void setSensorDustValueUpdate() {
    signalsToSend |= 1 << 1;
}

void setSensorAllergenValueUpdate() {
    signalsToSend |= 1 << 2;
}

void setSensorCleanMetalGridValueUpdate() {
    signalsToSend |= 1 << 3;
}

void setSensorReplaceFilterValueUpdate() {
    signalsToSend |= 1 << 4;
}

void setSensorCleanMonitorValueUpdate() {
    signalsToSend |= 1 << 5;
}

//-------------eventsToSend variable SETTERS

void setTurnedOffEvent() {
    eventsToSend |= 1 << 0;
}

void setTurnedOnEvent() {
    eventsToSend |= 1 << 1;
}

//-------------OTHER NECESSARY METHODS

uint8_t simulateAPsensorsAndSendUpdates()
{
    signalsToSend = 0;

    if(kForceAllSensorsON){
	sensorOdorValue = 1;
	sensorDustValue = 1;
	sensorAllergenValue = 1;
	sensorCleanMetalGridValue = 1;
	sensorReplaceFilterValue = 1;
	sensorCleanMonitorValue = 3;

	setSensorOdorValueUpdate();
	setSensorDustValueUpdate();
	setSensorAllergenValueUpdate();
	setSensorCleanMetalGridValueUpdate();
	setSensorReplaceFilterValueUpdate();
	setSensorCleanMonitorValueUpdate();
    }
    else{
     int randNum = generateRandomNumber(200);
     if(randNum < 40){
    	randNum = generateRandomNumber(6);
    	switch (randNum) {
    	case kSensorOdorValueUpdate:
           {
   		sensorOdorValue = !sensorOdorValue;
		setSensorOdorValueUpdate();
           }
           break;
    	case kSensorDustValueUpdate:
           {
   		sensorDustValue = !sensorDustValue;
		setSensorDustValueUpdate();
           }
           break;
    	case kSensorAllergenValueUpdate:
           {
   		sensorAllergenValue = !sensorAllergenValue;
		setSensorAllergenValueUpdate();
	   }
           break;
    	case kSensorCleanMetalGridValueUpdate:
           {
   		sensorCleanMetalGridValue = !sensorCleanMetalGridValue;
		setSensorCleanMetalGridValueUpdate();
	   }
           break;
	case kSensorReplaceFilterValueUpdate:
           {
   		sensorReplaceFilterValue = !sensorReplaceFilterValue;
		setSensorReplaceFilterValueUpdate();
	   }
           break;
	case kSensorCleanMonitorValueUpdate:
           {
		randNum = generateRandomNumber(2);
		if(randNum == 0){
		    if(sensorCleanMonitorValue < 3){
		        sensorCleanMonitorValue = sensorCleanMonitorValue + 1;
		        setSensorCleanMonitorValueUpdate();
     	   	    }
		}
		else if(sensorCleanMonitorValue > -1){
		    sensorCleanMonitorValue = sensorCleanMonitorValue - 1;
	      	    setSensorCleanMonitorValueUpdate();
     	   	}
   	   }
           break;
    	}
     }
    
    }

    return signalsToSend;
}

uint8_t checkForEventsToSend()
{
    eventsToSend = 0;
    // 0x01 == need to send event power turned off
    // 0x02 == need to send event power turned on

    if (powerStatusUpdateForEvent){
    	if (powerStatus == 0) {
            setTurnedOffEvent();
    	}
    	else{
            setTurnedOnEvent();
    	}    
    }

    return eventsToSend;
}

int generateRandomNumber(int maxNum)
{
    if(!isSRandInitialized){
   	srand(time(NULL));
	isSRandInitialized = 1;
    }

    int r = rand() % maxNum; //random between -1 and (maxNum - 1)
    return r;
}
