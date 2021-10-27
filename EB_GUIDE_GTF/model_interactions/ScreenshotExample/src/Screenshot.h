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

#ifndef GTF_SCREENSHOT_H
#define GTF_SCREENSHOT_H

#include <gtf/scenecapture/Output.h>
#include <gtf/container/Array.h>
#include <gtf/osal/UserspaceMutex.h>

#include <string>

namespace screenshot {

class Screenshot : public gtf::scenecapture::Output
{
public:
    Screenshot();

    Screenshot(uint32_t id);

    virtual bool isActive();
    void         flipImage(gtf::scenecapture::Bitmap const& bitmap);

    virtual void updateScene(uint32_t sceneId_, gtf::scenecapture::Bitmap const& bitmap);

    void activateScreenshot(const char* const path);

private:
    bool     active;
    uint32_t sceneId;

    std::string filePath;

    gtf::container::Array<uint8_t> buffer;
    gtf::osal::UserspaceMutex      mutex;
};

typedef gtf::utils::smartptr::RefCountedPtr<Screenshot> ScreenshotHandle;
} // namespace screenshot
#endif // GTF_EXAMPLE_SCREENSHOT_H
