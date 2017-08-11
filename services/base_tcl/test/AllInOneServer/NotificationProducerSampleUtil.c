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

#include <stdlib.h>
#include <ajtcl/aj_target.h>
#include <ajtcl/aj_status.h>
#include <ajtcl/aj_debug.h>
#include <ajtcl/services/NotificationProducer.h>
#include "NotificationProducerSampleUtil.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#define snprintf _snprintf
#endif

#define BUF_SIZE 500
#define MAX_MSG 5

AJNS_DictionaryEntry richAudioUrls[MAX_MSG];
AJNS_DictionaryEntry texts[MAX_MSG];
AJNS_DictionaryEntry customAttributes[MAX_MSG];

static uint8_t GetCmdLine(char* buf, size_t num)
{
    while (AJ_GetCmdLine(buf, num) == NULL) {
    }
    return (strlen(buf) > 0);
}

/**
 * A function that allows us to enter QA InputMode
 */
void Producer_SetupEnv(uint8_t* inputMode)
{
    char* value = getenv("INPUT_MODE");
    if (value) {
        *inputMode = (uint8_t)atoi(value);
    }
}

void Producer_GetNotificationFromUser(AJNS_NotificationContent* notificationContent, uint16_t* messageType, uint32_t* ttl, uint32_t* nextMessageTime)
{
    char buf[BUF_SIZE];
    char lang[BUF_SIZE];
    char msg[BUF_SIZE];
    char key[BUF_SIZE];
    char val[BUF_SIZE];
    char url[BUF_SIZE];
    uint8_t i;
    uint16_t defaultMessageType = *messageType;
    uint32_t defaultTtl = *ttl;
    uint32_t defaultNextMessageTime = *nextMessageTime;
    uint8_t defaultNumTexts = notificationContent->numTexts = 1;
    uint8_t defaultNumCustomAttributes = notificationContent->numCustomAttributes = 0;

    notificationContent->numCustomAttributes = 0;
    notificationContent->numTexts = 1;
    notificationContent->numAudioUrls = 0;

    AJ_StartReadFromStdIn();
    AJ_AlwaysPrintf(("Please enter the messageType 0=EMERGENCY, 1=WARNING, 2=INFO.\n"));
    AJ_AlwaysPrintf(("Empty string or invalid input will default to %u\n", defaultMessageType));
    if (GetCmdLine(buf, BUF_SIZE)) {
        char stringType[6]; // Maximum decimal value for uint16_t is 65535 which is 5 characters long
        *messageType = (uint16_t)atoi(buf);
        snprintf(stringType, 6, "%u", *messageType);
        if (!(strcmp(buf, stringType) == 0)) {             //they do not match, it is not int
            AJ_AlwaysPrintf(("Message Type is not an integer value. Defaulting to %u\n", defaultMessageType));
            *messageType = defaultMessageType;
        } else if (*messageType != AJNS_NOTIFICATION_MESSAGE_TYPE_EMERGENCY && *messageType != AJNS_NOTIFICATION_MESSAGE_TYPE_WARNING && *messageType != AJNS_NOTIFICATION_MESSAGE_TYPE_INFO) {
            AJ_AlwaysPrintf(("Message Type is not one of 0, 1 or 2. Defaulting to %u\n", defaultMessageType));
            *messageType = defaultMessageType;
        }
    }

    AJ_AlwaysPrintf(("Please enter the number of languages you wish to send the message in. Maximum %d\n", MAX_MSG));
    AJ_AlwaysPrintf(("Empty string or invalid data will default to %u\n", defaultNumTexts));
    if (GetCmdLine(buf, BUF_SIZE)) {
        notificationContent->numTexts = atoi(buf);
        if (notificationContent->numTexts < 1 || notificationContent->numTexts > MAX_MSG) {
            notificationContent->numTexts = defaultNumTexts;
        }
    }

    for (i = 0; i < notificationContent->numTexts; i++) {
        strncpy(lang, "en", BUF_SIZE);
        strncpy(msg, "Hello World", BUF_SIZE);

        AJ_AlwaysPrintf(("Please enter the message's language %u. Max 499 characters, Empty string acceptable\n", i + 1));
        if (GetCmdLine(buf, BUF_SIZE)) {
            strncpy(lang, buf, BUF_SIZE);
        }

        AJ_AlwaysPrintf(("Please enter the message's text in language %u. Max 499 characters, Empty string acceptable\n", i + 1));
        if (GetCmdLine(buf, BUF_SIZE)) {
            strncpy(msg, buf, BUF_SIZE);
        }

        texts[i].key = malloc(strlen(lang) + 1);
        if (snprintf((char*)texts[i].key, BUF_SIZE, "%s", lang) == -1) {
            break;
        }
        texts[i].value = malloc(strlen(msg) + 1);
        if (snprintf((char*)texts[i].value, BUF_SIZE, "%s", msg) == -1) {
            break;
        }
    }
    notificationContent->texts = texts;

    AJ_AlwaysPrintf(("Please enter the number of Custom Attributes you wish to send. Maximum %d\n", MAX_MSG));
    AJ_AlwaysPrintf(("Empty string or invalid data will default to %u\n", defaultNumCustomAttributes));
    if (GetCmdLine(buf, BUF_SIZE)) {
        notificationContent->numCustomAttributes = atoi(buf);
        if (notificationContent->numCustomAttributes < 0 || notificationContent->numCustomAttributes > MAX_MSG) {
            notificationContent->numCustomAttributes = defaultNumCustomAttributes;
        }
    }

    for (i = 0; i < notificationContent->numCustomAttributes; i++) {
        strncpy(key, "key", BUF_SIZE);
        strncpy(val, "val", BUF_SIZE);

        AJ_AlwaysPrintf(("Please enter the Custom Attribute key. Max 499 characters, Empty string acceptable\n"));
        if (GetCmdLine(buf, BUF_SIZE)) {
            strncpy(key, buf, BUF_SIZE);
        }

        AJ_AlwaysPrintf(("Please enter the Custom Attribute value. Max 499 characters, Empty string acceptable\n"));
        if (GetCmdLine(buf, BUF_SIZE)) {
            strncpy(val, buf, BUF_SIZE);
        }

        customAttributes[i].key = malloc(strlen(key) + 1);
        if (snprintf((char*)customAttributes[i].key, BUF_SIZE, "%s", key) == -1) {
            break;
        }
        customAttributes[i].value = malloc(strlen(val) + 1);
        if (snprintf((char*)customAttributes[i].value, BUF_SIZE, "%s", val) == -1) {
            break;
        }
    }
    notificationContent->customAttributes = customAttributes;

    AJ_AlwaysPrintf(("Please enter the ttl in the range %u - %u seconds.\n", AJNS_NOTIFICATION_TTL_MIN, AJNS_NOTIFICATION_TTL_MAX));
    AJ_AlwaysPrintf(("Empty string input will default to %u\n", defaultTtl));
    if (GetCmdLine(buf, BUF_SIZE)) {
        char stringType[11]; // Maximum decimal value for uint32_t is 4294967295 which is 10 characters long
        *ttl = (uint32_t)atoi(buf);
        snprintf(stringType, 11, "%u", *ttl);
        if (!(strcmp(buf, stringType) == 0)) {             //they do not match, it is not int
            AJ_AlwaysPrintf(("Ttl is not an integer value. Defaulting to %u\n", defaultTtl));
            *ttl = defaultTtl;
        } else if (*ttl < AJNS_NOTIFICATION_TTL_MIN || AJNS_NOTIFICATION_TTL_MAX < *ttl) {
            AJ_AlwaysPrintf(("Ttl is not in the range %u - %u. Defaulting to %u\n", AJNS_NOTIFICATION_TTL_MIN, AJNS_NOTIFICATION_TTL_MAX, defaultTtl));
            *ttl = defaultTtl;
        }
    }

    AJ_AlwaysPrintf(("Please enter an optional rich icon url. Max 499 characters.\n"));
    AJ_AlwaysPrintf(("Empty string will default to not setting this value\n"));
    if (GetCmdLine(buf, BUF_SIZE)) {
        notificationContent->richIconUrl = malloc(strlen(buf) + 1);
        if (snprintf((char*)notificationContent->richIconUrl, BUF_SIZE, "%s", buf) == -1) {
        }
    } else {
        notificationContent->richIconUrl = NULL;
    }

    AJ_AlwaysPrintf(("Please enter the number of rich audio urls you wish to send. Maximum %d\n", MAX_MSG));
    AJ_AlwaysPrintf(("Empty string or invalid data will default to 0\n"));
    if (GetCmdLine(buf, BUF_SIZE)) {
        notificationContent->numAudioUrls = atoi(buf);
        if (notificationContent->numAudioUrls > MAX_MSG) {
            notificationContent->numAudioUrls = 0;
        }
    }

    for (i = 0; i < notificationContent->numAudioUrls; i++) {
        strncpy(lang, "en", BUF_SIZE);
        strncpy(url, "http://myAudioUrl.wv", BUF_SIZE);

        AJ_AlwaysPrintf(("Please enter the url's language %u. Max 499 characters, Empty string acceptable\n", i + 1));
        if (GetCmdLine(buf, BUF_SIZE)) {
            strncpy(lang, buf, BUF_SIZE);
        }

        AJ_AlwaysPrintf(("Please enter the url in language %u. Max 499 characters, Empty string acceptable\n", i + 1));
        if (GetCmdLine(buf, BUF_SIZE)) {
            strncpy(url, buf, BUF_SIZE);
        }

        richAudioUrls[i].key = malloc(strlen(lang) + 1);
        if (snprintf((char*)richAudioUrls[i].key, BUF_SIZE, "%s", lang) == -1) {
            break;
        }
        richAudioUrls[i].value = malloc(strlen(url) + 1);
        if (snprintf((char*)richAudioUrls[i].value, BUF_SIZE, "%s", url) == -1) {
            break;
        }
    }
    notificationContent->richAudioUrls = richAudioUrls;

    AJ_AlwaysPrintf(("Please enter an optional icon object path. Max 499 characters.\n"));
    AJ_AlwaysPrintf(("Empty string will default to not setting this value\n"));
    if (GetCmdLine(buf, BUF_SIZE)) {
        notificationContent->richIconObjectPath = malloc(strlen(buf) + 1);
        if (snprintf((char*)notificationContent->richIconObjectPath, BUF_SIZE, "%s", buf) == -1) {
        }
    } else {
        notificationContent->richIconObjectPath = NULL;
    }

    AJ_AlwaysPrintf(("Please enter an optional audio object path. Max 499 characters.\n"));
    AJ_AlwaysPrintf(("Empty string will default to not setting this value\n"));
    if (GetCmdLine(buf, BUF_SIZE)) {
        notificationContent->richAudioObjectPath = malloc(strlen(buf) + 1);
        if (snprintf((char*)notificationContent->richAudioObjectPath, BUF_SIZE, "%s", buf) == -1) {
        }
    } else {
        notificationContent->richAudioObjectPath = NULL;
    }

    AJ_AlwaysPrintf(("Please enter an optional control panel service object path. Max 499 characters.\n"));
    AJ_AlwaysPrintf(("Empty string will default to not setting this value\n"));
    if (GetCmdLine(buf, BUF_SIZE)) {
        notificationContent->controlPanelServiceObjectPath = malloc(strlen(buf) + 1);
        if (snprintf((char*)notificationContent->controlPanelServiceObjectPath, BUF_SIZE, "%s", buf) == -1) {
        }
    } else {
        notificationContent->controlPanelServiceObjectPath = NULL;
    }

    AJ_AlwaysPrintf(("Please enter the interval for the next message. Default %u.\n", *nextMessageTime));
    AJ_AlwaysPrintf(("Empty string will keep default value\n"));
    if (GetCmdLine(buf, BUF_SIZE)) {
        char stringType[11]; // Maximum decimal value for uint32_t is 4294967295 which is 10 characters long
        *nextMessageTime = (uint32_t)atoi(buf);
        snprintf(stringType, 11, "%u", *nextMessageTime);
        if (!(strcmp(buf, stringType) == 0)) {             //they do not match, it is not int
            AJ_AlwaysPrintf(("Next message interval is not an integer value. Defaulting to %u\n", defaultNextMessageTime));
            *nextMessageTime = defaultNextMessageTime;
        }
    }
    AJ_StopReadFromStdIn();
}

