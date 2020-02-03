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

#include "RunStageImpl.h"
#include <gtf/configuration/ScalarValue.h>
#include <gtf/datapool/ScalarValue.h>
#include <gtf/decoder/MemImage.h>
#include <gtf/decoder/ImagePath.h>

#include "include/export/local/datapool/items.h"

static uint32_t const LOOP_TASK_DELAY = 500;

namespace imagedataexample {
bool RunStageImpl::getDependencies()
{
    if (false == container.valid())
    {
        return false;
    }

    taskDispatcher = container->get<gtf::osal::MainTaskDispatcher>();
    if (false == taskDispatcher.valid())
    {
        return false;
    }

    datapool = container->get<gtf::datapool::Datapool>();
    if (false == datapool.valid())
    {
        return false;
    }

    settings = container->get<gtf::configuration::Settings>();
    if (false == settings.valid())
    {
        return false;
    }

    factory = container->get<gtf::resourcesystem::Factory>();
    if (false == factory.valid())
    {
        return false;
    }

    return true;
}

RunStageImpl::RunStageImpl(const gtf::dependencyresolver::DependencyContainerHandle& container_)
    : container(container_)
    , loopCounter(0)
{
    if (getDependencies() == true)
    {
        // setup the loop processing task
        gtf::utils::delegate::Delegate<void()> const loopCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::processLoop>(this);
        processLoopTask.setExecuteCallback(loopCallback);

        //build an imageFilePath to an external image
        imagePath = getPathToFile("ImageFile");

        initializeDatapool();
        // read the initial values of all items in order to receive further notifications
        readAllDatapoolItems();

        // start the loop task
        taskDispatcher->performTaskDelayed(processLoopTask, LOOP_TASK_DELAY);
    }
}

RunStageImpl::~RunStageImpl()
{
    //cancel ongoing tasks
    if (taskDispatcher)
    {
        taskDispatcher->cancelTask(processLoopTask);
        taskDispatcher.reset();
    }

    //reset handles
    container.reset();
    datapool.reset();
    factory.reset();
    settings.reset();
}

void RunStageImpl::initializeDatapool()
{
    //get the path to the datapool description file, in order to load the datapool items
    gtf::configuration::PathValueHandle descriptionFile = getPathToFile("datapoolFile");
    if (false == descriptionFile.valid())
    {
        return;
    }

    gtf::datapool::Loader& loader = datapool->getLoader();
    loader.load(descriptionFile->get());

    datapool->update();
}

void RunStageImpl::readValueAtIndex(uint32_t index)
{
    gtf::datapool::ValueHandle value = datapool->getItemByIndex<gtf::datapool::Value>(index);

    // avoid reading items without a valid value, e.g. scripted values
    if (false == value.valid())
    {
        return;
    }

    value->read();
}

void RunStageImpl::readAllDatapoolItems()
{
    gtf::container::Array<uint32_t> items;

    if (false == datapool->getAllItems(items))
    {
        return;
    }

    for (gtf::container::Array<uint32_t>::Iterator it = items.begin(); it != items.end(); ++it)
    {
        uint32_t index = *it;
        readValueAtIndex(index);
    }
}

void RunStageImpl::loadExternalImage()
{
    if (false == imagePath.valid())
    {
        return;
    }

    gtf::resourcesystem::ResourceHandle image = factory->createResource();
    if (false == image)
    {
        return;
    }

    const gtf::resourcesystem::FormatHandle imagePathFormat = factory->createFormat(gtf::decoder::ImagePath::getFormatName());
    if (false == imagePathFormat)
    {
        return;
    }

    gtf::resourcesystem::DataHandle imageData = factory->createData(imagePathFormat);
    if (false == imageData)
    {
        return;
    }

    gtf::decoder::ImagePath imagePathSetter(imageData);

    //set the path of the resource
    if (false == imagePathSetter.setPath(imagePath->get()))
    {
        return;
    }

    if (false == image->attachData(imageData))
    {
        return;
    }

    // write the image handle into the datapool
    gtf::datapool::ResourceValueHandle imageHandle = datapool->getItemByIndex<gtf::datapool::ResourceValue>(LOCAL_DP_ID_DPUPDATESEXTERNALIMAGEIN);
    if (false == imageHandle.valid())
    {
        return;
    }

    imageHandle->set(image);
}

void RunStageImpl::loadDynamicImage()
{
    gtf::resourcesystem::ResourceHandle image = factory->createResource();
    if (false == image.valid())
    {
        return;
    }

    const gtf::resourcesystem::FormatHandle memImageFormat = factory->createFormat(gtf::decoder::MemImage::getFormatName());
    if (false == memImageFormat.valid())
    {
        return;
    }

    gtf::resourcesystem::DataHandle imageData = factory->createData(memImageFormat);
    if (false == imageData.valid())
    {
        return;
    }

    gtf::decoder::MemImage memImageSetter(imageData);

    
    gtf::decoder::MemImage::Info memImageInfo = imageCreator.createImageInfo();

    memImageInfo.pixelData = static_cast<uint8_t*>(memImageSetter.allocData(memImageInfo.pitch));
    if (NULL == memImageInfo.pixelData)
    {
        return;
    }

    imageCreator.createMandelbrotImage(memImageInfo, loopCounter);
    
    if (false == memImageSetter.setInfo(memImageInfo))
    {
        return;
    }

    //attach the DataHandle that contains the mandelbrotImage to the ResourceHandle.
    if (false == image->attachData(imageData))
    {
        return;
    }

    // create a ResourceValueHandle for loading the image into the datapool
    gtf::datapool::ResourceValueHandle imageHandle = datapool->getItemByIndex<gtf::datapool::ResourceValue>(LOCAL_DP_ID_DPUPDATESIMAGEIN);

    if (false == imageHandle.valid())
    {
        return;
    }

    imageHandle->set(image);
}

void RunStageImpl::processLoop()
{
    ++loopCounter;

    loadExternalImage();

    loadDynamicImage();

    datapool->commit();

    taskDispatcher->performTaskDelayed(processLoopTask, LOOP_TASK_DELAY);
}

gtf::configuration::PathValueHandle RunStageImpl::getPathToFile(const char* configurationItem)
{
    // load the an external image extern.png
    gtf::configuration::StringScalarHandle basePath = settings->getItem<gtf::configuration::StringScalar>("gtf.model.default.path");
    if (false == basePath.valid())
    {
        return gtf::configuration::PathValueHandle();
    }
    // retrieve the configuration value of the configuration  item in the model.json
    gtf::configuration::PathValueHandle filePath = settings->getItem<gtf::configuration::PathValue>(configurationItem);
    if (false == filePath.valid())
    {
        return gtf::configuration::PathValueHandle();
    }

    //set the complete path to the image
    filePath->setBasePath(basePath->get());
    return filePath;
}
} // namespace imagedataexample
