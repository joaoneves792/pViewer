#version 330 core

in vec2 uv;

uniform sampler2D frame;


#define LEFT 0
#define RIGHT 1
out vec4 color[2];


//YUV2RGB and RGB2YUV values taken from gstreamer source code
/* BT. 601 standard with the following ranges:
 * Y = [16..235] (of 255)
 * Cb/Cr = [16..240] (of 255)
 */
const vec3 rgboffset = vec3(-0.0625f, -0.5f, -0.5f);
const vec3 rcoeff = vec3(1.164f, 0.000f, 1.596f);
const vec3 gcoeff = vec3(1.164f,-0.391f,-0.813f);
const vec3 bcoeff = vec3(1.164f, 2.018f, 0.000f);

const vec3 yuvoffset = vec3(0.0625f, 0.5f, 0.5f);
const vec3 ycoeff = vec3(0.256816f, 0.504154f, 0.0979137f);
const vec3 ucoeff = vec3(-0.148246f, -0.29102f, 0.439266f);
const vec3 vcoeff = vec3(0.439271f, -0.367833f, -0.071438f);

vec3 rgb_to_yuv (vec3 val) {
  vec3 yuv;
  yuv.r = dot(val.rgb, rcoeff);
  yuv.g = dot(val.rgb, gcoeff);
  yuv.b = dot(val.rgb, bcoeff);
  yuv += rgboffset;
  return yuv;
}

vec3 yuv_to_rgb (vec3 val) {
  vec3 rgb;
  val += yuvoffset;
  rgb.r = dot(val.rgb, ycoeff);
  rgb.g = dot(val.rgb, ucoeff);
  rgb.b = dot(val.rgb, vcoeff);
  return rgb;
}


float rgb2luma(vec3 rgb){
    return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

float extractLumaRed(float red){
    return sqrt(red*0.299);
}

float extractLumaGreen(float green){
    return sqrt(green*0.587);
}

float extractLumaCyan(vec2 c){
    return sqrt(dot(c, vec2(0.587, 0.114)));
}



vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 hsv2hsl(vec3 c){
    float l = 0.5*c.b*(2-c.g);
    return vec3(c.r, (c.g*c.b)/(1-abs(2*l-1)), l);
}

vec3 hsl2hsv(vec3 c){
    float v = 0.5*(2.0*c.b+c.g*(1.0-abs(2*c.b-1.0)));
    return vec3(c.r, 2.0*(v-c.b)/v, v);
}

float blur_kernel[9] = float[](0.11f, 0.11f, 0.11f,
                               0.11f, 0.11f, 0.11f,
                               0.11f, 0.11f, 0.11f);

vec3 convolute(float[9] kernel, vec2 pos, vec2 texelSize){
    float up = clamp(pos.y + texelSize.y, 0.0f, 1.0f);
    float down = clamp(pos.y - texelSize.y, 0.0f, 1.0f);
    float left = clamp(pos.x - texelSize.x, 0.0f, 1.0f);
    float right = clamp(pos.x + texelSize.x, 0.0f, 1.0f);
    vec3 t[9] = vec3[](
        texture(frame, vec2(left, up)).rgb,
        texture(frame, vec2(pos.x, up)).rgb,
        texture(frame, vec2(right, up)).rgb,

        texture(frame, vec2(left, pos.y)).rgb,
        texture(frame, vec2(pos.x, pos.y)).rgb,
        texture(frame, vec2(right, pos.y)).rgb,

        texture(frame, vec2(left, down)).rgb,
        texture(frame, vec2(pos.x, down)).rgb,
        texture(frame, vec2(right, down)).rgb
    );

    return kernel[8]*t[0] + kernel[7]*t[1] + kernel[6]*t[2] +
           kernel[5]*t[3] + kernel[4]*t[4] + kernel[3]*t[5] +
           kernel[2]*t[6] + kernel[1]*t[7] + kernel[0]*t[8];
}

void main() {
    vec2 screenSize = textureSize(frame, 0);
    vec2 texelSize = 1.0f/screenSize;

    //vec3 blurredColor = textureLod(frame, uv, 3.0f).rgb;
    vec3 blurredColor = convolute(blur_kernel, uv, texelSize);
    vec3 yuvColor = rgb_to_yuv(blurredColor);

    float lumaRed =   extractLumaRed(texture(frame, uv).r);
    //float lumaGreen = extractLumaGreen(texture(frame, uv).g);
    float lumaCyan = extractLumaCyan(texture(frame, uv).gb);

    vec3 leftYUVColor  = lumaRed *   vec3(1.0f, yuvColor.g, yuvColor.b);
    //vec3 rightYUVColor = lumaGreen * vec3(1.0f, yuvColor.g, yuvColor.b);
    vec3 rightYUVColor = lumaCyan * vec3(1.0f, yuvColor.g, yuvColor.b);

    rightYUVColor.r = (rightYUVColor.r-(16.0f/255.0f))*1.2f+(16.0f/255.0f);
    vec3 rightHSV = rgb2hsv(yuv_to_rgb(rightYUVColor));
    rightHSV.s *= 0.9;
    vec3 rightHSL = hsv2hsl(rightHSV);
    rightHSL.b -= (10.0f/255.0f);
    rightHSV = hsl2hsv(rightHSL);

    leftYUVColor.r = (leftYUVColor.r-(16.0f/255.0f))*1.2f+(16.0f/255.0f);
    vec3 leftHSV = rgb2hsv(yuv_to_rgb(leftYUVColor));
    leftHSV.s *= 1.1;
    vec3 leftHSL = hsv2hsl(leftHSV);
    leftHSL.b -= (10.0f/255.0f);
    leftHSV = hsl2hsv(leftHSL);

    //vec3 left = yuv_to_rgb(leftYUVColor);
    vec3 left =  hsv2rgb(leftHSV);
    vec3 right = hsv2rgb(rightHSV);
    //vec3 right = yuv_to_rgb(rightYUVColor);

    color[LEFT]  = vec4(left, 1.0f);
    color[RIGHT] = vec4(right, 1.0f);

}
