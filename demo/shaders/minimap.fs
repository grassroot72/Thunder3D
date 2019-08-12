#version 120

uniform sampler2D diffmap;

varying vec2 texcoord;


void main()
{
  gl_FragColor = vec4(texture2D(diffmap, texcoord).rgb, 0.9);
}