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

uniform sampler2D u_emissiveStrengthMap;
uniform highp float u_emissiveStrengthMapStrength;
uniform highp vec4 u_emissiveMapHighlightColor;
uniform highp vec4 u_emissiveMapColor;

highp float sq(in float f)
{
  return f*f;
}

highp vec3 loadEmissiveColor(in highp vec3 emissive_, in highp float emissisveStrength_, in highp vec2 texCoord_)
{
    highp float emissiveMapvalue = texture2D(u_emissiveStrengthMap, texCoord_).x;
    highp float emissiveStrength  = sq(emissiveMapvalue) * u_emissiveStrengthMapStrength;
    highp float emissiveFactor = emissisveStrength_ + emissiveStrength;
    
    highp vec3 emissiveBaseColor = 
#if EMISSIVE_TEXTURE_ENABLED
    emissive_ * u_emissiveMapColor.rgb;
#else
    emissive_;
#endif

   highp float mixFac = emissiveMapvalue;
   return  mix(emissiveBaseColor, u_emissiveMapHighlightColor.rgb, mixFac) * emissiveFactor;
}

void main()
{
    RawData rawData = getRawData();  
    
#if TRANSPARENCY_ENABLED
    if(rawData.opacity < 0.01)
    {
        discard;
    }
#endif

    ProcessedData processedData = processData(rawData);

/// begin custom adaptions ///
    processedData.emissiveColor = loadEmissiveColor(rawData.emissive, rawData.emissiveStrength, rawData.texCoord0);
/// end custom adaptions ///

    highp vec4 cOut          = vec4(0.0);
      
    cOut.a = processedData.opacity;

    cOut.rgb += processedData.emissiveColor;
    
#if REFLECTION_CUBE_ENABLED
    cOut.rgb += processedData.specularColorReflection;
#endif

#if DYNAMIC_LIGHT_ENABLED || IMAGE_BASED_LIGHT_ENABLED
    GlobalBrdfInputData globalBrdfInput = prepareGlobalBrdfInputData(processedData);
#endif // DYNAMIC_LIGHT_ENABLED || IMAGE_BASED_LIGHT_ENABLED

#if DYNAMIC_LIGHT_ENABLED
    BrdfInputData brdfInput = prepareBrdfInputData(processedData);
    
    LightInputData lightInput;

    for(int i = 0; i < DIRECTIONAL_LIGHT_COUNT; ++i)
    {
        LightDataDirectional lightData = getLightDataDirectional(i);            
        if (prepareLightBrdfDirectional(lightData, processedData, lightInput))
        {
            cOut.rgb += evalBrdf(globalBrdfInput, brdfInput, lightInput);
        }
    }

    for(int i = 0; i < POINT_LIGHT_COUNT; ++i)
    {
        LightDataPoint lightData = getLightDataPoint(i);
        if (prepareLightBrdfPoint(lightData, processedData, lightInput))
        {
            cOut.rgb += evalBrdf(globalBrdfInput, brdfInput, lightInput);
        }    
    }

    for(int i = 0; i < SPOT_LIGHT_COUNT; ++i)
    {
        LightDataSpot lightData = getLightDataSpot(i);            
        if (prepareLightBrdfSpot(lightData, processedData, lightInput))
        {
            cOut.rgb += evalBrdf(globalBrdfInput, brdfInput, lightInput);
        }    
    }
#endif // DYNAMIC_LIGHT_ENABLED
        
#if IMAGE_BASED_LIGHT_ENABLED
    ImageBasedLightInputData iblInput = prepareImageBasedLightInputData(processedData);
    cOut.rgb += evalImageBasedLight(globalBrdfInput, iblInput);
#endif // IMAGE_BASED_LIGHT_ENABLED

    cOut.rgb += processedData.ambientColor;

#if LIGHT_MAP_ENABLED
    cOut.rgb += processedData.diffuseColorLightMap;
#endif 

#if OCCLUSION_MAP_ENABLED    
    cOut.rgb *= vec3(processedData.occlusion);
#endif

    fragmentColor = cOut;
}
