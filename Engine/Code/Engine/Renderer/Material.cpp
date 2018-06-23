#pragma once
#include "Material.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"

using namespace tinyxml2;

Material::Material( Shader &shader )
{
	AddShader( shader );
}

Material::Material( Material const &copy )
{
	m_id					= copy.m_id;

	// Shallow Copy
	m_samplerBindingPairs	= copy.m_samplerBindingPairs;
	m_textureBindingPairs	= copy.m_textureBindingPairs;

	// Deep Copy of Properties
	std::vector< ShaderAndMatDefaultsTuple > shaderGroup;
	// For each tuple member
	for each (ShaderAndMatDefaultsTuple thisShaderMatT in copy.m_shaderGroup)
	{
		Shader*					sharedShader = std::get<0>( thisShaderMatT );
		MaterialPropertyList	deepCopiedMatProperties;
		// For each Material Properties this tuple have
		for each (MaterialProperty* thisMatProperty in std::get<1>( thisShaderMatT ) )
		{
			MaterialProperty* clonedMatProperty = thisMatProperty->Clone();
			deepCopiedMatProperties.push_back( clonedMatProperty );
		}
		
		// Push back the newly made tuple
		ShaderAndMatDefaultsTuple customCopiedTuple = std::make_tuple( sharedShader, deepCopiedMatProperties );
		shaderGroup.push_back( customCopiedTuple );
	}

	m_shaderGroup = shaderGroup;
}

Material::~Material()
{
	// Delete all the MaterialPropertie(s)
	for( uint sIdx = 0; sIdx < m_shaderGroup.size(); sIdx++ )
	{
		MaterialPropertyList& thisPropertyList = std::get<1>( m_shaderGroup[ sIdx ] );
		for( uint pIdx = 0; pIdx < thisPropertyList.size(); pIdx++ )
		{
			delete thisPropertyList[ pIdx ];
		}
	}
}

Material::Material( XMLElement const &materialRoot )
{
	Renderer &existingRenderer = *Renderer::GetInstance();
	TODO("Relying on Renderer for Asset Loading is bad! Make Shader, Texture, etc.. classes have their own pool!");

	//////////////////
	// Root Element //
	//////////////////
	std::string rootName = materialRoot.Name();
	GUARANTEE_RECOVERABLE( rootName == "material", "Warning: Material's root has to be material!" );
	m_id = ParseXmlAttribute( materialRoot, "id", "" );
	GUARANTEE_OR_DIE( m_id != "", "Error, Material: id not found in root tag..!" );


	///////////////////
	// Shader Group  //
	///////////////////
	XMLElement const *shaderGroupElement  = materialRoot.FirstChildElement( "shadergroup" );
	GUARANTEE_OR_DIE( shaderGroupElement != nullptr, "Error, Material: Can't find the shadergroup tag in material XML.." );
	LoadTheShaderGroup( *shaderGroupElement, existingRenderer );								// Set ups all the default Textures and MaterialProperties


	//////////////////////
	// Get all Textures //
	//////////////////////
	BindPointTexturePairs bindWithSourceTexturePairs;
	FetchAllTextureBindingPairsTo( bindWithSourceTexturePairs, materialRoot );					// Now load custom textures


	//-------------------------------------------------------------------//

	//////////////////////
	// Set all Textures //
	//////////////////////
	for( std::map< int, std::string >::iterator thisTextureData  = bindWithSourceTexturePairs.begin(); 
												thisTextureData != bindWithSourceTexturePairs.end();
												thisTextureData++ )
	{
		unsigned int bindPoint				= (unsigned int) thisTextureData->first;
		m_textureBindingPairs[ bindPoint ]	= existingRenderer.CreateOrGetTexture( thisTextureData->second );
		m_samplerBindingPairs[ bindPoint ]	= existingRenderer.GetDefaultSampler();				// For now I'm binding default sampler, all the time
	}
	TODO("Material: Bind custom Samplers while loading from XML..");
	

	////////////////////////
	// Set all Properties //
	////////////////////////
	for( uint shaderIdx = 0; shaderIdx < m_shaderGroup.size(); shaderIdx++ )
		SetAllMaterialPropertiesFromXML( materialRoot, shaderIdx );								// Now load custom properties
}

