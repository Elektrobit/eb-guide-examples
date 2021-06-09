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

#ifndef GTF_PREPARE_STAGE_IMPL_H
#define GTF_PREPARE_STAGE_IMPL_H

#include <gtf/launcher/PrepareStage.h>

namespace corestageexample {
class PrepareStageImpl : public gtf::launcher::PrepareStage
{
public:
    PrepareStageImpl();

    ~PrepareStageImpl();
};
} // namespace corestageexample

#endif // GTF_PREPARE_STAGE_IMPL_H
