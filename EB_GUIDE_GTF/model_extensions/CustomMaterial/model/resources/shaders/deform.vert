///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

uniform highp float u_deformation;
uniform highp float u_customShaderConstant1;
uniform highp float u_customShaderConstant2;

#define E 2.7182818284590452353602874713527

void main()
{
    RawData rawInput = readInData();
    
    /// begin custom adaptions ///
    float steeringAngle = u_deformation;
    float val = pow( E, -0.0015 * pow(rawInput.position.y, 3.5));

    vec4 bendPos;

    bendPos = vec4( rawInput.position.x - (steeringAngle * (1.0 - val)), rawInput.position.y, rawInput.position.z * 1.0, 1.0 );

    if (rawInput.position.y > 0.1 && rawInput.position.y < 12.0) {
        if( rawInput.position.x > 0.0 ) {
            bendPos.y = bendPos.y + u_customShaderConstant1;
        } else {
            bendPos.y = bendPos.y + u_customShaderConstant2;
        }
    }

    rawInput.position = bendPos.xyz;
    /// end custom adaptions ///
    
    ProcessedData processedData = processData(rawInput);
    writeOutData(processedData);
    
    gl_Position = processedData.glPosition;
}
