/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
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

#include <stdlib.h>

#include "ControlPanelClient.h"
#include <ajtcl/alljoyn.h>
#include "PropertyStoreOEMProvisioning.h"

#ifndef NDEBUG
AJ_EXPORT uint8_t dbgAJSVCAPP = 1;
#endif

#define CPSC_CONNECT_TIMEOUT     (1000 * 60)
#define CPSC_CONNECT_PAUSE       (1000 * 10)
#define CPSC_UNMARSHAL_TIMEOUT   (1000 * 5)
#define CPSC_SLEEP_TIME          (1000 * 2)

#ifndef COMBINED_SERVICES
AJ_BusAttachment busAttachment;
uint8_t isBusConnected = FALSE;
static char busAddress[] = "org.alljoyn.BusNode";
uint32_t sessionId = 0;

#define CONTROL_ANNOUNCE_SIGNAL_RECEIVED  AJ_PRX_MESSAGE_ID(0, 1, 3)

const char* deviceManufactureName = "COMPANY";
const char* deviceProductName = "GENERIC BOARD";

static const char DEFAULT_LANGUAGE[] = "en";
static const char* DEFAULT_LANGUAGES[] = { DEFAULT_LANGUAGE };
static const char SUPPORTED_LANG2[] = "de-AT";
static const char* SUPPORTED_LANGUAGES[] = { DEFAULT_LANGUAGE, SUPPORTED_LANG2, NULL };
const char* const* propertyStoreDefaultLanguages = SUPPORTED_LANGUAGES;

/**
 * property array of structure with defaults
 */
static const char* DEFAULT_APP_NAMES[] = { "Controller" };
static const char* DEFAULT_DEVICE_NAMES[] = { "CPSTESTER" };
static const char DEFAULT_DESCRIPTION_LANG1[] = "AC IOE device";
static const char DEFAULT_DESCRIPTION_LANG2[] = "Mein erstes IOE Geraet";
static const char* DEFAULT_DESCRIPTIONS[] = { DEFAULT_DESCRIPTION_LANG1, DEFAULT_DESCRIPTION_LANG2 };
static const char DEFAULT_MANUFACTURER_LANG1[] = "Company A(EN)";
static const char DEFAULT_MANUFACTURER_LANG2[] = "Firma A(DE-AT)";
static const char* DEFAULT_MANUFACTURERS[] = { DEFAULT_MANUFACTURER_LANG1, DEFAULT_MANUFACTURER_LANG2 };
static const char* DEFAULT_DEVICE_MODELS[] = { "0.0.1" };
static const char* DEFAULT_DATE_OF_MANUFACTURES[] = { "2014-02-01" };
static const char* DEFAULT_SOFTWARE_VERSIONS[] = { "0.0.1" };
static const char* DEFAULT_HARDWARE_VERSIONS[] = { "0.0.1" };
static const char DEFAULT_SUPPORT_URL_LANG1[] = "www.company_a.com";
static const char DEFAULT_SUPPORT_URL_LANG2[] = "www.company_a.com/de-AT";
static const char* DEFAULT_SUPPORT_URLS[] = { DEFAULT_SUPPORT_URL_LANG1, DEFAULT_SUPPORT_URL_LANG2 };

const char** propertyStoreDefaultValues[AJSVC_PROPERTY_STORE_NUMBER_OF_KEYS] =
{
// "Default Values per language",                    "Key Name"
    NULL,                                           /*DeviceId*/
    NULL,                                           /*AppId*/
    DEFAULT_DEVICE_NAMES,                           /*DeviceName*/
// Add other runtime keys above this line
    DEFAULT_LANGUAGES,                              /*DefaultLanguage*/
    DEFAULT_APP_NAMES,                              /*AppName*/
    DEFAULT_DESCRIPTIONS,                           /*Description*/
    DEFAULT_MANUFACTURERS,                          /*Manufacturer*/
    DEFAULT_DEVICE_MODELS,                          /*ModelNumber*/
    DEFAULT_DATE_OF_MANUFACTURES,                   /*DateOfManufacture*/
    DEFAULT_SOFTWARE_VERSIONS,                      /*SoftwareVersion*/
    NULL,                                           /*AJSoftwareVersion*/
// Add other mandatory keys above this line
    DEFAULT_HARDWARE_VERSIONS,                      /*HardwareVersion*/
    DEFAULT_SUPPORT_URLS,                           /*SupportUrl*/
// Add other about keys above this line
};

