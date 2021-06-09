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
#include "EarlyStage.h"
#include "Tracing.h"

stageexample::EarlyStage::EarlyStage()
{
    GTF_INFO(GTF_Model_Stage_Example, gtf::dependencyresolver::InterfaceName<EarlyStage>::name() << " stage has been loaded.");
}

stageexample::EarlyStage::~EarlyStage()
{
    GTF_INFO(GTF_Model_Stage_Example, gtf::dependencyresolver::InterfaceName<EarlyStage>::name() << " stage has been unloaded.");
}
