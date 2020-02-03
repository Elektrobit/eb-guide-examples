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

#ifndef GAUSSIAN_BLUR_GLES20_SHADER_H
#define GAUSSIAN_BLUR_GLES20_SHADER_H

static const char* BlitVertexShaderGLES20 =
    "uniform highp mat4 uProjMat;                                                                             "
    "uniform highp mat4 uModelMat;                                                                            "
    "                                                                                                         "
    "attribute highp vec2 aPosition;                                                                          "
    "attribute highp vec2 aTexCoord;                                                                          "
    "                                                                                                         "
    "varying mediump vec2 vTexCoord;                                                                          "
    "                                                                                                         "
    "void main()                                                                                              "
    "{                                                                                                        "
    "    vec4 pos    = uModelMat * vec4(aPosition, 0.0, 1.0);                                                 "
    "    gl_Position = uProjMat * pos;                                                                        "
    "    vTexCoord   = vec2(aTexCoord.x, 1.0 - aTexCoord.y);                                                  "
    "}                                                                                                        ";

static const char* BlitVertexShaderWithAlphaMaskGLES20 =
    "uniform highp mat4 uProjMat;                                                                             "
    "uniform highp mat4 uModelMat;                                                                            "
    "uniform highp mat4 uObject2Alpha;                                                                        "
    "                                                                                                         "
    "attribute highp vec2 aPosition;                                                                          "
    "attribute highp vec2 aTexCoord;                                                                          "
    "                                                                                                         "
    "varying mediump vec2 vTexCoord;                                                                          "
    "varying mediump vec2 vAlphaTexCoord;                                                                     "
    "                                                                                                         "
    "void main()                                                                                              "
    "{                                                                                                        "
    "    vec4 pos       = uModelMat * vec4(aPosition, 0.0, 1.0);                                              "
    "    gl_Position    = uProjMat * pos;                                                                     "
    "    vTexCoord      = vec2(aTexCoord.x, 1.0 - aTexCoord.y);                                               "
    "    vAlphaTexCoord = (uObject2Alpha * vec4(aTexCoord, 0.0, 1.0)).xy;                                     "
    "}                                                                                                        ";

static const char* BlitFragmentShaderGLES20 =
    "uniform sampler2D uTexture0;                                                                             "
    "                                                                                                         "
    "varying mediump vec2 vTexCoord;                                                                          "
    "                                                                                                         "
    "void main()                                                                                              "
    "{                                                                                                        "
    "    gl_FragColor = texture2D(uTexture0, vTexCoord);                                                      "
    "}                                                                                                        ";

static const char* BlitFragmentShaderWithAlphaMaskGLES20 =
    "uniform sampler2D uTexture0;                                                                             "
    "uniform sampler2D uTexture1;                                                                             "
    "                                                                                                         "
    "varying mediump vec2 vTexCoord;                                                                          "
    "varying mediump vec2 vAlphaTexCoord;                                                                     "
    "                                                                                                         "
    "                                                                                                         "
    "void main()                                                                                              "
    "{                                                                                                        "
    "    highp vec4 color = texture2D(uTexture0, vTexCoord);                                                  "
    "                                                                                                         "
    "    color.w *= texture2D(uTexture1, vAlphaTexCoord).x;                                                   "
    "                                                                                                         "
    "    if(color.a > 0.0)                                                                                    "
    "    {                                                                                                    "
    "        gl_FragColor = color;                                                                            "
    "    }                                                                                                    "
    "    else                                                                                                 "
    "    {                                                                                                    "
    "        discard;                                                                                         "
    "    }                                                                                                    "
    "}                                                                                                        ";

#endif // GAUSSIAN_BLUR_GLES20_SHADER_H_INCLUDED
