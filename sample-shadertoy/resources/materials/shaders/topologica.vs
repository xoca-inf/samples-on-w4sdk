attribute vec3 w4_a_normal;
attribute vec2 w4_a_uv0;

varying vec3 v_toView;
varying vec3 v_normal;
varying vec3 v_toLight;
varying vec2 v_uv;
varying float v_aspect;

void w4_main()
{
    v_uv = w4_a_uv0;

    const vec3 lightPosition = vec3(0., 10., -10.);
    gl_Position = w4_u_projectionView * w4_u_model * vec4(w4_a_position, 1.0);
    v_normal  = w4_u_normalSpace * w4_a_normal;
    v_toView  = w4_u_eyePosition - gl_Position.xyz;
    v_toLight = lightPosition - gl_Position.xyz;
    v_aspect = w4_u_resolution.x / w4_u_resolution.y;
}
