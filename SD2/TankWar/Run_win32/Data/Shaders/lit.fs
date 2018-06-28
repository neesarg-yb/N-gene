#version 420 core
#define MAX_LIGHTS 8

// #define DEBUG_NORMAL
// #define DEBUG_TANGENT
// #define DEBUG_BITANGENT
// #define DEBUG_UV

layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal;
layout(binding = 2) uniform sampler2D gTexEmissive;
layout(binding = 4) uniform sampler2DShadow gTexShadow;

struct LightData 
{
   vec4     color;          // alpha is intensity

   vec3     position; 
   float    padding_01; 

   vec3     direction; 
   float    direction_factor;     // 1: directionLight, 0: pointLight

   vec3     attenuation; 
   float    dot_inner_angle;

   vec3     padding_02;
   float    dot_outer_angle; 

   vec3     padding_03;
   float    uses_shadow;

   mat4     shadow_vp;
};

struct LightFactor
{
  vec3 diffuse;
  vec3 specular;
};

// layout(binding = 3) uniform uboObjectLightData
// {
//    float SPECULAR_AMOUNT;
//    float SPECULAR_POWER;
//    vec2  padding_00;
// };

layout(binding = 4, std140) uniform uboLightsBlock
{
   vec4 AMBIENT; // alpha is intensity
   LightData LIGHTS[MAX_LIGHTS];
};

// material
uniform float SPECULAR_AMOUNT;
uniform float SPECULAR_POWER;

// camera related
uniform vec3 EYE_POSITION;

in vec2 passUV;
in vec4 passColor;
in vec3 passWorldPos;   
in vec3 passWorldNormal;
in vec3 passWorldTangent;
in vec3 passWorldBitangent;

// Outputs
layout(location = 0) out vec4 outColor; 
layout(location = 1) out vec4 outBloom;


#define ADD(a, b) (a) + (b)

LightFactor CalculateLighting( vec3 world_pos, vec3 eye_dir, vec3 normal, float spec_factor, float spec_power );
LightFactor CalculateLightFactor( vec3 position, vec3 eye_dir, vec3 normal, LightData light, float spec_factor, float spec_power );
float       GetAttenuation( float lightIntensity, float distanceFromLight, vec3 attenuationFactors );
float       GetShadowFactor( vec3 position, vec3 normal, LightData light );

// Entry Point
void main( void )
{
   // Get the surface colour
   vec4 tex_color 		    = texture( gTexDiffuse, passUV ) * passColor; 
   vec3 normal_color 	    = texture( gTexNormal, passUV ).xyz;
   vec4 emissive_color    = texture( gTexEmissive, passUV ); 

   // Interpolation is linear, so normals become not normal
   // over a surface, so renormalize it. * passColor.w
   vec3 world_vnormal    	= normalize(passWorldNormal);
   vec3 world_vtan    		= normalize(passWorldTangent);
   vec3 world_vbitan  		= normalize(passWorldBitangent);
   mat3 surface_to_world 	= transpose( mat3( world_vtan, world_vbitan, world_vnormal ) ); 

   // range map it to a surface normal
   vec3 surface_normal = normalize( normal_color * vec3( 2.0f, 2.0f, 1.0f ) + vec3( -1.0f, -1.0f, 0.0f ) ); 
   vec3 world_normal = surface_normal * surface_to_world; // tbn

   vec3 eye_dir = normalize( EYE_POSITION - passWorldPos ); 

   LightFactor lf = CalculateLighting( passWorldPos, 
      eye_dir, 
      world_normal, 
      SPECULAR_AMOUNT, 
      SPECULAR_POWER );

   // Add color of the lit surface to the reflected light
   // to get teh final color; 
   vec4 final_color = vec4(lf.diffuse, 1) * tex_color + vec4(lf.specular, 0);
   final_color.xyz = ADD(final_color.xyz, emissive_color.xyz); 


   // Lit outColor
   final_color 	= clamp( final_color, vec4(0), vec4(1) ); // not necessary - but overflow should go to bloom target (bloom effect)
   outColor 	= final_color;
   outColor		= clamp( outColor, vec4(0), vec4(1) );


   // out color for the Bloom Effect
   vec3 spec_bloom = max( lf.specular - vec3(1), 0.0f );                      // making the curve lower by 1; b/c specular over 1, we'll consider as bloom 
  
   outBloom.xyz = emissive_color.xyz * emissive_color.w + spec_bloom;              // We're also taking emissive_color in account while doing bloom
   outBloom     = clamp( outBloom, vec4(0), vec4(1) );                         // Clamp the lowered curve to positive ( 0 to 1 ) values
   outBloom.a   = final_color.w;

   // Debug defines
   #if defined(DEBUG_NORMAL)
    vec3 debug_color = (world_normal + vec3(1)) * .5f;
    outColor = vec4( debug_color, 1 );
   #elif defined(DEBUG_TANGENT)
    vec3 debug_color = (world_vtan + vec3(1)) * .5f;
    outColor = vec4( debug_color, 1 ); 
   #elif defined(DEBUG_BITANGENT)
    vec3 debug_color = (world_vbitan + vec3(1)) * .5f;
    outColor = vec4( debug_color, 1 ); 
   #elif defined(DEBUG_UV)
    vec3 debug_color = vec3( passUV, 0.f );
    outColor = vec4( debug_color, 1 );
   #endif

//   outColor = vec4( GetShadowFactor( passWorldPos, world_normal, LIGHTS[0] ) );
}


