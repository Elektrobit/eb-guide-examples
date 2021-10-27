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

uniform highp float u_globalTransparency; // extra opacity

void main()
{
    RawData rawData = getRawData();
    
    /// begin custom adaptions ///
    highp float opacity = rawData.opacity * u_globalTransparency;
    
#if TRANSPARENCY_ENABLED
    if(opacity < 0.01)
    {
        discard;
    }
#endif
    /// end custom adaptions ///

    ProcessedData processedData = processData(rawData);
    
    /// begin custom adaptions ///
    processedData.opacity = opacity; 
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
