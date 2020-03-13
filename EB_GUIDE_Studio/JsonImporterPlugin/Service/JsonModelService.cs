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


namespace JsonImporterPlugin.Service
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using System.IO;
    using System.Linq;
    using System.Threading.Tasks;

    using Elektrobit.Guide.Scheduler;
    using Elektrobit.Guide.Studio.Model.Actions;
    using Elektrobit.Guide.Studio.Model.Actions.Datapool;
    using Elektrobit.Guide.Studio.Model.Actions.Events;
    using Elektrobit.Guide.Studio.Model.Actions.Project.ModelInterfaceTransfer.ValueConverter;
    using Elektrobit.Guide.Studio.Model.Elements;
    using Elektrobit.Guide.Studio.Model.Elements.Datapool;
    using Elektrobit.Guide.Studio.Model.Elements.DefaultName;
    using Elektrobit.Guide.Studio.Model.Elements.Event;
    using Elektrobit.Guide.Studio.Model.Elements.Value;
    using Elektrobit.Guide.Studio.Model.Session;
    using Elektrobit.Guide.Studio.StandardTypes;
    using Elektrobit.Guide.Studio.TypeSystem;
    using Elektrobit.Guide.Utilities;

    using JsonImporterPlugin.Deserializer;
    using JsonImporterPlugin.Model;

    using log4net;

    using LanguageExt;

    using Newtonsoft.Json.Linq;

    [Export(typeof(IJsonModelService))]
    internal class JsonModelService : IJsonModelService
    {
        private static readonly ILog Logger = LogManager.GetLogger(typeof(JsonModelService));

        private readonly IDatapoolService _datapoolService;

        private readonly IEventService _eventService;

        private readonly IJsonFileReader _jsonFileReader;

        private readonly ITaskSchedulerProvider _schedulerProvider;

        private readonly IDefaultNameGenerator _nameGenerator;

        private readonly List<string> _importProblems;

        private readonly Dictionary<string, IModelValueToJTokenConverter> _converters;

        [ImportingConstructor]
        public JsonModelService(
            IDatapoolService datapoolService,
            IEventService eventService,
            IJsonFileReader jsonFileReader,
            ITaskSchedulerProvider schedulerProvider,
            IDefaultNameGenerator nameGenerator,
            [ImportMany]
            IEnumerable<IModelValueToJTokenConverter> valueConverters)
        {
            _datapoolService = datapoolService;
            _eventService = eventService;
            _jsonFileReader = jsonFileReader;
            _schedulerProvider = schedulerProvider;
            _nameGenerator = nameGenerator;
            _importProblems = new List<string>();
            _converters = valueConverters.ToDictionary(converter => converter.Type, converter => converter);
        }

        public async Task<ImportModelDataResult> ImportModelData(IProjectContext projectContext, string file)
        {
            _importProblems.Clear();
            var nullSession = new NullWriteSession();
            var fileName = Path.GetFileNameWithoutExtension(file);
            var usedNames = projectContext.Project.RootNamespace.Namespaces.Select(element => element.Name);
            var name = _nameGenerator.FindUniqueName(fileName, usedNames);
            var modelNamespace = new Namespace { Name = name };

            var modelData = _jsonFileReader.ReadModelDataFromJson(file, out var msg);
            if (modelData == null)
            {
                return ImportModelDataResult.Failed($"Failed to import data from '{fileName}.json': {msg}");
            }

            var datapool = modelData.Datapool;
            if (datapool != null)
            {
                ImportDatapool(nullSession, projectContext, datapool, modelNamespace);
            }

            var events = modelData.Events;
            if (events != null)
            {
                ImportEvents(nullSession, projectContext, events, modelNamespace);
            }

            if (_importProblems.Any())
            {
                var message = string.Join(", ", _importProblems);
                return ImportModelDataResult.Failed($"Failed to import data from '{fileName}'.json': {message}.");
            }

            await _schedulerProvider.ExecuteModelAction(
                projectContext,
                session => AddModelNamespace(
                    session,
                    projectContext,
                    modelNamespace));

            return ImportModelDataResult.Success($"Imported data from '{fileName}.json' to new namespace " +
                                                 $"'{modelNamespace.Name}'.");
        }

        private void ImportDatapool(
            IWriteSession session,
            IProjectContext projectContext,
            IList<JsonDpItem> dpItems,
            INamespace @namespace)
        {
            CheckForDuplicates("datapool items", dpItems.Select(i => i.Name).ToList());

            foreach (var dp in dpItems)
            {
                var type = TryGetDpItemType(projectContext, dp.Type);
                if (type == null)
                {
                    _importProblems.Add($"Invalid type '{dp.Type}' for a datapool item with name '{dp.Name}'");
                    continue;
                }

                var dpItemName = !string.IsNullOrWhiteSpace(dp.Name) ? dp.Name : null;
                var dpItem = _datapoolService.CreateDatapoolItem(session, projectContext, @namespace, type, dpItemName);
                var value = ConvertValue(dp.Value, type.BaseType);
                value.Match(
                    val => SetDpItemValue(session, dpItem, val),
                    errors => _importProblems.Add($"Invalid value '{dp.Value}' for a datapool item with name '{dp.Name}'"));
            }
        }

        private void ImportEvents(
            IWriteSession session,
            IProjectContext projectContext,
            IList<JsonEvent> events,
            INamespace @namespace)
        {
            var existingEvents = projectContext.Project.GetAllEvents().ToList();

            CheckForDuplicates("events", events.Select(e => e.Name).ToList());

            foreach (var evt in events)
            {
                var eventName = !string.IsNullOrWhiteSpace(evt.Name) ? evt.Name : null;
                var @event = _eventService.CreateEvent(session, projectContext, @namespace, eventName);
                var eventGroup = projectContext.Project.EventGroups.Items.FirstOrDefault(i => i.Name == evt.EventGroup)
                                 ?? projectContext.Project.EventGroups.Items.FirstOrDefault();

                if (!@event.EventGroup.Equals(eventGroup))
                {
                    _eventService.SetEventGroup(session, @event, eventGroup);
                }

                if (@namespace.Events.Items.GroupBy(e => e.EventId).Any(id => id.Count() > 1))
                {
                    _eventService.SetEventId(session, @event, existingEvents.FindFreeEventId());
                }

                if (evt.Parameters != null)
                {
                    CheckForDuplicates("event parameters", evt.Parameters.Select(p => p.Name).ToList());

                    foreach (var param in evt.Parameters)
                    {
                        var paramType = projectContext.TypeManager.FindType(param.Type);
                        if (paramType == null)
                        {
                            _importProblems.Add($"Invalid parameter type '{param.Type}' for a parameter with name " +
                                                $"'{param.Name}' in event '{evt.Name}'");
                            continue;
                        }

                        var parameter = _eventService.AddParameter(session, @event, paramType);
                        if (!string.IsNullOrWhiteSpace(param.Name))
                        {
                            SetParameterName(session, @event, parameter, param.Name);
                        }
                    }
                }

                existingEvents.Add(@event);
            }
        }

        private void AddModelNamespace(IWriteSession session, IProjectContext projectContext, INamespace @namespace)
        {
            var rootNamespace = projectContext.Project.RootNamespace;
            rootNamespace.Namespaces.Add(@namespace);

            session.Save(rootNamespace);
            session.SaveHierarchy(@namespace);
        }

        private void CheckForDuplicates(string elements, IList<string> names)
        {
            var duplicates = names.GroupBy(e => e).Where(name => name.Count() > 1).Select(i => i.Key).ToList();

            if (!duplicates.IsEmpty())
            {
                foreach (var duplicate in duplicates)
                {
                    _importProblems.Add($"Multiple {elements} with the same name '{duplicate}' found");
                }
            }
        }

        private Validation<Error, ModelValue> ConvertValue(JToken itemValue, IType type)
        {
            if (type is IListType listType)
            {
                var elementType = listType.ElementType;
                if (_converters.TryGetValue(elementType.Name, out var converterList))
                {
                    var list = itemValue.ToList().Select(converterList.ToModelValue).Sequence();
                    return list.Map(items => (ModelValue)new ListModelValue(listType, items));
                    
                }
            }
            else if (_converters.TryGetValue(type.Name, out var converter))
            {
                return converter.ToModelValue(itemValue);
            }

            return new Error($"No converter for type '{type.Name}' found");
        }

        private IType TryGetDpItemType(IProjectContext projectContext, string type)
        {
            try
            {
                return projectContext.TypeManager.GetDatapoolType(type);
            }
            catch (Exception ex)
            {
                Logger.InfoFormat($"Cannot get DatapoolType for '{type}'.", ex);
                return null;
            }
        }

        private void SetDpItemValue(IWriteSession session, IDpItem dpItem, ModelValue value)
        {
            dpItem.Value = value;
            session.SaveHierarchy(dpItem);
        }

        private void SetParameterName(IWriteSession session, IEvent @event, IEventParameter parameter, string name)
        {
            parameter.Name = name;
            session.SaveHierarchy(@event);
        }
    }
}
