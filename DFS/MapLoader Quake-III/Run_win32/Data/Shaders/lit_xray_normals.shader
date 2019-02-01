<shader  name      ="lit_xray_normals"
         cull      ="back"
         fill      ="solid"
         frontface ="ccw"
         layer		 ="0"
         queue		 ="opaque">
   
   <program define="DEBUG_NORMAL">
      	<vertex     file ="Data\Shaders\lit.vs" />
      	<fragment   file ="Data\Shaders\lit.fs" />
   </program>

   <blend>
      	<color   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
      	<alpha   op ="add" src ="src_alpha" dest ="inv_src_alpha" />
   </blend>

   <depth write ="false" test ="greater" />

   <!-- Material Defaults -->
   <material>
   		<texture bind="0" src="Data\Images\Defaults\white.png" />
   		<texture bind="1" src="Data\Images\Defaults\normal.png" />
   		<texture bind="2" src="Data\Images\Defaults\black.png" />
   		
   		<float bind="SPECULAR_AMOUNT" 	 value="1.0" />
   		<float bind="SPECULAR_POWER" 	    value="16.0" />
   </material>

</shader>