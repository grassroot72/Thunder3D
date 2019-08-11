#version 120

attribute vec3 v_coord;
attribute vec3 v_normal;
attribute vec3 v_tangent;
attribute vec2 v_texcoord;

uniform mat4 vp;
uniform mat4 m;
uniform mat3 m3x3_inv_transp;

uniform mat4 bias_lvp;

varying vec4 shadowcoord_ws;  /* shadowmap coordinate - world space */

varying vec4 pos_ws;          /* vertex coordinate - world space */
varying vec2 texcoord;     /* texutre coordinate - always in object space */
varying mat3 TBN_ws;          /* mapping from local surface coordinates to world coordinates */

												
void main()
{
  pos_ws = m*vec4(v_coord, 1.0);

  /* the signs and whether tangent is in TBN_ws[1] or TBN_ws[0] */
  /* depends on the tangent attribute, texture coordinates, and */
  /* the encoding of the normal map */
  TBN_ws[0] = normalize(vec3(m*vec4(v_tangent, 0.0)));
  TBN_ws[2] = normalize(m3x3_inv_transp*v_normal);
  TBN_ws[1] = normalize(cross(TBN_ws[2], TBN_ws[0]));

  texcoord = v_texcoord;
  shadowcoord_ws = bias_lvp*pos_ws;
  gl_Position = vp*pos_ws;
}