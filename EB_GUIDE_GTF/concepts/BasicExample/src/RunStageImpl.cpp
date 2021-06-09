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

#include "RunStageImpl.h"
#include "Tracing.h"

namespace basicexample {
RunStageImpl::RunStageImpl()
{
    GTF_INFO(GTF_Basic_Example, "Application successfully started.");
}

RunStageImpl::~RunStageImpl()
{
    GTF_INFO(GTF_Basic_Example, "Application successfully stopped.");
}

} // namespace basicexample
