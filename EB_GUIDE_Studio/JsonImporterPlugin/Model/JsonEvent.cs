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
