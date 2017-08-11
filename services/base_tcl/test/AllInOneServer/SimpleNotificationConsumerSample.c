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

#include <ajtcl/alljoyn.h>
#include <NotificationConsumerSample.h>
#include <ajtcl/services/NotificationConsumer.h>

#if (defined(__linux) || defined(_WIN32) || defined(__MACH__))
#include "NotificationConsumerSampleUtil.h"
#else
#define Consumer_SetupEnv(x, y) do { } while (0)
#define Consumer_GetActionFromUser(x) do { } while (0)
#endif

#ifndef NDEBUG
extern AJ_EXPORT uint8_t dbgAJSVCAPP;
#endif

static uint8_t inputMode = FALSE;
static uint8_t superAgentMode = TRUE;
static uint8_t nextAction = CONSUMER_ACTION_NOTHING;
static uint8_t processingAction = FALSE;
#define DISMISS_DELAY_PERIOD 30000
static AJ_Time dismissTime;
static AJ_Time* dismissTimer = NULL;

static AJNS_Consumer_NotificationReference savedNotification;

static AJ_Status ApplicationHandleNotify(AJNS_Notification* notification)
{
    int8_t indx;

    AJ_AlwaysPrintf(("******************** Begin New Message Received ********************\n"));

    if (notification == 0) {
        AJ_AlwaysPrintf(("Notification is NULL\n"));
        return AJ_OK;
    }

    AJ_AlwaysPrintf(("Message Id: %d\nVersion: %u\nDevice Id: %s\nDevice Name: %s\nApp Id: %s\nApp Name: %s\nMessage Type: %d\n",
                     notification->notificationId, notification->version, notification->deviceId, notification->deviceName, notification->appId, notification->appName, notification->messageType));

    if (notification->originalSenderName != 0 && strlen(notification->originalSenderName) > 0) {
        AJ_AlwaysPrintf(("OriginalSender bus unique name: %s\n", notification->originalSenderName));
    }

    for (indx = 0; indx < notification->content->numTexts; indx++) {
        AJ_AlwaysPrintf(("Language: %s  Message: %s.\n", notification->content->texts[indx].key, notification->content->texts[indx].value));
    }

    AJ_AlwaysPrintf(("Other parameters included:\n"));
    for (indx = 0; indx < notification->content->numCustomAttributes; indx++) {
        AJ_AlwaysPrintf(("Custom Attribute Key: %s  Custom Attribute Value: %s\n", notification->content->customAttributes[indx].key, notification->content->customAttributes[indx].value));
    }

    if (notification->content->richIconUrl != 0 && strlen(notification->content->richIconUrl) > 0) {
        AJ_AlwaysPrintf(("Rich Content Icon Url: %s\n", notification->content->richIconUrl));
    }

    if (notification->content->numAudioUrls) {
        AJ_AlwaysPrintf(("******************** Begin Rich Audio Content ********************\n"));
        for (indx = 0; indx < notification->content->numAudioUrls; indx++) {
            AJ_AlwaysPrintf(("Language: %s  Audio URL %s\n", notification->content->richAudioUrls[indx].key, notification->content->richAudioUrls[indx].value));
        }
        AJ_AlwaysPrintf(("******************** End Rich Audio Content ********************\n"));
    }

    if (notification->content->richIconObjectPath != 0 && strlen(notification->content->richIconObjectPath) > 0) {
        AJ_AlwaysPrintf(("Rich Content Icon Object Path: %s\n", notification->content->richIconObjectPath));
    }

    if (notification->content->richAudioObjectPath != 0 && strlen(notification->content->richAudioObjectPath) > 0) {
        AJ_AlwaysPrintf(("Rich Content Audio Object Path: %s\n", notification->content->richAudioObjectPath));
    }

    if (notification->content->controlPanelServiceObjectPath != 0 && strlen(notification->content->controlPanelServiceObjectPath) > 0) {
        AJ_AlwaysPrintf(("ControlPanelService object path: %s\n", notification->content->controlPanelServiceObjectPath));
    }

    if (processingAction == FALSE || dismissTimer == NULL) {
        // Save notification reference so that it can be later dimissed
        savedNotification.version = notification->version;
        savedNotification.notificationId = notification->notificationId;
        strncpy(savedNotification.appId, notification->appId, sizeof(savedNotification.appId) - 1);
        savedNotification.originalSenderName[0] = '\0';
        if (notification->originalSenderName != 0 && strlen(notification->originalSenderName) > 0) {
            strncpy(savedNotification.originalSenderName, notification->originalSenderName, sizeof(savedNotification.originalSenderName) - 1);
        }
        if (!inputMode && DISMISS_DELAY_PERIOD > 0 && dismissTimer == NULL) { // If feature is enabled and there is no set timer
            dismissTimer = &dismissTime; //  Setup the delay timer before dismissing this notification
            AJ_InitTimer(dismissTimer);
        }
    }
    AJ_AlwaysPrintf(("******************** End New Message Received ********************\n"));

    return AJ_OK;
}

