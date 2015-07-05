
 //   FRAGMENT SHADER written by J.P. 2015..

 
 // Used IRRLICHT 1.8.1..
 
 // We are limited to 4 maps so use the alpha and RGB channels with inititive..
 uniform sampler2D DiffuseMap;        // The "ClipMap" is determined in the Diffuse ALPHA Channel..
 uniform sampler2D NormalMap;         // The "Droplet" Effect MASK is in the "NormalMap" ALPHA Channel..
 uniform sampler2D SpecularMap;       // We could have Further functionality in the R, G or B channels if needed..
 uniform sampler2D GlossMap;          // We could have had Gloss as the alpha channel in the specular map..
                                      // Should we need the FOURTH Texture, well change Glossmap to be Specular ALPHA..
 uniform int       FShaderCommand;    // An Integer that is controlled from the program used to alter the operation of this Fragment Shader (press buttons 1 to 0..)
                                      // We also could have a "VProgramCommand" for the Vertex Program..
 uniform vec4      CamPosTEST;
 uniform vec3      LightPos001;       // This would become an array when it comes to many lights..
 uniform mat4      mWorld;            // Fed from IRRLICHT via Vetrex Shader.. ENSURES CORRECT LIGHTING after SCALE TRANSLATE and ROTATE.... 
                                      // MWORLD ensures that we have physically correct lighting on a normal map 
                                      // no mater what the SCALE, ROTATION or TRANSLATION is..
 varying vec2      UVCoordsXY;        // Passed through the Vertex Program..
 varying vec3      VNormal;           // Effectively Fragment Normal after passing through the Vertex Progam.
 varying vec3      Tangent;           // What is the feasibility of a "RGB pre-rendered Tangent Map"? 
 varying vec3      Binormal;          // BiNormal and NOT BiTangent.. (no specific reason) 
 varying vec4      CameraPosition;    // We have alight hooked to the camera for demonstrative prposes.
 varying vec3      VertexGLPosition;  // As needed.. 
 // varying vec3      LightPosition;     // As needed.. 
 varying vec3      LightColour;       // MUST IMPLEMENT THIS !!! 
                                      // We need an array of lights from the scene which is either processed 
                                      // for importance by the CPU in the code or in here by the GPU.. 
 #define MAX_LIGHTS 32                // The Maxcimum amount of lights supported by the shader..

 // =============================================================================
 vec4 Interpolate(float Slider, vec4 ArgA, vec4 ArgB) 
  {vec4 OutputVal = ArgB * Slider + ArgA * (1 - Slider);
   return OutputVal;
  }
 // =  M A I N  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
 void main()
  {//   VNormal           = normalize(vec3(mWorld * vec4(VNormal.xyz, 0 )).xyz);  // ???????
   // AGAIN.. HOW DO WE UPDATE THESE TO CHANGE WITH ETHE SKINNED ANIMATION ? 
   //   Tangent           = normalize(vec3(mWorld * vec4(Tangent.xyz, 0 )).xyz);
   //   Binormal          = normalize(vec3(mWorld * vec4(Binormal.xyz,0)).xyz);
   // vec3 VNormalORIG = VNormal;

   // -- IMAGE MAPS -- (get them first) 
   vec4 DiffuseMAPPEDCol  = texture2D(DiffuseMap,UVCoordsXY.xy); // ALPHA is "Clipmap"..
   vec4 NormalMAPPEDCol    = texture2D(NormalMap,UVCoordsXY.xy); // ALPHA is "Droplet Shader Mask"..
   vec3 SpecularMAPPEDCol  = texture2D(SpecularMap,UVCoordsXY.xy);
   vec3 GlossMAPEDCol      = texture2D(GlossMap,UVCoordsXY.xy);

   // - UN-RANGECOMPRESSED NORMALS - 
   vec3 UnCompressedNormal = (NormalMAPPEDCol -0.5) * 2.0;  

   // - LIGHTS - 
        LightPos001     = CamPosTEST.xyz;
   vec3 LightPos002 = vec3( CamPosTEST.x , 4500.0 , CamPosTEST.z); //Second light very high in the sky. (imitate sun)
   vec3 LightCol001 = vec3( 0.9 , 0.9 , 1.0 ); LightCol001.xyz *= 0.65; // Global Intensity Scale.
   vec3 LightCol002 = vec3( 0.8 , 0.8 , 1.0 ); LightCol002.xyz *= 0.99; // Global Intensity Scale.


   // - NORMALISED DIRECTIONS -
//   vec3 ViewDir     = normalize(CamPosTEST - VertexGLPosition);
//   vec3 LightDir001 = normalize(LightPos001 - VertexGLPosition); // Fragment Position??
//   vec3 LightDir002 = normalize(LightPos002 - VertexGLPosition);

  

   normalize(UnCompressedNormal); // Didnt really help our case..
   vec3 FinalNormal;
   // !!!!!!!!!!!!!!!!!!
   // FinalNormal = normalize( (UnCompressedNormal.x * Tangent)
   //            +  (UnCompressedNormal.y * Binormal)
   //            + (UnCompressedNormal.z * VNormal));
   // !!!!!!!!!!!!!!!!!!!!
   // What follows is a "Broken Down" version of the code above for us to see how it works..

   vec3 AddA;  vec3 AddB;  vec3 AddC;
   // - T A N G E N T - 
   AddA.x = UnCompressedNormal.x * Tangent.x; // STATIC !!  MUST BE ANIMATED!
   AddA.y = UnCompressedNormal.x * Tangent.y;
   AddA.z = UnCompressedNormal.x * Tangent.z;
   // - B I N O R M A L - 
   AddB.x = UnCompressedNormal.y * Binormal.x; // STATIC !!  MUST BE ANIMATED!
   AddB.y = UnCompressedNormal.y * Binormal.y;
   AddB.z = UnCompressedNormal.y * Binormal.z;
   // - V E R T E X   N O R M A L - (updated with skinned deformation) 
   AddC.x = UnCompressedNormal.z * VNormal.x; // DYNAMIC !!   IS ANIMATED!!
   AddC.y = UnCompressedNormal.z * VNormal.y;
   AddC.z = UnCompressedNormal.z * VNormal.z;
   vec3 AddFin;
   AddFin.x = AddA.x + AddB.x + AddC.x;  // Without Animated Tangents, we will have Animation Normal Deviation..
   AddFin.y = AddA.y + AddB.y + AddC.y;
   AddFin.z = AddA.z + AddB.z + AddC.z;
   FinalNormal =  normalize (AddFin);  

                             // WE NEED TO CORRECT THIS TO WORK WITH "SKINNED DEFORMATION"!!!..

   // HOW DO WE GET SKIN DEFORMATION TO RESULT IN CORRECT LIGHTING? 
   // Look at existing code..
   // HOW DO WE USE THE DYNAMIC NORMAL TO ANIMATE THE TANGENT..(Bitangent merely a cross between the other two)

   // Ramblings..

   // It could be a MIRACULOUS EQUATION that can UPDATE THE TANGENTS with what weve got in the shader which is not really much..(does not look likely)..
   // There could be some way to PRESERVE THE ORIGINAL PRE-DEFORMED GEOMETRY and get it into the shader as a MAP OF SOME SORT.. (approximations?)   
   // We could make peace with our problem and keep things in mind when we animate the model.. (Skinning versus GL Rotation etc..)..
   // If we, after exasperating trials, finally make peace with the fact that tangent influenced normals
   // cannot be perfected for skinned models, then we can at least know that the Z (outward) direction of the final normal is what we want it to be..
   // We could get away with these deviations because with moving models this may not be "disturbingly" apparent..
   // However, we shall keep searching for a mathematical solution..
   // HOW DO WE GET VALUES "FROM" THE GPU? We know how to SEND values to it..
   // HOW DOES COMPOSER HANDLE THIS PROBLEM ? 
   // NVIDIA COMPOSER SEEMS TO HAVE ANIMATED TANGENTS AND BINORMALS!!

   // Looking at Composer, we've learned that the animated tangents come from CPU code i.e. NOT DONE IN THE SHADERS..
   // IT IS BECOMING CLEAR THAT WE HAVE TO GET IRRLICHT TO ANIMATE THESE TANGENTS FROM THE CPU SIDE (CODE)..
   // Now.. Im a bit apprehensive to alter the ENGINE CODE, so what Ill do now is to try find a way
   // to somehow bypass the engine from our code and then incorporate some kind of TANGENT / BINORMAL ANIMATION
   // from our code sothat if it proves successful then someone can incorporate the
   // code into the next release of IRRLICHT with what is found in this example GPU/CPU code..



   // - NORMALISED DIRECTIONS - (used for Diffuse and Specularity) 
   vec3 ViewDir     = normalize(CamPosTEST - VertexGLPosition);
   vec3 LightDir001 = normalize(LightPos001 - VertexGLPosition); // Fragment Position??
   vec3 LightDir002 = normalize(LightPos002 - VertexGLPosition);
   // DIFFUSE BASE FOR ALL LIGHTS..
   float DiffBASE001 = max(dot(FinalNormal, (LightDir001)),0.0);
   float DiffBASE002 = max(dot(FinalNormal, (LightDir002)),0.0);
   // ADD INDIVIDUAL LIGHT COLOURS..
   vec3 DiffColByLight001 = DiffBASE001 * LightCol001; // All xyz..
   vec3 DiffColByLight002 = DiffBASE002 * LightCol002;
   // FINAL COLOURED DIFFUSE MIX..
   vec3 FinalDiffLightsColed = DiffColByLight001 + DiffColByLight002; // Combine the diffuse results of different lights..

   // S P E C U L A R I T Y   versus   G L O S S..
   // The equation below could perhaps be simplified..
   // Youll note that it looks somewhat strange in that uses a Power of a Power which is something I've never encountered before..
   // The reason for using this "Extension" of Phong's method is to Standardise the concept of gloss for the artist when it comes to 
   // mapping Gloss for Specularity..
   // Look in the example Gloss/Specular map (which may be mixed into a R, G, B or A channel in the Gloss/Specular map)
   // for a little "Gloss palette" (if there was space)..
   // I seems very few if any shader implementations out there truly understands that Specularity IS NOT Gloss..
   // Specularity is the amount of Light reflected or intensity of the Specular highlight (mathematically similar to reflection),
   // where Gloss is the Size of that Highlight..(Unpolished Metal Gloss has a "bigger" highlight and lower Specularity whereas
   // a shiny Apple would have a "smaller" highlight and higher specularity)..
   // What I tried here is to Standardise the Level of GREY SCALE as related to the sharpness of the highlight..

   // DONT REMOVE THE CLAMPS..
   float SpecBASE001 = clamp(pow(clamp(dot(normalize((ViewDir + LightDir001)),FinalNormal),0.0,1.0),  pow(2, (GlossMAPEDCol * 10.0)) ), 0.0 , 1.0 );
   float SpecBASE002 = clamp(pow(clamp(dot(normalize((ViewDir + LightDir002)),FinalNormal),0.0,1.0),  pow(2, (GlossMAPEDCol * 10.0)) ), 0.0 , 1.0 );

   vec3 SpecBasColled001 = LightCol001.xyz * SpecBASE001 ;
   vec3 SpecBasColled002 = LightCol002.xyz * SpecBASE002 ;
   vec3 SpecColledFINAL = (SpecBasColled001  +  SpecBasColled002); // Will be in loop..
   vec3 SpecularColour =  SpecularMAPPEDCol.xyz * SpecColledFINAL.xyz ; // SPECULAR is mathematically based on REFLECTION thus should reflect LIGHT COLOUR..
   // keep in mind that this (above) would eventually be a loop of some sort to handle
   // all the lights once we have more lights and a good system to feed the lights from the main program..
   // I am considering a "lights pool" from which we select the important ones and perhaps
   // combine or average , say, two lights in terms of position and colour into one light in the shader..
   // We dont really want to have each light in the program reflected verbatim in the shader..
   // Anyhow, this is future stuff..


   // -- DROPLET SHADER EFFECT -- 
   // " D R O P L E  T   S H A D E R "   P A R T   1   o f   3 ..   
   // Droplet Shading simulates the effect created when a light shines on, for instance,
   // a red round glassy gem in a ring where we have a Specular Highlight and then also a
   // reversed-bigger sort of highlight that is the same colour as the 
   // material which the gem is made of..
   // This effect is useful for, well, gems and particularly the irisses in eyes..

   float GlobalDropConcavity = -1.0; // Think of the "Convexity" of the "Lens" of an eye then of the "Concavity" of the "Iris"..
   vec3 AlteredUCN = UnCompressedNormal;
   // Invert the X and Y normals to simulate the Concavity of an Iris or back of a gem..
   AlteredUCN.x *= GlobalDropConcavity;    // Tweakable value.. Was originally -1.
   AlteredUCN.y *= GlobalDropConcavity;    // It is not a mathematically perfect thing so tweaking is O.K..
   vec3 DropletNormal = normalize((AlteredUCN.y * Binormal)  // The "Altered Normal" is an Inverse of the X and Y of the
                                 +(AlteredUCN.x * Tangent)   
                                 +(AlteredUCN.z * VNormal)); // "Original Normal" with the Z of the Normal UNTOUCHED..
                                 // Remember to change these once we get Skinned Deformation right..
   float GlobalDropFact = 0.75;
   float GlobalDropPhong = 8.0;
   vec3 DropletColled001 = LightCol001;   
   float DropletEffect001 = clamp(dot(normalize((ViewDir + LightDir001)),DropletNormal),0.0,1.0);  // Correct..
   DropletEffect001 = clamp(pow(DropletEffect001,  GlobalDropPhong ), 0.0 , 1.0 );
   // Phong 8.0 is good..(here we dont use the Standardising 
   // POW of POW as we manually controll the Sharpness of the "Inner Hhighlight")
   DropletEffect001 *= GlobalDropFact;
   DropletColled001.xyz *= DropletEffect001;
   vec3 DropletColled002 = LightCol002;   
   float DropletEffect002 = clamp(dot(normalize((ViewDir + LightDir002)),DropletNormal),0.0,1.0);  // Correct.. 
   DropletEffect002 = clamp(pow(DropletEffect002,  GlobalDropPhong ), 0.0 , 1.0 );
   DropletEffect002 *= GlobalDropFact;
   DropletColled002.xyz *= DropletEffect002;
   vec3 DropletFINAL = DropletColled001  +  DropletColled002;
   // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   // " D R O P L E T   S H A D E R "    P A R T    3   o f   3  . .
   vec4 PreDropletDiffuse = DiffuseMAPPEDCol; // DO THIS BEFORE OTHER CALCULATIONS..
   // We DONT want to use "already shaded" diffuse here as it will negate the effect..
   // Here we use the "Alpha hannel" of the "Mormal Map" to BLOCK OUT ANY MAPPED DIFFUSE..
   // It also serves as a "Mask" for the effect.. (steer clear of "if ()" as borders become visible)
   DiffuseMAPPEDCol.xyz *= (1.0 - NormalMAPPEDCol.w);
   gl_FragColor = DiffuseMAPPEDCol; // Re-apply the MASKED mapped diffuse to Gl_Col before further diffuse shading.. 
   gl_FragColor.xyz *= FinalDiffLightsColed.xyz;
   

   // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 
   // This is now added to the masked area.. Multiplication instead of "if ()" ensures smooth transition..
   gl_FragColor.x += DropletFINAL.x * PreDropletDiffuse.x * NormalMAPPEDCol.w;
   gl_FragColor.y += DropletFINAL.y * PreDropletDiffuse.y * NormalMAPPEDCol.w;
   gl_FragColor.z += DropletFINAL.z * PreDropletDiffuse.z * NormalMAPPEDCol.w;   
   gl_FragColor.w = DiffuseMAPPEDCol.w;



   // ADD THE SPECULAR HIGHLIGHT.. 
   gl_FragColor.x += SpecularColour.x;
   gl_FragColor.y += SpecularColour.y;
   gl_FragColor.z += SpecularColour.z;
   

   if (FShaderCommand == 0)  // -- DEFAULT FULL FUNCTION RENDER -- 
    {// INTERPOLATE BETWEEN THE RAW MAPPED DIFFUSE COLOUR AND THE SHADED COLOUR AS A WAY OF CONTROLLING AMBIENT LIGHTING..
     // Often Misunderstood, the "AMBIENT" value for a scene is merely how much of the ORIGINAL MAPPED DIFFUSE COLOUR is rendered..
     // "Ambience" in my understanding is NOT REALLY a "Material Specific" attribute but rather a "Scene Specific" attribute..
     // "Ambience" could even be expressed as the render PLUS Original Mapped Colour..
     // For this wed need to KEEP SPECULAR UNTOUCHED.. The concept is debatable...
     // Keep pressing 1, 2 and 0 on the keyboard during runtime to understand the concept..
     // You may notice a Framerate Change during these keypresses which is simply
     // because different remdermodes use different sections of this GPU code and returns from different places..
     float AmbientSlider = 0.5;  // 0 is pure MAPPED values and 1 is pure SHADED values.. 0.85 is a good balance when considering "Droplet Shading"..
     gl_FragColor = Interpolate(AmbientSlider, PreDropletDiffuse, gl_FragColor  );
    }
   // gl_FragColor = mix(gl_FragColor,PreDropletDiffuse , AmbientSlider) // cant ge mix to work??

   //|||  SHADER COMMANDS  ||||||
   if (FShaderCommand == 2) {gl_FragColor = DiffuseMAPPEDCol; return;}  // And skip everything else..
   if (FShaderCommand == 3) // Show FINAL NORMALS..
    {gl_FragColor.x = (FinalNormal.x / 2.0) + 0.5; // Our vanilla Range Compression representation.
     gl_FragColor.y = (FinalNormal.y / 2.0) + 0.5;
     gl_FragColor.z = (FinalNormal.z / 2.0) + 0.5;
     gl_FragColor.w = DiffuseMAPPEDCol.w;
     return;
    }

   if (FShaderCommand == 4)  // Show Diffuse by LIGHT_001 only..
   {// gl_FragColor.xyz = FinalDiffLightsColed.xyz * 0.6; // uncomment if you want both lights.. (slightly subdued)..
     gl_FragColor.xyz = DiffColByLight001.xyz * 0.7;    // if you want Diffuse for Light 001 only..
    gl_FragColor.w = DiffuseMAPPEDCol.w;
    return;
   }

   if (FShaderCommand == 5) // Show DIFFUSE and SPECULAR (altered for better viewing) 
    {float DiffSub = 0.3;
     float SpecSub = 1.25;
     gl_FragColor.x = FinalDiffLightsColed.x * DiffSub;
     gl_FragColor.x += (SpecularColour.x * SpecSub);
     gl_FragColor.y = FinalDiffLightsColed.y * DiffSub;
     gl_FragColor.y += (SpecularColour.y * SpecSub);
     gl_FragColor.z = FinalDiffLightsColed.z * DiffSub;
     gl_FragColor.z += (SpecularColour.z * SpecSub);
     float GreyDiff = ( FinalDiffLightsColed.x +  FinalDiffLightsColed.y +  FinalDiffLightsColed.z ) / 3.0;

     gl_FragColor.w = DiffuseMAPPEDCol.w;
     return; } 

   if (FShaderCommand == 6) // Show TANGENTS..
    {gl_FragColor.x = (Tangent.x / 2.0) + 0.5;
     gl_FragColor.y = (Tangent.y / 2.0) + 0.5;
     gl_FragColor.z = (Tangent.z / 2.0) + 0.5;
     gl_FragColor.w = DiffuseMAPPEDCol.w;
     return; 
    } 
   if (FShaderCommand == 7) 
    {gl_FragColor.x =(Binormal.x/2.0)+0.5; gl_FragColor.y =(Binormal.y/2.0)+0.5; gl_FragColor.z =(Binormal.z/2.0)+0.5;
     gl_FragColor.w = DiffuseMAPPEDCol.w;
     return; 
    } 
   if (FShaderCommand == 8) // UNCOMPRESSED MAP NORMALS..
    {// gl_FragColor.x = NormalMAPPEDCol.x; gl_FragColor.y = NormalMAPPEDCol.y; gl_FragColor.z = NormalMAPPEDCol.z;
     gl_FragColor.x =  (UnCompressedNormal .x / 2.0) + 0.5; 
     gl_FragColor.y =  (UnCompressedNormal .y / 2.0) + 0.5;
     gl_FragColor.z =  (UnCompressedNormal .z / 2.0) + 0.5;
     gl_FragColor.w = DiffuseMAPPEDCol.w;
     return;
    } 
   if (FShaderCommand == 9) // Vertex Normals..
    {gl_FragColor.x =  (VNormal .x / 2.0) + 0.5; 
     gl_FragColor.y =  (VNormal .y / 2.0) + 0.5;
     gl_FragColor.z =  (VNormal .z / 2.0) + 0.5;
     gl_FragColor.w = DiffuseMAPPEDCol.w; // So we can have clipmapping..
     return; 
    } 

   //     - F R E S N E L -     (many uses)!! This forms part of an experimental SSS Shader Im working on..
   /*  // Uncomment this for an interesting effect!..
    float Fresnel = dot(VNormal, ViewDir);
   //float Fresnel = dot(FinalNormal * 1, ViewDir);  // DropletNormal
   // float Fresnel = dot(DropletNormal, ViewDir);  // DropletNormal
	// Fresnel = clamp((Fresnel - 0.2) * 1000.0, 0.0, 1.0);
	Fresnel *= 2.5;
	//if ((1.0 - Fresnel) > 0.7)
	 // {
	   // gl_FragColor.x += (1.0 - Fresnel);
        float InvFresnel = (1.0 - Fresnel);
       // InvFresnel *= 1.0;
	  if (InvFresnel > 0.0) { gl_FragColor.x += InvFresnel;}
     // }
    //gl_FragColor.y = Fresnel;
	//gl_FragColor.z = Fresnel;
	  gl_FragColor.w =  0.00 + (1.0 - Fresnel);
   //gl_FragColor.x = InvFresnel;
   //gl_FragColor.y = InvFresnel;
   //gl_FragColor.z = InvFresnel;

   //*/

  }// -- END -- 
  // Credits:  IRRLICHT FORUM MEMBERS..





