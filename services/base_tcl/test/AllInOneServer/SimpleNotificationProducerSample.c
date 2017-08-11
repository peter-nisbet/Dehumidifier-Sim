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
#include <ajtcl/aj_crypto.h>

#include <NotificationProducerSample.h>
#include <ajtcl/services/NotificationCommon.h>
#include <ajtcl/services/NotificationProducer.h>

#if (defined(__linux) || defined(_WIN32) || defined(__MACH__))
#include "NotificationProducerSampleUtil.h"
#else
#define Producer_GetNotificationFromUser(...) do { } while (0)
#define Producer_SetupEnv(...) do { } while (0)
#define Producer_GetShouldDeleteNotificationFromUser(...) do { } while (0)
#define Producer_FreeNotification(...) do { } while (0)
#define Producer_ReadConfigFromEnv(...) do { } while (0)
#endif

#ifndef NDEBUG
extern AJ_EXPORT uint8_t dbgAJSVCAPP;
#endif

/**
 * Static consts - sample application specific
 */
const static char* lang1  = "en";
const static char* lang2 = "de-AT";
const static char* hello1 = "Hello AJL World";
const static char* hello2 = "Hallo AJL Welt";
const static char* onKey = "On";
const static char* offKey = "Off";
const static char* HelloVal = "Hello";
const static char* GoodbyeVal = "Goodbye";
const static char* Audio1URL = "http://www.getAudio1.org";
const static char* Audio2URL = "http://www.getAudio2.org";
const static char* Icon1URL = "http://www.getIcon1.org";
const static char* controlPanelServiceObjectPath = "/ControlPanel/MyDevice/areYouSure";
const static char* richIconObjectPath = "/icon/MyDevice";
const static char* richAudioObjectPath = "/audio/MyDevice";

#define NUM_TEXTS   2
static AJNS_DictionaryEntry textToSend[NUM_TEXTS];

#define NUM_CUSTOMS 2
static AJNS_DictionaryEntry customAttributesToSend[NUM_CUSTOMS];

#define NUM_RICH_AUDIO 2
static AJNS_DictionaryEntry richAudioUrls[NUM_RICH_AUDIO];

static uint8_t inputMode = 0;
static AJ_Time isMessageTime;
static uint8_t producerEnabled = 1;

#ifndef MESSAGES_INTERVAL
#define MESSAGES_INTERVAL 60000
#endif
#ifndef MESSAGES_INTERVAL_OFFSET
#define MESSAGES_INTERVAL_OFFSET 0
#endif

static uint32_t messagesInterval = MESSAGES_INTERVAL;
static uint32_t nextMessageTime = MESSAGES_INTERVAL;
static uint32_t initialIntervalOffset = MESSAGES_INTERVAL_OFFSET;

typedef enum _PriorityType {
    PRIORITY_TYPE_FIXED = 0,
    PRIORITY_TYPE_RANDOM = 1,
} PriorityType;
#ifndef MESSAGES_PRIORITY_TYPE
#define MESSAGES_PRIORITY_TYPE PRIORITY_TYPE_FIXED
#endif
static PriorityType priorityType = MESSAGES_PRIORITY_TYPE;
static const char* const PRIORITY_TYPES[] = { "Fixed", "Random" };

typedef enum _IntervalType {
    INTERVAL_TYPE_FIXED = 0,
    INTERVAL_TYPE_RANDOM = 1,
} IntervalType;

#ifndef MESSAGES_INTERVAL_TYPE
#define MESSAGES_INTERVAL_TYPE INTERVAL_TYPE_RANDOM
#endif
static IntervalType intervalType = MESSAGES_INTERVAL_TYPE;
static const char* const INTERVAL_TYPES[] = { "Fixed", "Random" };

#ifndef FIXED_MESSAGE_TYPE
#define FIXED_MESSAGE_TYPE AJNS_NOTIFICATION_MESSAGE_TYPE_INFO
#endif
static uint16_t messageType = FIXED_MESSAGE_TYPE;
static const char* const PRIORITIES[AJNS_NUM_MESSAGE_TYPES] = { "Emergency", "Warning", "Info" };

#ifndef FIXED_TTL
#define FIXED_TTL AJNS_NOTIFICATION_TTL_MIN // Note needs to be in the range AJNS_NOTIFICATION_TTL_MIN..AJNS_NOTIFICATION_TTL_MAX
#endif

