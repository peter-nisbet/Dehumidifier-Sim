
#define AJ_MODULE AJSVCAPP
#include <ajtcl/aj_debug.h>

#include <ajtcl/alljoyn.h>
#include "AppHandlers.h"
#include "ControlPanelComponents.h"
#include "ControlPanelProvided.h"

#include <ajtcl/services/ServicesCommon.h>
#include <ajtcl/services/ServicesHandlers.h>
#include <ajtcl/services/PropertyStore.h>

#include "PropertyStoreOEMProvisioning.h"

#include <ajtcl/aj_config.h>
#include <ajtcl/aj_link_timeout.h>

#ifndef NDEBUG
extern AJ_EXPORT uint8_t dbgAJSVCAPP;
#endif

static const char ServicePath_ControlPanelAirPurifier[] = "/ControlPanel";

/**
 * The interface name followed by the method signatures.
 *
 * See also .\inc\aj_introspect.h
 */
static const char* const interfaceAirPurifier[] = {
    "org.Danby.ControlPanel.Dehumidifier", 				/* The first entry is the interface name. */
    "?PowerON", 							/* Method at index 0. */           
    "?PowerOFF", 							/* Method at index 1. */           
    "?Reset", 								/* Method at index 2. */
    "@powerStatus=i",                  					/* Read/write property at index 3. */
    "@fanSpeedValue=i",                  				/* Read/write property at index 4. */
    "@timerValue=i",                  					/* Read/write property at index 5. */
    "@sensorOdorValue>i",                  				/* Read only property at index 6. */
    "@sensorDustValue>i",                  				/* Read only property at index 7. */
    "@sensorAllergenValue>i",                  				/* Read only property at index 8. */
    "@sensorCleanMetalGridValue>i",                  			/* Read only property at index 9. */
    "@sensorReplaceFilterValue>i",                  			/* Read only property at index 10. */
    "@sensorCleanMonitorValue>i",                  			/* Read only property at index 11. */
    "!sensorOdorValueChanged newSensorOdorValue>i",   			/* Signal at index 12 with one output string*/
    "!sensorDustValueChanged newSensorDustValue>i",   			/* Signal at index 13 with one output string*/
    "!sensorAllergenValueChanged newSensorAllergenValue>i",   		/* Signal at index 14 with one output string*/
    "!sensorCleanMetalGridValueChanged newSensorCleanMetalGridValue>i", /* Signal at index 15 with one output string*/
    "!sensorReplaceFilterValueChanged newSensorReplaceFilterValue>i",   /* Signal at index 16 with one output string*/
    "!sensorCleanMonitorValueChanged newSensorCleanMonitorValue>i",   	/* Signal at index 17 with one output string*/
    "!powerStatusChanged newPowerStatus>i",   				/* Signal at index 18 with one output string*/
    "!sensorsCleanMetalGridAndReplaceFilterValuesReset",   		/* Signal at index 19 with one output string*/
    "!fanSpeedValueChanged newFanSpeedValue>i",   			/* Signal at index 20 with one output string*/ 
    "!timerValueChanged newTimerValue>i",   				/* Signal at index 21 with one output string*/
    NULL
};


//EVENTS AND ACTIONS INTERFACES

static const char EventsInterfaceName[] = "org.heavenfresh.AirPurifier.Events";

static const char* EventsInterface[] =
{
    EventsInterfaceName,         /* The first entry is the interface name. */
    "@Version>q",                /* The Version Property */
    "!&PowerSetToOff",
    "!&PowerSetToOn",
    NULL
};

static const char ActionsInterfaceName[] = "org.heavenfresh.AirPurifier.Actions";

static const char* ActionsInterface[] =
{
    ActionsInterfaceName,         /* The first entry is the interface name. */
    "@Version>q",                 /* The Version Property */
    "?SetPowerToOff",
    "?SetPowerToOn",
    NULL
};

//EVENTS AND ACTIONS DEFINITIONS

/*//PROPERTIES DEFINITIONS
#define EVENTSANDACTIONS_GET_PROP                               AJ_APP_MESSAGE_ID(EVENTSANDACTIONS_OBJECT_INDEX, 0, AJ_PROP_GET)
#define EVENTSANDACTIONS_SET_PROP                               AJ_APP_MESSAGE_ID(EVENTSANDACTIONS_OBJECT_INDEX, 0, AJ_PROP_SET)

//EVENTS AND ACTIONS VERSIONS
#define EVENTS_VERSION_PROP                                     AJ_APP_PROPERTY_ID(EVENTSANDACTIONS_OBJECT_INDEX, 1, 0)
#define ACTIONS_VERSION_PROP                                    AJ_APP_PROPERTY_ID(EVENTSANDACTIONS_OBJECT_INDEX, 2, 0)*/

