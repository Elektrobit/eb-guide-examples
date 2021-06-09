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
#include <gtf/configuration/Settings.h>
#include "include/export/global/events/group_Application.h"
#include <gtf/eventsystem/ScalarValue.h>
#include "include/export/local/datapool/items.h"
#include <gtf/datapool/ScalarValue.h>
#include <stdio.h>
#include <gtf/configuration/ScalarValue.h>
#include <gtf/version/config.h>
#include <gtf/decoder/ImagePath.h>

static uint32_t const LOOP_TASK_DELAY = 500;

namespace applicationinteractiondemoexample {
RunStageImpl::RunStageImpl(gtf::dependencyresolver::DependencyContainerHandle const& containerToUse,
                                   gtf::osal::MainTaskDispatcherHandle const&                taskDispatcherToUse,
                                   gtf::datapool::DatapoolHandle const&                      datapoolToUse,
                                   gtf::eventsystem::global::EventQueueHandle const&         eventQueueToUse,
                                   gtf::configuration::SettingsHandle const&                 settingsToUse)
    : container(containerToUse)
    , taskDispatcher(taskDispatcherToUse)
    , datapool(datapoolToUse)
    , eventQueue(eventQueueToUse)
    , settings(settingsToUse)
    , windowHandle(NULL)
    , loopCounter(0)
{
    // setup the datapool and event processing task
    gtf::utils::delegate::Delegate<void()> const interactionCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::processInteraction>(this);
    processInteractionTask.setExecuteCallback(interactionCallback);

    // setup the loop processing task
    gtf::utils::delegate::Delegate<void()> const loopCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::processLoop>(this);
    processLoopTask.setExecuteCallback(loopCallback);

    // retrieve a handle to a Factory
    factory = container->get<gtf::resourcesystem::Factory>();

    if (!factory.valid())
    {
        return;
    }

    //build an imageFilePath to an external image
    imagePath = getPathToFile("ImageFile");

    // subscribe to the event, in order to recieve a notification in the handleEvents function
    eventQueue->subscribe(gtf::eventsystem::SubscriptionType::Event, GLOBAL_EVENT_GROUP_APPLICATION, GLOBAL_EVENT_MSG_ID_EVINTERPLAYOUT, this, gtf::eventsystem::global::EventQueue::Subscription::create<RunStageImpl, &RunStageImpl::handleEvents>(this));
    gtf::utils::delegate::Delegate<void()> const invokerCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::invoker>(this);
    eventQueue->setInvoker(invokerCallback);

    // initialize the datapool
    initializeDatapool();

    // initialize windowed list values
    datapool->update();
    updateWindowList();
    datapool->commit();

    // read the initial values of all items in order to receive further notifications
    readAllDatapoolItems();

    // process initial notifications
    invoker();

    // start the loop task
    taskDispatcher->performTaskDelayed(processLoopTask, LOOP_TASK_DELAY);
}

RunStageImpl::~RunStageImpl()
{
    //unsubscribe from event
    eventQueue->unsubscribe(gtf::eventsystem::SubscriptionType::Event, GLOBAL_EVENT_GROUP_APPLICATION, GLOBAL_EVENT_MSG_ID_EVINTERPLAYOUT, this);
    eventQueue.reset();

    //cancel ongoing tasks
    if (taskDispatcher)
    {
        taskDispatcher->cancelTask(processInteractionTask);
        taskDispatcher->cancelTask(processLoopTask);
        taskDispatcher.reset();
    }

    //reset handles
    container.reset();
    datapool.reset();
    factory.reset();
    settings.reset();
}

void RunStageImpl::handleEvents(const gtf::eventsystem::global::EventHandle& event)
{
    if (!event.valid())
    {
        return;
    }

    // declare a handle to the event's parameter in order to get it's value
    gtf::eventsystem::IntegerValueHandle myParameterHandle = event->getParameter<gtf::eventsystem::IntegerValue>(0);
    if (!myParameterHandle.valid())
    {
        return;
    }

    int32_t myParameter = myParameterHandle->get();

    // create the answer event to be sent to the model
    gtf::eventsystem::global::EventHandle responseEvent = eventQueue->createEvent(GLOBAL_EVENT_GROUP_APPLICATION, GLOBAL_EVENT_MSG_ID_EVINTERPLAYIN);
    if (!responseEvent.valid())
    {
        return;
    }

    // append the read parameter
    responseEvent->addParameter<gtf::eventsystem::IntegerValue>()->set(myParameter);

    // send the answer event to the model
    eventQueue->sendEvent(responseEvent);
}
void RunStageImpl::initializeDatapool()
{
    //get the path to the datapool description file, in order to load the datapool items
    gtf::configuration::PathValueHandle descriptionFile = getPathToFile("datapoolFile");
    if (!descriptionFile.valid())
    {
        return;
    }

    gtf::datapool::Loader& loader = datapool->getLoader();
    loader.load(descriptionFile->get());

    datapool->updateAndCommit();

    gtf::utils::delegate::Delegate<void()> const invokerCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::invoker>(this);
    datapool->setInvoker(invokerCallback);
}

void RunStageImpl::readValueAtIndex(uint32_t index)
{
    gtf::datapool::ValueHandle value = datapool->getItemByIndex<gtf::datapool::Value>(index);

    // avoid reading items without a valid value, e.g. scripted values
    if (!value.valid())
    {
        return;
    }

    value->read();
}

void RunStageImpl::readAllDatapoolItems()
{
    gtf::container::Array<uint32_t> items;
    datapool->getAllItems(items);

    for (gtf::container::Array<uint32_t>::Iterator it = items.begin(); it != items.end(); ++it)
    {
        uint32_t index = *it;
        readValueAtIndex(index);
    }
}

void RunStageImpl::invoker()
{
    //start the datapool and event processing task
    taskDispatcher->performTask(processInteractionTask);
}

void RunStageImpl::moveIntegerFromSentToReceivedDatapoolItem()
{
    int32_t itemValue = 0;
    bool    success   = readIntegerValue(LOCAL_DP_ID_DPINTERPLAYOUT, itemValue);
    if (!success)
    {
        return;
    }

    writeIntegerValue(LOCAL_DP_ID_DPINTERPLAYIN, itemValue);
}

void RunStageImpl::processInteraction()
{
    //get latest values
    datapool->update();

    gtf::container::Array<uint32_t> notifications;
    datapool->getNotifications(notifications);

    for (gtf::container::Array<uint32_t>::Iterator it = notifications.begin(); it != notifications.end(); ++it)
    {
        uint32_t const index = *it;
        switch (index)
        {
        case LOCAL_DP_ID_DPINTERPLAYOUT:
        {
            moveIntegerFromSentToReceivedDatapoolItem();
            break;
        }

        // update the windowed list when the datapool items bellow change
        case LOCAL_DP_ID_DPWINDOWPOSOUT:
        case LOCAL_DP_ID_DPWINDOWSIZEOUT:
            updateWindowList();
            break;
        default:
        {
            break;
        }
        }
    }

    // commit all changed properties
    datapool->commit();

    //release all event from the queue
    eventQueue->dispatch();
}

bool RunStageImpl::readIntegerValue(uint32_t datapoolIndex, int32_t& value)
{
    gtf::datapool::IntegerValueHandle valueHandle = datapool->getItemByIndex<gtf::datapool::IntegerValue>(datapoolIndex);
    if (!valueHandle.valid())
    {
        return false;
    }

    valueHandle->get(value);
    return true;
}

bool RunStageImpl::writeIntegerValue(uint32_t datapoolIndex, int32_t value)
{
    gtf::datapool::IntegerValueHandle valueHandle = datapool->getItemByIndex<gtf::datapool::IntegerValue>(datapoolIndex);
    if (!valueHandle.valid())
    {
        return false;
    }

    valueHandle->set(value);
    return true;
}

void RunStageImpl::updateWindowList()
{
    uint32_t listLength = 999;
    // read the window position
    int32_t windowPosition = 0;
    if (!readIntegerValue(LOCAL_DP_ID_DPWINDOWPOSOUT, windowPosition))
    {
        return;
    }

    // read the window size
    int32_t windowSize = 0;
    if (!readIntegerValue(LOCAL_DP_ID_DPWINDOWSIZEOUT, windowSize))
    {
        return;
    }

    // set a handle to the List
    gtf::datapool::StringListHandle windowList = datapool->getItemByIndex<gtf::datapool::StringList>(LOCAL_DP_ID_DPWINDOWLISTIN);
    if (!windowList.valid())
    {
        return;
    }

    if (windowList->getImplType() != gtf::datapool::Item::ImplType::WindowedList)
    {
        windowList->reset(gtf::datapool::List::Mode::Windowed);
    }

    // resize the list
    windowList->resize(listLength);

    //create a window
    if (!windowHandle.valid())
    {
        windowHandle = windowList->createWindow();
    }

    if (!windowHandle.valid())
    {
        return;
    }

    // set the corresponding position and size of the window
    windowHandle->set(windowPosition, windowSize);

    uint32_t endPosition = MIN(listLength, static_cast<uint32_t>(windowPosition + windowSize));

    // update the window content
    for (uint32_t currentPosition = windowPosition; currentPosition < endPosition; ++currentPosition)
    {
        switch (currentPosition)
        {
        case 0:
        {
            windowList->setElement(currentPosition, GTF_VERSION_STRING);
            break;
        }
        case 1:
        {
            windowList->setElement(currentPosition, GTF_VERSION_COMPANY);
            break;
        }

        case 2:
        {
            windowList->setElement(currentPosition, GTF_PRODUCT_NAME_STRING);
            break;
        }
        default:
        {
            char tmpBufferUTF8[50];
            sprintf(tmpBufferUTF8, "line %d", currentPosition);
            windowList->setElement(currentPosition, tmpBufferUTF8);
            break;
        }
        }
    }
}

void RunStageImpl::changeBooleanValues()
{
    bool value = ((loopCounter % 2) != 0);

    gtf::datapool::BoolValueHandle booleanValue = datapool->getItemByIndex<gtf::datapool::BoolValue>(LOCAL_DP_ID_DPUPDATESBOOLIN);
    if (!booleanValue.valid())
    {
        return;
    }

    booleanValue->set(value);
}

void RunStageImpl::changeIntValues()
{
    int32_t value = static_cast<int32_t>(loopCounter);

    gtf::datapool::IntegerValueHandle integerValue = datapool->getItemByIndex<gtf::datapool::IntegerValue>(LOCAL_DP_ID_DPUPDATESINT32IN);
    if (!integerValue.valid())
    {
        return;
    }

    integerValue->set(value);
}

void RunStageImpl::changeStringValues()
{
    char tmpBufferUTF8[50];
    sprintf(tmpBufferUTF8, "0x%X", loopCounter);

    gtf::datapool::StringValueHandle stringValue = datapool->getItemByIndex<gtf::datapool::StringValue>(LOCAL_DP_ID_DPUPDATESSTRINGIN);
    if (!stringValue.valid())
    {
        return;
    }

    stringValue->set(tmpBufferUTF8);
}

void RunStageImpl::loadExternalImage()
{
    if (!imagePath.valid())
    {
        return;
    }

    gtf::resourcesystem::ResourceHandle image = factory->createResource();
    if (!image)
    {
        return;
    }

    const gtf::resourcesystem::FormatHandle imagePathFormat = factory->createFormat(gtf::decoder::ImagePath::getFormatName());
    if (!imagePathFormat)
    {
        return;
    }

    gtf::resourcesystem::DataHandle imageData = factory->createData(imagePathFormat);
    if (!imageData)
    {
        return;
    }

    gtf::decoder::ImagePath imagePathSetter(imageData);

    //set the path of the resource
    if (!imagePathSetter.setPath(imagePath->get()))
    {
        return;
    }

    if (!image->attachData(imageData))
    {
        return;
    }

    // write the image handle into the datapool
    gtf::datapool::ResourceValueHandle imageHandle = datapool->getItemByIndex<gtf::datapool::ResourceValue>(LOCAL_DP_ID_DPUPDATESEXTERNALIMAGEIN);
    if (!imageHandle.valid())
    {
        return;
    }

    imageHandle->set(image);
}

void RunStageImpl::loadDynamicImage()
{
    gtf::resourcesystem::ResourceHandle image = factory->createResource();
    if (!image.valid())
    {
        return;
    }

    const gtf::resourcesystem::FormatHandle memImageFormat = factory->createFormat(gtf::decoder::MemImage::getFormatName());
    if (!memImageFormat.valid())
    {
        return;
    }

    gtf::resourcesystem::DataHandle imageData = factory->createData(memImageFormat);
    if (!imageData.valid())
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

    if (!memImageSetter.setInfo(memImageInfo))
    {
        return;
    }

    //attach the DataHandle that contains the mandelbrotImage to the ResourceHandle.
    if (!image->attachData(imageData))
    {
        return;
    }

    // create a ResourceValueHandle for loading the image into the datapool
    gtf::datapool::ResourceValueHandle imageHandle = datapool->getItemByIndex<gtf::datapool::ResourceValue>(LOCAL_DP_ID_DPUPDATESIMAGEIN);

    if (!imageHandle.valid())
    {
        return;
    }

    imageHandle->set(image);
}

void RunStageImpl::processLoop()
{
    ++loopCounter;

    changeBooleanValues();

    changeIntValues();

    changeStringValues();

    loadExternalImage();

    loadDynamicImage();

    datapool->commit();

    taskDispatcher->performTaskDelayed(processLoopTask, LOOP_TASK_DELAY);
}

gtf::configuration::PathValueHandle RunStageImpl::getPathToFile(const char* configurationItem)
{
    // load the an external image extern.png
    gtf::configuration::StringScalarHandle basePath = settings->getItem<gtf::configuration::StringScalar>("gtf.model.default.path");
    if (!basePath.valid())
    {
        return gtf::configuration::PathValueHandle();
    }
    // retrieve the configuration value of the configuration  item in the model.json
    gtf::configuration::PathValueHandle filePath = settings->getItem<gtf::configuration::PathValue>(configurationItem);
    if (!filePath.valid())
    {
        return gtf::configuration::PathValueHandle();
    }

    //set the complete path to the image
    filePath->setBasePath(basePath->get());
    return filePath;
}

} // namespace applicationinteractiondemoexample
