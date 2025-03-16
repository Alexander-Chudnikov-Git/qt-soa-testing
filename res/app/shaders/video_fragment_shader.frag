#version 330 core

uniform bool enableRoundedEdges;
uniform bool enableAntialiasing;

uniform sampler2D tex;
uniform float radius;
uniform vec2 resolution;

in vec4 texc;
out vec4 fragColor;

float roundedBoxSDF(vec2 CenterPosition, vec2 Size, float Radius)
{
    return length(max(abs(CenterPosition) - Size + Radius, 0.0)) - Radius;
}

void main(void)
{
    vec4 originalColor = texture2D(tex, texc.st);

    if (!enableRoundedEdges)
    {
        fragColor = originalColor;
        return;
    }

    vec2 uv = texc.st;
    vec2 normalizedUV = uv * resolution / min(resolution.x, resolution.y);
    vec2 center = resolution / (2.0 * min(resolution.x, resolution.y));
    vec2 size = vec2(resolution.x, resolution.y) / (2.0 * min(resolution.x, resolution.y));

    float distance = roundedBoxSDF(normalizedUV - center, size, radius);

    float alpha = 1.0;
    if (enableAntialiasing)
    {
        float pixelSize = 1.0 / min(resolution.x, resolution.y);
        float antialiasWidth = pixelSize;
        alpha = smoothstep(antialiasWidth, -antialiasWidth, distance);
    }
    else
    {
        alpha = distance < 0.0 ? 1.0 : 0.0;
    }

    fragColor = vec4(originalColor.rgb, originalColor.a * alpha);
}
