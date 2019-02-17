<shader  name      ="pickbuffer"
         cull      ="back"
         fill      ="solid"
         frontface ="ccw"
         layer		 ="0"
         queue		 ="opaque">
   
   <program>
      	<vertex     file ="Data\Shaders\pickbuffer.vs" />
      	<fragment   file ="Data\Shaders\pickbuffer.fs" />
   </program>

   <blend>
      	<color   op ="add" src ="one" dest ="zero" />
      	<alpha   op ="add" src ="one" dest ="zero" />
   </blend>

   <depth write ="true" test ="less" />
</shader>