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

#include "DownscalingFeature.h"
#include <gtf/datapool/ScalarValue.h>
#include <gtf/decoder/ImagePath.h>
#include <gtf/decoder/EncodedMemImage.h>

namespace downscalingFeature
{
    DownscalingFeature::DownscalingFeature(const gtf::scene::TypeResolverHandle& resolver_,
        const gtf::properties::PropertyObserverHandle& observer_,
        const gtf::resourcesystem::FactoryHandle& factory_,
        const gtf::configuration::SettingsHandle& settings_,
        const gtf::osal::filesystem::PathHandle& path_)
        :observer(observer_), factory(factory_), settings(settings_), path(path_)
    {
        featurePropertyReferences[FeaturePropertyReferences::Height].featureType.name = "DownScalingFeature";
        featurePropertyReferences[FeaturePropertyReferences::Height].property.name = "targetHeight";

        featurePropertyReferences[FeaturePropertyReferences::Width].featureType.name = "DownScalingFeature";
        featurePropertyReferences[FeaturePropertyReferences::Width].property.name = "targetWidth";

        featurePropertyReferences[FeaturePropertyReferences::ImagePath].featureType.name = "DownScalingFeature";
        featurePropertyReferences[FeaturePropertyReferences::ImagePath].property.name = "imagePath";

        // the feature uses the image property of the Image widget
        widgetPropertyReferences[PropertyReferences::Image].containerType.name = "GtfImage";
        widgetPropertyReferences[PropertyReferences::Image].property.name = "image";

        resolver_->resolveContainerProperties(widgetPropertyReferences, PropertyReferences::Count);
        resolver_->resolveFeatureProperties(featurePropertyReferences, FeaturePropertyReferences::Count);

        modelPath_ = settings->getItem<gtf::configuration::StringScalar>("gtf.model.path");
    }

    void DownscalingFeature::connect(gtf::properties::Container& model_)
    {
        container = &model_;

        gtf::properties::Properties& widgetProperties = container->getProperties();

        imageValue = widgetProperties.get<gtf::properties::ResourceValue>(widgetPropertyReferences[PropertyReferences::Image].property.identifier);
        if (!imageValue.valid())
        {
            return;
        }

        heightValue = widgetProperties.get<gtf::properties::IntegerValue>(featurePropertyReferences[FeaturePropertyReferences::Height].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::Height].property.identifier);
        if (!heightValue.valid())
        {
            return;
        }

        widthValue = widgetProperties.get<gtf::properties::IntegerValue>(featurePropertyReferences[FeaturePropertyReferences::Width].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::Width].property.identifier);
        if (!widthValue.valid())
        {
            return;
        }

        imagePathValue = widgetProperties.get<gtf::properties::StringValue>(featurePropertyReferences[FeaturePropertyReferences::ImagePath].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::ImagePath].property.identifier);
        if (!imagePathValue.valid())
        {
            return;
        }

        imagePathFormat = factory->createFormat(gtf::decoder::ImagePath::getFormatName());
        if (!imagePathFormat)
        {
            return;
        }


        encodedMemImageFormat = factory->createFormat(gtf::decoder::EncodedMemImage::getFormatName());
        if (!encodedMemImageFormat)
        {
            return;
        }

        gtf::properties::PropertyKey heightPropertyKey = gtf::properties::PropertyKey(container, featurePropertyReferences[FeaturePropertyReferences::Height].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::Height].property.identifier);
        gtf::properties::PropertyKey widthPropertyKey = gtf::properties::PropertyKey(container, featurePropertyReferences[FeaturePropertyReferences::Width].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::Width].property.identifier);
        gtf::properties::PropertyKey imagePathPropertyKey = gtf::properties::PropertyKey(container, featurePropertyReferences[FeaturePropertyReferences::ImagePath].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::ImagePath].property.identifier);


        //subscribe to property changes
        observer->subscribe(heightPropertyKey, this, gtf::properties::PropertyObserver::Callback::create<DownscalingFeature, &DownscalingFeature::handleUpdate>(this));
        observer->subscribe(widthPropertyKey, this, gtf::properties::PropertyObserver::Callback::create<DownscalingFeature, &DownscalingFeature::handleUpdate>(this));
        observer->subscribe(imagePathPropertyKey, this, gtf::properties::PropertyObserver::Callback::create<DownscalingFeature, &DownscalingFeature::handleUpdate>(this));

        handleUpdate(imagePathPropertyKey);
    }

    void DownscalingFeature::disconnect()
    {
        gtf::properties::PropertyKey heightPropertyKey = gtf::properties::PropertyKey(container, featurePropertyReferences[FeaturePropertyReferences::Height].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::Height].property.identifier);
        gtf::properties::PropertyKey widthPropertyKey = gtf::properties::PropertyKey(container, featurePropertyReferences[FeaturePropertyReferences::Width].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::Width].property.identifier);
        gtf::properties::PropertyKey imagePathPropertyKey = gtf::properties::PropertyKey(container, featurePropertyReferences[FeaturePropertyReferences::ImagePath].featureType.identifier, featurePropertyReferences[FeaturePropertyReferences::ImagePath].property.identifier);

        observer->unsubscribe(heightPropertyKey, this);
        observer->unsubscribe(widthPropertyKey, this);
        observer->unsubscribe(imagePathPropertyKey, this);

        heightValue.reset();
        widthValue.reset();
        imageValue.reset();
        imagePathValue.reset();
    }

    bool DownscalingFeature::handleUpdate(const gtf::properties::PropertyKey&)
    {
        //construct full path to image
        if (!modelPath_)
        {
            return false;
        }

        path->setPath(imagePathValue->get(), modelPath_->get());
        if (!path->isExisting())
        {
            return false;
        }

        gtf::resourcesystem::ResourceHandle resourceHandle = factory->createResource();
        if (!resourceHandle.valid())
        {
            return false;
        }

        const gtf::resourcesystem::DataHandle imagePathData = factory->createData(imagePathFormat);
        if (!imagePathData)
        {
            return false;
        }

        gtf::decoder::ImagePath imagePathSetter(imagePathData);

        //set the path of the resource
        if (!imagePathSetter.setPath(path->getPath()))
        {
            return false;
        }

        if (!resourceHandle->attachData(imagePathData))
        {
            return false;
        }

        gtf::resourcesystem::DataHandle encodedMemImageData = resourceHandle->getDataAs(encodedMemImageFormat);
        if (!encodedMemImageData)
        {
            return false;
        }

        gtf::decoder::EncodedMemImage::ScaleInfo scaleInformation;

        scaleInformation.width = widthValue->get();
        scaleInformation.height = heightValue->get();

        gtf::decoder::EncodedMemImage encodedMemImage(encodedMemImageData);

        if (!encodedMemImage.setScaleInfo(scaleInformation))
        {
            return false;
        }

        resourceHandle.reset();

        gtf::resourcesystem::ResourceHandle scaledMemImage = factory->createResource();
        if (!scaledMemImage.valid())
        {
            return false;
        }

        if (!scaledMemImage->attachData(encodedMemImageData))
        {
            return false;
        }

        imageValue->set(scaledMemImage);

        return true;
    }

    DownscalingFeature::~DownscalingFeature()
    {
    }
}
