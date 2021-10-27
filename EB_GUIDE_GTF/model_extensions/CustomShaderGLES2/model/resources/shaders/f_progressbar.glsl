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

uniform highp sampler2D  u_barImage;         
uniform sampler2D   u_colorationImage;  
uniform highp float u_progress;   
uniform bool        u_enableDynamicColoration;

#ifdef GLES3

#define texture2D texture 
in highp vec2 v_texCoord0;
out highp vec4 fragmentColor;

#else

varying mediump vec2 v_texCoord0;
#define fragmentColor gl_FragColor

#endif // GLES3

void main()
{
    highp vec2 mask = texture2D(u_barImage, vec2(v_texCoord0.x, 1.0 - v_texCoord0.y)).xy;
    highp vec2 colLookUp = vec2(mask.y, 0.5);
    if(u_enableDynamicColoration)
    {
        colLookUp.x = u_progress/100.0;
    }
    highp vec4 color = texture2D(u_colorationImage, colLookUp);

    color = color * mask.x * step(mask.y, u_progress / 100.0);

    fragmentColor = color;

}
