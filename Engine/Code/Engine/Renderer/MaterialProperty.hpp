#pragma once
#include <string>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"

class MaterialProperty
{
public:
	virtual ~MaterialProperty() { }

public:
	virtual void	Bind		( unsigned int programHandle ) = 0;
	virtual void	SetData		( void const *data ) = 0;
	virtual size_t	GetByteSize	() const = 0;
	
	virtual MaterialProperty* Clone() const = 0;

public:
	std::string m_name;
};

class MaterialPropertyFloat : public MaterialProperty
{
public:
	 MaterialPropertyFloat( char const *name, float data );
	~MaterialPropertyFloat() override { }
	
public:
	void	Bind		( unsigned int programHandle ) override;
	void	SetData		( void const *data ) override;
	size_t	GetByteSize	() const override;

	virtual MaterialProperty* Clone() const;

public:
	float m_data;
};

class MaterialPropertyVector2 : public MaterialProperty
{
public:
	 MaterialPropertyVector2( char const *name, Vector2 const &data );
	~MaterialPropertyVector2() override { }
	
public:
	void	Bind		( unsigned int programHandle ) override;
	void	SetData		( void const *data ) override;
	size_t	GetByteSize	() const override;

	virtual MaterialProperty* Clone() const;

public:
	Vector2 m_data;
};

class MaterialPropertyVector3 : public MaterialProperty
{
public:
	 MaterialPropertyVector3( char const *name, Vector3 const &data );
	~MaterialPropertyVector3() override { }

public:
	void	Bind		( unsigned int programHandle ) override;
	void	SetData		( void const *data ) override;
	size_t	GetByteSize	() const override;

	virtual MaterialProperty* Clone() const;

public:
	Vector3 m_data;
};

class MaterialPropertyVector4 : public MaterialProperty
{
public:
	 MaterialPropertyVector4( char const *name, Vector4 const &data );
	~MaterialPropertyVector4() override { }

public:
	void	Bind		( unsigned int programHandle ) override;
	void	SetData		( void const *data ) override;
	size_t	GetByteSize	() const override;

	virtual MaterialProperty* Clone() const;

public:
	Vector4 m_data;
};

class MaterialPropertyRgba : public MaterialProperty
{
public:
	 MaterialPropertyRgba( char const *name, Rgba const &data );
	~MaterialPropertyRgba() override { }

public:
	void	Bind		( unsigned int programHandle ) override;
	void	SetData		( void const *data ) override;
	size_t	GetByteSize	() const override;

	virtual MaterialProperty* Clone() const;

public:
	Rgba m_data;
};