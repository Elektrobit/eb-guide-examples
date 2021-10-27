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
    using System.Windows.Input;

    using Elektrobit.Guide.Studio.Control;
    using Elektrobit.Guide.Studio.Model.Elements.Datapool;
    using Elektrobit.Guide.Ui.Controls.TagFilter;

    using Prism.Commands;
    using Prism.Mvvm;

    internal class VariantTagItem : BindableBase, ITagItem
    {
        private readonly Func<IDpItem, bool> _filter;

        private IEnumerable<string> _nameHighlightParts;

        private bool _isEnabled;

        public ICommand RemoveCommand { get; }

        public string Group { get; }

        public string GroupIcon => "Icon.Skins-S";

        public string Name { get; }

        public PropertySourceIndicator Indicator { get; }

        public IEnumerable<string> NameHighlightParts
        {
            get => _nameHighlightParts;
            set => SetProperty(ref _nameHighlightParts, value);
        }

        public bool IsEnabled
        {
            get => _isEnabled;
            set => SetProperty(ref _isEnabled, value);
        }

        public VariantTagItem(
            string name,
            string group,
            PropertySourceIndicator indicator,
            Func<IDpItem, bool> filter,
            Action<ITagItem> removeCallback)
        {
            _filter = filter;
            Name = name;
            Group = group;
            IsEnabled = true;
            Indicator = indicator;
            RemoveCommand = new DelegateCommand(() => removeCallback(this));
        }

        public void Dispose()
        {
        }

        public bool Equals(ITagItem other)
        {
            if (ReferenceEquals(other, null))
            {
                return false;
            }

            if (ReferenceEquals(this, other))
            {
                return true;
            }

            return Equals(Name, other.Name) && Equals(Group, other.Group);
        }

        public bool Filter(object item)
        {
            return item is IDpItem dpItem && _filter(dpItem);
        }

        public bool IsSuperset(ITagItem other)
        {
            return Equals(other);
        }
    }
}