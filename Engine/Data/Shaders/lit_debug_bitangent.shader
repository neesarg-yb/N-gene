<shader  name      ="lit_debug_bitangent"
         cull      ="back"
         fill      ="solid"
         frontface ="ccw"
         layer     ="0"
         queue     ="opaque">
   
   <program define="DEBUG_BITANGENT">
      <vertex     file ="Data\Shaders\lit.vs" />
      <fragment   file ="Data\Shaders\lit.fs" />
   </program>

   <blend>
      <color   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
      <alpha   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
   </blend>

   <depth write ="true" test ="less" />

</shader>