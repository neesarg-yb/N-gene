#include "Engine/Core/EngineCommon.hpp"

class RenderBuffer
{
   public:
      RenderBuffer();   // initialize data
      ~RenderBuffer();  // cleanup OpenGL resource 

      // copies data to the GPU
      bool CopyToGPU( size_t const byte_count, void const *data ); 

   public:
      GLuint handle;       // OpenGL handle to the GPU buffer, defualt = NULL; 
      size_t buffer_size;  // how many bytes are in this buffer, default = 0
};