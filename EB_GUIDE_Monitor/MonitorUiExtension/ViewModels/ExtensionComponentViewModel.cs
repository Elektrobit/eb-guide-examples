////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

using System.ComponentModel;
using Elektrobit.Guide.Ui.Mvvm;

namespace MonitorUiExtension.ViewModels
{
    using Elektrobit.Guide.Monitor.Ui.Docking;

    /// <summary>
    /// View model class used by the extension view 
    /// </summary>
    public class ExtensionComponentViewModel : ViewModel, IDockableComponentViewModel
    {
        public void Dispose()
        {
            // nothing here
        }
    }
}
