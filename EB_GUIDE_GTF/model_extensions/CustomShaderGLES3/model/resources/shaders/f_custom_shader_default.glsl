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

uniform highp vec4 u_fillColor;

#ifdef GLES3

#define texture2D texture
out highp vec4 fragmentColor;

#else

#define fragmentColor gl_FragColor

#endif // GLES3

void main()
{
    fragmentColor = u_fillColor;
}
