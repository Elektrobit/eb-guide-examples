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

#ifndef MULTISAMPLE_GLES20_SHADER_H_INCLUDED
#define MULTISAMPLE_GLES20_SHADER_H_INCLUDED

static const char* OffscreenVertexShaderGLES20 = R"(
uniform highp mat4 uProjMat;
uniform highp mat4 uModelMat;
uniform lowp vec4 uColor;

attribute highp vec2 aPosition;

varying lowp vec4 vColor;

void main()
{
    vColor = uColor;
    vec4 pos = uModelMat * vec4(aPosition, 0.0, 1.0);
    gl_Position = uProjMat * pos;
}
)";

static const char* OffscreenFragmentShaderGLES20 = R"(
varying lowp vec4 vColor;

void main()
{
    gl_FragColor = vColor;
}
)";

static const char* VertexShaderGLES20 = R"(
uniform highp mat4 uProjMat;
uniform highp mat4 uModelMat;

attribute highp vec2 aPosition;
attribute highp vec2 aTexCoord;

varying mediump vec2 vTexCoord;

void main()
{
    vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
    
    vec4 pos = uModelMat * vec4(aPosition, 0.0, 1.0);
    gl_Position = uProjMat * pos;    
}
)";

static const char* FragmentShaderGLES20 = R"(
uniform sampler2D uTexture;

varying mediump vec2 vTexCoord;

void main()
{
    gl_FragColor = texture2D(uTexture, vTexCoord);
}
)";

static const char* VertexShaderWithAlphaMaskGLES20 = R"(
uniform highp mat4 uProjMat;
uniform highp mat4 uModelMat;
uniform highp mat4 uObject2Alpha;

attribute highp vec2 aPosition;
attribute highp vec2 aTexCoord;

varying mediump vec2 vTexCoord;
varying mediump vec2 vAlphaTexCoord;

void main()
{
    vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
    vAlphaTexCoord = (uObject2Alpha * vec4(aTexCoord, 0.0, 1.0)).xy;

    vec4 pos = uModelMat * vec4(aPosition, 0.0, 1.0);
    gl_Position = uProjMat * pos;
}
)";

static const char* FragmentShaderWithAlphaMaskGLES20 = R"(
uniform sampler2D uTexture;
uniform sampler2D uTextureAlphaMask;

varying mediump vec2 vTexCoord;
varying mediump vec2 vAlphaTexCoord;

void main()
{
    highp vec4 color = texture2D(uTexture, vTexCoord);
    color.w *= texture2D(uTextureAlphaMask, vAlphaTexCoord).x;
    color.rgb *= color.a;    

    if (color.a > 0.0)
    {
        gl_FragColor = color;
    }
    else
    {
        discard;
    }
}
)";

#endif // MULTISAMPLE_GLES20_SHADER_H_INCLUDED
