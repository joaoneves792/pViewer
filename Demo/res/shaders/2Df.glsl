#version 330 core

in vec2 uv;

uniform sampler2D renderedTexture;

uniform float texWidth;
uniform float texHeight;
uniform float flip;

out vec4 color;

#define vRatio (16.0f/9.0f)

void main() {

    const float ratio1 = vRatio;
    float ratio2 = texWidth / texHeight;
    float scaleRatio;

    //adjust scalingp
    if(ratio2 >= ratio1)
        scaleRatio = (vRatio) / texWidth;
    else
        scaleRatio =  1.0f / texHeight;

    float w3 = texWidth * scaleRatio;
    float h3 = texHeight * scaleRatio;

    vec2 correctUV = (flip < 0)?vec2(uv.x, -uv.y+1.0f):uv;
    vec2 centeredUV = correctUV - vec2(0.5-((w3/vRatio)/2.0f) , (0.5-(h3/2.0f)));

    vec2 adjustedUV = centeredUV * vec2(vRatio/w3, 1.0f/h3);

    color.rgba = texture(renderedTexture, adjustedUV).rgba;
    color.a = color.a * step(adjustedUV.x, 1.0f);
    color.a = color.a * step(adjustedUV.y, 1.0f);
    color.a = color.a * step(0.0f, adjustedUV.x);
    color.a = color.a * step(0.0f, adjustedUV.y);
}
