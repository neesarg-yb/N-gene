<shader  name      	="skybox"
         cull      	="front"
         fill      	="solid"
         frontface 	="ccw"
         layer		   ="0"
         queue		   ="opaque">
   <!--  queue		="opaque/alpha" -->
   
   <!--
   <program define="DEBUG_NORMAL">
   -->
   <program>
      	<vertex     file ="Data\Shaders\skybox.vs" />
      	<fragment   file ="Data\Shaders\skybox.fs" />
   </program>

   <blend>
      	<color   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
      	<alpha   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
   </blend>

   <depth write ="false" test ="always" />

</shader>