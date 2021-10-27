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
#include "ExtendedContainerWidget.h"
#include <gtf/properties/ScalarValue.h>


extendedcontainerwidget::ExtendedContainerWidget::ExtendedContainerWidget(const gtf::scene::TypeResolverHandle& resolver_, gtf::properties::PropertyObserverHandle& observer_)
    : observer(observer_)
{
    // Define the property references that are used in the implementation.
    widgetPropRefs[PropertyReferences::DisplayStatus].containerType.name =
        "GtfExtendedContainerWidget";
    widgetPropRefs[PropertyReferences::DisplayStatus].property.name =
        "displayStatus";
    widgetPropRefs[PropertyReferences::IsVisible].containerType.name =
        "GtfAbstractVisualWidget";
    widgetPropRefs[PropertyReferences::IsVisible].property.name = "visible";

    resolver_->resolveContainerProperties(widgetPropRefs, PropertyReferences::Count);
}

extendedcontainerwidget::ExtendedContainerWidget::~ExtendedContainerWidget()
{ // nothing to do
}

void extendedcontainerwidget::ExtendedContainerWidget::connect(gtf::properties::Container& model_) {

    container = &model_;

    gtf::properties::PropertyKey displayStatusKey = gtf::properties::PropertyKey(container, widgetPropRefs[PropertyReferences::DisplayStatus].property.identifier);

    gtf::properties::Properties& widgetProperties = container->getProperties();

    displayStateHandle = widgetProperties.get < gtf::properties::IntegerValue>(widgetPropRefs[PropertyReferences::DisplayStatus].property.identifier);
    if (!displayStateHandle)
    {
        return;
    }

    observer->subscribe(displayStatusKey, this, gtf::properties::PropertyObserver::Callback::create<ExtendedContainerWidget, &ExtendedContainerWidget::handleUpdate>(this));

    handleUpdate(displayStatusKey);
}

void extendedcontainerwidget::ExtendedContainerWidget::disconnect() {

    gtf::properties::PropertyKey displayStatusKey = gtf::properties::PropertyKey(container, widgetPropRefs[PropertyReferences::DisplayStatus].property.identifier);
    observer->unsubscribe(displayStatusKey, this);

    container.reset();
    displayStateHandle.reset();
}

bool extendedcontainerwidget::ExtendedContainerWidget::handleUpdate(const gtf::properties::PropertyKey& key)
{
    GTF_UNUSED_PARAM(key);
    // Read the current value of the "displayStatus" property.
    int32_t displayState = displayStateHandle->get();

    gtf::properties::Children & children = container->getChildren();
    uint32_t childCount = children.count();

    for (uint16_t i = 0; i < childCount; ++i)
    {
        gtf::properties::ContainerHandle curChild = children.get(i);
        bool widgetVisible = false;

        if (displayState == 0)
        { // ALL: All child widgets are visible.
            widgetVisible = true;
        }
        else if ((displayState == 1) && (i == 0))
        { // FIRST: Set first child to visible.
            widgetVisible = true;
        }
        else
        { // NONE: Set no child to visible.
            widgetVisible = false;
        }

        // commit visibility change for every child widget
        gtf::properties::Properties& childProperties = curChild->getProperties();

        gtf::properties::BoolValueHandle childVisibility = childProperties.get<gtf::properties::BoolValue>(widgetPropRefs[PropertyReferences::IsVisible].property.identifier);

        if (!childVisibility.valid())
        {
            continue;
        }
        childVisibility->set(widgetVisible);
    }
    return true;
}

