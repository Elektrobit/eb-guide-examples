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

#ifndef DOWNSCALING_WIDGET_FEATURE_H
#define DOWNSCALING_WIDGET_FEATURE_H

#include <gtf/scene/element/Controller.h>
#include <gtf/scene/TypeResolver.h>
#include <gtf/configuration/Settings.h>
#include <gtf/osal/filesystem/Path.h>

#include <gtf/properties/PropertyObserver.h>
#include <gtf/properties/ScalarValue.h>
#include <gtf/resourcesystem/Factory.h>
#include <gtf/configuration/ScalarBuilder.h>

namespace downscalingFeature
{
    class DownscalingFeature : public gtf::scene::element::Controller
    {
    public:
        DownscalingFeature(const gtf::scene::TypeResolverHandle& resolver_,
            const gtf::properties::PropertyObserverHandle&       observer_,
            const gtf::resourcesystem::FactoryHandle&            factory_,
            const gtf::configuration::SettingsHandle&            settings,
            const gtf::osal::filesystem::PathHandle&             path);

        ~DownscalingFeature();

        virtual void connect(gtf::properties::Container& model_);

        virtual void disconnect();

    protected:
        bool handleUpdate(const gtf::properties::PropertyKey& key);

    private:
        gtf::properties::ContainerHandle        container;
        gtf::properties::PropertyObserverHandle observer;
        gtf::resourcesystem::FactoryHandle      factory;
        gtf::configuration::SettingsHandle      settings;
        gtf::osal::filesystem::PathHandle       path;

        gtf::configuration::StringScalarHandle  modelPath_;
        gtf::properties::IntegerValueHandle     heightValue;
        gtf::properties::IntegerValueHandle     widthValue;
        gtf::properties::StringValueHandle      imagePathValue;
        gtf::properties::ResourceValueHandle    imageValue;
        gtf::resourcesystem::FormatHandle       imagePathFormat;
        gtf::resourcesystem::FormatHandle       encodedMemImageFormat;

        struct FeaturePropertyReferences
        {
            enum
            {
                Height,
                Width,
                ImagePath,
                Count
            };
        };

        gtf::scene::TypeResolver::FeaturePropertyReference featurePropertyReferences[FeaturePropertyReferences::Count];

        struct PropertyReferences
        {
            enum
            {
                Image,
                Count
            };
        };

        gtf::scene::TypeResolver::ContainerPropertyReference widgetPropertyReferences[PropertyReferences::Count];
    };

} // namespace downscalingFeature
GTF_DEFINE_INTERFACE_NAME(downscalingFeature::DownscalingFeature)
#endif // DOWNSCALING_WIDGET_FEATURE_H
