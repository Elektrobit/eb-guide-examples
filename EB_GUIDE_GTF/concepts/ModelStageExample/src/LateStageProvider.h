////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

#ifndef GTF_LATE_STAGE_PROVIDER_H
#define GTF_LATE_STAGE_PROVIDER_H

#include <gtf/model/StageProvider.h>

namespace stageexample {
class LateStageProvider : public gtf::model::StageProvider
{
public:
    virtual const char* after();

    virtual const char* before();

    virtual const char* stage();
};
} // namespace stageexample

#endif // GTF_LATE_STAGE_PROVIDER_H