//--------------------------------------------------------------------------------------
LightFactor CalculateLighting( vec3 world_pos, 
   vec3 eye_dir, 
   vec3 normal, 
   float spec_factor, 
   float spec_power ) 
{
   LightFactor lf; 
   lf.diffuse = AMBIENT.xyz * AMBIENT.w; 
   lf.specular = vec3(0.0f); 

   spec_factor *= SPECULAR_AMOUNT; 
   spec_power *= SPECULAR_POWER; 

   for (uint i = 0; i < MAX_LIGHTS; ++i) {
      LightFactor l = CalculateLightFactor( world_pos, eye_dir, normal, LIGHTS[i], spec_factor, spec_power ); 
      lf.diffuse += l.diffuse;
      lf.specular += l.specular; 
   }

   lf.diffuse = clamp( lf.diffuse, vec3(0.0f), vec3(1.0f) ); 
   return lf; 
}


//--------------------------------------------------------------------------------------
LightFactor CalculateLightFactor( vec3 position, 
   vec3 eye_dir, 
   vec3 normal, 
   LightData light, 
   float spec_factor, 
   float spec_power )
{
   LightFactor lf; 

   vec3 light_color = light.color.xyz;

   // get my direction to the light, and distance
   vec3 light_dir = light.position - position; // direction TO the light
   float dist = length(light_dir); 
   light_dir /= dist; 


   float shadowing = GetShadowFactor( position, normal, light ); 

   // 
   vec3 light_forward = normalize(light.direction); 

   // get the power
   float light_power = light.color.w; 

   // figure out how far away angle-wise I am from the forward of the light (useful for spot lights)
   float dot_angle = dot( light_forward, -light_dir ); 

   // falloff for spotlights.
   float angle_attenuation = smoothstep( light.dot_outer_angle, light.dot_inner_angle, dot_angle ); 
   light_power = light_power * angle_attenuation; 

   // get actual direction light is pointing (spotlights point in their "forward", point lights point everywhere (ie, toward the point))
   light_dir = mix(light_dir, -light_forward, light.direction_factor);

   float attenuation = clamp( GetAttenuation( light_power, dist, light.attenuation ), 0, 1 ); 
   float spec_attenuation = clamp( GetAttenuation( light_power, dist, light.attenuation ), 0, 1 );

   // Finally, calculate dot3 lighting
   float dot3 = dot( light_dir, normal ); 
   float diffuse_factor = clamp( attenuation * dot3, 0.0f, 1.0f );

   // specular
   vec3 r = reflect(-light_dir, normal); 
   float spec_amount = max(dot(r, eye_dir), 0.0f); 
   float spec_intensity = (spec_attenuation * spec_factor) * pow(spec_amount, spec_power); 

   lf.diffuse = shadowing * light_color * diffuse_factor;
   lf.specular = shadowing * light_color * spec_intensity; 

   return lf; 
}

//--------------------------------------------------------------------------------------
float GetAttenuation( float lightIntensity, float distanceFromLight, vec3 attenuationFactors )
{
  float a = attenuationFactors.x;
  float b = distanceFromLight * attenuationFactors.y;
  float c = distanceFromLight * distanceFromLight * attenuationFactors.z;

  return lightIntensity / ( a + b + c );
}

//--------------------------------------------------------------------------------------
// return 1 if fully lit, 0 if should be fully in shadow (ignores light)
float GetShadowFactor( vec3 position, vec3 normal, LightData light )
{
   float shadow = light.uses_shadow;
   if (shadow == 0.0f) {
      return 1.0f; 
   }

   // so, we're lit, so we will use the shadow sampler
   float bias_factor = max( dot( light.direction, normal ), 0.0f ); 
   bias_factor = sqrt(1 - (bias_factor * bias_factor)); 
   position -= light.direction * bias_factor * .25f; 

   vec4 clip_pos = light.shadow_vp * vec4(position, 1.0f);
   vec3 ndc_pos = clip_pos.xyz / clip_pos.w; 

   // put from -1 to 1 range to 0 to 1 range
   ndc_pos = (ndc_pos + vec3(1)) * .5f;
   
   // can give this a "little" bias
   // treat every surface as "slightly" closer"
   // returns how many times I'm pass (GL_LESSEQUAL)
   float is_lit = texture( gTexShadow, ndc_pos ).r; 
   // float my_depth = ndc_pos.z; 
   
   // use this to feathre shadows near the border
   float min_uv = min( ndc_pos.x, ndc_pos.y ); 
   float max_uv = max( ndc_pos.x, ndc_pos.y ); 
   float blend = 1.0f - min( smoothstep(0.0f, .05f, min_uv), smoothstep(1.0, .95, max_uv) ); 

   // step returns 0 if nearest_depth is less than my_depth, 1 otherwise.
   // if (nearest_depth) is less (closer) than my depth, that is shadow, so 0 -> shaded, 1 implies light
   // float is_lit = step( my_depth, nearest_depth ); // 

   // scale by shadow amount
   return mix( light.uses_shadow * is_lit, 1.0f, blend );  
}