#version 430 core

out float FragColor;
in vec2 TexCoords;

uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform sampler2D gDepthMap;

uniform mat4 projection;
uniform mat4 invProjection;
uniform mat4 view;

uniform vec3 samples[64];
uniform int kernelNbr;
uniform vec2 noiseScale;

const float radius = 0.5; // Augmenté pour être visible en View Space
const float bias = 0.025;

// Fonction pour reconstruire la position en View Space à partir du Depth Buffer
vec3 getFragPos(vec2 coords) {
    float depth = texture(gDepthMap, coords).r;
    // Conversion NDC [-1, 1]
    vec4 clipSpacePos = vec4(coords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpacePos = invProjection * clipSpacePos;
    return viewSpacePos.xyz / viewSpacePos.w;
}

void main()
{
    // 1. Récupérer les données du pixel
    vec3 fragPos = getFragPos(TexCoords);
    vec3 normal   = texture(gNormal, TexCoords).rgb;
    // Important : On passe la normale en View Space pour qu'elle soit cohérente avec fragPos
    normal = normalize(mat3(view) * normal);
    
    // 2. Récupérer le vecteur de bruit et créer la base TBN
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;
    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    // 3. Calculer l'occlusion
    float occlusion = 0.0;
    for(int i = 0; i < kernelNbr; ++i)
    {
        // On oriente l'échantillon vers l'hémisphère de la normale
        vec3 samplePos = TBN * samples[i]; 
        samplePos = fragPos + samplePos * radius; 
        
        // On projette la position de l'échantillon pour retrouver ses coordonnées UV
        vec4 offset = vec4(samplePos, 1.0);
        offset      = projection * offset;    
        offset.xyz /= offset.w;               
        offset.xyz  = offset.xyz * 0.5 + 0.5; 
        
        // Lire la profondeur réelle à cette position projetée
        float sampleDepth = getFragPos(offset.xy).z;

        // Vérification de la portée (évite l'occlusion sur des objets trop lointains derrière)
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }

    // On inverse et on moyenne
    occlusion = 1.0 - (occlusion / float(kernelNbr));
    FragColor = occlusion;
}