Material* Material::CreateNewFromFile( std::string pathToXMLFile )
{
	// Loading: Material XML
	XMLDocument materialDoc;
	materialDoc.LoadFile( pathToXMLFile.c_str() );
	const XMLElement* materialDefRoot = materialDoc.RootElement();
	GUARANTEE_OR_DIE( materialDefRoot != nullptr, "Error: Material() couldn't find the rootElement of XML file..!" );
	return new Material( *materialDefRoot );
}

uint Material::AddShader( Shader &shader )
{
	ShaderAndMatDefaultsTuple newShaderTuple = std::make_tuple( &shader, MaterialPropertyList() );
	m_shaderGroup.push_back( newShaderTuple );

	return (uint)m_shaderGroup.size();
}

void Material::SetBaseShader( Shader &shader )
{
	if( m_shaderGroup.size() > 0 )
	{
		// Empty the property list before deletion
		MaterialPropertyList& propertyList = std::get<1>( m_shaderGroup[0] );
		for( uint pListIdx = 0; pListIdx < propertyList.size(); pListIdx++ )
			delete propertyList[ pListIdx ];

		m_shaderGroup.erase( m_shaderGroup.begin() );
	}

	ShaderAndMatDefaultsTuple newShaderTuple = std::make_tuple( &shader, MaterialPropertyList() );
	m_shaderGroup.insert( m_shaderGroup.begin(), newShaderTuple );
}

Shader const* Material::GetShader( uint sIdx /* = 0 */ ) const
{
	if( sIdx < m_shaderGroup.size() )
		return std::get<0>( m_shaderGroup[sIdx] );
	else
		return nullptr;
}

void Material::SetTexture2D( unsigned int const bind, Texture *resourceTexture )
{
	m_textureBindingPairs[ bind ] = resourceTexture;
}

void Material::SetSampler( unsigned int const bind, Sampler *sampler )
{
	m_samplerBindingPairs[ bind ] = sampler;
}

Texture const* Material::GetTexture( unsigned int const idx )
{
	return m_textureBindingPairs[ idx ];
}

Sampler const* Material::GetSampler( unsigned int const idx )
{
	return m_samplerBindingPairs[ idx ];
}

void Material::SetProperty( char const *name, float floatValue, uint sIdx /* = 0 */ )
{
	MaterialProperty* existingProperty = GetExistingPropertyOfByteSizeOrErase( name, sizeof(floatValue), sIdx );

	// If property already exists
	if( existingProperty != nullptr )
		existingProperty->SetData( &floatValue );
	else
	{
		existingProperty = new MaterialPropertyFloat( name, floatValue );

		MaterialPropertyList& propertyList = std::get<1>( m_shaderGroup[ sIdx ] );
		propertyList.push_back( existingProperty );
	}
}

void Material::SetProperty( char const *name, Vector2 const &vector2Value, uint sIdx /* = 0 */ )
{
	MaterialProperty* existingProperty = GetExistingPropertyOfByteSizeOrErase( name, sizeof(vector2Value), sIdx );

	// If property already exists
	if( existingProperty != nullptr )
		existingProperty->SetData( &vector2Value );
	else
	{
		existingProperty = new MaterialPropertyVector2( name, vector2Value );

		MaterialPropertyList& propertyList = std::get<1>( m_shaderGroup[ sIdx ] );
		propertyList.push_back( existingProperty );
	}
}

void Material::SetProperty( char const *name, Vector3 const &vector3Value, uint sIdx /* = 0 */ )
{
	MaterialProperty* existingProperty = GetExistingPropertyOfByteSizeOrErase( name, sizeof(vector3Value), sIdx );

	// If property already exists
	if( existingProperty != nullptr )
		existingProperty->SetData( &vector3Value );
	else
	{
		existingProperty = new MaterialPropertyVector3( name, vector3Value );

		MaterialPropertyList& propertyList = std::get<1>( m_shaderGroup[ sIdx ] );
		propertyList.push_back( existingProperty );
	}
}