static AJ_Status ApplicationHandleDismiss(int32_t notificationId, const char* appId)
{
    AJ_AlwaysPrintf(("******************** Begin New Dismiss Received ********************\n"));
    AJ_AlwaysPrintf(("Notification Id: %d\nApp Id: %s\n", notificationId, appId));
    if (savedNotification.version > 0 && !strcmp(appId, savedNotification.appId) && notificationId == savedNotification.notificationId) {
        AJ_AlwaysPrintf(("Notification dimissed: Version %u sent from OriginalSender %s\n", savedNotification.version, savedNotification.originalSenderName));
        if (processingAction == FALSE) {
            savedNotification.version = 0;
        }
    }
    AJ_AlwaysPrintf(("******************** End New Dismiss Received ********************\n"));

    return AJ_OK;
}

AJ_Status NotificationConsumer_Init()
{
    AJ_Status status = AJ_OK;

    Consumer_SetupEnv(&inputMode, &superAgentMode);
    AJ_AlwaysPrintf(("Init(): Set Consumer to detect SuperAgent option is turned %s\n", superAgentMode ? "ON" : "off"));
    status = AJNS_Consumer_Start(superAgentMode, &ApplicationHandleNotify, &ApplicationHandleDismiss);
    memset(&savedNotification, 0, sizeof(AJNS_Consumer_NotificationReference));

    AJ_AlwaysPrintf(("\n---------------------\nNotification Consumer started!\n"));
    AJ_AlwaysPrintf(("DISMISS_DELAY_PERIOD: %u ms\n", DISMISS_DELAY_PERIOD));
    AJ_AlwaysPrintf(("---------------------\n\n"));

    return status;
}

static void DismissActionCompleted(AJ_Status status, void* context)
{
    AJ_AlwaysPrintf(("DismissActionCompleted() with status=%s\n", AJ_StatusText(status)));
    if (!inputMode) {
        nextAction = CONSUMER_ACTION_NOTHING;
    }
    processingAction = FALSE;
    savedNotification.version = 0;
    return;
}

/**
 * Meant to simulate scenario where an Action is set when a new Notification is received.
 */
static void Consumer_DoAction(AJ_BusAttachment* busAttachment)
{
    if (inputMode) {
        Consumer_GetActionFromUser(&nextAction);
        AJ_AlwaysPrintf(("Action received is %u\n", nextAction));
    }
    switch (nextAction) {
    case CONSUMER_ACTION_DISMISS:
        processingAction = TRUE;
        AJNS_Consumer_DismissNotification(busAttachment, savedNotification.version, savedNotification.notificationId, savedNotification.appId, savedNotification.originalSenderName, &DismissActionCompleted);
        break;

    case CONSUMER_ACTION_NOTHING:
        savedNotification.version = 0;
        if (!inputMode && DISMISS_DELAY_PERIOD == 0) { // default behaviour is to dismiss next notification
            nextAction = CONSUMER_ACTION_DISMISS;
        }
        break;
    }
}

void NotificationConsumer_DoWork(AJ_BusAttachment* busAttachment)
{
    if (savedNotification.version > 0 && !processingAction) {
        if (dismissTimer != NULL) { // Dismiss timer is set
            if (AJ_GetElapsedTime(dismissTimer, TRUE) > DISMISS_DELAY_PERIOD) { // Delay timer expired
                dismissTimer = NULL;                  // Disable/delete timer
                nextAction = CONSUMER_ACTION_DISMISS; // Dismiss last saved notification
                Consumer_DoAction(busAttachment);
            }
        } else { // Saved notidication awaits action
            Consumer_DoAction(busAttachment);
        }
    }
    return;
}

AJ_Status NotificationConsumer_Finish()
{
    return AJ_OK;
}
