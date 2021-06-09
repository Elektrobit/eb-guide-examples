////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace TagFilterPlugin
{
    using Elektrobit.Guide.Ui.Controls.TagFilter;

    /// <summary>
    /// Stores persistent information of a <see cref="VariantTagItem"/>.
    /// </summary>
    internal class VariantTagItemOptions : TagItemOptions
    {
        /// <summary>
        /// The type of the tag, e.g. filter by language or skin.
        /// </summary>
        public string VariantType { get; set; }
    }
}
