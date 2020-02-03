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
#include <gtf/datapool/Value.h>
#include "include/export/local/datapool/items.h"
#include <gtf/datapool/ScalarValue.h>
#include <gtf/datapool/ListValue.h>

namespace datapoolexample {
RunStageImpl::RunStageImpl(gtf::configuration::SettingsHandle const&  settingsToUse,
                                   gtf::osal::MainTaskDispatcherHandle const& taskDispatcherToUse,
                                   gtf::datapool::DatapoolHandle const&       datapoolToUse)
    : settings(settingsToUse)
    , taskDispatcher(taskDispatcherToUse)
    , datapool(datapoolToUse)
{
    // setup the datapool processing task
    gtf::utils::delegate::Delegate<void()> const processDatapoolCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::processDatapool>(this);
    processDatapoolTask.setExecuteCallback(processDatapoolCallback);

    initializeDatapool();
    // read the initial values of all items in order to receive further notifications
    readAllDatapoolItems();
}

RunStageImpl::~RunStageImpl()
{
    // reset all handle
    settings.reset();
    datapool.reset();
    taskDispatcher->cancelTask(processDatapoolTask);
    taskDispatcher.reset();
}

void RunStageImpl::initializeDatapool()
{
    //get the path to the datapool description file, in order to load the datapool items
    gtf::configuration::PathValueHandle descriptionFile = getDatapoolDescription();
    if (false == descriptionFile.valid())
    {
        return;
    }

    gtf::datapool::Loader& loader = datapool->getLoader();
    loader.load(descriptionFile->get());

    gtf::utils::delegate::Delegate<void()> const invokerCallback = gtf::utils::delegate::Delegate<void()>::create<RunStageImpl, &RunStageImpl::invoker>(this);
    datapool->setInvoker(invokerCallback);

    datapool->update();
}

gtf::configuration::PathValueHandle RunStageImpl::getDatapoolDescription()
{
    // get the path to the model
    gtf::configuration::StringScalarHandle basePath = settings->getItem<gtf::configuration::StringScalar>("gtf.model.default.path");
    if (false == basePath.valid())
    {
        return gtf::configuration::PathValueHandle();
    }
    // retrieve the configuration value of the "datapoolFile" item in the model.json
    gtf::configuration::PathValueHandle filePath = settings->getItem<gtf::configuration::PathValue>("datapoolFile");
    if (false == filePath.valid())
    {
        return gtf::configuration::PathValueHandle();
    }

    filePath->setBasePath(basePath->get());
    return filePath;
}

void RunStageImpl::readAllDatapoolItems()
{
    gtf::container::Array<uint32_t> items;

    if (false == datapool->getAllItems(items))
    {
        return;
    }

    for (gtf::container::Array<uint32_t>::Iterator it = items.begin(), e = items.end(); it != e; ++it)
    {
        uint32_t index = *it;
        readValueAtIndex(index);
    }
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

void RunStageImpl::processDatapool()
{
    //get latest values
    datapool->update();

    gtf::container::Array<uint32_t> notifications;
    if (false == datapool->getNotifications(notifications))
    {
        return;
    }

    //iterate through all notifications and call the appropriate handlers
    for (gtf::container::Array<uint32_t>::Iterator it = notifications.begin(), e = notifications.end(); it != e; ++it)
    {
        uint32_t const index = *it;
        switch (index)
        {
        case LOCAL_DP_ID_MYSENTVALUE:
        {
            moveIntegerFromSentToReceivedDatapoolItem();
            break;
        }
        case LOCAL_DP_ID_MYSENTLIST:
        {
            moveIntegerListFromSentToDoubledReceived();
            break;
        }
        default:
        {
            break;
        }
        }
    }

    //commit changed values
    datapool->commit();
}

void RunStageImpl::invoker()
{
    //start the datapool processing task
    taskDispatcher->performTask(processDatapoolTask);
}

void RunStageImpl::moveIntegerFromSentToReceivedDatapoolItem()
{
    gtf::datapool::IntegerValueHandle valueHandle = datapool->getItemByIndex<gtf::datapool::IntegerValue>(LOCAL_DP_ID_MYSENTVALUE);
    if (false == valueHandle.valid())
    {
        return;
    }

    // write the value to DP_ID_myReceivedValue
    gtf::datapool::IntegerValueHandle responseValueHandle = datapool->getItemByIndex<gtf::datapool::IntegerValue>(LOCAL_DP_ID_MYRESPONSEVALUE);
    if (false == responseValueHandle.valid())
    {
        return;
    }

    // read the value from DP_ID_mySentValue
    int32_t const itemValue = valueHandle->get();
    responseValueHandle->set(itemValue);
}

void RunStageImpl::moveIntegerListFromSentToDoubledReceived()
{
    // get sent list handle
    gtf::datapool::IntegerListHandle sentList = datapool->getItemByIndex<gtf::datapool::IntegerList>(LOCAL_DP_ID_MYSENTLIST);
    if (false == sentList.valid())
    {
        return;
    }

    //get response list handle
    gtf::datapool::IntegerListHandle responseList = datapool->getItemByIndex<gtf::datapool::IntegerList>(LOCAL_DP_ID_MYRESPONSELIST);
    if (false == responseList.valid())
    {
        return;
    }

    //clear response list in case it already contains items
    responseList->resize(0);

    //iterate over sent list and append doubled values to response list
    uint32_t const size = sentList->size();
    for (uint32_t i = 0; i < size; i++)
    {
        int32_t const currentElement = sentList->getElement(i);
        if (currentElement != 0)
        {
            responseList->append(2 * currentElement);
        }
    }
}
} // namespace datapoolexample