void Material::SetProperty( char const *name, Vector4 const &vector4Value, uint sIdx /* = 0 */ )
{
	MaterialProperty* existingProperty = GetExistingPropertyOfByteSizeOrErase( name, sizeof(vector4Value), sIdx );

	// If property already exists
	if( existingProperty != nullptr )
		existingProperty->SetData( &vector4Value );
	else
	{
		existingProperty = new MaterialPropertyVector4( name, vector4Value );

		MaterialPropertyList& propertyList = std::get<1>( m_shaderGroup[ sIdx ] );
		propertyList.push_back( existingProperty );
	}
}

void Material::SetProperty( char const *name, Rgba const &rgbaValue, uint sIdx /* = 0 */ )
{
	MaterialProperty* existingProperty = GetExistingPropertyOfByteSizeOrErase( name, sizeof(rgbaValue), sIdx );

	// If property already exists
	if( existingProperty != nullptr )
		existingProperty->SetData( &rgbaValue );
	else
	{
		existingProperty = new MaterialPropertyRgba( name, rgbaValue );

		MaterialPropertyList& propertyList = std::get<1>( m_shaderGroup[ sIdx ] );
		propertyList.push_back( existingProperty );
	}
}

void Material::LoadTheShaderGroup( XMLElement const &shaderGroupElement, Renderer &existingRenderer )
{
	BindPointTexturePairs defaultTexturePairs;

	// For every shader tags in this shadergroup
	for( XMLElement const	*shaderElement  = shaderGroupElement.FirstChildElement( "shader" );
							 shaderElement != nullptr;
							 shaderElement  = shaderElement->NextSiblingElement( "shader" ) )
	{

		//////////////////////////
		// Get Shader File Name //
		//////////////////////////
		std::string shaderFileName = ParseXmlAttribute( *shaderElement, "fileName", "" );
		GUARANTEE_OR_DIE( shaderFileName != "", "Error, Material: shader fileName can't be empty string..!" );


		//////////////////////////////
		// Get Defaults from Shader //
		//////////////////////////////
		XMLDocument shaderDoc;
		std::string pathToXMLFile = "Data//Shaders//" + shaderFileName + ".shader";
		shaderDoc.LoadFile( pathToXMLFile.c_str() );

		const XMLElement* shaderRoot = shaderDoc.RootElement();
		GUARANTEE_OR_DIE( shaderRoot != nullptr, "Error, Material: couldn't find the rootElement of shader XML file..!" );

		// Get default material XMLElement
		XMLElement const *defaultMaterialDataElement = shaderRoot->FirstChildElement( "material" );

		// Fetch all the default textures
		if( defaultMaterialDataElement != nullptr )
		{
			FetchAllTextureBindingPairsTo( defaultTexturePairs, *defaultMaterialDataElement );
		}

		/////////////////////////////////////////
		// Set the Shader & Default Properties //
		/////////////////////////////////////////
		Shader* thisShader = existingRenderer.CreateOrGetShader( shaderFileName.c_str() );
		ShaderAndMatDefaultsTuple newShaderTuple = std::make_tuple( thisShader, MaterialPropertyList() );
		m_shaderGroup.push_back( newShaderTuple );

		uint currentShaderIdx = (uint) m_shaderGroup.size() - 1U;
		// Set all the default properties
		if( defaultMaterialDataElement != nullptr )
		{
			SetAllMaterialPropertiesFromXML( *defaultMaterialDataElement, currentShaderIdx );		
		}
	}

	// Set default textures
	for( std::map< int, std::string >::iterator thisTextureData  = defaultTexturePairs.begin(); 
												thisTextureData != defaultTexturePairs.end();
												thisTextureData++ )
	{
		unsigned int bindPoint				= (unsigned int) thisTextureData->first;
		m_textureBindingPairs[ bindPoint ]	= existingRenderer.CreateOrGetTexture( thisTextureData->second );
		m_samplerBindingPairs[ bindPoint ]	= existingRenderer.GetDefaultSampler();					// For now I'm binding default sampler, all the time
	}
}

MaterialProperty* Material::GetExistingPropertyOfByteSizeOrErase( const char *name, size_t byteSize, uint sIdx /* = 0 */ )
{
	MaterialPropertyList &properties = std::get<1>( m_shaderGroup[ sIdx ] );

	// Loop through all the m_propertie(s)
	for( unsigned int idx = 0; idx < properties.size(); idx++ )
	{
		MaterialProperty& thisProperty = *properties[ idx ];

		// If property name found
		if ( thisProperty.m_name == name )
		{
			// Check if byteSize matches
			if ( thisProperty.GetByteSize() == byteSize )
				return &thisProperty;
			else
			{
				// If not, erase the existing property of different byteSize
				properties.erase( properties.begin() + idx );
				return nullptr;
			}
		}
	}
	
	return nullptr;
}

