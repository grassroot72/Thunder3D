#version 120

attribute vec4 v_coord;   // attributes - font vertex + texture coordinates (vec2vec2)
uniform mat4 mvp;

varying vec2 texcoord;


void main()
{
  texcoord = v_coord.zw;
  gl_Position = mvp*vec4(v_coord.xy, 0.0, 1.0);
}