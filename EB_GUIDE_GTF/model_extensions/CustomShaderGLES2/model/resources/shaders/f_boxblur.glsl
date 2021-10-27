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

// custom uniforms
uniform int u_blurIntensity;

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
  highp vec4 color = vec4(0.0);
  if (u_blurIntensity <= 0)
  {
    color = gtfGetChildTextureColor(v_texCoord0);
  }
  else
  {
    highp vec2 invSize = 1.0 / gtfChildTextureSize;
    highp vec4 sum = vec4(0.0);

    for (int x = -u_blurIntensity; x <= u_blurIntensity; ++x)
    {
      for (int y = -u_blurIntensity; y <= u_blurIntensity; ++y)
      {
          mediump vec2 uv = v_texCoord0 + vec2(x, y) * invSize;
          sum += gtfGetChildTextureColor(uv);
      }
    }
    color = sum / vec4((2 * u_blurIntensity + 1) * (2 * u_blurIntensity + 1));
  }

  fragmentColor = color;
}
