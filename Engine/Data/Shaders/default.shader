<shader  name      ="default"
         cull      ="back"
         fill      ="solid"
         frontface ="ccw"
         layer     ="0"
         queue     ="opaque">

   <program>
      <vertex     file ="Data\Shaders\default.vs" />
      <fragment   file ="Data\Shaders\default.fs" />
   </program>

   <blend>
      <color   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
      <alpha   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
   </blend>

   <depth write ="true" test ="less" />

</shader>