#ifndef TEMPLATEOBJECT_H
#define TEMPLATEOBJECT_H

// Will require the struct and enums from DynamicObject.h to be loaded first. (Share the data structure with it)
// Check DynamicObjectManager to see how this class is included. (Used by the dynamic object manager)

#include <irrlicht.h>
using namespace irr;

class TemplateObject
{
    public:
		// -- Functions
		TemplateObject(core::stringw name); // Define the template and give it a name
		virtual ~TemplateObject(); // Destructor

		void setName(core::stringw name); // Set the name of the template
		core::stringw getName(); // Get the name of the template

		void setType(core::stringc name); // Set the type of the template
		void setType(TYPE type); // Set the type of the template
		TYPE getType(); // Get the type of this template

		cproperty getProperties();
		void setProperties(cproperty prop);		
		cproperty getProp_base();
		void setProp_base(cproperty prop);

		void setMaterialType(E_MATERIAL_TYPE mType);
		E_MATERIAL_TYPE getMaterialType();

		void setScale(f32 scale);
		f32 getScale();

		// Public Variables - Data
		core::stringc templateObjectName;//The original object name

		core::stringc meshFile; // Mesh filename

		DynamicObject_Animation currentAnim;
		vector<DynamicObject_Animation> animations; // Animations in the template
		core::stringw script; // Pre-defined script in the template object

		stringw type; // Contain the type "text" of the object (referenced in the "set" XML name.

		stringw description; //Contain the description of the template
		stringw author; // Contain the author name of the template
		stringw licence; // Contain the licence type for the model
		stringw category; // Category of the item
		stringc thumbnail;
      
    private:

		stringw templateName; // Name of the template

		TYPE objectType; // Type of the template
		stringc typeText; // Text version of the type 

		cproperty prop_base; // Base properties
		cproperty prop_level; // Increase per level
		cproperty properties; // Active properties

		f32 templateScale; // Scale of the object
		E_MATERIAL_TYPE materialType;
        
		
};

#endif // TEMPLATEOBJECT_H
