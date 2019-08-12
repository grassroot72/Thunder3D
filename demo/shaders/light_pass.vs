#version 120

attribute vec3 v_coord;

uniform mat4 lvp;
uniform mat4 m;


void main() {
	gl_Position = lvp*m*vec4(v_coord, 1.0);
}
