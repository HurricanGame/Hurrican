/* Input */
in vec4 v_Color;
in vec2 v_Texcoord0;
uniform sampler2D u_Texture0;
/* Output */
out vec4 v_FragColor;

void main()
{
    /* Sample the current texture */
    vec4 p = texture( u_Texture0, v_Texcoord0 ) * v_Color;
    float l = floor(gl_FragCoord.y);
    if (mod(l, 3.0) < 2.0)
        v_FragColor = p;
    else
        v_FragColor = vec4(p.rgb * 0.5, 1.0);
}
