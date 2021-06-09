////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

namespace MonitorUiExtensionMacro.MacroService.ScriptBuilder.Converters.MessageConverters
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Windows.Media;

    using Elektrobit.Guide.Monitor.Connection.Protocols;
    using Elektrobit.Guide.Monitor.Connection.Protocols.GtfValues;
    using Elektrobit.Guide.Monitor.Model.Datapools;
    using Elektrobit.Guide.Monitor.Scripting.MonitorContext;
    using Elektrobit.Guide.Monitor.Service;
    using Elektrobit.Guide.Monitor.Service.Namespace;
    using Elektrobit.Guide.Monitor.Utilities;
    using Elektrobit.Guide.Utilities;

    using MonitorUiExtensionMacro.MacroService.ScriptBuilder.Template;

    public abstract class MessageConverterBase<T> : IMessageToScriptConverter where T : IMessage
    {
        private readonly IModelService _modelService;

        private readonly INamespaceService _namespaceService;

        private readonly IModelValueConverter _valueConverter;

        protected MessageConverterBase(IModelService modelService, INamespaceService namespaceService, IModelValueConverter valueConverter)
        {
            _modelService = modelService;
            _namespaceService = namespaceService;
            _valueConverter = valueConverter;
        }

        protected abstract ScriptAction Convert(string monitorContextId, T message);

        public ScriptAction Convert(string monitorContextId, IMessage message)
        {
            return Convert(monitorContextId, (T)message);
        }

        protected QualifiedIdentifier GetDatapoolItemFullName(string datapoolItemId)
        {
            var datapoolItems = _modelService.Model?.Datapool.Modules.First().Items;
            var datapoolItem = datapoolItems?.FirstOrDefault(item => item.InternalItemId == datapoolItemId);

            if (datapoolItem == null)
            {
                return null;
            }

            return GetQualifiedIdentifier(datapoolItem.NamespaceId, datapoolItem.Name);
        }

        protected QualifiedIdentifier GetEventFullName(uint eventId, uint groupId)
        {
            var @event = _modelService.Model?.Events.FirstOrDefault(ev => ev.EventId == eventId && ev.GroupId == groupId);

            if (@event == null)
            {
                return null;
            }

            return GetQualifiedIdentifier(@event.NamespaceId, @event.Name);
        }

        protected string ConvertIGtfValueToString(IGtfValue gtfValue)
        {
            var modelValue = _valueConverter.ToModelValue(gtfValue);
            dynamic value = modelValue.Value;
            return ValueToString(value);
        }

        private QualifiedIdentifier GetQualifiedIdentifier(string namespaceId, string elementName)
        {
            var namespaceObject = _namespaceService.GetNamespace(namespaceId);
            var path = _namespaceService.GetNamespacePath(namespaceObject);
            var qualifiedName = path.Select(ns => ns.Name).Concat(elementName);

            return new QualifiedIdentifier(qualifiedName.ToArray());
        }

        protected IEnumerable<string> CreateParameterList(IEnumerable<object> paramObjects)
        {
            var objects = paramObjects as object[] ?? paramObjects.ToArray();

            if(objects.IsEmpty())
            {
                return Enumerable.Empty<string>();
            }

            return CreateParameterList(objects.ToArray());
        }

        protected IEnumerable<string> CreateParameterList(params object[] paramObjects)
        {
            return paramObjects
                .Select((dynamic v) => ValueToString(v) as string)
                .Where( x => x != null);
        }

        protected string ValueToString(string value)
        {
            return $"\"" + value + "\"";
        }

        protected string ValueToString(object value)
        {
            return value.ToString();
        }

        protected string ValueToString(float value)
        {
            return $"{value}f";
        }

        protected string ValueToString(int value)
        {
            return value.ToString();
        }

        protected string ValueToString(uint value)
        {
            return value.ToString();
        }

        protected string ValueToString(Color color)
        {
            return "new Color(){"
                + $"R = {ValueToString(color.R)}, "
                + $"G = {ValueToString(color.G)}, "
                + $"B = {ValueToString(color.B)}, "
                + $"A = {ValueToString(color.A)}"
                + "}";
        }

        protected string ValueToString(bool value)
        {
            return value.ToString().ToLower();
        }

        protected string ValueToString(IEnumerable<object> objects)
        {
            if(objects.Any())
            {
                throw new ArgumentException("This method is only for empty ienumerables!");
            }

            return null;
        }

        protected string ValueToString(QualifiedIdentifier qualifiedIdentifier)
        {
            var parts = qualifiedIdentifier.NamespacePath
                .Concat(qualifiedIdentifier.ItemName)
                .Where(str => !string.IsNullOrEmpty(str))
                .Select(e => $"\"{e}\"");

            return $"new QualifiedIdentifier({string.Join(", ", parts)})";
        }

        protected string ValueToString(IGtfValue value)
        {
            var modelValue = _valueConverter.ToModelValue(value);
            return ValueToString((dynamic)modelValue.Value);
        }
    }
}