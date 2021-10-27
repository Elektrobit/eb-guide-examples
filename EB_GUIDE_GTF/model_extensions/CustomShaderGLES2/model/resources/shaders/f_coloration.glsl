// Copyright (c) Elektrobit Automotive GmbH
// Alle Rechte vorbehalten. All Rights Reserved.
//
// Information contained herein is subject to change without notice.
// Elektrobit retains ownership and all other rights in the software and each
// component thereof.
// Any reproduction of the software or components thereof without the prior
// written permission of Elektrobit is prohibited.
////////////////////////////////////////////////////////////////////////////////

uniform sampler2D u_image;

#ifdef GLES3

#define texture2D texture
in mediump vec2 v_texCoord0;
out highp vec4 fragmentColor;

#else

varying mediump vec2 v_texCoord0;
#define fragmentColor gl_FragColor

#endif // GLES3

void main()
{
    highp vec4 finalColor = texture2D(u_image, vec2(v_texCoord0.x, 1.0 - v_texCoord0.y));

    fragmentColor = finalColor;
}
