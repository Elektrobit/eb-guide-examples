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
#include "BufferSwapHooks.h"
#include "Tracing.h"

void BufferSwapHooksExample::preSwap(uint32_t)
{
    GTF_NOTICE(GTF_Buffer_Swap_Hooks, "Buffers are about to be swapped.");
}

void BufferSwapHooksExample::postSwap(uint32_t)
{
    GTF_NOTICE(GTF_Buffer_Swap_Hooks, "Buffers have just been swapped.");
}

BufferSwapHooksExample::~BufferSwapHooksExample()
{
}
