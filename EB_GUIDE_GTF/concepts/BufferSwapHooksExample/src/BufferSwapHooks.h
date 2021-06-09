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

#ifndef GTF_BUFFER_SWAP_HOOKS_H
#define GTF_BUFFER_SWAP_HOOKS_H

#include <gtf/displayfactory/DisplayHooks.h>

class BufferSwapHooksExample : public gtf::displayfactory::DisplayHooks
{
public:
    virtual void preSwap(uint32_t sceneID_) override;

    virtual void postSwap(uint32_t sceneID_) override;

    virtual ~BufferSwapHooksExample() override;
};
GTF_DEFINE_INTERFACE_NAME(BufferSwapHooksExample)
#endif // GTF_BUFFER_SWAP_HOOKS_H_INCLUDED
