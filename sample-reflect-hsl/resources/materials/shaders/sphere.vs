attribute vec3 w4_a_normal;

varying vec3 vN;
varying vec3 vE;

void w4_main()
{
    vec4 pos = w4_u_model * vec4(w4_a_position, 1.0);
    vN = w4_u_normalSpace * w4_a_normal;
    vE = w4_u_eyePosition - pos.xyz;
    gl_Position = w4_u_projectionView * pos;
}

