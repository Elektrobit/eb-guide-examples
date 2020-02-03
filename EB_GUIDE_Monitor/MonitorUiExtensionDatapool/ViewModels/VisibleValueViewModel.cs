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
    using System.Text;
    using System.Threading.Tasks;

    using Elektrobit.Guide.Monitor.Model.Datapools;
    using Elektrobit.Guide.Monitor.Model.Value;
    using Elektrobit.Guide.Monitor.Service.Datapool;
    using Elektrobit.Guide.Monitor.Service.Datapool.Observables;

    using Prism.Mvvm;

    /// <summary>
    /// This class encapsulates value datapool item handling
    /// </summary>
    public class VisibleValueViewModel : BindableBase, IDatapoolItemObserver
    {
        private readonly IDatapoolService _service;

        private readonly DatapoolItem _item;

        private IDatapoolItemObservable _observable;

        public string Name => _item.Name;

        private object _value;
        public object Value
        {
            get => _value;

            private set => SetProperty(ref _value, value);
        }

        public VisibleValueViewModel(IDatapoolService service, DatapoolItem item)
        {
            _service = service;
            _item = item;
        }

        public async Task Subscribe()
        {
            _observable = _service.GetObservable(_item);

            await _observable.Subscribe(this);
            await _observable.RequestValue();
        }

        public async Task Unsubscribe()
        {
            await _observable.Unsubscribe(this);
            _observable = null;
            Value = string.Empty;
        }

        public async void OnValueChanged(IDatapoolItemObservable source, ModelValue value)
        {
            if (_observable is ListDatapoolItemObservable listObservable)
            {
                await ReadListValue(listObservable);
            }
            else
            {
                ReadScalarValue(value);
            }
        }

        private void ReadScalarValue(ModelValue value)
        {
            // Set plain value
            Value = value.Value;
        }

        private async Task ReadListValue(ListDatapoolItemObservable listObservable)
        {
            // list values must be read one by one
            // so read length first and then item by item
            // In this example the first 3 items are read 
            // and concatenated as a string

            var concatenatedValue = new StringBuilder();

            // cast observable to list observable
            var listLength = await listObservable.GetListLength();

            concatenatedValue.Append($"[{listLength}] ");

            // if available, read 3 values
            for (uint counter = 0; counter < 3 && counter < listLength; counter++)
            {
                var modelValue = await listObservable.ReadValue(counter);
                concatenatedValue.Append(modelValue.Value);
                concatenatedValue.Append(", ");
            }

            concatenatedValue.Append("...");

            Value = concatenatedValue;
        }
    }
}