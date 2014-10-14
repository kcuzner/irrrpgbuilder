#ifndef TERRAINTILE_H
#define TERRAINTILE_H

#include <irrlicht.h>
#include <iostream>
#include <vector>

#include "../fx/ShaderCallBack.h"

#include "Vegetation.h"

#include "../tinyXML/tinyxml.h"

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

		typedef struct
		{
			s32 id;
			f32 value;
			bool tree;
			int type;
			vector3df pos;
			vector3df rot;
			vector3df sca;
<<<<<<< .mine
		}TerrainData;

        TerrainTile(ISceneManager* smgr, ISceneNode* parent, vector3df pos, stringc name, bool custom=false, bool param=true);
		virtual ~TerrainTile();

		void createTerrain(ISceneNode* parent, vector3df pos, stringc name, bool param=true);
		void createCustom(ISceneNode* parent, vector3df pos, stringc name, stringc model);

        void paintVegetation(vector3df clickPos, bool erase);

        void transformMesh(vector3df clickPos, f32 radius, f32 radius2, f32 strength, bool norecalc=false);
		void transformMeshByVertices(vector<TerrainData> list, bool norecalc);
		void transformMeshToValue(vector3df clickPos, f32 radius, f32 radius2, f32 strength, f32 value, bool norecalc=false);
		bool checkModified();

        stringc getName();
        ISceneNode* getNode();
		f32 getNodeScale();

        ITriangleSelector* getTriangleSelector();
		

        void mergeToTile(TerrainTile* tile);

        void saveToXML(TiXmlElement* parentElement);
        bool loadFromXML(TiXmlElement* parentElement);

        f32 getHeightAt(vector3df pos);
		f32 getVerticeHeight(vector3df pos);

        void showDebugData(bool show);

		void assignTerrainShader(scene::ISceneNode* node);
		void assignWaterShader(scene::ISceneNode* node);

		
		void recalculate(bool simple=false); //Recalculate all the mesh (collision + normals), simple is without the collision

		void clean();

        

		// variables
		core::stringc customname;
		bool custom;
		IMesh* baseMesh;

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

        void transformMeshByVertex(s32 id, f32 y, bool addVegetation, bool norecalc=false);

		bool needrecalc;

		IMeshBuffer* meshBuffer;
		S3DVertex* mb_vertices;
		u16* mb_indices;
		IVideoDriver * driver;
};

#endif // TERRAINTILE_H
=======
		}TerrainData;

        TerrainTile(ISceneManager* smgr, ISceneNode* parent, vector3df pos, stringc name, bool custom=false, bool param=true);
		virtual ~TerrainTile();

		void createTerrain(ISceneNode* parent, vector3df pos, stringc name, bool param=true);
		void createCustom(ISceneNode* parent, vector3df pos, stringc name, stringc model);

        void paintVegetation(vector3df clickPos, bool erase);

        void transformMesh(vector3df clickPos, f32 radius, f32 radius2, f32 strength, bool norecalc=false);
		void transformMeshByVertices(vector<TerrainData> list, bool norecalc);
		void transformMeshToValue(vector3df clickPos, f32 radius, f32 radius2, f32 strength, f32 value, bool norecalc=false);
		bool checkModified();

        stringc getName();
        ISceneNode* getNode();
		f32 getNodeScale();

        ITriangleSelector* getTriangleSelector();
		

        void mergeToTile(TerrainTile* tile);

        void saveToXML(TiXmlElement* parentElement);
        bool loadFromXML(TiXmlElement* parentElement);

        f32 getHeightAt(vector3df pos);
		f32 getVerticeHeight(vector3df pos);

        void showDebugData(bool show);

		void assignTerrainShader(scene::ISceneNode* node);
		void assignWaterShader(scene::ISceneNode* node);

		
		void recalculate(bool simple=false); //Recalculate all the mesh (collision + normals), simple is without the collision

		void clean();

        

		// variables
		core::stringc customname;
		bool custom;

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

        void transformMeshByVertex(s32 id, f32 y, bool addVegetation, bool norecalc=false);

		bool needrecalc;

		IMeshBuffer* meshBuffer;
		S3DVertex* mb_vertices;
		u16* mb_indices;
		IVideoDriver * driver;
};

#endif // TERRAINTILE_H
>>>>>>> .r592
