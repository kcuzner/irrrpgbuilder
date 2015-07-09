
 //  VERTEX PROGRAM  Jacques. Pretorius 2015 

 uniform mat4 mWorld;    

 varying vec3 Tangent;
 varying vec3 Binormal;
 varying vec2 UVCoordsXY;
 varying vec3 VNormal;
 varying vec3 VertexGLPosition;
 varying vec3 NormalBB;
 varying vec3 TangentBB;
 varying vec3 BinormalBB;

 void main()
  {

   UVCoordsXY        = gl_MultiTexCoord0.xy;          // Just what it says! 
   Tangent           = vec3(normalize(gl_MultiTexCoord1));  // Acuired here and  passed on to the FRAGMENT SHADER
   Binormal          = vec3(normalize(gl_MultiTexCoord2));
   VNormal           = gl_Normal; // Must happen here..
   VNormal           = normalize(vec3(mWorld * vec4(VNormal.xyz, 0 )).xyz);  // ???????

   // WHAT DO WE DO WITH THESE TO HAVE THEM UPDATED DURING SKINNED DEFORMATION ?!?!?! 
   Tangent           = normalize(vec3(mWorld * vec4(Tangent.xyz, 0 )).xyz);
   Binormal          = normalize(vec3(mWorld * vec4(Binormal.xyz,0)).xyz);



   gl_Position       = gl_ModelViewProjectionMatrix * gl_Vertex;    // UNSEEN, This ensures that OBJECT TRANSLATION is considered..
   VertexGLPosition  = vec3(mWorld * vec4(gl_Vertex.xyz,1)).xyz;  // This ensures that OBJECT TRANSLATION is considered..
  }

