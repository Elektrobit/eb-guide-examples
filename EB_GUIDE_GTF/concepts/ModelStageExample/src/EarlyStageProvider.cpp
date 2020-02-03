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
#include <gtf/model/InitializedStage.h>
#include <gtf/model/PreparedStage.h>

const char* stageexample::EarlyStageStageProvider::after()
{
    return gtf::dependencyresolver::InterfaceName<gtf::model::PreparedStage>::name();
}

const char* stageexample::EarlyStageStageProvider::before()
{
    return gtf::dependencyresolver::InterfaceName<gtf::model::InitializedStage>::name();
}

const char* stageexample::EarlyStageStageProvider::stage()
{
    return gtf::dependencyresolver::InterfaceName<EarlyStage>::name();
}
