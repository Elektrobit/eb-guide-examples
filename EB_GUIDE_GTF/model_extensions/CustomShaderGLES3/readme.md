# Custom Shader Widgets

Adds several custom widgets with special shaders. These widgets are presented in two example models.
* CustomShaderGLES2 shows the widgets which work with the openGLRenderer as the active renderer
* CustomShaderGLES3 shows the widgets which work with the openGL3Renderer as the active renderer

Incorrect configuration or shader code will result in failed shader compilation. In this case a solid red box is displayed.

## How to add the custom shader widgets example to your model

The custom shader specification is done in the ShaderWidgetDescriptors.json file, and must be added to the resources folder of your model. In addition the images folder and the shaders folder must be copied to the resources folder.
This file needs to be added to the model in the project settings `Configure -> Profiles -> Model` as follows:

```json
{
 "gtf": {
    "model": {
      "descriptorFiles": [
        "resources/ShaderWidgetDescriptors.json"
      ]
    }
  }
}
```

## How to add your own custom shader widget

A custom shader widget can be added by defining a *widget descriptor* in a .json file. Widget type must be `"GtfCustomShader"` to enable the JSON object `"shaderSpecification"`. For details have a look at the EB GUIDE GTF user guide.

Example of a custom shader widget descriptor:

```json
{
  "widgets": [
    {
      "name": "MyCustomShader",
      "alias": "CustomShader",
      "type": "GtfCustomShader",
      "category": "CustomShaderWidget",
      "description": "My custom shader.",
      "canHaveChildren": false,
      "useOutputFromChildren": true,
      "transparencyMode": 1,
      "properties": [
        {
          "name": "u_customUniform",
          "type": "integer",
          "description": "The description.",
          "defaultValue": "1"
        }
      ],
      "shaderSpecification": {
        "gles2": {
          "fragmentShaderPath": "resources/shaders/gles2FragmentShader.frag",
          "vertexShaderPath": "resources/shaders/gles2VertexShader.vert"
        },
        "gles3": {
          "fragmentShaderPath": "resources/shaders/gles3FragmentShader.frag",
          "vertexShaderPath": "resources/shaders/gles3VertexShader.vert"
        }
      }
    }
  ]
}
```

### Adding shader uniform properties
Every defined property within the JSON `"properties"` object is mapped to the shader uniform with the same name and type of the registered shader file.

#### Type mapping:

| JSON "type" value | Uniform type |
| --------------------  | --- |
| "bool" | bool |
| "float" | float |
|  "integer" | int |
| "color" | vec4 |
| "list_float" | see below |
| "list_integer" | see below |

#### Using lists
Lists can be used to map arbitrary arrays of data in shaders e.g. it can be defined as a vec3[4], float[16] or int[8]. Correct amount of values must be modelled in the UI.

### Adding shader file path
You can specify different shaders for each Graphics API type, i.e. `"gles2"` and `"gles3"`.
You may also specify the same shader for multiple graphics API types, in case they are compatible.
For GLES, the shaders are assumed to be written in GLSL, and may have any file name/suffix.

The shader specific options are defined as follows:
* `"fragmentShaderPath"`: The path to the fragment shader, relative to the model directory.
* `"vertexShaderPath"`: The path to the vertex shader, relative to the model directory.

### Additional features
Additional features can be used with custom shader widgets. The configuration is described below.

#### Use output texture from child widgets
* See example custom shader widget `BoxBlur`
* Widget descriptor configuration
  + `"useOutputFromChildren": true`
  + `"canHaveChildren": true`
* use function `highp vec4 gtfGetChildTextureColor(vec2 texCoord)` in fragment shader to retrieve color from child widget

#### Configure transparency mode
* Widget descriptor configuration
  + `"transparencyMode": 0` disable transparency
  + `"transparencyMode": 1` use additive blending
  + `"transparencyMode": 2` use premultiplied blending

#### Use alpha mask
* See example custom shader widget `BoxBlur`
* Widget descriptor configuration
  + `"useAlphaMask": true`
* Add an alpha mask widget to the view
* Make the custom shader widget a child of the alpha mask widget to apply the alpha mask

## References:
[EB GUIDE GTF examples](../../readme.md)
 EB GUIDE GTF user guide