void Producer_FreeNotification(AJNS_NotificationContent* notificationContent)
{
    uint8_t i;

    for (i = 0; i < notificationContent->numTexts; i++) {
        free((char*)texts[i].key);
        free((char*)texts[i].value);
    }

    for (i = 0; i < notificationContent->numCustomAttributes; i++) {
        free((char*)customAttributes[i].key);
        free((char*)customAttributes[i].value);
    }

    for (i = 0; i < notificationContent->numAudioUrls; i++) {
        free((char*)richAudioUrls[i].key);
        free((char*)richAudioUrls[i].value);
    }

    if (notificationContent->richIconUrl) {
        free((char*)notificationContent->richIconUrl);
    }
    if (notificationContent->richIconObjectPath) {
        free((char*)notificationContent->richIconObjectPath);
    }
    if (notificationContent->richAudioObjectPath) {
        free((char*)notificationContent->richAudioObjectPath);
    }
    if (notificationContent->controlPanelServiceObjectPath) {
        free((char*)notificationContent->controlPanelServiceObjectPath);
    }
}

void Producer_GetShouldDeleteNotificationFromUser(AJ_BusAttachment* busAttachment, uint8_t* delMsg, uint16_t* delMsgType)
{
    AJ_Status status = AJ_OK;
    char buf[BUF_SIZE];

    AJ_StartReadFromStdIn();
    AJ_AlwaysPrintf(("Please enter 0-2 to select message type you want to delete.\n"));
    AJ_AlwaysPrintf(("Empty string or invalid input will default to not sending delete message\n"));
    if (GetCmdLine(buf, BUF_SIZE)) {
        char stringType[6]; // Maximum decimal value for uint16_t is 65535 which is 5 characters long
        *delMsgType = (uint16_t)atoi(buf);
        snprintf(stringType, 6, "%u", *delMsgType);
        if (!(strcmp(buf, stringType) == 0)) {             //they do not match, it is not int
            AJ_AlwaysPrintf(("Message Type is not an integer value. Skipping deletion\n"));
            *delMsg = FALSE;
        } else if (*delMsgType != AJNS_NOTIFICATION_MESSAGE_TYPE_EMERGENCY && *delMsgType != AJNS_NOTIFICATION_MESSAGE_TYPE_WARNING && *delMsgType != AJNS_NOTIFICATION_MESSAGE_TYPE_INFO) {
            AJ_AlwaysPrintf(("Message Type is not one of 0, 1 or 2. Skipping deletion\n"));
            *delMsg = FALSE;
        } else {
            *delMsg = TRUE;
        }
    }
    AJ_StopReadFromStdIn();
}


