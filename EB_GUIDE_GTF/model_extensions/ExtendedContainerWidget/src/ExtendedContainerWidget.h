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

#ifndef GTF_EXTENDED_CONTAINER_WIDGET_H_INCLUDED
#define GTF_EXTENDED_CONTAINER_WIDGET_H_INCLUDED

#include <gtf/properties/Container.h>
#include <gtf/properties/PropertyObserver.h>
#include <gtf/properties/ScalarValue.h>
#include <gtf/scene/TypeResolver.h>
#include <gtf/scene/element/Controller.h>

namespace extendedcontainerwidget
{
    class ExtendedContainerWidget : public gtf::scene::element::Controller
    {
    public:
        ExtendedContainerWidget(const gtf::scene::TypeResolverHandle& resolver_, gtf::properties::PropertyObserverHandle& observer_);

        ~ExtendedContainerWidget();

        virtual void connect(gtf::properties::Container& model_);

        virtual void disconnect();

    protected:
        bool handleUpdate(const gtf::properties::PropertyKey& key);

    private:

        gtf::properties::PropertyObserverHandle observer;
        gtf::properties::ContainerHandle container;
        gtf::properties::IntegerValueHandle displayStateHandle;

        struct PropertyReferences
        {
            enum Value
            {
                DisplayStatus = 0,
                IsVisible = 1,
                Count = 2
            };
        };

        gtf::scene::TypeResolver::ContainerPropertyReference widgetPropRefs[PropertyReferences::Count];
    };
}
GTF_DEFINE_INTERFACE_NAME(extendedcontainerwidget::ExtendedContainerWidget)
#endif // GTF_EXTENDED_CONTAINER_WIDGET_H_INCLUDED
