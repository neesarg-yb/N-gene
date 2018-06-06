#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

class UniformBuffer
{
public:
	 UniformBuffer( size_t byteSize, void const *data );
	~UniformBuffer();

public:
	void		SetCPUData( size_t byteSize, void const *data );	// Copy the data to CPU & dirty the buffer
	void		SetGPUData( size_t byteSize, void const *data );	// Sets the CPU & GPU buffers - clears the dirty flag
	void		UpdateGPU();										// If is dirty, updates the GPU; clears the flag
	
	void*		GetCPUBuffer();				// Sets the dirty tag
	void const* GetCPUBuffer() const;		// returns const* - so does not set dirty tag

	size_t		GetSize() const;			// Get this buffer's size in bytes

public:
	// Template Helpers
      template <typename T> 
      void Set( T const &v )				// Let's me just set a structure, and it'll figure out the size
      { 
         SetCPUData( sizeof(T), &v ); 
      }

      template <typename T>
      T const* As() const					// Gets a constant reference to the CPU buffer as known struct 
      {
         return (T const*) GetCPUBuffer(); 
      }

      template <typename T>
      T* As() 
      {
         return (T*) GetCPUBuffer();
      }

public:
	GLuint	GetHandle() const;

public:
	RenderBuffer	*m_renderBuffer		= nullptr;
	void			*m_CPUBuffer		= nullptr;
	bool			 m_isDirty			= true;

public:
	template <typename T>
      static UniformBuffer* For( T const &v )	// Convenience constructor, will create a constant buffer for a specific struct
      {
		  size_t		 sizeOfT	= sizeof( T );
		  UniformBuffer *newUBO		= new UniformBuffer( sizeOfT, &v );

		  return newUBO;
      }
};