static char machineIdVar[MACHINE_ID_LENGTH + 1] = { 0 };
static char* machineIdVars[] = { machineIdVar };
static char deviceNameVar[DEVICE_NAME_VALUE_LENGTH + 1] = { 0 };
static char* deviceNameVars[] = { deviceNameVar };

PropertyStoreConfigEntry propertyStoreRuntimeValues[AJSVC_PROPERTY_STORE_NUMBER_OF_RUNTIME_KEYS] =
{
//  {"Buffers for Values per language", "Buffer Size"},                  "Key Name"
    { machineIdVars,             MACHINE_ID_LENGTH + 1 },               /*DeviceId*/
    { machineIdVars,             MACHINE_ID_LENGTH + 1 },               /*AppId*/
    { deviceNameVars,            DEVICE_NAME_VALUE_LENGTH + 1 },        /*DeviceName*/
// Add other runtime keys above this line
};

#include "AllJoynLogo.h"

const char* aboutIconMimetype = AJ_LogoMimeType;
const uint8_t* aboutIconContent = AJ_LogoData;
const size_t aboutIconContentSize = AJ_LogoSize;
const char* aboutIconUrl = AJ_LogoURL;

uint8_t isControlPanelAnnounce(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;
    AJ_Arg array1, array2, struct1;
    uint16_t uint1, uint2;
    char* buffer;

    status = AJ_UnmarshalArgs(msg, "q", &uint1);
    if (status != AJ_OK) {
        return FALSE;
    }

    status = AJ_UnmarshalArgs(msg, "q", &uint2);
    if (status != AJ_OK) {
        return FALSE;
    }
    status = AJ_UnmarshalContainer(msg, &array1, AJ_ARG_ARRAY);
    if (status != AJ_OK) {
        return FALSE;
    }
    do {
        status = AJ_UnmarshalContainer(msg, &struct1, AJ_ARG_STRUCT);
        if (status != AJ_OK) {
            return FALSE;
        }
        status = AJ_UnmarshalArgs(msg, "o", &buffer);
        if (status != AJ_OK) {
            return FALSE;
        }

        status = AJ_UnmarshalContainer(msg, &array2, AJ_ARG_ARRAY);
        if (status != AJ_OK) {
            return FALSE;
        }
        do {
            if ((status = AJ_UnmarshalArgs(msg, "s", &buffer)) != AJ_OK) {
                if (status == AJ_ERR_NO_MORE) {
                    break;
                } else {
                    return FALSE;
                }
            }
            if (strcmp(buffer, "org.alljoyn.ControlPanel.ControlPanel") == 0) {
                return TRUE;
            }
        } while (1);
        status = AJ_UnmarshalCloseContainer(msg, &array2);
        if (status != AJ_OK) {
            break;
        }
        status = AJ_UnmarshalCloseContainer(msg, &struct1);
        if (status != AJ_OK) {
            break;
        }
    } while (1);
    return FALSE;
}

static const char PWD[] = "ABCDEFGH";

uint32_t PasswordCallback(uint8_t* buffer, uint32_t bufLen)
{
    memcpy(buffer, PWD, sizeof(PWD));
    return sizeof(PWD) - 1;
}
#endif

/**
 * Static consts.
 */
static const uint16_t ServicePort = 1000;
uint32_t CPSsessionId = 0;
/**
 * Static non consts.
 */

