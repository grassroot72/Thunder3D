#version 120

attribute vec2 v_coord;
uniform mat4 mvp;


void main() {
  gl_Position = mvp*vec4(v_coord, 0.0, 1.0);
}