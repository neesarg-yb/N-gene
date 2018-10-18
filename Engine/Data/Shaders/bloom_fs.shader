<shader  name      ="bloom_fs"
         cull      ="back"
         fill      ="solid"
         frontface ="ccw"
         layer     ="0"
         queue     ="opaque">

   <program>
      <vertex     file ="Data\Shaders\bloom_fs.vs" />
      <fragment   file ="Data\Shaders\bloom_fs.fs" />
   </program>

   <blend>
      <color   op ="add" src ="one" dest ="zero" />
      <alpha   op ="add" src ="one" dest ="one" />
   </blend>

   <depth write ="false" test ="always" />

</shader>