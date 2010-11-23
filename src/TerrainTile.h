#ifndef TERRAINTILE_H
#define TERRAINTILE_H

#include <irrlicht.h>
#include <iostream>
#include <vector>

#include "ShaderCallBack.h"

#include "Vegetation.h"

#include "tinyXML/tinyxml.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

class TerrainTile
{
    public:
        TerrainTile(ISceneManager* smgr, ISceneNode* parent, vector3df pos, stringc name);

        void paintVegetation(vector3df clickPos, bool erase);

        void transformMesh(vector3df clickPos, f32 radius, f32 strength);
        void transformMeshToZero(vector3df clickPos, f32 radius, f32 strength);
        void transformMeshDOWN(vector3df clickPos, f32 radius, f32 strength);

        stringc getName();
        ISceneNode* getNode();
		f32 getNodeScale();

        ITriangleSelector* getTriangleSelector();
		

        void mergeToTile(TerrainTile* tile);

        void saveToXML(TiXmlElement* parentElement);
        bool loadFromXML(TiXmlElement* parentElement);

        f32 getHeightAt(vector3df pos);

        void showDebugData(bool show);

        virtual ~TerrainTile();
    protected:
        IMeshBuffer* getMeshBuffer();

    private:
        ISceneManager* smgr;
        ISceneNode* node;
        ISceneNode* ocean;

        f32 scale;
		f32 nodescale;

        ITriangleSelector* selector;

        vector<Vegetation*> vegetationVector;

		static const irr::f32 vegetationRange;

        Vegetation* getVegetationAt(vector3df pos);

        void transformMeshByVertex(s32 id, f32 y, bool addVegetation);
};

#endif // TERRAINTILE_H
