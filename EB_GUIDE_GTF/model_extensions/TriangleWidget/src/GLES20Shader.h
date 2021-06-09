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

#ifndef CUSTOM_WIDGET_TRIANGLE_GLES20_SHADER_H_INCLUDED
#define CUSTOM_WIDGET_TRIANGLE_GLES20_SHADER_H_INCLUDED

static const char* VertexShaderGLES20 =
"uniform highp mat4 uProjMat;                          \n"
"uniform highp mat4 uModelMat;                         \n"
"uniform lowp vec4  uColor;                            \n"
"                                                      \n"
"attribute highp vec2 aPosition;                       \n"
"                                                      \n"
"varying lowp vec4 vColor;                             \n"
"                                                      \n"
"void main()                                           \n"
"{                                                     \n"
"    vColor = uColor;                                  \n"
"    vec4 pos = uModelMat * vec4(aPosition, 0.0, 1.0); \n"
"    gl_Position = uProjMat * pos;                     \n"
"}                                                     \n";

static const char* FragmentShaderGLES20 =
"varying lowp vec4 vColor;                             \n"
"                                                      \n"
"void main()                                           \n"
"{                                                     \n"
"    gl_FragColor = vColor;                            \n"
"}                                                     \n";

static const char* VertexShaderWithAlphaMaskGLES20 =
"uniform highp mat4 uProjMat;                                \n"
"uniform highp mat4 uModelMat;                               \n"
"uniform lowp  vec4 uColor;                                  \n"
"uniform highp mat4 uObject2Alpha;                           \n"
"                                                            \n"
"attribute highp vec2 aPosition;                             \n"
"                                                            \n"
"varying lowp vec4 vColor;                                   \n"
"varying highp vec2 vAlphaTex;                               \n"
"                                                            \n"
"void main()                                                 \n"
"{                                                           \n"
"    vColor = uColor;                                        \n"
"    vec4 pos = uModelMat * vec4(aPosition, 0.0, 1.0);       \n"
"    highp vec2 tex = vec2(aPosition.x, aPosition.y);        \n"
"    vAlphaTex = (uObject2Alpha * vec4(tex, 0.0, 1.0)).xy;   \n"
"                                                            \n"
"    gl_Position = uProjMat * pos;                           \n"
"}                                                           \n" ;

static const char* FragmentShaderWithAlphaMaskGLES20 =
"uniform sampler2D uTextureAlphaMask;                                \n"
"                                                                    \n"
"varying lowp vec4  vColor;                                          \n"
"varying highp vec2 vAlphaTex;                                       \n"
"                                                                    \n"
"void main()                                                         \n"
"{                                                                   \n"
"    highp vec4 color = vColor;                                      \n"
"    color.w *= texture2D(uTextureAlphaMask, vAlphaTex).x;           \n"
"                                                                    \n"
"    if(color.a > 0.0)                                               \n"
"    {                                                               \n"
"        gl_FragColor = color;                                       \n"
"    }                                                               \n"
"    else                                                            \n"
"    {                                                               \n"
"        discard;                                                    \n"
"    }                                                               \n"
"}                                                                   \n" ;

#endif // CUSTOM_WIDGET_TRIANGLE_GLES20_SHADER_H_INCLUDED
