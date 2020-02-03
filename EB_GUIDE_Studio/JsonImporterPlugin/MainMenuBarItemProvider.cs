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
