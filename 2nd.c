#extension GL_OES_standard_derivatives : enable

precision highp float;

uniform vec2 mouse;
uniform vec2 resolution;
uniform float time;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy / resolution.xy;

    // Centered and aspect corrected coordinates
    vec2 p = uv - 0.5;
    p.x *= resolution.x / resolution.y;

    // Polar coordinates
    float r = length(p);
    float a = atan(p.y, p.x);

    // Time varying pattern with a bit more complexity
    float f = 0.5 + 0.5 * cos(10.0 * (a + sin(time)) - time + r * 2.0);

    // Flashing aspect - periodic flash that gets stronger at the center of the image
    float flash = 0.5 * (1.0 + sin(time)) * (1.0 - r);

    // Loops and fractal-like characteristics
    float loops = sin(5.0 * (r + time));
    f *= loops;

    // Combining color channels
    vec3 color = vec3(f + flash, sin(f + flash + time / 3.0), cos(f + flash + time / 2.0));

    // Convert to HSV and adjust saturation and brightness
    vec3 hsv = rgb2hsv(color);
    hsv.y = clamp(hsv.y * 0.5, 0.0, 1.0);  // reduce saturation
    hsv.z = clamp(hsv.z * 1.4, 0.0, 1.0);  // increase brightness

    // Convert back to RGB
    color = hsv2rgb(hsv);

    // Circle elements, less impactful
    float circles = 0.2 * sin(10.0 * length(uv - 0.5) - time) + 0.8;
    color *= circles;

    gl_FragColor = vec4(color, 2.0);
}
