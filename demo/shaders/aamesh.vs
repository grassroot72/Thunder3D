#version 120

attribute vec3 v_coord;
attribute vec2 v_texcoord;
uniform mat4 vp;
uniform mat4 m;
uniform vec2 tex_offset;

varying vec2 texCoord;


void main()
{
  mat4 mvp = vp*m;
  vec4 vertex_coord = vec4(v_coord.x, v_coord.y + 0.08, v_coord.z, 1.0);

  texCoord = v_texcoord + tex_offset;
  gl_Position = mvp*vertex_coord;
}