#define EVENTSANDACTIONS_OBJECT_ID                              EVENTSANDACTIONS_OBJECT_INDEX
#define EVENTSANDACTIONS_OBJECT_DESC                            AJ_DESCRIPTION_ID(EVENTSANDACTIONS_OBJECT_ID, 0, 0, 0)

//EVENTS DESCRIPTION DEFINITIONS
#define EVENTS_INTERFACE_DESC                                   AJ_DESCRIPTION_ID(EVENTSANDACTIONS_OBJECT_ID, 3, 0, 0)
#define EVENTS_POWER_SET_TO_OFF_SLS_DESC                        AJ_DESCRIPTION_ID(EVENTSANDACTIONS_OBJECT_ID, 3, 2, 0)
#define EVENTS_POWER_SET_TO_ON_SLS_DESC                         AJ_DESCRIPTION_ID(EVENTSANDACTIONS_OBJECT_ID, 3, 3, 0)

//ACTIONS DESCRIPTION DEFINITIONS
#define ACTIONS_INTERFACE_DESC                                  AJ_DESCRIPTION_ID(EVENTSANDACTIONS_OBJECT_ID, 4, 0, 0)
#define ACTIONS_SET_POWER_TO_OFF_DESC                           AJ_DESCRIPTION_ID(EVENTSANDACTIONS_OBJECT_ID, 4, 2, 0)
#define ACTIONS_SET_POWER_TO_ON_DESC                            AJ_DESCRIPTION_ID(EVENTSANDACTIONS_OBJECT_ID, 4, 3, 0)

static const char* const languages[] = { "en", "es" };
#define DESCRIPTION_LENGTH 64
char description[DESCRIPTION_LENGTH] = "";
static const char* DescriptionLookup(uint32_t descId, const char* lang)
{
    printf("\nControlPanelComponents.c DescriptionLookup\n");

    const char* actualLanguage;
    int8_t langIndex = AJSVC_PropertyStore_GetLanguageIndex(lang);
    char deviceName[DEVICE_NAME_VALUE_LENGTH] = { '\0' };
    uint8_t i;
    const char* deviceNamePerLanguage;

    /*AJ_InfoPrintf(("Looking up description for o:%u i:%u m:%u a:%u", (descId >> 24) & 0xFF, (descId >> 16) & 0xFF, (descId >> 8) & 0xFF, (descId >> 0) & 0xFF));printf("

    if (langIndex != AJSVC_PROPERTY_STORE_ERROR_LANGUAGE_INDEX) {
        actualLanguage = AJSVC_PropertyStore_GetLanguageName(langIndex);

        AJ_InfoPrintf((" language=%s\n", actualLanguage));

        deviceNamePerLanguage = AJSVC_PropertyStore_GetValueForLang(AJSVC_PROPERTY_STORE_DEVICE_NAME, langIndex);
        if (deviceNamePerLanguage == NULL) {
            deviceNamePerLanguage = AJSVC_PropertyStore_GetValueForLang(AJSVC_PROPERTY_STORE_DEVICE_NAME, AJSVC_PropertyStore_GetCurrentDefaultLanguageIndex());
            if (deviceNamePerLanguage == NULL) {
                AJ_ErrPrintf(("DeviceName for language=%s does not exist!\n", actualLanguage));
            }
        }
        if (deviceNamePerLanguage != NULL) {
            strncpy(deviceName, deviceNamePerLanguage, DEVICE_NAME_VALUE_LENGTH);
        }
        for (i = 0; i < DEVICE_NAME_VALUE_LENGTH; i++) { // Replace any illegal/escaped XML characters with '_'
            if (deviceName[i] == '>' || deviceName[i] == '<' || deviceName[i] == '"' || deviceName[i] == '\'' || deviceName[i] == '&') {
                deviceName[i] = '_';
            }
        }*/

        switch (descId) {
        case (EVENTSANDACTIONS_OBJECT_DESC):
	    return "EVENTSANDACTIONS_OBJECT_DESC";

            //snprintf(description, DESCRIPTION_LENGTH, "%s Events and Actions [%s]", deviceName, actualLanguage);
            //return description;

        case (EVENTS_INTERFACE_DESC):
	    return "EVENTS_INTERFACE_DESC";

            //snprintf(description, DESCRIPTION_LENGTH, "%s Events [%s]", deviceName, actualLanguage);
            //return description;

        case (EVENTS_POWER_SET_TO_OFF_SLS_DESC):
            return "Triggerred when the Air Purifier is turned OFF";

        case (EVENTS_POWER_SET_TO_ON_SLS_DESC):
            return "Triggerred when the Air Purifier is turned ON";

        case (ACTIONS_INTERFACE_DESC):
	    return "ACTIONS_INTERFACE_DESC)";

            //snprintf(description, DESCRIPTION_LENGTH, "%s Actions [%s]", deviceName, actualLanguage);
            //return description;

        case (ACTIONS_SET_POWER_TO_OFF_DESC):
            return "Set Power to OFF";

        case (ACTIONS_SET_POWER_TO_ON_DESC):
            return "Set Power to ON";

        default:
            return NULL;
        }
    /*}
    AJ_WarnPrintf(("\nError: Unsupported language=%s\n", lang == NULL ? "NULL" : lang));

    return NULL;*/
}