static uint32_t messageTtl;

static uint32_t remaining_offset = 0;
static uint8_t haveInitialOffset = 0;

/**
 * Initialize the Notifications that will be sent during this sample app
 */
static AJNS_NotificationContent notificationContent;
static void InitNotification()
{

    notificationContent.numCustomAttributes = NUM_CUSTOMS;
    customAttributesToSend[0].key   = onKey;
    customAttributesToSend[0].value = HelloVal;
    customAttributesToSend[1].key   = offKey;
    customAttributesToSend[1].value = GoodbyeVal;
    notificationContent.customAttributes = customAttributesToSend;

    notificationContent.numTexts = NUM_TEXTS;
    textToSend[0].key   = lang1;
    textToSend[0].value = hello1;
    textToSend[1].key   = lang2;
    textToSend[1].value = hello2;
    notificationContent.texts = textToSend;

    notificationContent.numAudioUrls = NUM_RICH_AUDIO;
    richAudioUrls[0].key   = lang1;
    richAudioUrls[0].value = Audio1URL;
    richAudioUrls[1].key   = lang2;
    richAudioUrls[1].value = Audio2URL;
    notificationContent.richAudioUrls = richAudioUrls;

    notificationContent.richIconUrl = Icon1URL;
    notificationContent.richIconObjectPath = richIconObjectPath;
    notificationContent.richAudioObjectPath = richAudioObjectPath;
    notificationContent.controlPanelServiceObjectPath = controlPanelServiceObjectPath;

    messageTtl = FIXED_TTL;
}

/**
 * Initialize service
 */
AJ_Status NotificationProducer_Init()
{
    AJ_Status status = AJ_OK;
    uint32_t random;

    InitNotification();
    Producer_SetupEnv(&inputMode);
    if (!inputMode) {
        uint8_t priorityTypeRandom = (priorityType == PRIORITY_TYPE_RANDOM);
        uint8_t intervalTypeRandom = (intervalType == INTERVAL_TYPE_RANDOM);
        Producer_ReadConfigFromEnv(&producerEnabled, &notificationContent, &messagesInterval, &intervalTypeRandom, &initialIntervalOffset,
                                   &messageType, &priorityTypeRandom, &messageTtl);
        priorityType = priorityTypeRandom ? PRIORITY_TYPE_RANDOM : PRIORITY_TYPE_FIXED;
        intervalType = intervalTypeRandom ? INTERVAL_TYPE_RANDOM : INTERVAL_TYPE_FIXED;
    }

    status = AJNS_Producer_Start();

    AJ_AlwaysPrintf(("\n---------------------\nNotification Producer started!\n"));
    AJ_AlwaysPrintf(("Interval:     %u ms\n", messagesInterval));
    AJ_AlwaysPrintf(("Offset:       %u ms\n", initialIntervalOffset));
    AJ_AlwaysPrintf(("IntervalType: %s (%u)\n", INTERVAL_TYPES[intervalType], intervalType));
    AJ_AlwaysPrintf(("Priority      %s (%u)\n", PRIORITIES[messageType], messageType));
    AJ_AlwaysPrintf(("PriorityType: %s (%u)\n", PRIORITY_TYPES[priorityType], priorityType));
    AJ_AlwaysPrintf(("TTL:          %u s\n", messageTtl));
    if (!inputMode) {
        int8_t i;
        AJ_AlwaysPrintf(("NumTexts:     %u\n", notificationContent.numTexts));
        for (i = 0; i < notificationContent.numTexts; i++) {
            AJ_AlwaysPrintf(("MsgText:  %s (%d)\n", notificationContent.texts->key, i));
            AJ_AlwaysPrintf(("MsgText:  %s (%d)\n", notificationContent.texts->value, i));
        }
    }
    AJ_AlwaysPrintf(("---------------------\n\n"));
    AJ_InitTimer(&isMessageTime);
    if (!inputMode) {
        if (producerEnabled) {
            nextMessageTime = messagesInterval;
            if (intervalType == INTERVAL_TYPE_RANDOM) { // Randomize next message time if interval type is RANDOM
                AJ_RandBytes((uint8_t*)&random, sizeof(random));
                nextMessageTime = random % messagesInterval;
            }
            if (initialIntervalOffset > 0) { // Delay the initial message by the given offset amount
                nextMessageTime += initialIntervalOffset;
                haveInitialOffset = 1;
            }
            AJ_AlwaysPrintf(("Next message will be sent in %u ms\n", nextMessageTime));
        } else {
            AJ_AlwaysPrintf(("Producer disabled. No messages will be sent\n"));
        }
    } else {
        isMessageTime.seconds -= nextMessageTime / 1000; // Expire next message timer
        isMessageTime.milliseconds -= nextMessageTime % 1000; // Expire next message timer
    }

    return status;
}