int runningTestNum = 0;
int lastTestRun = -1;
static uint16_t numTests = NUMBER_OF_TESTS;

/**
 * Define Externs
 */
static CPSTest testsToRun[NUMBER_OF_TESTS];


char* CPSAnnounceMatch = "sessionless='t',interface='org.alljoyn.ControlPanel.Announcement'";

static AJ_Object controllerObjectList[] =
{
    AJCPS_CONTROLLEE_GENERATED_OBJECTS
    { NULL }
};

/**
 * Objects implemented by the application. The first member in the AJ_Object structure is the path.
 * The second is the collection of all interfaces at that path.
 */
void CPS_Init()
{
    AJ_Object* controllerObject = controllerObjectList;

    while (controllerObject->path) {
        if (controllerObject->flags & (AJ_OBJ_FLAG_ANNOUNCED | AJ_OBJ_FLAG_IS_PROXY)) {
            controllerObject->flags &= ~(AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED);
        }
        ++controllerObject;
    }
    AJ_RegisterObjectList(controllerObjectList, AJCPS_OBJECT_LIST_INDEX);
    TestsInit(testsToRun);
}


AJ_Status CPS_StartService(AJ_BusAttachment* bus, const char* busAddress, uint32_t timeout, uint8_t connected)
{
    AJ_Status status = AJ_OK;
    while (TRUE) {
        AJ_InfoPrintf(("Attempting to connect to bus '%s'\n", busAddress));

        status = AJ_Connect(bus, busAddress, timeout);
        if (status != AJ_OK) {
            AJ_ErrPrintf(("Failed to connect to bus '%s', sleeping for %d seconds...\n", busAddress, CPSC_CONNECT_PAUSE / 1000));
            AJ_Sleep(CPSC_CONNECT_PAUSE);
            continue;
        }

        AJ_InfoPrintf(("Connected successfully\n"));
        isBusConnected = TRUE;

        status = AJ_BusSetSignalRule(bus, CPSAnnounceMatch, AJ_BUS_SIGNAL_ALLOW);
        if (status != AJ_OK) {
            AJ_ErrPrintf(("Could not set Announcement Interface AddMatch\n"));
            return status;
        }
        break;
    }
    ;
    return status;
}

void CPS_IdleConnectedHandler(AJ_BusAttachment*bus)
{
    AJ_Status status;
    AJ_Message msg;
    uint16_t numParam;

    if (runningTestNum == lastTestRun || CPSsessionId == 0) {
        return;
    }

    if (runningTestNum == numTests) {
        runningTestNum = 0;
        AJ_InfoPrintf(("Finished running all the tests\n"));
        exit(0);
    }

    status = AJ_MarshalMethodCall(&busAttachment, &msg, testsToRun[runningTestNum].msgId,
                                  announceSender, CPSsessionId, 0, CPSC_CONNECT_TIMEOUT);

    for (numParam = 0; numParam < testsToRun[runningTestNum].numParams; numParam++) {
        if (status == AJ_OK) {
            status = AJ_MarshalArgs(&msg, "q", testsToRun[runningTestNum].param[numParam]);
        }
    }

    if (status == AJ_OK) {
        status = AJ_DeliverMsg(&msg);
        lastTestRun = runningTestNum;
    }

    AJ_InfoPrintf(("MakeMethodCall() test number %i resulted in a status of %s.\n", lastTestRun + 1, AJ_StatusText(status)));
}

AJSVC_ServiceStatus CPS_NotifySessionAccepted(uint32_t sessionId, const char* sender)
{
    char dot = '.';
    uint16_t i;

    for (i = 0; i < strlen(sender); i++) {
        if (*(sender + i) == announceSender[i]) {
            if (*(sender + i) == dot) {
                CPSsessionId = sessionId;
                AJ_InfoPrintf(("Control Panel Service Connected Successfully to announce.\n"));
                return AJSVC_SERVICE_STATUS_HANDLED;
            }
            continue;
        }
        break;
    }
    AJ_ErrPrintf(("Could not connect.\n"));
    CPSsessionId = 0;
    return AJSVC_SERVICE_STATUS_NOT_HANDLED;
}

