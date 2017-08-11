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

#ifndef _ONBOARDINGMANAGER_H_
#define _ONBOARDINGMANAGER_H_

/** @defgroup OnboardingManager Onboarding State Management
 *
 *  @{
 */

#include <ajtcl/alljoyn.h>
#include <ajtcl/services/OnboardingService.h>

/**
 * Is there a Wi-Fi connection?
 */
uint8_t AJOBS_IsWiFiConnected();

/*
 * Modify these variables to change the service's behavior
 */
#define AJOBS_PASSCODE_MAX_LENGTH 128 /**< 64 characters, hexified */
#define AJOBS_ERROR_MESSAGE_LEN 32    /**< error message length */

/**
 * Maximum number of scan info results.
 */
#define AJOBS_MAX_SCAN_INFOS 5

/**
 * authentication wildcards
 */
#define AJOBS_AUTH_TYPE_MIN_OF_WIFI_AUTH_TYPE   ((int8_t)(-4)) /**< Minimum value for authentication type*/
#define AJOBS_AUTH_TYPE_WPA2_AUTO               ((int8_t)(-3)) /**< WPA2 with Automatic cipher*/
#define AJOBS_AUTH_TYPE_WPA_AUTO                ((int8_t)(-2)) /**< WPA with Authomatic cipher */
#define AJOBS_AUTH_TYPE_ANY                     ((int8_t)(-1)) /**< ANY authenticaion type */

/**
 * authentication types
 */
typedef enum _AJOBS_AuthType {
    AJOBS_AUTH_TYPE_OPEN = 0,
    AJOBS_AUTH_TYPE_WEP = 1,
    AJOBS_AUTH_TYPE_WPA_TKIP = 2,
    AJOBS_AUTH_TYPE_WPA_CCMP = 3,
    AJOBS_AUTH_TYPE_WPA2_TKIP = 4,
    AJOBS_AUTH_TYPE_WPA2_CCMP = 5,
    AJOBS_AUTH_TYPE_WPS = 6,
    AJOBS_AUTH_TYPE_MAX_OF_WIFI_AUTH_TYPE = 7
} AJOBS_AuthType;

/**
 * Onboarding states
 */
typedef enum _AJOBS_State {
    AJOBS_STATE_NOT_CONFIGURED = 0,
    AJOBS_STATE_CONFIGURED_NOT_VALIDATED = 1,
    AJOBS_STATE_CONFIGURED_VALIDATING = 2,
    AJOBS_STATE_CONFIGURED_VALIDATED = 3,
    AJOBS_STATE_CONFIGURED_ERROR = 4,
    AJOBS_STATE_CONFIGURED_RETRY = 5,
} AJOBS_State;

/**
 * The default softAP fallback behavior can be changed at
 * compile time, and usually is the case, but this API provides
 * a way to change the default fallback behavior at runtime
 * as well, giving the user two options.
 *
 * @param fallback      The new default SoftAP fallback behavior
 *                      0 = don't fall back to soft AP
 *                      1 = fall back to soft AP after the number of retries expires
 */
AJ_Status AJOBS_SetSoftAPFallback(uint8_t fallback);

/**
 * Onboarding state getter.
 * @return state
 */
int8_t AJOBS_GetState();

/**
 * Onboarding state setter.
 * @param state
 */
void AJOBS_SetState(int8_t state);

/**
 * Onboarding information
 */
typedef struct _AJOBS_Info {
    char ssid[AJOBS_SSID_MAX_LENGTH + 1];            /**< ssid */
    char pc[AJOBS_PASSCODE_MAX_LENGTH + 1];          /**< password in hex format */
    int8_t authType;                                 /**< authType - Value is one of AJOBS_AuthType */
    int8_t validationPending;                        /**< validationPending - if Non-Zero, these credentials not verified */
} AJOBS_Info;

/**
 * Deafult Onboarding information
 */
#define AJOBS_INFO_DEFAULT { "", "", AJOBS_AUTH_TYPE_ANY, -1 }

/**
 * perform a check of whether the provided configuration info is valid
 * @param ssid
 * @param pc
 * @param authType
 * @return success
 */
uint8_t AJOBS_IsInfoValid(const char* ssid, const char* pc, int8_t authType);

/**
 * Onboarding info updater.
 * @param ssid
 * @param pc
 * @param authType
 * @return aj_status
 */
AJ_Status AJOBS_UpdateInfo(const char* ssid, const char* pc, int8_t authType);

/**
 * Onboarding last scan time.
 */
const AJ_Time* AJOBS_GetLastScanTime();

/**
 * Onboarding scan information.
 */
