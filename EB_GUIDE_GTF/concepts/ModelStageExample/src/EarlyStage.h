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

#ifndef GTF_EARLY_STAGE_H
#define GTF_EARLY_STAGE_H

#include <gtf/model/Stage.h>

namespace stageexample {
class EarlyStage : public gtf::model::Stage
{
public:
    EarlyStage();

    ~EarlyStage();
};
} // namespace stageexample
GTF_DEFINE_INTERFACE_NAME(stageexample::EarlyStage)
#endif // GTF_EARLY_STAGE_H
