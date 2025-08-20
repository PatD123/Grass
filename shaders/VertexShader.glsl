#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTransform;
layout(location = 6) in float aBladeDir;
layout(location = 10) in float aBladeScaling;

out vec3 Normal;
out vec3 FragPos;
out float RelativeHeight;

uniform mat4 Proj_View;
uniform float BladeHeight;
uniform float Time;

mat4 translationMatrix(vec3 t);
mat4 rotationY(float angle);
vec3 permute(vec3 x);
float snoise(vec2 v);
float remap(float v, float inMin, float inMax, float outMin, float outMax);
float easeIn(float x, float k);

void main()
{
    // Init
    vec3 pos = aPos;
    pos.y *= aBladeScaling;
    float bladeHeight = BladeHeight * aBladeScaling;
    mat4 transform = translationMatrix(aTransform);
    mat4 bladeDir = rotationY(aBladeDir);

    vec4 pos_1 = transform * vec4(pos, 1.0);

    // Allows us to "hash" per vertex

    RelativeHeight = pos.y / bladeHeight;

    // The taller it is, the more we lean.

    float windNoiseSample = snoise(vec2(pos_1.xz * 0.8 + Time * 0.3));
    float windLeanAngle = remap(windNoiseSample, -1.0, 1.0, 0.10, 0.50);
    windLeanAngle = easeIn(windLeanAngle, 5.0) * 1.25;
    float bend = RelativeHeight * windLeanAngle;

    // Transform position: only offset x/z, not y

    vec3 windOffset = (bladeDir * vec4(1.0, 0.0, 1.0, 0.0)).xyz * bend;

    vec4 worldPos = bladeDir * vec4(pos, 1.0);

    gl_Position = Proj_View * transform * worldPos;
    FragPos = vec3(transform * worldPos);
    Normal = mat3(transpose(inverse(transform * bladeDir))) * aNormal;
    RelativeHeight = bend;
}

mat4 translationMatrix(vec3 t) {
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        t.x, t.y, t.z, 1.0
    );
}

mat4 rotationY(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    return mat4(
         c, 0.0,  s, 0.0,
         0.0, 1.0, 0.0, 0.0,
        -s, 0.0,  c, 0.0,
         0.0, 0.0, 0.0, 1.0
    );
}

vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax) {
    return outMin + (v - inMin) * (outMax - outMin) / (inMax - inMin);
}

float easeIn(float x, float k) {
    return pow(x, k);
}