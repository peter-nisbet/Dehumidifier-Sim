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

#ifndef CONSTRAINTLIST_H_
#define CONSTRAINTLIST_H_

/** @defgroup ConstraintList Contraint List
 *
 *  @{
 */

#include <ajtcl/services/Definitions.h>
#include <ajtcl/services/Common/BaseWidget.h>
#include <ajtcl/alljoyn.h>

/**
 * ConstraintList structure
 */
typedef struct {
    const void* value;                            //!< The constraint value
    const char* const* display;                   //!< The constraint display text
    const char* (*getDisplay)(uint16_t language); //!< Getter for the display text

} ConstraintList;

/**
 * Marshal ConstraintList into given reply message
 * @param widget - pointer to the widget structure
 * @param constraints - the constraints array
 * @param reply - the message to marshal into
 * @param numConstraints - the number of constraints
 * @param signature - signature of value
 * @param language - language requested
 */
AJ_Status marshalConstraintList(BaseWidget* widget, ConstraintList* constraints, AJ_Message* reply, uint16_t numConstraints,
                                const char* signature, uint16_t language);

/** @} */
#endif /* CONTRAINTVALUES_H_ */
