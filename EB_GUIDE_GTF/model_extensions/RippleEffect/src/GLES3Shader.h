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

#ifndef CUSTOM_WIDGET_RIPPLE_EFFECT_GLES3_SHADER_H_INCLUDED
#define CUSTOM_WIDGET_RIPPLE_EFFECT_GLES3_SHADER_H_INCLUDED

static const char* VertexShaderGLES3 =
"uniform highp mat4 uProjMat;                                                                             "
"uniform highp mat4 uModelMat;                                                                            "
"                                                                                                         "
"layout (location = 0) in highp vec2 aPosition;                                                           "
"layout (location = 1) in highp vec2 aTexCoord;                                                           "
"                                                                                                         "
"out mediump vec2 vTexCoord;                                                                              "
"                                                                                                         "
"void main()                                                                                              "
"{                                                                                                        "
"    vec4 pos    = uModelMat * vec4(aPosition, 0.0, 1.0);                                                 "
"    gl_Position = uProjMat * pos;                                                                        "
"    vTexCoord   = vec2(aTexCoord.x, 1.0 - aTexCoord.y);                                                  "
"}                                                                                                        ";

static const char* VertexShaderWithAlphaMaskGLES3 =
"uniform highp mat4 uProjMat;                                                                             "
"uniform highp mat4 uModelMat;                                                                            "
"uniform highp mat4 uObject2Alpha;                                                                        "
"                                                                                                         "
"layout (location = 0) in highp vec2 aPosition;                                                           "
"layout (location = 1) in highp vec2 aTexCoord;                                                           "
"                                                                                                         "
"out mediump vec2 vTexCoord;                                                                              "
"out mediump vec2 vAlphaTexCoord;                                                                         "
"                                                                                                         "
"void main()                                                                                              "
"{                                                                                                        "
"    vec4 pos       = uModelMat * vec4(aPosition, 0.0, 1.0);                                              "
"    gl_Position    = uProjMat * pos;                                                                     "
"    vTexCoord      = vec2(aTexCoord.x, 1.0 - aTexCoord.y);                                               "
"    vAlphaTexCoord = (uObject2Alpha * vec4(aTexCoord, 0.0, 1.0)).xy;                                     "
"}                                                                                                        ";

static const char* FragmentShaderGLES3 =
"uniform highp float uRippleTime;                                                                         "
"uniform highp float uRippleEnable;                                                                       "
"uniform sampler2D   u_texture0;                                                                          "
"                                                                                                         "
"in mediump vec2 vTexCoord;                                                                               "
"                                                                                                         "
"out highp vec4 fragmentColor;                                                                            "
"                                                                                                         "
"void main()                                                                                              "
"{                                                                                                        "
"    highp vec2 p  = -1.0 + 2.0 * vTexCoord;                                                              "
"    highp float l = length(p);                                                                           "
"    highp vec2 uv = vTexCoord + uRippleEnable * (p / l)*cos(l * 12.0 - 4.0 * uRippleTime / 100.0)*0.03;  "
"    fragmentColor = texture(u_texture0, uv);                                                             "
"}                                                                                                        ";

static const char* FragmentShaderWithAlphaMaskGLES3 =
"uniform highp float uRippleTime;                                                                         "
"uniform highp float uRippleEnable;                                                                       "
"uniform sampler2D   u_texture0;                                                                          "
"uniform sampler2D   u_texture1;                                                                          "
"                                                                                                         "
"in mediump vec2 vTexCoord;                                                                               "
"in mediump vec2 vAlphaTexCoord;                                                                          "
"                                                                                                         "
"out highp vec4 fragmentColor;                                                                            "
"                                                                                                         "
"void main()                                                                                              "
"{                                                                                                        "
"    highp vec2 p  = -1.0 + 2.0 * vTexCoord;                                                              "
"    highp float l = length(p);                                                                           "
"    highp vec2 uv = vTexCoord + uRippleEnable * (p / l)*cos(l * 12.0 - 4.0 * uRippleTime / 100.0)*0.03;  "
"    highp vec4 color = texture(u_texture0, uv);                                                          "
"                                                                                                         "
"    color.w *= texture(u_texture1, vAlphaTexCoord).x;                                                    "
"                                                                                                         "
"    if(color.a > 0.0)                                                                                    "
"    {                                                                                                    "
"        fragmentColor = color;                                                                           "
"    }                                                                                                    "
"    else                                                                                                 "
"    {                                                                                                    "
"        discard;                                                                                         "
"    }                                                                                                    "
"}                                                                                                        " ;

#endif // CUSTOM_WIDGET_RIPPLE_EFFECT_GLES3_SHADER_H_INCLUDED
