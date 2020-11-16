/* Input */
in vec4 v_Color;
in vec2 v_Texcoord0;
uniform sampler2D u_Texture0;
uniform int u_WindowWidth;
uniform int u_WindowHeight;
uniform int u_Time;
/* Output */
out vec4 v_FragColor;

// Curvature
const float Curvature = 6.0;
// Color bleeding
const float color_bleeding = 1.1;
const float bleeding_range = 1.3;

vec2 crt_coords(vec2 uv, float bend)
{
    uv -= 0.5;
    uv *= 2.;
    uv.x *= 1. + pow(abs(uv.y)/bend, 2.);
    uv.y *= 1. + pow(abs(uv.x)/bend, 2.);
    
    uv /= 2.;
    return uv + .5;
}

float vignette(vec2 uv, float size, float smoothness, float edgeRounding)
{
    uv -= .5;
    uv *= size;
    float amount = sqrt(pow(abs(uv.x), edgeRounding) + pow(abs(uv.y), edgeRounding));
    return smoothstep(0., smoothness, 1.0 - amount);
}

vec4 get_color_bleeding(vec4 current_color,vec4 color_left)
{
    vec4 c = current_color*vec4(color_bleeding,0.5,1.0-color_bleeding,1);
    vec4 l = color_left*vec4(1.0-color_bleeding,0.5,color_bleeding,1);
    return c + l;
}

float random(vec2 uv)
{
    return fract(sin(dot(uv, vec2(15.5151, 42.2561))) * 12341.14122 * sin(float(u_Time) * 0.03));
}

float noise(vec2 uv)
{
    vec2 i = floor(uv);
    vec2 f = fract(uv);
    
    float a = random(i);
    float b = random(i + vec2(1.,0.));
    float c = random(i + vec2(0., 1.));
    float d = random(i + vec2(1.));
    
    vec2 u = smoothstep(0., 1., f);
    
    return mix(a,b, u.x) + (c - a) * u.y * (1. - u.x) + (d - b) * u.x * u.y; 
}

void main()
{
    vec2 xy = crt_coords(v_Texcoord0, Curvature);

    float pixel_size = 1.0/float(u_WindowWidth)*bleeding_range;
    /* Sample the current texture */
    vec4 color = mix(v_Color * vignette(xy, 1.9, 0.6, Curvature*2.0), vec4(noise(xy * 75.)), 0.05);
    /* Color bleeding */
    vec4 current_color = texture2D(u_Texture0, xy) * color;
    vec4 color_left = texture2D(u_Texture0,vec2(xy.x-pixel_size, xy.y)) * color;
    vec4 p = get_color_bleeding(current_color,color_left);
    /* Add scanlines */
    float r = float(u_WindowHeight / 216);
    float f = mod(gl_FragCoord.y, r) / r;
    v_FragColor = mix(p, vec4(p.rgb * 0.5, p.a), f);
}
