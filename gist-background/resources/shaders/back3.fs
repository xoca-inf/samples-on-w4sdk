uniform sampler2D texture0;
varying vec2 v_TexCoord;

// The MIT License
// Copyright © 2020 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


// Signed distance and gradient to a cross. Probably faster than
// central differences or automatic differentiation/dual numbers.


// List of other 2D distances+gradients:
//
// Circle:   https://www.shadertoy.com/view/WltSDj
// Box:      https://www.shadertoy.com/view/wlcXD2
// Cross:    https://www.shadertoy.com/view/WtdXWj
// Segment:  https://www.shadertoy.com/view/WtdSDj
// Hexagon:  https://www.shadertoy.com/view/WtySRc


// return distance (.x) and gradient (.yz)
vec3 sdgCross( in vec2 p, in vec2 b )
{
    vec2 s = sign(p);

    p = abs(p);

    vec2  q = ((p.y>p.x)?p.yx:p.xy) - b;
    float h = max( q.x, q.y );
    vec2  o = max( (h<0.0)?vec2(b.y-b.x,0.0)-q:q, 0.0 );
    float l = length(o);

    vec3  r = (h<0.0 && -q.x<l)?vec3(-q.x,1.0,0.0):vec3(l,o/l);

    return vec3( sign(h)*r.x, s*((p.y>p.x)?r.zy:r.yz) );
}

void w4_main()
{

    vec2 p = v_TexCoord-0.5;

    // size
    vec2 si = 0.5 + 0.3*cos( w4_u_time + vec2(0.0,1.57) + 0.0 );
    if( si.x<si.y ) si=si.yx;
    // corner radious
    float ra = 0.1*(0.5+0.5*sin(w4_u_time*1.2));

    // sdf(p) and gradient(sdf(p))
    vec3 dg = sdgCross(p,si);
    float d = dg.x-ra;
    vec2 g = dg.yz;

    // central differenes based gradient, for comparison
    // g = vec2(dFdx(d),dFdy(d))/(2.0/iResolution.y);

    // coloring
    vec3 col = vec3(0.5+0.5*g,0.5+0.5*sign(d));
    col *= 1.0 - 0.5*exp(-16.0*abs(d));
    col *= 0.9 + 0.1*cos(150.0*d);
    col = mix( col, vec3(1.0), 1.0-smoothstep(0.0,0.01,abs(d)) );

    gl_FragColor = vec4( col, 1.0 );
}