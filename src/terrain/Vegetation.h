#ifndef VEGETATION_H
#define VEGETATION_H

#include <irrlicht.h>
#include <vector>
#include "../App.h"
#include "../tinyXML/tinyxml.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

//this is an kind of vegetation database, it loads the available trees from the XML and store it.
class VegetationSeed
{
public:
    static VegetationSeed* getInstance()
    {
        static VegetationSeed* instance = 0;
        if(!instance) instance = new VegetationSeed();
        return instance;
    }

    stringc getTrunkMesh(int type)
    {
        return trunks[type];
    }

    stringc getLeafsMesh(int type)
    {
        return leafs[type];
    }

    int getTotalOfTypes()
    {
        return trunks.size();
    }

	vector<stringw> getNames()
	{
		return name;
	}

	vector<stringc> getThumb()
	{
		return thumb;
	}

	vector<bool> getEnabled()
	{
		return enabled;
	}

	void setEnabled(vector<bool> enable)
	{
		enabled=enable;
	}

private:
    //the index of the vector is the type of the tree.
    vector<stringc> trunks;
    vector<stringc> leafs;
	vector<stringw> name;
	vector<stringc> thumb;
	vector<bool> enabled;

    VegetationSeed()
    {
        TiXmlDocument doc("../media/vegetation/vegetation.xml");
        if (!doc.LoadFile())
        {
            #ifdef APP_DEBUG
            cout << "DEBUG : VEGETATION : FAILED TO LOAD VEGETATION.XML!" << endl;
            #endif
            return;
        }

        TiXmlElement* root = doc.FirstChildElement( "IrrRPG_Builder_Vegetation" );
        TiXmlNode* vXML = root->FirstChild( "vegetation" );

        while( vXML != NULL )
        {
            trunks.push_back(stringc(vXML->ToElement()->Attribute("trunk_mesh")));
            leafs.push_back(stringc(vXML->ToElement()->Attribute("leafs_mesh")));
			name.push_back(stringw(vXML->ToElement()->Attribute("name")));
			thumb.push_back(stringc(vXML->ToElement()->Attribute("thumb")));
			enabled.push_back(true);
            vXML = root->IterateChildren( "vegetation", vXML );
        }
    }
};

class Vegetation
{
    public:
        Vegetation(int type = -1);//added optional type (-1 means random)
        virtual ~Vegetation();

        vector3df getPosition();
		int getType();
        void setPosition(vector3df pos);
        void setRotation(vector3df rot);
        void setScale(vector3df scale);
		inline ISceneNode* getNode() {return trunk;}
		inline vector<stringw> getNames() {return VegetationSeed::getInstance()->getNames();}
		inline vector<stringc> getThumb() {return VegetationSeed::getInstance()->getThumb();}
		inline vector<bool> getEnabled() {return VegetationSeed::getInstance()->getEnabled();}
		inline int getTotalModels() {return VegetationSeed::getInstance()->getTotalOfTypes();}


        void showDebugData(bool show);

    protected:
    private:
        ISceneNode* trunk;
        IMeshSceneNode* leafs;

        //TODO: remove fake shadow when shadowmaps are available
        IMeshSceneNode* fakeShadow;

		int vegeType;
        f32 size;
		vector<bool> enabled;
};

#endif // VEGETATION_H
