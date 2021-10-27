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

uniform highp mat4 u_object2World;

#ifdef GLES3
layout(location = 0) in highp vec2  a_position0;
layout (location = 1) in highp vec2 a_texCoord0; 

out mediump vec2 v_texCoord0;

#else // GLES2
attribute highp vec2 a_position0;
attribute highp vec2 a_texCoord0;

varying mediump vec2 v_texCoord0;

#endif // GLES3

void main()
{
    highp vec4 pos  = vec4(a_position0, 0.0, 1.0);
    highp vec4 opos = u_object2World * pos;
    
    v_texCoord0    = vec2(a_texCoord0.x, a_texCoord0.y);

    gl_Position = opos;
}

