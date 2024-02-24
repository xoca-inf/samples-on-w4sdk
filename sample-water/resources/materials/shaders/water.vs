attribute vec3 w4_a_normal;
attribute vec2 w4_a_uv0;

varying vec3 vN;
varying vec3 vP;
varying vec2 vUV;
varying vec3 vE;
varying vec3 v_toView;

void w4_main()
{
      vec4 pos = w4_getVertexPosition();
      vN = w4_u_normalSpace * w4_a_normal;
      vP = pos.xyz;
      vE = w4_u_eyePosition;
      vUV = w4_a_uv0;
      gl_Position = w4_u_projectionView * pos;

      v_toView = w4_u_eyePosition - gl_Position.xyz;
}