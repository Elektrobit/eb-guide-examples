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

uniform highp float u_baseTransparency;
uniform highp float u_type;

highp float sqr(in highp float x)
{
  return x*x;
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

    /// begin custom adaptions ///
    if (u_type >= 1.0)
    {
      highp float luminance = dot(vec3(0.27, 0.67, 0.06), cOut.xyz);
      highp float reinhardLuminace = luminance / (1.0 + luminance);

      if (u_type == 1.0)
        cOut.a *= u_baseTransparency + (1.0 - u_baseTransparency) * sqrt(reinhardLuminace);
      else if (u_type == 2.0)
        cOut.a *= u_baseTransparency + (1.0 - u_baseTransparency) * sqr(reinhardLuminace);
      else if (u_type == 3.0)
        cOut.a *= u_baseTransparency + (1.0 - u_baseTransparency) * (reinhardLuminace);
    }
    /// end custom adaptions ///

    fragmentColor = cOut;
}
