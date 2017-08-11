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

#ifndef HTTPCONTROL_H_
#define HTTPCONTROL_H_

/** @defgroup HttpControl Http Control
 * details Http Control Struct and Functions that assist in Marshaling
 * HttpControl into a message.
 *  @{
 */

#include <ajtcl/alljoyn.h>

/**
 * HttpControl structure - used to define a url for a controlpanel
 */
typedef struct {
    const char* url;         //!< The url of the Http control
    const char* (*getUrl)(); //!< Getter for the url of the Http control
} HttpControl;

/**
 * Initialize the HttpControl structure
 * @param httpControl - pointer to HttpControl structure
 */
void initializeHttpControl(HttpControl* httpControl);

/**
 * Marshal HttpControlUrl into given reply message
 * @param httpControl - httpcontrol to marshal
 * @param reply - the reply message to fill
 * @param language - the language requested
 * @return aj_status - success/failure
 */
AJ_Status marshalHttpControlUrl(HttpControl* httpControl, AJ_Message* reply, uint16_t language);

/** @} */
#endif /* HTTPCONTROL_H_ */
