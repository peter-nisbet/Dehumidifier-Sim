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
#include <ajtcl/aj_debug.h>
#include "ServerSampleUtil.h"
#include "PropertyStoreOEMProvisioning.h"
#include <ajtcl/services/PropertyStore.h>

/**
 * Allows the DeviceName to be set dynamically from the environment
 */
void ServerSample_SetupEnv()
{
    char* deviceName = getenv("AJ_DEVICE_NAME");
    if (deviceName) {
        AJ_AlwaysPrintf(("Setting DeviceName from env to: %s\n", deviceName));
        /* set the DeviceName for the default language */
        AJSVC_PropertyStore_SetValueForLang(AJSVC_PROPERTY_STORE_DEVICE_NAME, AJSVC_PropertyStore_GetCurrentDefaultLanguageIndex(), deviceName);
    }
}
