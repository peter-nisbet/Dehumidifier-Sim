/******************************************************************************
 * Copyright (c) 2013 - 2014, AllSeen Alliance. All rights reserved.
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

#ifndef CONTROLPANELPROVIDED_H_
#define CONTROLPANELPROVIDED_H_

#include <ajtcl/alljoyn.h>

//-------------ON ACTION METHODS

void AppHandle_PowerON_AirPurifier();
void AppHandle_PowerOFF_AirPurifier();
void AppHandle_ResetAirPurifier();

//-------------PROPERTIES GETTERS

uint8_t  getPowerStatus();
uint16_t getFlowValue();
uint16_t getTimerValue();
int16_t  getSensorCleanMonitorValue();
uint8_t  getSensorOdorValue();
uint8_t  getSensorDustValue();
uint8_t  getSensorAllergenValue();
uint8_t  getSensorCleanMetalGridValue();
uint8_t  getSensorReplaceFilterValue();
uint8_t  getPowerStatusUpdate();
uint8_t  getResetUpdate();
uint8_t  getFlowValueUpdate();
uint8_t  getTimerValueUpdate();

//-------------PROPERTIES SETTERS

void setPowerStatus(uint8_t p);
void setFlowValue(uint16_t f);
void setTimerValue(uint16_t t);
void setPowerStatusUpdate(uint8_t p);
void setResetUpdate(uint8_t r);
void setFlowValueUpdate(uint8_t f);
void setTimerValueUpdate(uint8_t t);
void setPowerStatusUpdateForEvent(uint8_t p);

//-------------OTHER NECESSARY METHODS

uint8_t simulateAPsensorsAndSendUpdates();
uint8_t checkForEventsToSend();

#endif /* CONTROLPANELPROVIDED_H_ */

