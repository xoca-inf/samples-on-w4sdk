attribute vec3 w4_a_tangent;
attribute vec3 w4_a_normal;
attribute vec2 w4_a_uv0;

varying vec2 vTexCoord;

varying vec3 v_normal;
varying vec3 v_tangent;
varying vec3 v_toLight;
varying vec3 v_toView;

varying mat3 TBN;


void w4_main()
{
    const vec3 lightPosition = vec3(0., 20., 20.);
    v_normal = w4_u_normalSpace * w4_a_normal;
    v_tangent = w4_u_normalSpace * w4_a_tangent;

    vec3 T = normalize(vec3(w4_u_model * vec4(v_tangent,   0.0)));
    vec3 N = normalize(vec3(w4_u_model * vec4(v_normal,    0.0)));
    vec3 B = normalize(cross(N, T));
    TBN = mat3(T, B, N);

    gl_Position = w4_u_projectionView * w4_u_model * vec4(w4_a_position , 1.0);

    v_toLight = lightPosition - gl_Position.xyz;

    v_toView = w4_u_eyePosition - gl_Position.xyz;
    vTexCoord = vec2(w4_a_uv0.x, 1.0 - w4_a_uv0.y);
}

