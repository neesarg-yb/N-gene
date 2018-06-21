#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/MaterialProperty.hpp"

class Renderer;

typedef std::vector< MaterialProperty* >			MaterialPropertyList;
typedef std::tuple< Shader*, MaterialPropertyList >	ShaderAndMatDefaultsTuple;

typedef std::map< int, std::string >				BindPointTexturePairs;

class Material
{
public:
	 Material( Material const &copy );
	 Material( Shader &shader );
	~Material();

private:
	 Material( XMLElement const &materialRoot );
	
public:
	static Material* CreateNewFromFile( std::string pathToXMLFile );

public:
	inline bool IsValid() const { return m_shaderGroup.size() > 0; }

public:
	uint			AddShader	 ( Shader &shader );	// Adds new shader to the m_shaderGroup; returns total number of Shaders present, now
	void			SetBaseShader( Shader &shader );	// Sets the Shader* at m_shaderGroup[0]
	Shader const*	GetShader( uint sIdx = 0 ) const;

	// Properties
	void			SetTexture2D( unsigned int const bind, Texture *resourceTexture );	// Always set sampler which goes with same bind point
	void			SetSampler	( unsigned int const bind, Sampler *sampler );
	Texture const*	GetTexture	( unsigned int const idx );
	Sampler const*	GetSampler	( unsigned int const idx );

	// Property setters
	void		SetProperty	( char const *name, float		   floatValue,		uint sIdx = 0 );
	void		SetProperty	( char const *name, Vector2 const &vector2Value,	uint sIdx = 0 );
	void		SetProperty ( char const *name, Vector3 const &vector3Value,	uint sIdx = 0 );
	void		SetProperty	( char const *name, Vector4 const &vector4Value,	uint sIdx = 0 );
	void		SetProperty	( char const *name, Rgba	const &rgbaValue,		uint sIdx = 0 );

//	// Special case helpers for common properties
// 	void		SetTint		( Rgba const &tint );
// 	void		SetSpecular	( float amount, float power );

public:
	std::string									m_id;
	std::vector< ShaderAndMatDefaultsTuple >	m_shaderGroup;
	std::map< unsigned int, Texture const* >	m_textureBindingPairs;
	std::map< unsigned int, Sampler const* >	m_samplerBindingPairs;

private:
	void				LoadTheShaderGroup( XMLElement const &shaderGroupElement, Renderer &existingRenderer );
	MaterialProperty*	GetExistingPropertyOfByteSizeOrErase( const char *name, size_t byteSize, uint sIdx = 0 );
	void				FetchAllTextureBindingPairsTo	( BindPointTexturePairs		&container, XMLElement const &materialRoot );
	void				SetAllMaterialPropertiesFromXML	( XMLElement const &materialRoot, uint sIdx = 0 );
};