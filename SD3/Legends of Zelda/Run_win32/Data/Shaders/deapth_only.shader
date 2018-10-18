<shader  name      ="deapth_only"
         cull      ="back"
         fill      ="solid"
         frontface ="ccw"
         layer		 ="0"
         queue		 ="opaque">
   
   <program>
      	<vertex     file ="Data\Shaders\deapth_only.vs" />
      	<fragment   file ="Data\Shaders\deapth_only.fs" />
   </program>

   <blend>
      	<color   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
      	<alpha   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
   </blend>

   <depth write ="true" test ="less" />
</shader>