#version 410 core

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexNormal;
uniform sampler2D uTexSpecular;
uniform sampler2D uTexAmbientOcclusion;
uniform sampler2D uTexEmissive;
uniform sampler2D uTexDissolve;
uniform sampler2D uTexCaustic;

uniform vec4 uAmbientLight;
uniform vec3 uCameraPosition;

uniform int uLightCount;
uniform vec3 uLightPosition[16];
uniform vec3 uLightDirection[16];
uniform vec4 uLightColor[16];
uniform int uIsDirectionalLight[16]; //bool
uniform float uMinLightDistance[16];
uniform float uMaxLightDistance[16];
uniform float uPowerAtMinDistance[16];
uniform float uPowerAtMaxDistance[16];
uniform float uInnerCosAngle[16];
uniform float uOuterCosAngle[16];
uniform float uPowerInsideInnerAngle[16];
uniform float uPowerOutsideOuterAngle[16];

uniform float uSpecularIntensity;
uniform float uSpecularPower;

uniform vec4 uFogColor;
uniform float uMinFogDistance;
uniform float uMaxFogDistance;

uniform vec4 uEmissiveColor;

uniform vec4 uDissolveColor;
uniform float uDissolveFactor;

uniform float uPulseTime;
uniform vec4 uPulseColor;

in vec3 passPosition;
in vec3 passTangent;
in vec3 passBitangent;
in vec2 passUV0;

out vec4 outColor;


//-----------------------------------------------------------------------------------------------
// Because I like Forseth's example
struct SurfaceLight
{
	vec3 surface;
	vec3 specular;
};



//-----------------------------------------------------------------------------------------------
vec4 VectorAsColor(vec3 vec)
{
	return vec4((vec + vec3(1.0f)) * vec3(.5f), 1.0f);
}


//-----------------------------------------------------------------------------------------------
vec4 GetAmbientLight()
{
	vec3 ambientColor = uAmbientLight.rgb;
	float ambientIntensity = uAmbientLight.a;
	vec4 ambientLight = vec4(ambientColor * ambientIntensity, 1.f);
	return ambientLight;
};


//-----------------------------------------------------------------------------------------------
vec4 GetEmissiveLight()
{
	float emissiveValue = texture(uTexEmissive, passUV0).r;
	vec4 finalEmissiveColor = vec4(uEmissiveColor.xyz * emissiveValue, 1.f);
	return finalEmissiveColor;
};


//-----------------------------------------------------------------------------------------------
float CalculateLightDistance(vec3 vecToLight, int isDirectionalLight, vec3 lightDirection)
{
	//branchless
	float regDistance = length(vecToLight);
	float directionalDistance = dot(vecToLight, -lightDirection);
	float finalDistance = mix(regDistance, directionalDistance, isDirectionalLight)	;
	return finalDistance;
}


//-----------------------------------------------------------------------------------------------
vec3 CalculateDirToLight(vec3 vecToLight, int isDirectionalLight, vec3 lightDirection)
{
	//branchless
	vec3 regDir = normalize(vecToLight);
	vec3 directionalDir = -lightDirection;
	vec3 finalDir = mix(regDir, directionalDir, isDirectionalLight);
	return finalDir;
}


//-----------------------------------------------------------------------------------------------
SurfaceLight CalculateSurfaceLight( vec3 dirToEye, float specIntensity,
	vec3 surfacePosition, vec3 surfaceNormal,
	vec3 lightPosition, vec3 lightDirection,
	float minLightDist, float maxLightDist,
	float powerAtMinDist, float powerAtMaxDist,
	float innerCosAngle, float outerCosAngle,
	float powerInsideAngle, float powerOutsideAngle,
	int isDirectionalLight, vec3 lightColor)
{
	// Get distance and direction
	vec3 vecToLight = lightPosition - surfacePosition;
	float distToLight = CalculateLightDistance(vecToLight, isDirectionalLight, lightDirection);
	vec3 dirToLight = CalculateDirToLight(vecToLight, isDirectionalLight, lightDirection);
	
	// Calculate attenuation
	float distAttenuation = smoothstep(minLightDist, maxLightDist, distToLight);
	distAttenuation = mix(powerAtMinDist, powerAtMaxDist, distAttenuation);

	float angle = dot(lightDirection, -dirToLight);
	float angleAttenuation = smoothstep(innerCosAngle, outerCosAngle, angle);
	angleAttenuation = mix(powerInsideAngle, powerOutsideAngle, angleAttenuation);
	float attenuation = distAttenuation * angleAttenuation;	

	// Dot3
	float dot3Factor = max(dot(surfaceNormal, dirToLight), 0.f) * attenuation;
	vec3 dot3Color = lightColor * dot3Factor;

	//Specular
	vec3 halfVector = normalize(dirToLight + dirToEye);
	float specFactor = max(dot(surfaceNormal, halfVector), 0.f);
	specFactor = pow(specFactor, uSpecularPower) * specIntensity * attenuation;
	vec3 specColor = lightColor * specFactor;

	SurfaceLight result;
	result.surface = dot3Color;
	result.specular = specColor;
	return result;
}


