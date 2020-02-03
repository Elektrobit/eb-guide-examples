////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtension.ViewModels
{
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Monitor.Ui.Docking;

    /// <summary>
    /// Factory class for creating a dockable component view model which is representing a tab in monitor
    /// </summary>
    [Export(typeof(IDockableComponentFactory))]
    internal class ExtensionViewModelFactory : IDockableComponentFactory
    {
        /// <summary>
        /// Implemented id property, which defines an unique ID for that compponent.
        /// </summary>
        public string ContentIdBase { get; }

        /// <summary>
        /// Implemented title property. This property is used for e.g. tab title or Layout entry.
        /// 
        /// </summary>
        public string Title { get; }

        
        [ImportingConstructor]
        public ExtensionViewModelFactory()
        {
            Title = "Monitor Extension";
            ContentIdBase = "monitor_extension_id";
        }

        /// <summary>
        /// Factory method for component
        /// </summary>
        /// <returns></returns>
        public IDockableComponentViewModel CreateComponent()
        {
            return new ExtensionComponentViewModel();
        }
    }
}
