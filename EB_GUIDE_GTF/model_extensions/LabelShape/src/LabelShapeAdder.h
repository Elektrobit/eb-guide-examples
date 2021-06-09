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

#ifndef GTF_LABEL_SHAPE_ADDER_H_INCLUDED
#define GTF_LABEL_SHAPE_ADDER_H_INCLUDED

#include <gtf/scene/element/Controller.h>
#include <gtf/scene/TypeResolver.h>

using namespace gtf;

namespace labelshapewidgetfeature
{
	/*!
	\brief Helper class to add the SineLabelShape to the widget.
	*/
	class SineCurveShapeAdder : public gtf::scene::element::Controller
	{
	public:
		SineCurveShapeAdder(const scene::TypeResolverHandle& resolver_);

		virtual void connect(properties::Container& model_) override;
		virtual void disconnect() override;

	private:
		properties::ContainerHandle                        model;
		gtf::scene::TypeResolver::FeaturePropertyReference labelShapeFeatureReference;
		scene::TypeResolverHandle                          resolver;
	};
} // namespace labelshapewidgetfeature

GTF_DEFINE_INTERFACE_NAME(labelshapewidgetfeature::SineCurveShapeAdder);


#endif // GTF_LABEL_SHAPE_ADDER_H_INCLUDED
