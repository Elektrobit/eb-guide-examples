namespace JsonImporterPlugin.Model
{
    using System.Collections.Generic;

    using Newtonsoft.Json;

    internal class JsonEvent
    {
        [JsonProperty("name", Required = Required.Always)]
        public string Name { get; set; }

        [JsonProperty("event_group", Required = Required.Always)]
        public string EventGroup { get; set; }

        [JsonProperty("parameters")]
        public IList<JsonEventParameter> Parameters { get; set; }
    }
}
