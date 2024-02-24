attribute vec3 w4_a_normal;
attribute vec3 w4_a_tangent;
attribute vec2 w4_a_uv0;

varying vec2 vUV;
varying vec3 vP;
varying vec3 vT;
varying vec3 vB;
varying vec3 vN;
varying vec3 vE;

void w4_main()
{
    vec4 pos = w4_u_model * vec4(w4_a_position, 1.0);
    vP = pos.xyz;
    vE = w4_u_eyePosition;

    vT = normalize(w4_u_normalSpace * w4_a_tangent);
    vN = normalize(w4_u_normalSpace * w4_a_normal);
    vB = normalize(cross(vT, vN));

    vUV = w4_a_uv0;
    gl_Position = w4_u_projectionView * pos;
}