AJSVC_ServiceStatus CPS_MessageProcessor(AJ_BusAttachment* bus, AJ_Message* msg, AJ_Status*msgStatus)
{
    AJSVC_ServiceStatus service_Status = AJSVC_SERVICE_STATUS_HANDLED;
    switch (msg->msgId) {
    case CONTROL_ANNOUNCE_SIGNAL_RECEIVED:
        AJ_InfoPrintf(("Received Announce Signal from %s.\n", msg->sender));
        if (CPSsessionId == 0) {
            AJ_SessionOpts sessionOpts = {
                AJ_SESSION_TRAFFIC_MESSAGES,
                AJ_SESSION_PROXIMITY_ANY,
                AJ_TRANSPORT_ANY,
                TRUE
            };

            if (!isControlPanelAnnounce(msg)) {
                break;
            }
            strcpy(announceSender, msg->sender);

            AJ_BusJoinSession(bus, msg->sender, ServicePort, &sessionOpts);
        }
        break;

        ALL_REPLY_CASES
            runningTestNum++;
        CPS_IdleConnectedHandler(bus);
        break;

    case AJ_SIGNAL_SESSION_LOST:
        CPSsessionId = 0;
        break;

    default:
        service_Status = AJSVC_SERVICE_STATUS_NOT_HANDLED;
        break;
    }
    return service_Status;
}

void CPS_Finish()
{
    return;
}

#ifndef COMBINED_SERVICES
int AJ_Main(void)
{
    AJ_Status status = AJ_OK;

    /* Required: Need to initialize once before starting */
    AJ_Initialize();

    CPS_Init();

    while (TRUE) {

        /* AllJoyn related section */
        AJ_Message msg;

        if (!isBusConnected) {
            status = CPS_StartService(&busAttachment, busAddress, CPSC_CONNECT_TIMEOUT, isBusConnected);
        }

        status = AJ_UnmarshalMsg(&busAttachment, &msg, CPSC_UNMARSHAL_TIMEOUT);

        if (AJ_ERR_TIMEOUT == status) { // nothing on bus, do our own thing
            CPS_IdleConnectedHandler(&busAttachment);
            continue;
        }

        if (AJ_OK == status) {
            if  (msg.msgId == AJ_REPLY_ID(AJ_METHOD_JOIN_SESSION)) {
                if (msg.hdr->msgType == AJ_MSG_ERROR) {
                    AJ_ErrPrintf(("Could not connect session.\n"));
                } else {
                    uint32_t replyCode;

                    AJ_UnmarshalArgs(&msg, "uu", &replyCode, &sessionId);
                    if (replyCode == AJ_JOINSESSION_REPLY_SUCCESS) {
                        CPS_NotifySessionAccepted(sessionId, msg.sender);
                    }
                }
            } else {
                if (CPS_MessageProcessor(&busAttachment, &msg, &status) == AJSVC_SERVICE_STATUS_NOT_HANDLED) {
                    //pass to build in
                    status = AJ_BusHandleBusMessage(&msg);
                }
            }
        }

        /* Messages MUST be discarded to free resources. */
        AJ_CloseMsg(&msg);

        if (status == AJ_ERR_READ) {
            AJ_ErrPrintf(("AllJoyn disconnect.\n"));
            AJ_Disconnect(&busAttachment);
            isBusConnected = FALSE;

            /* Sleep a little while before trying to reconnect. */
            AJ_Sleep(CPSC_SLEEP_TIME);
        }
    }
    AJ_InfoPrintf(("Control Panel Sample exiting with status 0x%04x.\n", status));
    return status;
}


#ifdef AJ_MAIN
int main()
{
    return AJ_Main();
}
#endif
#endif

