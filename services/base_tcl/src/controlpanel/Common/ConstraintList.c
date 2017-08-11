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

#include <ajtcl/services/Common/ConstraintList.h>
#include <ajtcl/services/Common/ControlMarshalUtil.h>
#include <ajtcl/services/Widgets/PropertyWidget.h>

AJ_Status marshalConstraintList(BaseWidget* widget, ConstraintList* constraints, AJ_Message* reply, uint16_t numConstraints,
                                const char* signature, uint16_t language)
{
    AJ_Status status;
    AJ_Arg arrayArg, opParams;
    uint16_t cnt;
    PropertyWidget* propWidget = (PropertyWidget*)widget;
    const void* value;
    const char* display;

    status = StartComplexOptionalParam(reply, &opParams, PROPERTY_CONSTRAINT_LIST, PROPERTY_CONSTRAINT_LIST_SIG);
    if (status != AJ_OK) {
        return status;
    }

    status = AJ_MarshalContainer(reply, &arrayArg, AJ_ARG_ARRAY);
    if (status != AJ_OK) {
        return status;
    }

    for (cnt = 0; cnt < numConstraints; cnt++) {
        if (propWidget->optParams.getConstraint) {
            display = propWidget->optParams.getConstraint(propWidget, cnt, &value, language);
        } else if (constraints[cnt].getDisplay != 0) {
            value = constraints[cnt].value;
            display = constraints[cnt].getDisplay(language);
        } else {
            value = constraints[cnt].value;
            display = constraints[cnt].display[language];
        }
        status = AddConstraintValue(reply, signature, value, display);
        if (status != AJ_OK) {
            return status;
        }
    }

    status = AJ_MarshalCloseContainer(reply, &arrayArg);
    if (status != AJ_OK) {
        return status;
    }

    return AJ_MarshalCloseContainer(reply, &opParams);
}
