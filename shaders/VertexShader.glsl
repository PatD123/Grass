#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out float RelativeHeight;

uniform mat4 Transform;
uniform mat4 WindDir;
uniform float BladeHeight;
uniform float Time;

// Simplex 2D noise
//
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

// Remap
float remap(float v, float inMin, float inMax, float outMin, float outMax) {
    return outMin + (v - inMin) * (outMax - outMin) / (inMax - inMin);
}

// Ease-in curve
float easeIn(float x, float k) {
    return pow(x, k);
}

void main()
{
    // Allows us to "hash" per vertex

    RelativeHeight = aPos.y / BladeHeight;

    // The taller it is, the more we lean.

    float windNoiseSample = snoise(vec2(aPos.xz * 0.8 + Time * 0.05));
    float windLeanAngle = remap(windNoiseSample, -1.0, 1.0, 0.25, 0.40);
    windLeanAngle = easeIn(windLeanAngle, 2.0) * 1.25;
    float bend = RelativeHeight * windLeanAngle;

    // Transform position: only offset x/z, not y

    vec3 windOffset = (WindDir * vec4(1.0, 0.0, 1.0, 0.0)).xyz * bend;

    vec4 worldPos = WindDir * vec4(aPos, 1.0) + vec4(windOffset, 1.0);

    gl_Position = Transform * worldPos;
    Normal = aNormal;
}