/**
 * A NULL terminated collection of all interfaces.
 */
static const AJ_InterfaceDescription interfacesCollection[] = {
    AJ_PropertiesIface,     /* This must be included for any interface that has properties. */
    interfaceAirPurifier,
    EventsInterface,
    ActionsInterface,
    NULL
};

/**
 * Objects implemented by the application. The first member in the AJ_Object structure is the path.
 * The second is the collection of all interfaces at that path.
 */
static const AJ_Object AppObjects[] = {
    { ServicePath_ControlPanelAirPurifier, interfacesCollection, AJ_OBJ_FLAG_ANNOUNCED },
    { NULL }
};

/*
 * Property identifiers for the properies this application implements
 * Encode a property id from the object path, interface, and member indices.
 */
#define AIRPURIFIER_CONTROLLEE_POWER_STATUS_ID    			AJ_APP_PROPERTY_ID(0, 1, 3)
#define AIRPURIFIER_CONTROLLEE_FLOW_VALUE_ID    			AJ_APP_PROPERTY_ID(0, 1, 4)
#define AIRPURIFIER_CONTROLLEE_TIMER_VALUE_ID    			AJ_APP_PROPERTY_ID(0, 1, 5)
#define AIRPURIFIER_CONTROLLEE_SENSOR_ODOR_VALUE_ID    			AJ_APP_PROPERTY_ID(0, 1, 6)
#define AIRPURIFIER_CONTROLLEE_SENSOR_DUST_VALUE_ID    			AJ_APP_PROPERTY_ID(0, 1, 7)
#define AIRPURIFIER_CONTROLLEE_SENSOR_ALLERGEN_VALUE_ID    		AJ_APP_PROPERTY_ID(0, 1, 8)
#define AIRPURIFIER_CONTROLLEE_SENSOR_CLEANMETALGRID_VALUE_ID    	AJ_APP_PROPERTY_ID(0, 1, 9)
#define AIRPURIFIER_CONTROLLEE_SENSOR_REPLACEFILTER_VALUE_ID    	AJ_APP_PROPERTY_ID(0, 1, 10)
#define AIRPURIFIER_CONTROLLEE_SENSOR_CLEANMONITOR_VALUE_ID    		AJ_APP_PROPERTY_ID(0, 1, 11)

void ControlPanelComponents_RegisterAppObjects()
{
    //AJ_RegisterObjects(AppObjects, NULL);
    
    //AppObjects[1] = getEventsAndActionsObject();

    AJ_RegisterDescriptionLanguages(languages);
    AJ_RegisterObjectListWithDescriptions(AppObjects, 1, DescriptionLookup);
}

