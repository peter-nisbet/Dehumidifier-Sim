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

#ifndef PRODUCER_SAMPLE_UTIL_H_
#define PRODUCER_SAMPLE_UTIL_H_

#include <ajtcl/alljoyn.h>

void Producer_SetupEnv(uint8_t* inputMode);
void Producer_GetNotificationFromUser(AJNS_NotificationContent* notificationContent, uint16_t* messageType, uint32_t* ttl, uint32_t* nextMessageTime);
void Producer_GetShouldDeleteNotificationFromUser(AJ_BusAttachment* busAttachment, uint8_t* delMsg, uint16_t* delMsgType);
void Producer_FreeNotification(AJNS_NotificationContent* notificationContent);
void Producer_ReadConfigFromEnv(uint8_t* producerEnabled, AJNS_NotificationContent* notificationContent,
                                uint32_t* messagesInterval, uint8_t* intervalTypeRandom, uint32_t* initialIntervalOffset,
                                uint16_t* messagePriority, uint8_t* priorityTypeRandom, uint32_t* messageTtl);

#endif /* PRODUCER_SAMPLE_UTIL_H_ */

