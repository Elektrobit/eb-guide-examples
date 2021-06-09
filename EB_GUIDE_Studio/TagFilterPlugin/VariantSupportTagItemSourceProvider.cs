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

namespace TagFilterPlugin
{
    using System.Collections.Generic;
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Studio.Datapool.ViewModels;
    using Elektrobit.Guide.Ui.Controls.TagFilter;

    [Export(typeof(ITagItemSourceProvider))]
    internal class VariantSupportTagItemSourceProvider : ITagItemSourceProvider
    {
        // We provide tags for the Datapool component
        public string SourceId => DatapoolComponentTagFilters.TAG_FILTER_ID;

        public IEnumerable<ITagItemSource> GetSources(object context)
        {
            yield return new VariantTagItemSource();
        }
    }
}
