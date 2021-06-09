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

#ifndef TGA_IMAGE_LOADER_INITIALIZED_STAGE_IMPL_H
#define TGA_IMAGE_LOADER_INITIALIZED_STAGE_IMPL_H

#include <gtf/model/InitializedStage.h>
#include <gtf/resourcesystem/DecodingManager.h>

namespace tgaimageloader
{
    class InitializedStageImpl : public gtf::model::InitializedStage
    {
    public:
        InitializedStageImpl(gtf::container::Array<gtf::resourcesystem::DecoderHandle>& decoders_, const gtf::resourcesystem::DecodingManagerHandle& manager);
        virtual ~InitializedStageImpl();

    private:
        gtf::container::Array<gtf::resourcesystem::DecoderHandle> decoders;
        gtf::resourcesystem::DecodingManagerHandle           manager;
    };
}// namespace tgaimageloader

#endif //TGA_IMAGE_LOADER_INITIALIZED_STAGE_IMPL_H
