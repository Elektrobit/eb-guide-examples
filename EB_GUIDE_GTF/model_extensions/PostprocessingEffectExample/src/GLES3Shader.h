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

#ifndef POSTPROCESSINGEXAMPLE_GLES3SHADER_H_INCLUDED
#define POSTPROCESSINGEXAMPLE_GLES3SHADER_H_INCLUDED

static const char* f_chromaticAberrationShader_GLES3 = R"(
uniform sampler2D   u_texture0;
uniform highp float u_intensity;
in mediump vec2     v_texcoord0;
out highp vec4      fragmentColor;

void main()
{	
	mediump float distFromCenter = length(v_texcoord0.xy - vec2(0.5, 0.5));

	mediump float offset = (u_intensity/10.0) * distFromCenter;
	
	mediump vec2 texcoordR = vec2(v_texcoord0.x - offset, v_texcoord0.y);
	mediump vec2 texcoordG = vec2(v_texcoord0.x + offset, v_texcoord0.y);
	mediump vec2 texcoordB = vec2(v_texcoord0.x, v_texcoord0.y - offset);

	mediump float r = texture(u_texture0, texcoordR).r;
	mediump float g = texture(u_texture0, texcoordG).g;
	mediump float b = texture(u_texture0, texcoordB).b;    

	fragmentColor = vec4(r, g, b, texture(u_texture0, v_texcoord0).a);
}
)";

#endif // POSTPROCESSINGEXAMPLE_GLES3SHADER_H_INCLUDED
