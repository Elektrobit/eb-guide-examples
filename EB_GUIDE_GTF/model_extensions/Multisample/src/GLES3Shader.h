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

#ifndef MULTISAMPLE_GLES3_SHADER_H_INCLUDED
#define MULTISAMPLE_GLES3_SHADER_H_INCLUDED

static const char* OffscreenVertexShaderGLES3 = R"(
uniform highp mat4 uProjMat;
uniform highp mat4 uModelMat;

layout(location = 0) in highp vec2 aPosition;

void main()
{
    vec4 pos = uModelMat * vec4(aPosition, 0.0, 1.0);
    gl_Position = uProjMat * pos;
}
)";

static const char* OffscreenFragmentShaderGLES3 = R"(
uniform lowp vec4 uColor;

out highp vec4 fragmentColor;

void main()
{
    fragmentColor = uColor;
}
)";

static const char* VertexShaderGLES3 = R"(
uniform highp mat4 uModelMat;
uniform highp mat4 uProjMat;

layout(location = 0) in highp vec2 aPosition;
layout(location = 1) in highp vec2 aTexCoord;

out mediump vec2 vTexCoord;

void main()
{
    vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);

    vec4 pos = uModelMat * vec4(aPosition, 0.0, 1.0);
    gl_Position = uProjMat * pos;
}
)";

static const char* FragmentShaderGLES3 = R"(
uniform sampler2D uTexture;

in mediump vec2 vTexCoord;

out highp vec4 fragmentColor;

void main()
{
    fragmentColor = texture(uTexture, vTexCoord);
}
)";

static const char* VertexShaderWithAlphaMaskGLES3 = R"(
uniform highp mat4 uProjMat;
uniform highp mat4 uModelMat;
uniform highp mat4 uObject2Alpha;

layout(location = 0) in highp vec2 aPosition;
layout(location = 1) in highp vec2 aTexCoord;

out mediump vec2 vTexCoord;
out mediump vec2 vAlphaTexCoord;

void main()
{
    vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
    vAlphaTexCoord = (uObject2Alpha * vec4(aTexCoord, 0.0, 1.0)).xy;
    
    vec4 pos    = uModelMat * vec4(aPosition, 0.0, 1.0);
    gl_Position = uProjMat * pos;
}
)";


static const char* FragmentShaderWithAlphaMaskGLES3 = R"(
uniform sampler2D uTexture;
uniform sampler2D uTextureAlphaMask;

in mediump vec2 vTexCoord;
in mediump vec2 vAlphaTexCoord;

out highp vec4 fragmentColor;

void main()
{
    highp vec4 color = texture(uTexture, vTexCoord);
    color.a *= texture(uTextureAlphaMask, vAlphaTexCoord).x;
    color.rgb *= color.a;

    if (color.a > 0.0)
    {
       fragmentColor = color;
    }
    else
    {
        discard;
    }
}
)";

#endif // MULTISAMPLE_GLES3_SHADER_H_INCLUDED
