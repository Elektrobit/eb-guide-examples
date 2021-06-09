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

#ifndef GTF_RANDOM_FOCUS_FEATURES_H_INCLUDED
#define GTF_RANDOM_FOCUS_FEATURES_H_INCLUDED

#include <gtf/focus/Features.h>
#include <gtf/scene/TypeResolver.h>

 namespace focusedpolicywidgetfeature
{
    /*!
    \brief Implementation of focus features.
    */
    class RandomFocusFeature : public gtf::focus::Features
    {
    public:
        RandomFocusFeature(const gtf::scene::TypeResolverHandle& resolver_);
        virtual void getFeatures(gtf::container::Array<gtf::properties::Features::Feature>& focusFeatures);

    protected:
        gtf::scene::TypeResolverHandle resolver;

        void addIfFound(gtf::container::Array<gtf::properties::Features::Feature>& listToAdd, const char* name);
    };
}


#endif // GTF_RANDOM_FOCUS_FEATURES_H_INCLUDED
