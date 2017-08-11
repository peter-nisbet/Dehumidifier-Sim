
#include <ajtcl/alljoyn.h>

/*
 * The value of the arguments are the indices of the
 * object path in AppObjects, interface in sampleInterfaces, and
 * member indices in the interface.
 * The 'cat' index is 1 because the first entry in sampleInterface is the interface name.
 * This makes the first index (index 0 of the methods) the second string in
 * sampleInterface[] which, for illustration purposes is a dummy entry.
 * The index of the method we implement for basic_service, 'cat', is 1 which is the third string
 * in the array of strings sampleInterface[].
 *
 * See also .\inc\aj_introspect.h
 */

#define AIRPURIFIER_CONTROLLEE_GET_PROPERTY_VALUE   				AJ_APP_MESSAGE_ID(0, 0, AJ_PROP_GET)
#define AIRPURIFIER_CONTROLLEE_SET_PROPERTY_VALUE   				AJ_APP_MESSAGE_ID(0, 0, AJ_PROP_SET)
#define AIRPURIFIER_CONTROLLEE_METHOD_POWER_ON					AJ_APP_MESSAGE_ID(0, 1, 0) 
#define AIRPURIFIER_CONTROLLEE_METHOD_POWER_OFF 				AJ_APP_MESSAGE_ID(0, 1, 1) 
#define AIRPURIFIER_CONTROLLEE_METHOD_RESET 					AJ_APP_MESSAGE_ID(0, 1, 2)
#define AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_ODOR_VALUE_CHANGED   		AJ_APP_MESSAGE_ID(0, 1, 12)
#define AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_DUST_VALUE_CHANGED   		AJ_APP_MESSAGE_ID(0, 1, 13)
#define AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_ALLERGEN_VALUE_CHANGED   		AJ_APP_MESSAGE_ID(0, 1, 14)
#define AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_CLEANMETALGRID_VALUE_CHANGED   	AJ_APP_MESSAGE_ID(0, 1, 15)
#define AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_REPLACEFILTER_VALUE_CHANGED   	AJ_APP_MESSAGE_ID(0, 1, 16)
#define AIRPURIFIER_CONTROLLEE_SIGNAL_SENSOR_CLEANMONITOR_VALUE_CHANGED   	AJ_APP_MESSAGE_ID(0, 1, 17)
#define AIRPURIFIER_CONTROLLEE_SIGNAL_POWER_STATUS_CHANGED   			AJ_APP_MESSAGE_ID(0, 1, 18)
#define AIRPURIFIER_CONTROLLEE_SIGNAL_SENSORS_CLEANMETALGRID_AND_REPLACEFILTER_VALUES_RESET	AJ_APP_MESSAGE_ID(0, 1, 19)
#define AIRPURIFIER_CONTROLLEE_SIGNAL_FLOW_VALUE_CHANGED			AJ_APP_MESSAGE_ID(0, 1, 20)
#define AIRPURIFIER_CONTROLLEE_SIGNAL_TIMER_VALUE_CHANGED			AJ_APP_MESSAGE_ID(0, 1, 21)

//EVENTS AND ACTIONS DEFINITIONS
#define EVENTSANDACTIONS_OBJECT_INDEX                           0

#define EVENTS_POWER_SET_TO_OFF_SLS                        	AJ_APP_MESSAGE_ID(EVENTSANDACTIONS_OBJECT_INDEX, 2, 1)
#define EVENTS_POWER_SET_TO_ON_SLS                        	AJ_APP_MESSAGE_ID(EVENTSANDACTIONS_OBJECT_INDEX, 2, 2)

#define ACTIONS_SET_POWER_TO_OFF                                AJ_APP_MESSAGE_ID(EVENTSANDACTIONS_OBJECT_INDEX, 3, 1)
#define ACTIONS_SET_POWER_TO_ON                                 AJ_APP_MESSAGE_ID(EVENTSANDACTIONS_OBJECT_INDEX, 3, 2)

//-------------REQUIRED METHODS TO IMPLEMENT

void ControlPanelComponents_RegisterAppObjects();

//-------------GET AND SET METHODS FOR PROPERTY VALUE

AJ_Status GetPropertyValue(AJ_Message* replyMsg, uint32_t propId, void* context);
AJ_Status SetPropertyValue(AJ_Message* replyMsg, uint32_t propId, void* context);

//-------------SEND SIGNAL METHOD FOR CHANGES IN PROPERTY VALUE

AJ_Status SendSignalPropertyValueChanged(AJ_BusAttachment* busAttachment, uint32_t propSignal, uint32_t sessionId);

//-------------SEND EVENTS METHOD

AJ_Status SendEvent(AJ_BusAttachment* busAttachment, uint32_t eventId);