/**
 * Allow the user the possibility to delete sent Notifications when DoWork is called.
 * Give the user an option to delete a notification after one was sent.
 */
static void PossiblyDeleteNotification(AJ_BusAttachment* busAttachment)
{
    AJ_Status status;
    uint8_t delMsg = FALSE;
    uint16_t delMsgType = AJNS_NOTIFICATION_MESSAGE_TYPE_INFO;

    if (inputMode) {
        Producer_GetShouldDeleteNotificationFromUser(busAttachment, &delMsg, &delMsgType);
        if (delMsg) {
            status = AJNS_Producer_DeleteLastNotification(busAttachment, delMsgType);
            AJ_AlwaysPrintf(("Delete Last Message Type: %d returned: '%s'\n", delMsgType, AJ_StatusText(status)));
        }
    }
}

/**
 * Meant to simulate scenario where sometimes Notifications are sent when
 * DoWork is called and sometimes not and also toggle between a regular
 * notification and a notication with action.
 * Send the notification every MESSAGES_INTERVAL milliseconds.
 */
static void PossiblySendNotification(AJ_BusAttachment* busAttachment)
{
    AJ_Status status;
    uint16_t messageType = FIXED_MESSAGE_TYPE;
    uint32_t ttl = messageTtl;
    uint32_t serialNum;
    uint32_t random;
    uint32_t elapsed = AJ_GetElapsedTime(&isMessageTime, TRUE);
    int32_t notificationId;

    if (elapsed >= nextMessageTime) {
        if (!inputMode) {
            notificationContent.controlPanelServiceObjectPath = ((notificationContent.controlPanelServiceObjectPath == NULL) ? controlPanelServiceObjectPath : NULL); // Toggle notification with action ON/OFF
            if (priorityType == PRIORITY_TYPE_RANDOM) { // Randomize message type if priority type is RANDOM
                AJ_RandBytes((uint8_t*)&random, sizeof(random));
                messageType = (uint16_t)(random % AJNS_NUM_MESSAGE_TYPES);
            }
        } else {
            Producer_GetNotificationFromUser(&notificationContent, &messageType, &ttl, &nextMessageTime);
        }
        status = AJNS_Producer_SendNotification(busAttachment, &notificationContent, messageType, ttl, &serialNum);
        AJNS_Producer_GetLastNotificationId(messageType, &notificationId);
        AJ_AlwaysHdrPrintf(("Send Message Type: %u with TTL: %u secs and notificationId: %i returned: '%s'\n", messageType, ttl, notificationId, AJ_StatusText(status)));
        if (inputMode) {
            Producer_FreeNotification(&notificationContent);
            PossiblyDeleteNotification(busAttachment);
        }
        if (!inputMode) {
            if (haveInitialOffset) {     // clear this and set nextMessageTime back to the default
                haveInitialOffset = 0;
                nextMessageTime = messagesInterval;
            }
            if (intervalType == INTERVAL_TYPE_RANDOM) { // Randomize next message time if interval type is RANDOM
                AJ_RandBytes((uint8_t*)&random, sizeof(random));
                if (elapsed < (messagesInterval + remaining_offset)) {
                    remaining_offset += messagesInterval - elapsed;
                } else {
                    remaining_offset = 0;     // If we stalled too much simply randomize from now and cleared carried over offset
                }
                nextMessageTime = remaining_offset + (random % messagesInterval); // randomize within the next time window of messagesInterval
            }
        }
        AJ_AlwaysPrintf(("Next message will be sent in %u ms\n", nextMessageTime));
        AJ_InitTimer(&isMessageTime);
    }
}

void NotificationProducer_DoWork(AJ_BusAttachment* busAttachment)
{
    PossiblySendNotification(busAttachment);
}

AJ_Status NotificationProducer_Finish()
{
    return AJ_OK;
}
