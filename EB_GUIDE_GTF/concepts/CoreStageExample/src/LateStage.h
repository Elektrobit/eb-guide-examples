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

#ifndef GTF_LATE_STAGE_H
#define GTF_LATE_STAGE_H

#include <gtf/launcher/Stage.h>

namespace corestageexample {
class LateStage : public gtf::launcher::Stage
{
public:
    LateStage();

    ~LateStage();
};
} // namespace corestageexample

GTF_DEFINE_INTERFACE_NAME(corestageexample::LateStage);
#endif // GTF_LATE_STAGE_H
