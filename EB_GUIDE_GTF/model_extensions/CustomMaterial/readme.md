# Custom Material Widgets

Adds several custom materials with special shaders for usage in the scene graph widget.

## Setup
The custom shader specification is done in the _resources/MaterialWidgetDescriptors.json_ file.
Add this file to the model in the project settings `Configure -> Profiles -> Model`.

You can specify custom shaders for each BRDF type (PBR GGX, PBR Phong, Phong). Inherit the widget from the appropriate types:
* **GtfPbrGgxMaterialWidget**
* **GtfPbrPhongMaterialWidget**
* **GtfPhongMaterialWidget**

The shader specific options look similar to this:

```
"shaderSpecification":{
    "gles2":{
        "fragmentShaderPath":"resources/shaders/gles2FragmentShader.frag"
        "vertexShaderPath":"resources/shaders/gles2VertexShader.vert"
    },
    "gles3":{
        "fragmentShaderPath":"resources/shaders/gles3FragmentShader.frag"
        "vertexShaderPath":"resources/shaders/gles3VertexShader.vert"
    },
    "isTransparent":true
}
```

You can specify different shaders for each Graphics API type, e.g. `gles2` and `gles3`.
You may also specify the same shader for multiple graphics API types, in case they are compatible.
For GLES, the shaders are assumed to be written in GLSL, and may have any file name/suffix.

The shader specific options are defined as follows:
* **fragmentShaderPath**: The path to the fragment shader, relative to the model directory.
* **vertexShaderPath**: The path to the vertex shader, relative to the model directory.
* **isTransparent**: Set to true, if the material is meant to be transparent, to false if it is always opaque. If not specified, the transparency is determined automatically based on the opacity property or the **Opaque texture** widget feature.

## Custom shader input

A shader can have up to any number of custom uniforms of the following types [and mapped GLSL types]:
* **float** [uniform float]
* **color** [uniform vec4]
* **image** [uniform sampler2D]

They are specified as usual widget properties similar to
```
"name":"customShaderConstant0",
"type":"float",
"category":"Common",
"description":"A custom shader constant",
"defaultValue":"0.2"
```

The names must match the uniform names in the shaders.

For instructions see [EB GUIDE GTF examples](../../readme.md)
