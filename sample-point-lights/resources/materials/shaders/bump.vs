attribute vec3 w4_a_normal;
attribute vec3 w4_a_tangent;
attribute vec2 w4_a_uv0;

varying vec3 v_position;
varying vec3 v_eyePosition;
varying vec3 v_normal;
varying vec3 v_tangent;

varying mat3 v_TBN;
varying vec2 v_TexCoord;

void w4_main()
{
    vec4 pos = w4_getVertexPosition();

    v_position = pos.xyz;
    v_normal  = w4_u_normalSpace * w4_a_normal;
    v_tangent = w4_u_normalSpace * w4_a_tangent;
    v_eyePosition  = w4_u_eyePosition;

    vec3 T = normalize(v_tangent);
    vec3 N = normalize(v_normal);
    vec3 B = normalize(cross(T, N));

    v_TBN = mat3(T, B, N);

    v_TexCoord = w4_a_uv0;
    gl_Position = w4_u_projectionView * pos;
}