float CalculateFogRatio()
{
	vec3 vecToEye = uCameraPosition - passPosition;
	float len = length(vecToEye);	

	float ratio = (len - uMinFogDistance) / (uMaxFogDistance - uMinFogDistance);
	ratio = clamp(ratio, 0.f, 1.f);
	return ratio;	
}


float CalculateDissolveFactor()
{
	float dissolveThreshold = texture(uTexDissolve, passUV0).r;
	if(uDissolveFactor > dissolveThreshold)
	{
		discard;
	}

	float dissolveValue = uDissolveFactor;
	if(uDissolveFactor + 0.05f > dissolveThreshold)
	{
		dissolveValue += 0.4f;
	}
	else if (uDissolveFactor + 0.1f < dissolveThreshold)
	{
		dissolveValue = 0.f;
	}

	return dissolveValue;
}


void main()
{
	// Calculate early in order to discard early
	float dissolveFactor = clamp(CalculateDissolveFactor(), 0.f, 1.f);
	
	vec3 surfaceTangent = normalize(passTangent);
	vec3 surfaceBitangent = normalize(passBitangent);
	vec3 surfaceNormal = cross(surfaceBitangent, surfaceTangent);
	
	surfaceBitangent = cross(surfaceTangent, surfaceNormal);
	mat3 tbn = mat3(surfaceTangent, surfaceBitangent, surfaceNormal);
	//tbn = transpose(tbn); // if col major!

	vec4 diffuse = texture(uTexDiffuse, passUV0);

	vec3 normal = texture(uTexNormal, passUV0).xyz;
	normal = normal * vec3(2.f, 2.f, 1.f) - vec3(1.f, 1.f, 0.f);
	normal = normalize(normal);
	normal = tbn * normal;

	vec4 ambientOcclusion = vec4(texture(uTexAmbientOcclusion, passUV0).xyz, 1.f);
	vec3 dirToEye = normalize(uCameraPosition - passPosition);	

	float specIntensity = texture(uTexSpecular, passUV0).r;
	specIntensity *= uSpecularIntensity;


	// Main lighting loop
	vec3 surfaceLight = vec3(0.f);
	vec3 specLight = vec3(0.f);
	for (int lightIndex = 0; lightIndex < uLightCount; ++lightIndex)
	{
		SurfaceLight surfaceLightValue = CalculateSurfaceLight
		(
			dirToEye, specIntensity,
			passPosition, normal,
			uLightPosition[lightIndex], uLightDirection[lightIndex],
			uMinLightDistance[lightIndex], uMaxLightDistance[lightIndex],
			uPowerAtMinDistance[lightIndex], uPowerAtMaxDistance[lightIndex],
			uInnerCosAngle[lightIndex], uOuterCosAngle[lightIndex],
			uPowerInsideInnerAngle[lightIndex], uPowerOutsideOuterAngle[lightIndex],
			uIsDirectionalLight[lightIndex], uLightColor[lightIndex].rgb
		);

		surfaceLight += surfaceLightValue.surface;
		specLight += surfaceLightValue.specular;
	}

	vec4 totalSurfaceLight = vec4(surfaceLight, 1.f);
	vec4 totalSpecLight = vec4(specLight, 1.f);
	
	vec4 ambientLight = GetAmbientLight();
	vec4 emissiveLight = GetEmissiveLight();

	vec4 preFogColor = diffuse * (ambientLight + totalSurfaceLight + emissiveLight) * ambientOcclusion + totalSpecLight;
	preFogColor = clamp(preFogColor, vec4(0.f), vec4(1.f));

	float pulseFactor = texture(uTexCaustic, passUV0).r * uPulseTime;
	vec4 pulseColor = uPulseColor * clamp(pulseFactor, 0.f, 1.f);
	preFogColor = clamp(preFogColor + pulseColor, vec4(0.f), vec4(1.f));

	preFogColor = mix(preFogColor, uDissolveColor, dissolveFactor);

	float fogRatio = CalculateFogRatio();
	outColor = mix(preFogColor, uFogColor, fogRatio);
}