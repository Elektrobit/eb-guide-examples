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
#include <gtf/configuration/ScalarValue.h>
#include <gtf/datapool/Value.h>
#include <gtf/datapool/ScalarValue.h>
#include <gtf/datapool/ListValue.h>

#include <stdio.h>

#include "include/export/local/datapool/items.h"
#include "RunStageImpl.h"
#include <gtf/version/config.h>

namespace windowlistexample {
RunStageImpl::RunStageImpl(gtf::configuration::SettingsHandle const&  settingsToUse,
                                   gtf::osal::MainTaskDispatcherHandle const& taskDispatcherToUse,
                                   gtf::datapool::DatapoolHandle const&       datapoolToUse)
    : settings(settingsToUse)
    , taskDispatcher(taskDispatcherToUse)
    , datapool(datapoolToUse)
    , windowHandle(NULL)
{
    // setup the datapool processing task
    gtf::utils::delegate::Delegate<void()> const processDatapoolCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::processDatapoolItems>(this);
    processDatapoolTask.setExecuteCallback(processDatapoolCallback);

    initializeDatapool();
    updateWindowList();
    datapool->commit();
}

RunStageImpl::~RunStageImpl()
{
}

void RunStageImpl::invoker()
{
    //start the datapool processing task
    taskDispatcher->performTask(processDatapoolTask);
}

void RunStageImpl::initializeDatapool()
{
    //get the path to the datapool description file, in order to load the datapool items
    gtf::configuration::PathValueHandle descriptionFile = getDatapoolDescription();
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

gtf::configuration::PathValueHandle RunStageImpl::getDatapoolDescription()
{
    // get the path to the model
    gtf::configuration::StringScalarHandle basePath = settings->getItem<gtf::configuration::StringScalar>("gtf.model.default.path");
    if (!basePath.valid())
    {
        return gtf::configuration::PathValueHandle();
    }
    // retrieve the configuration value of the "datapoolFile" item in the model.json
    gtf::configuration::PathValueHandle filePath = settings->getItem<gtf::configuration::PathValue>("datapoolFile");
    if (!filePath.valid())
    {
        return gtf::configuration::PathValueHandle();
    }

    filePath->setBasePath(basePath->get());
    return filePath;
}

void RunStageImpl::readValueAtIndex(const uint32_t index)
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

    if (!datapool->getAllItems(items))
    {
        return;
    }

    for (gtf::container::Array<uint32_t>::Iterator it = items.begin(); it != items.end(); ++it)
    {
        uint32_t index = *it;
        readValueAtIndex(index);
    }
}

void RunStageImpl::processDatapoolItems()
{
    //get latest values
    datapool->update();

    gtf::container::Array<uint32_t> notifications;
    if (!datapool->getNotifications(notifications))
    {
        return;
    }

    for (gtf::container::Array<uint32_t>::Iterator it = notifications.begin(); it != notifications.end(); ++it)
    {
        uint32_t const index = *it;
        switch (index)
        {
        // update the windowed list when the datapool items bellow change
        case LOCAL_DP_ID_WINDOWPOSITION:
        case LOCAL_DP_ID_WINDOWSIZE:
        case LOCAL_DP_ID_WINDOWLISTSIZE:
            updateWindowList();
        default:
            break;
        }
    }

    // commit all changed properties
    datapool->commit();
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

void RunStageImpl::updateWindowList()
{
    // read the window position
    int32_t windowPosition = 0;
    if (!readIntegerValue(LOCAL_DP_ID_WINDOWPOSITION, windowPosition))
    {
        return;
    }

    // read the window size
    int32_t windowSize = 0;
    if (!readIntegerValue(LOCAL_DP_ID_WINDOWSIZE, windowSize))
    {
        return;
    }

    // read the length of the list
    int32_t listLength = 0;
    if (!readIntegerValue(LOCAL_DP_ID_WINDOWLISTSIZE, listLength))
    {
        return;
    }

    gtf::datapool::StringListHandle windowList = datapool->getItemByIndex<gtf::datapool::StringList>(LOCAL_DP_ID_MYRECEIVEDWINDOWLIST);
    if (!windowList.valid())
    {
        return;
    }

    // set the type of the list to Windowed
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

    const uint32_t endPosition = MIN(static_cast<uint32_t>(listLength), static_cast<uint32_t>(windowPosition + windowSize));

    // update the window content
    for (uint32_t currentPosition = windowPosition; currentPosition < endPosition; currentPosition++)
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
} // namespace windowlistexample

