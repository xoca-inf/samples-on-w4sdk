attribute vec3 w4_a_normal;
attribute vec2 w4_a_uv0;

varying vec2 vUV;

void w4_main()
{
    vUV = w4_a_uv0;
    gl_Position = w4_u_projectionView * w4_getVertexPosition();
}
