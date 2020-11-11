/* Input */
varying vec4 v_Color;
varying vec2 v_Texcoord0;
uniform sampler2D u_Texture0;

void main()
{
    /* Sample the current texture */
    vec4 p = texture2D(u_Texture0, v_Texcoord0) * v_Color;
    float l = floor(gl_FragCoord.y);
    if (mod(l, 3.0) < 2.0)
        gl_FragColor = p;
    else
        gl_FragColor = vec4(p.rgb * 0.5, 1.0);
}
