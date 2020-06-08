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
    using System;

    using Elektrobit.Guide.Ui.Controls.TagFilter;

    /// <summary>
    /// Serializes a tag item created by <see cref="VariantTagItemSource"/>.
    /// </summary>
    internal class VariantTagItemSerializer : ITagItemSerializer
    {
        public bool CanSerialize(ITagItem tagItem)
        {
            return tagItem is VariantTagItem;
        }

        public TagItemOptions Serialize(ITagItem tagItem)
        {
            var variantTagItem = (VariantTagItem)tagItem;

            return new VariantTagItemOptions { IsEnabled = variantTagItem.IsEnabled, VariantType = variantTagItem.Name };
        }

        public bool CanDeserialize(TagItemOptions tagItemOption)
        {
            return tagItemOption is VariantTagItemOptions;
        }

        public ITagItem Deserialize(TagItemOptions tagItemOption, Action<ITagItem> removeCallback)
        {
            var variantTagItemOption = (VariantTagItemOptions)tagItemOption;
            var variantTagItem = CreateVariantTagItem(removeCallback, variantTagItemOption.VariantType);
            if (variantTagItem == null)
            {
                return null;
            }

            variantTagItem.IsEnabled = tagItemOption.IsEnabled;
            return variantTagItem;
        }

        private static VariantTagItem CreateVariantTagItem(Action<ITagItem> removeCallback, string variantType)
        {
            if (Equals(variantType, VariantTagItemSource.LANGUAGE_SUPPORT))
            {
                return VariantTagItemSource.CreateLanguageSupportTagItem(removeCallback);
            }

            if (Equals(variantType, VariantTagItemSource.SKIN_SUPPORT))
            {
                return VariantTagItemSource.CreateSkinSupportTagItem(removeCallback);
            }

            if (Equals(variantType, VariantTagItemSource.NO_SUPPORT))
            {
                return VariantTagItemSource.CreateNoSupportTagItem(removeCallback);
            }

            return null;
        }
    }
}
