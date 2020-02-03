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

#ifndef RIPPLE_EFFECT_DESC_H_INCLUDED
#define RIPPLE_EFFECT_DESC_H_INCLUDED

#include <gtf/metainformation/DescriptorProvider.h>

//!
//! \brief Implementation of the widget descriptor.
//! \Details This descriptor is used by GTF to determine the properties of the custom widget
//!
class RippleEffectDesc : public gtf::metainformation::DescriptorProvider
{
public:

    //!
    //! \brief Retrieves all action definitions contained in this DescriptorProvider.
    //!
    //! \param[out] count_ will be set by the method and determines the count of valid action descriptions.
    //!
    //! \return array of action descriptions, NULL if there are no action descriptions
    //!
    virtual gtf::metainformation::ActionDescriptor const* GetActions(uint32_t& count_) const;

    //!
    //! \brief Retrieves all popup stack definitions contained in this DescriptorProvider.
    //!
    //! \param count_[out] will be set by the method and determines the count of valid popup stack descriptions.
    //!
    //! \return array of popup stack descriptions, NULL if there are no popup stack descriptions.
    //!
    virtual gtf::metainformation::PopupStackDescriptor const* GetPopupStacks(uint32_t& count_) const;

    //!
    //! \brief Retrieves all widget definitions contained in this DescriptorProvider.
    //!
    //! \param count_[out] Will be set by the method and determines the count of valid widget descriptions.
    //!
    //! \return An array of widget descriptions, NULL if there are no widget descriptions.
    //!
    virtual gtf::metainformation::WidgetDescriptor const* GetWidgets(uint32_t& count_) const;

    //!
    //! \brief Retrieves all widget feature definitions contained in this DescriptorProvider.
    //!
    //! \param count_[out] Will be set by the method and determines the count of valid widget feature descriptions.
    //!
    //! \return An array of widget feature descriptions, NULL if there are no widget feature descriptions.
    //!
    virtual gtf::metainformation::WidgetFeatureDescriptor const* GetWidgetFeatures(uint32_t& count_) const;

    //!
    //! \brief Retrieves all resource definitions contained in this DescriptorProvider.
    //!
    //! \param count_[out] will be set by the method and determines the count of valid resource descriptions.
    //!
    //! \return An array of resource descriptions, NULL if there are no resource descriptions.
    //!
    virtual gtf::metainformation::ResourceDescriptor const* GetResourceTypes(uint32_t& count_) const;

    //!
    //! \brief Resolves the types of all PropertyDescriptors using the given TypeManager
    //!
    //! \param typeManager[in] The TypeManager to use for resolving the types.
    //!
    //! \return true on success, false on failure, when some types could not be resolved.
    //!
    //! \details Must be called before publishing the interface.
    //!
    virtual bool ResolveTypes(const gtf::typesystem::TypeManager* typeManager_);

    //! \brief The name of the widget.
    static const char* WidgetName;
};

#endif // RIPPLE_EFFECT_DESC_H_INCLUDED
