attribute vec3 w4_a_normal;
attribute vec2 w4_a_uv0;

varying vec2 vTexCoord;

void w4_main()
{
    vTexCoord = vec2(w4_a_uv0.x, 1.0 - w4_a_uv0.y);
    gl_Position = w4_u_projectionView * w4_getVertexPosition();
}

