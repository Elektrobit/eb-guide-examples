namespace JsonImporterPlugin.Model
{
    using Newtonsoft.Json;

    internal class JsonEventParameter
    {
        [JsonProperty("name", Required = Required.Always)]
        public string Name { get; set; }

        [JsonProperty("type", Required = Required.Always)]
        public string Type { get; set; }
    }
}
