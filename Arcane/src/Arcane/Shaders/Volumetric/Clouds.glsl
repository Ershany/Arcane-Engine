#shader-type vertex
#version 430 core

layout(location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Raw cube vertex in [-0.5, 0.5] local space - interpolated to fragment for ray exit point
out vec3 LocalPos;

void main()
{
    LocalPos = position;
    gl_Position = projection * view * model * vec4(position, 1.0);
}


#shader-type fragment
#version 430 core

in vec3 LocalPos;

out vec4 FragColour;

uniform sampler3D noiseTexture3D;

// All in local object space (cube occupies [-0.5, 0.5]^3)
uniform vec3 cameraLocalPos;
uniform vec3 sunDirLocal;   // direction FROM scene TOWARD sun
uniform vec3 sunColor;

// Cloud authoring controls
uniform float cloudCoverage;    // [0,1]  fraction of sky covered
uniform float cloudDensity;     // extinction coefficient
uniform float cloudAbsorption;  // [0,1]  absorption vs scattering ratio
uniform vec3  cloudAlbedo;      // cloud tint color

#define PI          3.14159265359
#define MAX_STEPS   64
#define LIGHT_STEPS 8

// Slab-method ray-box intersection for a unit cube in [-0.5, 0.5].
// Returns (tNear, tFar). tFar < tNear means no intersection.
vec2 rayBoxIntersect(vec3 ro, vec3 rd)
{
    vec3 invRd = 1.0 / rd;
    vec3 tMin  = (-0.5 - ro) * invRd;
    vec3 tMax  = ( 0.5 - ro) * invRd;
    vec3 t1    = min(tMin, tMax);
    vec3 t2    = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar  = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar);
}

// Henyey-Greenstein phase function
float henyeyGreenstein(float cosTheta, float g)
{
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * PI * pow(max(0.001, 1.0 + g2 - 2.0 * g * cosTheta), 1.5));
}

// Dual-lobe cloud phase: strong forward scattering (silver lining) + soft back scatter
float cloudPhase(float cosTheta)
{
    return mix(henyeyGreenstein(cosTheta, 0.7), henyeyGreenstein(cosTheta, -0.15), 0.3);
}

// Sample cloud density at local-space position pos in [-0.5, 0.5].
float sampleDensity(vec3 pos)
{
    vec3 uv = clamp(pos + 0.5, 0.0, 1.0);

    vec4 noise  = texture(noiseTexture3D, uv);
    float base   = noise.r;   // low-freq Perlin-Worley base shape
    float detail = noise.g;   // medium Worley erosion
    float fine   = noise.b;   // fine Worley erosion

    // Height gradient: thicker in the middle band, thin at top and bottom
    float hf        = uv.y;
    float heightGrad = smoothstep(0.0, 0.2, hf) * smoothstep(1.0, 0.75, hf);

    // Remap base density against coverage threshold
    float density = max(0.0, base - (1.0 - cloudCoverage));

    // Erode edges with detail noise to break up uniform blobs
    density = max(0.0, density - detail * (1.0 - density) * 0.35);
    density = max(0.0, density - fine  * (1.0 - density) * 0.15);

    return density * heightGrad;
}

// March toward the sun from pos to estimate self-shadowing.
// Returns transmittance [0,1] along that shadow ray.
float lightMarch(vec3 pos)
{
    const float reach    = 0.8;
    float       stepSize = reach / float(LIGHT_STEPS);
    float       totalDensity = 0.0;

    for (int i = 0; i < LIGHT_STEPS; ++i)
    {
        pos += sunDirLocal * stepSize;
        if (any(greaterThan(abs(pos), vec3(0.51)))) break;
        totalDensity += sampleDensity(pos) * stepSize;
    }

    // Beer-Lambert transmittance toward sun
    float beers = exp(-totalDensity * cloudDensity);

    // Powder effect (Nubis): dark cloud cores scatter less light back out
    float powder = 1.0 - exp(-totalDensity * cloudDensity * 2.0);

    return max(0.02, beers * powder * 2.0);
}

void main()
{
    vec3 rayDir = normalize(LocalPos - cameraLocalPos);

    // Ray-box intersection: tStart is ray entry into the cloud box, tEnd is exit
    vec2 tBox    = rayBoxIntersect(cameraLocalPos, rayDir);
    float tStart = max(tBox.x, 0.0);
    float tEnd   = tBox.y;

    if (tEnd <= tStart) discard;

    float stepSize = (tEnd - tStart) / float(MAX_STEPS);

    // Accumulate density-weighted sun transmittance for cloud lighting
    float sunLightAccum    = 0.0;
    float densityWeightSum = 0.0;
    float viewTransmittance = 1.0;

    vec3 currPos = cameraLocalPos + rayDir * (tStart + stepSize * 0.5);

    for (int i = 0; i < MAX_STEPS; ++i)
    {
        float density = sampleDensity(currPos);

        if (density > 0.001)
        {
            float lightT = lightMarch(currPos);
            float weight  = density * stepSize * viewTransmittance;
            sunLightAccum    += lightT * weight;
            densityWeightSum += weight;
            viewTransmittance *= exp(-density * stepSize * cloudDensity * cloudAbsorption);
            if (viewTransmittance < 0.01) break;
        }

        currPos += rayDir * stepSize;
    }

    float alpha = clamp(1.0 - viewTransmittance, 0.0, 1.0);
    if (alpha < 0.002) discard;

    // Average fraction of sunlight reaching visible cloud particles [0=shadowed, 1=fully lit]
    float avgSunT = densityWeightSum > 0.001 ? clamp(sunLightAccum / densityWeightSum, 0.0, 1.0) : 0.0;

    // Phase function adds a brightness boost only in the forward-scattering cone (silver lining).
    // The raw HG output is multiplied by a small constant so the boost only fires near cosTheta=1.
    float cosTheta   = dot(rayDir, sunDirLocal);
    float phaseBoost = clamp(cloudPhase(cosTheta) * 3.0 - 0.1, 0.0, 0.5);
    float litAmount  = clamp(avgSunT + phaseBoost, 0.0, 1.0);

    // cloudAlbedo directly controls cloud color; sun lighting shifts between shadow and lit tone
    vec3 shadowTint = cloudAlbedo * vec3(0.5, 0.6, 0.85) * 0.3;  // sky-blue ambient in deep shadow
    vec3 litTint    = cloudAlbedo * sunColor;
    vec3 cloudColor = mix(shadowTint, litTint, litAmount);

    FragColour = vec4(cloudColor, alpha);
}
