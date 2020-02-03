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

#ifndef CUSTOM_WIDGET_TRIANGLE_GLES3_SHADER_H_INCLUDED
#define CUSTOM_WIDGET_TRIANGLE_GLES3_SHADER_H_INCLUDED

static const char* VertexShaderGLES3 =
"uniform highp mat4 uProjMat;                          \n"
"uniform highp mat4 uModelMat;                         \n"
"                                                      \n"
"layout (location = 0) in highp vec2 aPosition;        \n"
"                                                      \n"
"void main()                                           \n"
"{                                                     \n"
"    vec4 pos = uModelMat * vec4(aPosition, 0.0, 1.0); \n"
"    gl_Position = uProjMat * pos;                     \n"
"}                                                     \n";

static const char* FragmentShaderGLES3 =
"uniform lowp vec4 uColor;                             \n"
"                                                      \n"
"out highp vec4 fragmentColor;                         \n"
"                                                      \n"
"void main()                                           \n"
"{                                                     \n"
"    fragmentColor = uColor;                           \n"
"}                                                     \n";

static const char* VertexShaderWithAlphaMaskGLES3 =
"uniform highp mat4 uProjMat;                               \n"
"uniform highp mat4 uModelMat;                              \n"
"uniform highp mat4 uObject2Alpha;                          \n"
"                                                           \n"
"layout(location = 0) in highp vec2 aPosition;              \n"
"                                                           \n"
"out highp vec2 vAlphaTex;                                  \n"
"                                                           \n"
"void main()                                                \n"
"{                                                          \n"
"    highp vec2 tex = vec2(aPosition.x, aPosition.y);       \n"
"    vAlphaTex = (uObject2Alpha * vec4(tex, 0.0, 1.0)).xy;  \n"
"                                                           \n"
"    highp vec4 pos = uModelMat * vec4(aPosition, 0.0, 1.0);\n"
"    gl_Position = uProjMat * pos;                          \n"
"}                                                          \n";


static const char* FragmentShaderWithAlphaMaskGLES3 =
"uniform lowp vec4 uColor;                                        \n"
"uniform sampler2D uTextureAlphaMask;                             \n"
"                                                                 \n"
"in highp  vec2 vAlphaTex;                                        \n"
"out highp vec4 fragmentColor;                                    \n"
"                                                                 \n"
"void main()                                                      \n"
"{                                                                \n"
"    highp vec4 color = uColor;                                   \n"
"    color.w *= texture(uTextureAlphaMask, vAlphaTex).x;          \n"
"                                                                 \n"
"    if(color.a > 0.0)                                            \n"
"    {                                                            \n"
"        fragmentColor = color;                                   \n"
"    }                                                            \n"
"    else                                                         \n"
"    {                                                            \n"
"        discard;                                                 \n"
"    }                                                            \n"
"}                                                                \n";

#endif // CUSTOM_WIDGET_TRIANGLE_GLES3_SHADER_H_INCLUDED