void Producer_ReadConfigFromEnv(uint8_t* producerEnabled, AJNS_NotificationContent* notificationContent,
                                uint32_t* messagesInterval, uint8_t* intervalTypeRandom, uint32_t* initialIntervalOffset,
                                uint16_t* messagePriority, uint8_t* priorityTypeRandom, uint32_t* messageTtl)
{
    char* value;
    char* msgText;
    char* msgLang;

    value = getenv("AJNS_PRODUCER_ENABLED");
    if (value) {
        AJ_AlwaysPrintf(("Read AJNS_PRODUCER_ENABLED from environment: '%s'\n", value));
        if (strcasecmp(value, "true") == 0) {
            *producerEnabled = 1;
        } else {
            *producerEnabled = 0;
            return;
        }
    }

    msgText = getenv("AJNS_MSG_TEXT");
    msgLang = getenv("AJNS_MSG_LANG");

    if (msgText || msgLang) {
        // only send one language in this case
        notificationContent->numTexts = 1;
    }
    if (msgLang) {
        AJ_AlwaysPrintf(("Read AJNS_MSG_LANG from environment: '%s'\n", msgLang));
        notificationContent->texts[0].key = msgLang;
    }
    if (msgText) {
        AJ_AlwaysPrintf(("Read AJNS_MSG_TEXT from environment: '%s'\n", msgText));
        notificationContent->texts[0].value = msgText;
    }

    value = getenv("AJNS_MSG_INTERVAL");
    if (value) {
        AJ_AlwaysPrintf(("Read AJNS_MSG_INTERVAL from environment: '%s'\n", value));
        *messagesInterval = (uint32_t)atoi(value);
    }

    value = getenv("AJNS_MSG_INTERVAL_TYPE");
    if (value) {
        AJ_AlwaysPrintf(("Read AJNS_MSG_INTERVAL_TYPE from environment: '%s'\n", value));
        if (strcasecmp(value, "random") == 0) {
            *intervalTypeRandom = 1;
        } else {
            *intervalTypeRandom = 0;
        }
    }

    value = getenv("AJNS_MSG_INTERVAL_OFFSET");
    if (value) {
        AJ_AlwaysPrintf(("Read AJNS_MSG_INTERVAL from environment: '%s'\n", value));
        *initialIntervalOffset = (uint32_t)atoi(value);
    }

    value = getenv("AJNS_MSG_PRIORITY");
    if (value) {
        AJ_AlwaysPrintf(("Read AJNS_MSG_PRIORITY from environment: '%s'\n", value));
        *messagePriority = (uint16_t)atoi(value);
    }

    value = getenv("AJNS_MSG_PRIORITY_TYPE");
    if (value) {
        AJ_AlwaysPrintf(("Read AJNS_MSG_PRIORITY_TYPE from environment: '%s'\n", value));
        if (strcasecmp(value, "random") == 0) {
            *priorityTypeRandom = 1;
        } else {
            *priorityTypeRandom = 0;
        }
    }

    value = getenv("AJNS_MSG_TTL");
    if (value) {
        AJ_AlwaysPrintf(("Read AJNS_MSG_TTL from environment: '%s'\n", value));
        *messageTtl = (uint32_t)atoi(value);
    }

}
