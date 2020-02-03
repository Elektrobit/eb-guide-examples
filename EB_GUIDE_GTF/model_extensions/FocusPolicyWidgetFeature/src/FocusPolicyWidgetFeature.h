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

#ifndef GTF_FOCUSED_POLICY_WIDGET_FEATURE_H_INCLUDED
#define GTF_FOCUSED_POLICY_WIDGET_FEATURE_H_INCLUDED

#include <gtf/focus/Policy.h>
#include <gtf/scene/TypeResolver.h>
#include <gtf/properties/ContainerObserver.h>

namespace focusedpolicywidgetfeature {

class FocusPolicyFeature : public gtf::focus::Policy
{
public:
    FocusPolicyFeature(const gtf::properties::ContainerObserverHandle& observer_);

    virtual ~FocusPolicyFeature();

    virtual void connect(gtf::properties::Container& model_);

    virtual void disconnect();

    void findSuitableRandomChild();

    void structureUpdate(const gtf::properties::ContainerHandle&, gtf::container::Array<gtf::properties::ContainerObserver::SubscriberNotificationData>&);

    virtual void initialize(gtf::focus::Manager& manager_);

    virtual const gtf::properties::ContainerHandle& getFocusableSubTree() const;

    virtual bool next();

    virtual bool previous();

    virtual bool moveFocusTo(const gtf::properties::ContainerHandle& child);

protected:
    gtf::properties::ContainerObserverHandle observer;
    gtf::focus::ManagerHandle        manager;
    gtf::properties::ContainerHandle model;
    uint32_t                         currentFocusedChildIndex;
    uint32_t                         lastFocusedChildIndex;
    uint32_t                         childCount;
};
} // namespace focusedpolicywidgetfeature
GTF_DEFINE_INTERFACE_NAME(focusedpolicywidgetfeature::FocusPolicyFeature)
#endif // GTF_FOCUSED_POLICY_WIDGET_FEATURE_H_INCLUDED