typedef struct _AJOBS_ScanInfo {
    char ssid[AJOBS_SSID_MAX_LENGTH + 1];            /**< ssid */
    int8_t authType;                                 /**< authType - Value is one of AJOBS_AuthType */
} AJOBS_ScanInfo;

/**
 * Onboarding scan infos variable getter.
 * @return an array of scan info structures of the last saved scan.
 */
const AJOBS_ScanInfo* AJOBS_GetScanInfos();

/**
 * Onboarding scan infos count variable getter.
 * @return number of scan info structures in the last saved scan.
 */
uint8_t AJOBS_GetScanInfoCount();

/**
 * Last connection error code
 */
typedef enum _AJOBS_LastError {
    AJOBS_STATE_LAST_ERROR_VALIDATED = 0,
    AJOBS_STATE_LAST_ERROR_UNREACHABLE = 1,
    AJOBS_STATE_LAST_ERROR_UNSUPPORTED_PROTOCOL = 2,
    AJOBS_STATE_LAST_ERROR_UNAUTHORIZED = 3,
    AJOBS_STATE_LAST_ERROR_ERROR_MESSAGE = 4,
} AJOBS_LastError;

/**
 * Onboarding error
 */
typedef struct _AJOBS_Error {
    int16_t code;                                /**< code */
    char message[AJOBS_ERROR_MESSAGE_LEN + 1];   /**< message */
} AJOBS_Error;

/**
 * Onboarding error getter.
 * @return an error structure.
 */
const AJOBS_Error* AJOBS_GetError();

/**
 * Structure that holds all relevant configuration settings for:
 * a) the SoftAP definition and
 * b) parameter for the recovery/retry algorithm when connection attempts fail
 */
typedef struct _AJOBS_Settings {
    /**
     * Wait time for clients to connect to SoftAP station (ms)
     */
    uint32_t AJOBS_WAIT_FOR_SOFTAP_CONNECTION; // 600000
    /**
     * Number of retry attempts after failed attempt to connect to an already validated configuration.
     */
    uint8_t AJOBS_MAX_RETRIES; // 2
    /**
     * Wait time between retries (ms)
     */
    uint32_t AJOBS_WAIT_BETWEEN_RETRIES; // 180000
    /**
     * SoftAP SSID name.
     **/
    char AJOBS_SoftAPSSID[AJOBS_SSID_MAX_LENGTH + 1]; // AJ_...
    /**
     * Whether the SoftAP SSID is hidden
     */
    uint8_t AJOBS_SoftAPIsHidden; // FALSE
    /**
     * SoftAP passpharse. If NULL means OPEN network otherwise assumes WPA2 8 to 63 characters long.
     */
    char* AJOBS_SoftAPPassphrase; // NULL
    /**
     * Whether to reset the Wi-Fi chip when switching to Idle mode
     */
    uint8_t AJOBS_RESET_WIFI_ON_IDLE; // TRUE;
} AJOBS_Settings;

#define AJOBS_DEFAULT_SETTINGS { 600000, 2, 180000, { 0 }, FALSE, NULL, TRUE };             /**< default settings */

/**
 * Start Onboarding service framework passing settings and persistence callbacks/
 * @param settings
 * @return aj_status
 */
AJ_Status AJOBS_Start(const AJOBS_Settings* settings);

//Soft AP definitions
/**
 * Length of manufacture name section in the SoftAPSSID
 */
#define AJOBS_DEVICE_MANUFACTURE_NAME_LEN 7
/**
 * Length of product name section in the SoftAPSSID
 */
#define AJOBS_DEVICE_PRODUCT_NAME_LEN 13
/**
 * Length of serial id section in the SoftAPSSID
 */
#define AJOBS_DEVICE_SERIAL_ID_LEN 7

/**
 * Clear onboarding information.
 * @return aj_status
 */
AJ_Status AJOBS_ClearInfo();

/**
 * Establish a Wi-Fi connection either as:
 * a) a SoftAP station with a connected client or
 * b) as a client to the currently configured AP
 * @return aj_status
 */
AJ_Status AJOBS_EstablishWiFi();

/**
 * Switch to retry state toggling between:
 * a) waiting for an Onboarder application to connect to the SoftAP till timeout of AJOBS_WAIT_BETWEEN_RETRIES and
 * b) attempting to reconnect using the current validated configuration
 * @return aj_status
 */
AJ_Status AJOBS_SwitchToRetry();

/**
 * Disconnect from the current Wi-Fi connection and go into Idle mode.
 * @return aj_status
 */
AJ_Status AJOBS_DisconnectWiFi();

/** @} */ //end of group 'OnboardingManager'
 #endif /* _ONBOARDINGMANAGER_H_ */
