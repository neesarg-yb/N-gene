<shader  name      ="combine_fs"
         cull      ="back"
         fill      ="solid"
         frontface ="ccw"
         layer     ="0"
         queue     ="opaque">

   <program>
      <vertex     file ="Data\Shaders\combine_fs.vs" />
      <fragment   file ="Data\Shaders\combine_fs.fs" />
   </program>

   <blend>
         <color   op ="add" src ="one" dest ="one" />
         <alpha   op ="add" src ="one" dest ="one" />
   </blend>

   <depth write ="false" test ="always" />

</shader>