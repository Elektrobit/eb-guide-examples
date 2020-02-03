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

#include "LateStageProvider.h"
#include "LateStage.h"
#include <gtf/launcher/RunStage.h>

const char* corestageexample::LateStageProvider::after()
{
    return gtf::dependencyresolver::InterfaceName<gtf::launcher::RunStage>::name();
}

const char* corestageexample::LateStageProvider::before()
{
    return NULL;
}

const char* corestageexample::LateStageProvider::stage()
{
    return gtf::dependencyresolver::InterfaceName<LateStage>::name();
}
