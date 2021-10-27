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
#include <gtf/model/LoadedStage.h>
#include <gtf/model/RunningStage.h>

const char* stageexample::LateStageProvider::after()
{
    return gtf::dependencyresolver::InterfaceName<gtf::model::LoadedStage>::name();
}

const char* stageexample::LateStageProvider::before()
{
    return gtf::dependencyresolver::InterfaceName<gtf::model::RunningStage>::name();
}

const char* stageexample::LateStageProvider::stage()
{
    return gtf::dependencyresolver::InterfaceName<LateStage>::name();
}
