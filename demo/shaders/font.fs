#version 120

uniform sampler2D diffmap;
uniform vec4 onecolor;

varying vec2 texcoord;


void main()
{
  gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(diffmap, texcoord).r)*onecolor;
}