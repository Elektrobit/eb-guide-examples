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

#include "FocusPolicyWidgetFeature.h"
#include <gtf/properties/Container.h>
#include <stdlib.h>

focusedpolicywidgetfeature::FocusPolicyFeature::FocusPolicyFeature(const gtf::properties::ContainerObserverHandle& observer_)
    : observer(observer_)
    , currentFocusedChildIndex(gtf::types::limits::uint32_max)
    , lastFocusedChildIndex(gtf::types::limits::uint32_max)
    , childCount(gtf::types::limits::uint32_max)
{
}

focusedpolicywidgetfeature::FocusPolicyFeature::~FocusPolicyFeature()
{
}

void focusedpolicywidgetfeature::FocusPolicyFeature::connect(gtf::properties::Container& model_)
{
    model = &model_;
    observer->subscribe(model, this, gtf::utils::delegate::Delegate<void(const gtf::properties::ContainerHandle&, const gtf::container::Array<gtf::properties::ContainerObserver::SubscriberNotificationData>&)>::create<FocusPolicyFeature, &FocusPolicyFeature::structureUpdate>(this));
}

void focusedpolicywidgetfeature::FocusPolicyFeature::disconnect()
{
    observer->unsubscribe(model, this);
    model.reset();
    manager.reset();
}

void focusedpolicywidgetfeature::FocusPolicyFeature::findSuitableRandomChild()
{
    if (!model)
    {
        return;
    }

    childCount = model->getChildren().count();
    if (childCount == 0)
    {
        return;
    }

    lastFocusedChildIndex = currentFocusedChildIndex;

    currentFocusedChildIndex = rand() % childCount;
}

void focusedpolicywidgetfeature::FocusPolicyFeature::structureUpdate(const gtf::properties::ContainerHandle&, const gtf::container::Array<gtf::properties::ContainerObserver::SubscriberNotificationData>&) 
{
    findSuitableRandomChild();
}

void focusedpolicywidgetfeature::FocusPolicyFeature::initialize(gtf::focus::Manager& manager_)
{
    manager = &manager_;

    findSuitableRandomChild();
}

const gtf::properties::ContainerHandle& focusedpolicywidgetfeature::FocusPolicyFeature::getFocusableSubTree() const
{
    if (!model)
    {
        static gtf::properties::ContainerHandle invalid;
        return invalid;
    }

    return model->getChildren().get(currentFocusedChildIndex);
}

bool focusedpolicywidgetfeature::FocusPolicyFeature::next()
{
    findSuitableRandomChild();
    return true;
}

bool focusedpolicywidgetfeature::FocusPolicyFeature::previous()
{
    if (lastFocusedChildIndex == gtf::types::limits::uint32_max)
    {
        return false;
    }

    currentFocusedChildIndex = lastFocusedChildIndex;
    return true;
}

bool focusedpolicywidgetfeature::FocusPolicyFeature::moveFocusTo(const gtf::properties::ContainerHandle& child)
{
    gtf::properties::Children& children = model->getChildren();
    for (uint32_t index = 0; index < childCount; ++index)
    {
        if (child == children.get(index))
        {
            currentFocusedChildIndex = index;
            return true;
        }
    }
    return false;
}
