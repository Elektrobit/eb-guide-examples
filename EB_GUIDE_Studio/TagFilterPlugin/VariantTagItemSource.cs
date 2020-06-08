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
    using System;
    using System.Collections.Generic;

    using Elektrobit.Guide.Studio.Control;
    using Elektrobit.Guide.Studio.Model.Elements.Datapool;
    using Elektrobit.Guide.Ui.Controls.TagFilter;
    using Elektrobit.Guide.Ui.Misc;

    internal class VariantTagItemSource : ITagItemSource
    {
        private const string GROUP = "Variant support";
        internal const string LANGUAGE_SUPPORT = "Language support";
        internal const string SKIN_SUPPORT = "Skin support";
        internal const string NO_SUPPORT = "No variant support";

        public ITagItemSerializer Serializer { get; }

        public VariantTagItemSource()
        {
            Serializer = new VariantTagItemSerializer();
        }

        public IEnumerable<ITagItem> GetMatchingItems(FilterQuery filterQuery, Action<ITagItem> removeCallback)
        {
            // Depending on the filter text entered by the user only show matching tags
            if (FilterUtils.Filter(LANGUAGE_SUPPORT, filterQuery))
            {
                yield return CreateLanguageSupportTagItem(removeCallback);
            }

            if (FilterUtils.Filter(SKIN_SUPPORT, filterQuery))
            {
                yield return CreateSkinSupportTagItem(removeCallback);
            }

            if (FilterUtils.Filter(NO_SUPPORT, filterQuery))
            {
                yield return CreateNoSupportTagItem(removeCallback);
            }
        }

        public IEnumerable<ITagItem> GetSuggestedItems(Action<ITagItem> removeCallback)
        {
            // These tags are suggested to the user when he clicks into the tag filter box
            yield return CreateLanguageSupportTagItem(removeCallback);
            yield return CreateSkinSupportTagItem(removeCallback);
            yield return CreateNoSupportTagItem(removeCallback);
        }

        internal static VariantTagItem CreateNoSupportTagItem(Action<ITagItem> removeCallback)
        {
            return new VariantTagItem(
                NO_SUPPORT,
                GROUP,
                PropertySourceIndicator.Private,
                HasNoVariantSupport,
                removeCallback);
        }

        internal static VariantTagItem CreateSkinSupportTagItem(Action<ITagItem> removeCallback)
        {
            return new VariantTagItem(
                SKIN_SUPPORT,
                GROUP,
                PropertySourceIndicator.Skin,
                HasSkinSupport,
                removeCallback);
        }

        internal static VariantTagItem CreateLanguageSupportTagItem(Action<ITagItem> removeCallback)
        {
            return new VariantTagItem(
                LANGUAGE_SUPPORT,
                GROUP,
                PropertySourceIndicator.Language,
                HasLanguageSupport,
                removeCallback);
        }

        private static bool HasLanguageSupport(IDpItem item)
        {
            return item.IsLanguageDpItem();
        }

        private static bool HasSkinSupport(IDpItem item)
        {
            return item.IsSkinDpItem();
        }

        private static bool HasNoVariantSupport(IDpItem item)
        {
            return item.HasAnyVariantValue() == false;
        }
    }
}