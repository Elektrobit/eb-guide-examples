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

#include "InitializedStageImpl.h"

namespace tgaimageloader
{
    InitializedStageImpl::InitializedStageImpl(gtf::container::Array<gtf::resourcesystem::DecoderHandle>& decoders_, const gtf::resourcesystem::DecodingManagerHandle& manager_)
        : decoders(decoders_)
        , manager(manager_)
    {
        manager->registerDecoders(decoders);
    }

    InitializedStageImpl::~InitializedStageImpl()
    {
        manager->unregisterDecoders(decoders);
    }

} // namespace tgaimageloader

