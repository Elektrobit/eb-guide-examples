namespace JsonImporterPlugin.Model
{
    using System.Collections.Generic;

    using Newtonsoft.Json;

    internal class JsonModelData
    {
        [JsonProperty("datapool")]
        public IList<JsonDpItem> Datapool { get; set; }

        [JsonProperty("events")]
        public IList<JsonEvent> Events { get; set; }
    }
}