AJ_Status GetPropertyValue(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    AJ_Status status = AJ_ERR_UNEXPECTED;
    
    switch (propId) {
   	case AIRPURIFIER_CONTROLLEE_POWER_STATUS_ID:
	{
  	     printf("\n*****GetPropertyValue AIRPURIFIER_CONTROLLEE_POWER_STATUS_ID*****\n");

	     status = AJ_MarshalArgs(replyMsg, "i", getPowerStatus());
             status = AJ_OK;
	}
	break;

 	case AIRPURIFIER_CONTROLLEE_FLOW_VALUE_ID:
	{
  	     printf("\n*****GetPropertyValue AIRPURIFIER_CONTROLLEE_FLOW_VALUE_ID*****\n");

	     status = AJ_MarshalArgs(replyMsg, "i", getFanSpeedValue());
             status = AJ_OK;
	}
	break;
	
  	case AIRPURIFIER_CONTROLLEE_TIMER_VALUE_ID:
	{
  	     printf("\n*****GetPropertyValue AIRPURIFIER_CONTROLLEE_TIMER_VALUE_ID*****\n");

	     status = AJ_MarshalArgs(replyMsg, "i", getTimerValue());
             status = AJ_OK;
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SENSOR_ODOR_VALUE_ID:
	{
  	     printf("\n*****GetPropertyValue AIRPURIFIER_CONTROLLEE_SENSOR_ODOR_VALUE_ID*****\n");

	     status = AJ_MarshalArgs(replyMsg, "i", getSensorOdorValue());
             status = AJ_OK;
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SENSOR_DUST_VALUE_ID:
	{
  	     printf("\n*****GetPropertyValue AIRPURIFIER_CONTROLLEE_SENSOR_DUST_VALUE_ID*****\n");

	     status = AJ_MarshalArgs(replyMsg, "i", getSensorDustValue());
             status = AJ_OK;
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SENSOR_ALLERGEN_VALUE_ID:
	{
  	     printf("\n*****GetPropertyValue AIRPURIFIER_CONTROLLEE_SENSOR_ALLERGEN_VALUE_ID*****\n");

       	     status = AJ_MarshalArgs(replyMsg, "i", getSensorAllergenValue());
             status = AJ_OK;
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SENSOR_CLEANMETALGRID_VALUE_ID:
	{
  	     printf("\n*****GetPropertyValue AIRPURIFIER_CONTROLLEE_SENSOR_CLEANMETALGRID_VALUE_ID*****\n");

	     status = AJ_MarshalArgs(replyMsg, "i", getSensorCleanMetalGridValue());
             status = AJ_OK;
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SENSOR_REPLACEFILTER_VALUE_ID:
	{
  	     printf("\n*****GetPropertyValue AIRPURIFIER_CONTROLLEE_SENSOR_REPLACEFILTER_VALUE_ID*****\n");

	     status = AJ_MarshalArgs(replyMsg, "i", getSensorReplaceFilterValue());
             status = AJ_OK;
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SENSOR_CLEANMONITOR_VALUE_ID:
	{
  	     printf("\n*****GetPropertyValue AIRPURIFIER_CONTROLLEE_SENSOR_CLEANMONITOR_VALUE_ID*****\n");

	     status = AJ_MarshalArgs(replyMsg, "i", getSensorCleanMonitorValue());
             status = AJ_OK;
	}
	break;
    }

    return status;
}

AJ_Status SetPropertyValue(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    AJ_Status status = AJ_ERR_UNEXPECTED;
    
    switch (propId) {
   	case AIRPURIFIER_CONTROLLEE_POWER_STATUS_ID:
	{
	     uint16_t* integer;
             AJ_UnmarshalArgs(replyMsg, "i", &integer);
	     setPowerStatus(integer);
             status = AJ_OK;

  	     printf("\n*****SetPropertyValue powerStatus:%i|*****\n", getPowerStatus());
	}
	break;

 	case AIRPURIFIER_CONTROLLEE_FLOW_VALUE_ID:
	{
	     uint16_t* integer;
             AJ_UnmarshalArgs(replyMsg, "i", &integer);
	     setFanSpeedValue(integer);
             status = AJ_OK;

  	     printf("\n*****SetPropertyValue fanSpeedValue:%i|*****\n", getFanSpeedValue());
	}
	break;
	
  	case AIRPURIFIER_CONTROLLEE_TIMER_VALUE_ID:
	{
	     uint16_t* integer;
             AJ_UnmarshalArgs(replyMsg, "i", &integer);
	     setTimerValue(integer);
             status = AJ_OK;
	
	     printf("\n*****SetPropertyValue timerValue:%i|*****\n", getTimerValue());
	}
	break;
    }

    return status;
}

//*****SEND SIGNAL METHOD FOR SENSORS

AJ_Status SendSignalPropertyValueChanged(AJ_BusAttachment* busAttachment, uint32_t propSignal, uint32_t sessionId)
{
    AJ_Status status = AJ_ERR_UNEXPECTED;
    AJ_Message msg;

    status = AJ_MarshalSignal(busAttachment, &msg, propSignal, NULL, sessionId, 0, 0);
    if (status != AJ_OK) {
        return status;
    }

    switch (propSignal) {
  	case AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_ODOR_VALUE_CHANGED:
	{
  	     printf("\n*****SendSignalPropertyValueChanged AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_ODOR_VALUE_CHANGED*****\n");

       	     status = AJ_MarshalArgs(&msg, "i", getSensorOdorValue());
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_DUST_VALUE_CHANGED:
	{
  	     printf("\n*****SendSignalPropertyValueChanged AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_DUST_VALUE_CHANGED*****\n");

       	     status = AJ_MarshalArgs(&msg, "i", getSensorDustValue());
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_ALLERGEN_VALUE_CHANGED:
	{
  	     printf("\n*****SendSignalPropertyValueChanged AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_ALLERGEN_VALUE_CHANGED*****\n");

       	     status = AJ_MarshalArgs(&msg, "i", getSensorAllergenValue());
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_CLEANMETALGRID_VALUE_CHANGED:
	{
  	     printf("\n*****SendSignalPropertyValueChanged AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_CLEANMETALGRID_VALUE_CHANGED*****\n");

       	     status = AJ_MarshalArgs(&msg, "i", getSensorCleanMetalGridValue());
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_REPLACEFILTER_VALUE_CHANGED:
	{
  	     printf("\n*****SendSignalPropertyValueChanged AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_REPLACEFILTER_VALUE_CHANGED*****\n");

       	     status = AJ_MarshalArgs(&msg, "i", getSensorReplaceFilterValue());
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_CLEANMONITOR_VALUE_CHANGED:
	{
  	     printf("\n*****SendSignalPropertyValueChanged AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_CLEANMONITOR_VALUE_CHANGED*****\n");

       	     status = AJ_MarshalArgs(&msg, "i", getSensorCleanMonitorValue());
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SIGNAL_POWER_STATUS_CHANGED:
	{
  	     printf("\n*****SendSignalPropertyValueChanged AIRPURIFIER_CONTROLLEE_SIGNAL_POWER_STATUS_CHANGED*****\n");

       	     status = AJ_MarshalArgs(&msg, "i", getPowerStatus());
	}
	break;

  	case AIRPURIFIER_CONTROLLEE_SIGNAL_FLOW_VALUE_CHANGED:
	{
  	     printf("\n*****SendSignalPropertyValueChanged AIRPURIFIER_CONTROLLEE_SIGNAL_FLOW_VALUE_CHANGED*****\n");
       	     status = AJ_MarshalArgs(&msg, "i", getFanSpeedValue());
	}

  	case AIRPURIFIER_CONTROLLEE_SIGNAL_TIMER_VALUE_CHANGED:
	{
  	     printf("\n*****SendSignalPropertyValueChanged AIRPURIFIER_CONTROLLEE_SIGNAL_TIMER_VALUE_CHANGED*****\n");

       	     status = AJ_MarshalArgs(&msg, "i", getTimerValue());
	}

  	case AIRPURIFIER_CONTROLLEE_SIGNAL_SENSORS_CLEANMETALGRID_AND_REPLACEFILTER_VALUES_RESET:
	{
  	     printf("\n*****SendSignalPropertyValueChanged AIRPURIFIER_CONTROLLEE_SIGNAL_SENSORS_CLEANMETALGRID_AND_REPLACEFILTER_VALUES_RESET*****\n");

       	     status = AJ_MarshalArgs(&msg, "", NULL);
	}

	break;
    }

    if (status != AJ_OK) {
        return status;
    }

    return AJ_DeliverMsg(&msg);
}

//*****EVENTS AND ACTIONS METHOD

AJ_Status SendEvent(AJ_BusAttachment* busAttachment, uint32_t eventId)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;

    status = AJ_MarshalSignal(busAttachment, &msg, eventId, NULL, 0, ALLJOYN_FLAG_SESSIONLESS, 0);
    if (status != AJ_OK) {
        goto ErrorExit;
    }
    status = AJ_DeliverMsg(&msg);
    if (status != AJ_OK) {
        goto ErrorExit;
    }
    status = AJ_CloseMsg(&msg);
    if (status != AJ_OK) {
        goto ErrorExit;
    }
    AJ_AlwaysPrintf(("Event sent successfully\n"));
    return status;

ErrorExit:

    AJ_AlwaysPrintf(("Event sending failed with status=%s\n", AJ_StatusText(status)));
    return status;
}