void Material::FetchAllTextureBindingPairsTo( BindPointTexturePairs &container, XMLElement const &materialRoot )
{
	for( XMLElement const	*textureElement  = materialRoot.FirstChildElement( "texture" ); 
							 textureElement != nullptr; 
							 textureElement  = textureElement->NextSiblingElement( "texture" ) )
	{
		int bind		= ParseXmlAttribute( *textureElement, "bind", 0 );
		std::string src	= ParseXmlAttribute( *textureElement, "src", "" );
		GUARANTEE_OR_DIE( src != "", "Error, Material: src of a texture can't be an empty string..!" );

		container[ bind ] = src;
	}
}

void Material::SetAllMaterialPropertiesFromXML( XMLElement const &materialRoot, uint sIdx /* = 0 */ )
{
	// Float Properties
	for( XMLElement const	*floatElement  = materialRoot.FirstChildElement( "float" );
							 floatElement != nullptr;
							 floatElement  = floatElement->NextSiblingElement( "float" ) )
	{
		std::string bindName	= ParseXmlAttribute( *floatElement, "bind", "" );
		float		value		= ParseXmlAttribute( *floatElement, "value", 0.f );
		GUARANTEE_OR_DIE( bindName != "", "Error, Material: bindName of a float property can't be empty string.." );

		SetProperty( bindName.c_str(), value, sIdx );
	}

	// Vector2 Properties
	for( XMLElement const	*vec2Element  = materialRoot.FirstChildElement( "vec2" );
							 vec2Element != nullptr;
							 vec2Element  = vec2Element->NextSiblingElement( "vec2" ) )
	{
		std::string bindName	= ParseXmlAttribute( *vec2Element, "bind", "" );
		Vector2		value		= ParseXmlAttribute( *vec2Element, "value", Vector2::ZERO );
		GUARANTEE_OR_DIE( bindName != "", "Error, Material: bindName of a vec2 property can't be empty string.." );
		
		SetProperty( bindName.c_str(), value, sIdx );
	}

	// Vector3 Properties
	for( XMLElement const	*vec3Element  = materialRoot.FirstChildElement( "vec3" );
							 vec3Element != nullptr;
							 vec3Element  = vec3Element->NextSiblingElement( "vec3" ) )
	{
		std::string bindName	= ParseXmlAttribute( *vec3Element, "bind", "" );
		Vector3		value		= ParseXmlAttribute( *vec3Element, "value", Vector3::ZERO );
		GUARANTEE_OR_DIE( bindName != "", "Error, Material: bindName of a vec3 property can't be empty string.." );
		
		SetProperty( bindName.c_str(), value, sIdx );
	}

	// Vector4 Properties
	for( XMLElement const	*vec4Element  = materialRoot.FirstChildElement( "vec4" );
							 vec4Element != nullptr;
							 vec4Element  = vec4Element->NextSiblingElement( "vec4" ) )
	{
		std::string bindName	= ParseXmlAttribute( *vec4Element, "bind", "" );
		Vector4		value		= ParseXmlAttribute( *vec4Element, "value", Vector4::ZERO );
		GUARANTEE_OR_DIE( bindName != "", "Error, Material: bindName of a vec4 property can't be empty string.." );
		
		SetProperty( bindName.c_str(), value, sIdx );
	}

	// Rgba Properties
	for( XMLElement const	*rgbaElement  = materialRoot.FirstChildElement( "rgba" );
							 rgbaElement != nullptr;
							 rgbaElement  = rgbaElement->NextSiblingElement( "rgba" ) )
	{
		std::string bindName	= ParseXmlAttribute( *rgbaElement, "bind", "" );
		Rgba		value		= ParseXmlAttribute( *rgbaElement, "value", RGBA_WHITE_COLOR );
		GUARANTEE_OR_DIE( bindName != "", "Error, Material: bindName of a rgba property can't be empty string.." );
		
		SetProperty( bindName.c_str(), value, sIdx );
	}
}