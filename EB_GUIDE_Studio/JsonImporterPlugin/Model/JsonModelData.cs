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

    internal class JsonModelData
    {
        [JsonProperty("datapool")]
        public IList<JsonDpItem> Datapool { get; set; }

        [JsonProperty("events")]
        public IList<JsonEvent> Events { get; set; }
    }
}
