namespace JsonImporterPlugin.Model
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    internal class JsonDpItem
    {
        [JsonProperty("name", Required = Required.Always)]
        public string Name { get; set; }

        [JsonProperty("type", Required = Required.Always)]
        public string Type { get; set; }

        [JsonProperty("value", Required = Required.Always)]
        public JToken Value { get; set; }
    }
}
