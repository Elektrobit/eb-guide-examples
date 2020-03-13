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


namespace MenuActionPlugin
{
    using System.Collections.Generic;
    using System.ComponentModel.Composition;

    using Elektrobit.Guide.Studio.Workbench;
    using Elektrobit.Guide.Ui.ViewModels;

    [Export(typeof(IMenuItemProvider))]
    internal class MainMenuBarItemProvider : IMenuItemProvider
    {
        private readonly IMenuFactory _menuFactory;

        // We're providing a new menu for the main menu bar
        public string MenuId => WorkbenchMenus.MAIN_MENU_BAR;

        [ImportingConstructor]
        public MainMenuBarItemProvider(IMenuFactory menuFactory)
        {
            _menuFactory = menuFactory;
        }

        public IEnumerable<IMenuItemViewModel> CreateMenuItems(object context)
        {
            // Create the items of the custom actions menu by using the IMenuFactory interface.
            // This will call the IMenuItemProvider implementations registered for the custom menu ID.
            var items = _menuFactory.CreateMenu(CustomMenuItemProvider.ID, context);

            yield return new MenuItemViewModel { Header = "Custom actions", Items = items };
        }
    }
}
