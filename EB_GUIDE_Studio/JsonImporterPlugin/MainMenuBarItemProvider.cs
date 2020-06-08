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


namespace JsonImporterPlugin
{
    using System.Collections.Generic;
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Studio.Workbench;
    using Elektrobit.Guide.Ui.ViewModels;

    [Export(typeof(IMenuItemProvider))]
    internal class MainMenuBarItemProvider : IMenuItemProvider
    {
        private readonly IMenuFactory _menuFactory;

        public string MenuId => WorkbenchMenus.MAIN_MENU_BAR;

        [ImportingConstructor]
        public MainMenuBarItemProvider(IMenuFactory menuFactory)
        {
            _menuFactory = menuFactory;
        }

        public IEnumerable<IMenuItemViewModel> CreateMenuItems(object context)
        {
            var items = _menuFactory.CreateMenu(ImportMenuItemProvider.ID, context);

            yield return new MenuItemViewModel { Header = "Import", Items = items, Order = 2 };
        }
    }
}
