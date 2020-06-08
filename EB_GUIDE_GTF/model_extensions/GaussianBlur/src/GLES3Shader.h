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

#ifndef GAUSSIAN_BLUR_GLES3_SHADER_H
#define GAUSSIAN_BLUR_GLES3_SHADER_H

static const char* BlurVertexShaderGLES3 =
    "layout (location = 0) in highp vec2 aPosition;                                                       "
    "layout (location = 1) in highp vec2 aTexCoord;                                                       "
    "                                                                                                     "
    "out mediump vec2 vTexCoord;                                                                          "
    "                                                                                                     "
    "void main()                                                                                          "
    "{                                                                                                    "
    "    gl_Position = vec4(vec2(2.0) * aPosition - vec2(1.0), 0.0, 1.0);                                 "
    "    vTexCoord   = aTexCoord;                                                                         "
    "}                                                                                                    ";

static const char* BlurFragmentShaderGLES3 =
    "uniform sampler2D   uTexture0;                                                                       "    
    "uniform highp vec2  uDirection;                                                                      "
    "uniform highp float uWeights[32];                                                                    "
    "uniform highp float uOffsets[32];                                                                    "
    "uniform highp int   uWeightCount;                                                                    "
    "                                                                                                     "
    "in mediump vec2 vTexCoord;                                                                           "    
    "out highp vec4 fragmentColor;                                                                        "
    "                                                                                                     "
    "void main()                                                                                          "
    "{                                                                                                    "
    "    mediump vec2 uv = vTexCoord + uDirection * uOffsets[0];                                          "
    "    highp vec4 color = uWeights[0] * texture(uTexture0, uv);                                         "
    "    for(int i = 1; i < uWeightCount; ++i)                                                            "
    "    {                                                                                                "
    "        mediump vec2 uv0 = vTexCoord + uDirection * uOffsets[i];                                     "
    "        mediump vec2 uv1 = vTexCoord - uDirection * uOffsets[i];                                     "
    "                                                                                                     "
    "        color += uWeights[i] * texture(uTexture0, uv0);                                              "
    "        color += uWeights[i] * texture(uTexture0, uv1);                                               "
    "    }                                                                                                "
    "    fragmentColor = color;                                                                           "
    "}                                                                                                    ";


static const char* BlitVertexShaderGLES3 =
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

static const char* BlitVertexShaderWithAlphaMaskGLES3 =
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

static const char* BlitFragmentShaderGLES3 =
"uniform sampler2D uTexture0;                                                                             "
"                                                                                                         "
"in mediump vec2 vTexCoord;                                                                               "
"                                                                                                         "
"out highp vec4 fragmentColor;                                                                            "
"                                                                                                         "
"void main()                                                                                              "
"{                                                                                                        "
"    fragmentColor = texture(uTexture0, vTexCoord);                                                       "
"}                                                                                                        ";

static const char* BlitFragmentShaderWithAlphaMaskGLES3 =
"uniform sampler2D uTexture0;                                                                             "
"uniform sampler2D uTexture1;                                                                             "
"                                                                                                         "
"in mediump vec2 vTexCoord;                                                                               "
"in mediump vec2 vAlphaTexCoord;                                                                          "
"                                                                                                         "
"out highp vec4 fragmentColor;                                                                            "
"                                                                                                         "
"void main()                                                                                              "
"{                                                                                                        "
"    highp vec4 color = texture(uTexture0, vTexCoord);                                                    "
"                                                                                                         "
"    color.w *= texture(uTexture1, vAlphaTexCoord).x;                                                     "
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

#endif // GAUSSIAN_BLUR_GLES3_SHADER_H_INCLUDED
