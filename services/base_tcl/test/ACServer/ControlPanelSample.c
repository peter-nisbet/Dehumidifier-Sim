/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
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

#include <ajtcl/services/ControlPanelService.h>
#include "ControlPanelProvided.h"
#include "ControlPanelGenerated.h"
#include "ControlPanelSample.h"
#include <ajtcl/aj_link_timeout.h>
#include <ajtcl/services/PropertyStore.h>
#include "ControlPanelComponents.h"
#include "ControlPanelProvided.h"
#include "AppHandlers.h"
//#include "CURLwebServiceRequest.h"

#ifndef NDEBUG
extern AJ_EXPORT uint8_t dbgAJSVCAPP;
#endif

/*static AJ_Object AJCPS_ObjectList[] = {
    AJCPS_CONTROLLEE_GENERATED_OBJECTS
    { NULL }
};

AJ_Status Controlee_Init()
{
    AJ_Status status = AJCPS_Start(AJCPS_ObjectList, &GeneratedMessageProcessor, &IdentifyMsgOrPropId, &IdentifyMsgOrPropIdForSignal, &IdentifyRootMsgOrPropId);
    WidgetsInit();
    return status;
}*/

void Controlee_DoWork(AJ_BusAttachment* busAttachment)
{
    //AJ_AlwaysPrintf(("\nControlPanelSample.c Controlee_DoWork()\n"));

    if(sendPowerStatusUpdate(busAttachment)){
       return;
    }

    uint8_t isAirPurifierPoweredON = getPowerStatus();
    if(!isAirPurifierPoweredON){
	return;
    }

    sendFlowValueUpdate(busAttachment);
    sendTimerValueUpdate(busAttachment);
    sendResetUpdate(busAttachment);
    sendSensorUpdates(busAttachment);

    return;
}

int sendPowerStatusUpdate(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    uint8_t sendPowerStatusUpdate = getPowerStatusUpdate();
    if(sendPowerStatusUpdate){

	status = SendSignalPropertyValueChanged(busAttachment, AIRPURIFIER_CONTROLLEE_SIGNAL_POWER_STATUS_CHANGED, getCurrentSessionId());

        if (status == AJ_OK) {
	    //MAKE SURE SENDING THIS SIGNAL ONLY ONCE
	    setPowerStatusUpdate(0);
        }
	return 1;
    }
    return 0;
}

int sendResetUpdate(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    uint8_t sendResetUpdate = getResetUpdate();
    if(sendResetUpdate){
	
	status = SendSignalPropertyValueChanged(busAttachment, AIRPURIFIER_CONTROLLEE_SIGNAL_SENSORS_CLEANMETALGRID_AND_REPLACEFILTER_VALUES_RESET, getCurrentSessionId());

        if (status == AJ_OK) {
	    //MAKE SURE SENDING THIS SIGNAL ONLY ONCE
	    setResetUpdate(0);
        }
	return 1;
    }
    return 0;
}

int sendFlowValueUpdate(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    uint8_t sendFlowValueUpdate = getFanSpeedValueUpdate();
    if(sendFlowValueUpdate){
	
	status = SendSignalPropertyValueChanged(busAttachment, AIRPURIFIER_CONTROLLEE_SIGNAL_FLOW_VALUE_CHANGED, getCurrentSessionId());

        if (status == AJ_OK) {
	    //MAKE SURE SENDING THIS SIGNAL ONLY ONCE
	    setFanSpeedValueUpdate(0);
        }
	return 1;
    }
    return 0;
}

int sendTimerValueUpdate(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    uint8_t sendTimerValueUpdate = getTimerValueUpdate();
    if(sendTimerValueUpdate){

	status = SendSignalPropertyValueChanged(busAttachment, AIRPURIFIER_CONTROLLEE_SIGNAL_TIMER_VALUE_CHANGED, getCurrentSessionId());

        if (status == AJ_OK) {
	    //MAKE SURE SENDING THIS SIGNAL ONLY ONCE
	    setTimerValueUpdate(0);
        }
	return 1;
    }
    return 0;
}

void sendSensorUpdates(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    uint8_t sendUpdates = simulateAPsensorsAndSendUpdates();
    if (sendUpdates > 0) {

        if ((sendUpdates & (1 << 0)) != 0) {
	    status = SendSignalPropertyValueChanged(busAttachment, AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_ODOR_VALUE_CHANGED, getCurrentSessionId());

            if (status == AJ_OK && getSensorOdorValue()) {
	        //UA_sendPushNotifToAlliOSDevices("\"AP: Sensor Odor activated.\"");
	    }
        }
        if ((sendUpdates & (1 << 1)) != 0) {
	    status = SendSignalPropertyValueChanged(busAttachment, AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_DUST_VALUE_CHANGED, getCurrentSessionId()); 
    
            if (status == AJ_OK && getSensorDustValue()) {
	        //UA_sendPushNotifToAlliOSDevices("\"AP: Sensor Dust activated.\"");
	    } 
        }
        if ((sendUpdates & (1 << 2)) != 0) {
	    status = SendSignalPropertyValueChanged(busAttachment, AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_ALLERGEN_VALUE_CHANGED, getCurrentSessionId());  

            if (status == AJ_OK && getSensorAllergenValue()) {
	        //UA_sendPushNotifToAlliOSDevices("\"AP: Sensor Allergen activated.\"");
	    }   
        }
	if ((sendUpdates & (1 << 3)) != 0) {
	    	status = SendSignalPropertyValueChanged(busAttachment, AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_CLEANMETALGRID_VALUE_CHANGED, getCurrentSessionId()); 
 
            	if (status == AJ_OK && getSensorCleanMetalGridValue()) {
	        	//UA_sendPushNotifToAlliOSDevices("\"AP: Sensor Clean Metal Grid activated.\"");
	    	}     
        }
        if ((sendUpdates & (1 << 4)) != 0) {
	    	status = SendSignalPropertyValueChanged(busAttachment, AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_REPLACEFILTER_VALUE_CHANGED, getCurrentSessionId());

            	if (status == AJ_OK && getSensorReplaceFilterValue()) {
	        	//UA_sendPushNotifToAlliOSDevices("\"AP: Sensor Replace Filter activated.\"");
	    	} 
        }	
        if ((sendUpdates & (1 << 5)) != 0) {
	    status = SendSignalPropertyValueChanged(busAttachment, AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_CLEANMONITOR_VALUE_CHANGED, getCurrentSessionId());

            if (status == AJ_OK) {
	   	switch(getSensorCleanMonitorValue())
	   	{
			case 0:
	   	    		//UA_sendPushNotifToAlliOSDevices("\"AP: Sensor Clean Monitor - [No pollution detected]\"");
		    		break;
			case 1:
	   	    		//UA_sendPushNotifToAlliOSDevices("\"AP: Sensor Clean Monitor - [Some pollution detected, 1st warning]\"");
		    		break;
			case 2:
	   	    		//UA_sendPushNotifToAlliOSDevices("\"AP: Sensor Clean Monitor - [Some pollution detected, 2nd warning]\"");
		    		break;
			case 3:
	   	    		//UA_sendPushNotifToAlliOSDevices("\"AP: Sensor Clean Monitor - [Severe pollution detected]\"");
		    		break;
	   	}
	    } 
        }
    }
}

AJ_Status Controlee_Finish(AJ_BusAttachment* busAttachment)
{
    return AJ_OK;
}
