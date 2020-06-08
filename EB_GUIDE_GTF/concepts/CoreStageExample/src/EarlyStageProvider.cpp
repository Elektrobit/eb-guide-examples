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

#include "EarlyStageProvider.h"
#include "EarlyStage.h"
#include <gtf/launcher/RunStage.h>
#include <gtf/launcher/PrepareStage.h>

const char* corestageexample::EarlyStageStageProvider::after()
{
    return gtf::dependencyresolver::InterfaceName<gtf::launcher::PrepareStage>::name();
}

const char* corestageexample::EarlyStageStageProvider::before()
{
    return gtf::dependencyresolver::InterfaceName<gtf::launcher::RunStage>::name();
}

const char* corestageexample::EarlyStageStageProvider::stage()
{
    return gtf::dependencyresolver::InterfaceName<EarlyStage>::name();
}
