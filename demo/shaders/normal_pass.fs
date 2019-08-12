#version 120

uniform mat4 v_inv;
uniform sampler2D diffmap;
uniform sampler2D normmap;
uniform sampler2D specmap;
uniform sampler2DShadow shadmap;
uniform vec4 light_pos;         /* light coordination - world space */

varying vec4 pos_ws;
varying vec2 texcoord;
varying mat3 TBN_ws;

varying vec4 shadowcoord_ws;

 
void main()
{
  vec3 view_dir = normalize(vec3(v_inv*vec4(0.0, 0.0, 0.0, 1.0) - pos_ws));
  vec4 normal_os = texture2D(normmap, texcoord);
  vec3 coord_os = 2.0*normal_os.rgb - vec3(1.0);
  vec3 normal_ws = normalize(TBN_ws*coord_os);

  /* no attenuation */
  float attenuation;
  vec3 l_dir;
  attenuation = 1.0;

  /* directional ligth, since in c program, we use ortho projection for light */
  vec3 pos_to_light_ws = vec3(light_pos - pos_ws);
  l_dir = normalize(pos_to_light_ws);

  /* scene ambient */
  vec4 scene_ambient = vec4(0.2, 0.2, 0.2, 1.0);

  /* initialize light */
  vec4 light_diffuse = vec4(1.0, 1.0, 1.0, 1.0);
  vec4 light_specular = vec4(1.0, 1.0, 1.0, 1.0);

  /* initialize material */
  vec4 material_ambient = vec4(0.2, 0.2, 0.2, 1.0);
  vec4 material_diffuse;
  vec4 material_specular;
  float material_shininess = 8.0;

  /* calculate the ambiant */
  vec3 ambient = vec3(scene_ambient)*vec3(material_ambient);

  /* get the material diffuse color from diffuse texture */
  material_diffuse = texture2D(diffmap, texcoord);
  vec3 diffuse = attenuation*
                 vec3(light_diffuse)*
                 vec3(material_diffuse)*
                 max(0.0, dot(normal_ws, l_dir));
 
  /* get the material specular color from specular texture */
  material_specular = texture2D(specmap, texcoord);
  vec3 specular;
  /* light source on the wrong side? */
  if(dot(normal_ws, l_dir) < 0.0) {  
    /* no specular reflection */
    specular = vec3(0.0, 0.0, 0.0);
  }
  else {
    /* light source on the right side */
    specular = attenuation*
               vec3(light_specular)*
               vec3(material_specular)*
               pow(max(0.0, dot(reflect(-l_dir, normal_ws), view_dir)), material_shininess);
  }


	/* sample the shadow map 4 times */
  /* visibility */
  float visi = 1.0;
  float depth;
  int i;
	for(i = 0; i < 4; i++) {
    depth = shadow2D(shadmap, vec3(shadowcoord_ws.xy /*+ poissonDisk[index]/700.0*/,
                                  (shadowcoord_ws.z/* - bias*/)/shadowcoord_ws.w)).r;
    visi -= 0.15*(1.0 - depth);
  }

  gl_FragColor = vec4(ambient + visi*diffuse + visi*specular, 1.0);
}