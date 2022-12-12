#version 420

in vec4 fColor;		// color;			
in vec4 fNormal;
in vec4 fVertWorldLocation;	// using only matModel
in vec4 fUVx2;
in vec4 fTangent;
in vec4 fBinormal;

#define MAX_LIGHT_SOURCE 10
out vec4 pixelOutputColor;

uniform bool bUseRGBA_Color;
uniform bool bIsFlameObject;

uniform bool bUseDiscardTexture;

uniform vec4 debugColour;

struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	    // rgb = highlight colour, w = power
	vec4 attenuation;	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	    // Spot, directional lights
	vec2 angle;	    	// x = inner angle, y = outer angle
	int type;
	                    // 0 = pointlight
					    // 1 = spot light
					    // 2 = directional light
	int turnON;			// 0 for off, 1 for on	    
};

uniform vec4 eyeLocation;
uniform vec4 RGBA_Color;
uniform bool bDoNotLight;
uniform sLight Light[MAX_LIGHT_SOURCE];

uniform vec4 specularColour;			// RGB object hightlight COLOUR
										// For most material, this is white (1,1,1)
										// For metals google "metal specular hightlight colour"
										// For plastic, it's the same colour as the diffuse
										// W value is the "specular power" or "Shininess" 
										// Starts at 1, and goes to 10,000s
										//	1 = not shiny 
										// 10 = "meh" shiny

uniform sampler2D texture0;		// "Brick texture"
uniform sampler2D texture1;		// "Lady Gaga"
uniform sampler2D texture2;		// "Lady Gaga"
uniform sampler2D texture3;		// "Lady Gaga"
uniform sampler2D texture4;		// "Brick texture"
uniform sampler2D texture5;		// "Lady Gaga"
uniform sampler2D texture6;		// "Lady Gaga"
uniform sampler2D texture7;		// "Lady Gaga"

uniform vec4 texRatio_0_3;		// x = texture0, y = texture1, etc. 0 to 1
uniform vec4 texRatio_4_7;		// 0 to 1

uniform samplerCube skyboxTexture;
// When true, applies the skybox texture
uniform bool bIsSkyboxObject;

// HACK: colour the island
uniform bool bIsIlandModel;

void main()
{
	if (bIsSkyboxObject)
	{
		vec3 cubeMapColour = texture( skyboxTexture, fNormal.xyz ).rgb;
		pixelOutputColor.rgb = cubeMapColour.rgb;
		pixelOutputColor.a = 1.0f;
		return;
	}

	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
    vec3 materialColor = fColor.rgb;
    

	float alphaTransparency = RGBA_Color.w;

	if(bUseRGBA_Color)
	{
		materialColor = RGBA_Color.rgb;
	}
	else
	{	
		vec3 textColour0 = texture( texture0, fUVx2.st ).rgb;		
		vec3 textColour1 = texture( texture1, fUVx2.st ).rgb;	
		vec3 textColour2 = texture( texture2, fUVx2.st ).rgb;	
		vec3 textColour3 = texture( texture3, fUVx2.st ).rgb;	
		
		
		materialColor =   (textColour0.rgb * texRatio_0_3.x) 
						 + (textColour1.rgb * texRatio_0_3.y) 
						 + (textColour2.rgb * texRatio_0_3.z) 
						 + (textColour3.rgb * texRatio_0_3.w);
	}
    

    if ( bDoNotLight )
	{
		// Set the output colour and exit early
		// (Don't apply the lighting to this)
		pixelOutputColor = vec4(materialColor.rgb, alphaTransparency);
		return;
	}

    vec3 normal = normalize(fNormal.xyz);
	for(int i = 0; i < MAX_LIGHT_SOURCE; i++)
	{
		if(Light[i].turnON == 0)
		{
			continue;
		}
		
		if( Light[i].type == 2)
		{
			vec3 lightContrib = Light[i].diffuse.rgb;
			float dotProd = dot(-Light[i].direction.xyz, normal);//normalize(normal.xyz));
			dotProd = max( 0.0f, dotProd);
			lightContrib *= dotProd;
			finalObjectColour.rgb += (materialColor.rgb * Light[i].diffuse.rgb * lightContrib);
			continue;
		}
		
		vec3 vLightToVertex = Light[i].position.xyz - fVertWorldLocation.xyz;
		float distanceToLight = length(vLightToVertex);
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, fNormal.xyz);
		dotProduct = max( 0.0f, dotProduct );
		vec3 lightDiffuseContrib = dotProduct * Light[i].diffuse.rgb;
		
		vec3 lightSpecularContrib = vec3(0.0f);
		//if(Light[i].type == 0)
		//{
			vec3 reflectVector = reflect( -lightVector, normal);//normalize(normal.xyz) );
			
			vec3 eyeVector = normalize(eyeLocation.xyz - fVertWorldLocation.xyz);
			float objectSpecularPower = specularColour.w;
			lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower ) * (specularColour.rgb * Light[i].specular.rgb);
			
			float atten = 1.0f /( Light[i].attenuation.x + Light[i].attenuation.y * distanceToLight +	Light[i].attenuation.z * distanceToLight*distanceToLight );  
			
			lightDiffuseContrib *= atten;
			lightSpecularContrib *= atten;
		//}
		
		if(Light[i].type == 1)
		{
			vec3 vertexToLight = normalize(fVertWorldLocation.xyz - Light[i].position.xyz);
			
			float currentLightRayAngle = dot( vertexToLight.xyz, Light[i].direction.xyz );
			
			currentLightRayAngle = max(0.0f, currentLightRayAngle);
			float outerConeAngleCos = cos(radians(Light[i].angle.y));
			float innerConeAngleCos = cos(radians(Light[i].angle.x));
			
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
	
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									(innerConeAngleCos - outerConeAngleCos);
									
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}		
		}
		
		finalObjectColour.rgb += (materialColor.rgb * lightDiffuseContrib.rgb) + (specularColour.rgb  * lightSpecularContrib.rgb );
	}
	pixelOutputColor = vec4(finalObjectColour.rgb, alphaTransparency);
	vec3 ambient = 0.35 * materialColor;
	pixelOutputColor.rgb += ambient;
}