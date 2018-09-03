<shader  name      ="debug"
         cull      ="back"
         fill      ="solid"
         frontface ="ccw"
         layer     ="0"
         queue     ="opaque">

   <program>
      <vertex     file ="Data\Shaders\debug.vs" />
      <fragment   file ="Data\Shaders\debug.fs" />
   </program>

   <blend>
      <color   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
      <alpha   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
   </blend>

   <depth write ="true" test ="less" />

</shader>