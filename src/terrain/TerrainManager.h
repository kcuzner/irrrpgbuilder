#ifndef TERRAINMANAGER_H
#define TERRAINMANAGER_H

#include <map>
#include "TerrainTile.h"
#include "../App.h"

class TerrainManager
{
    public:

	    static TerrainManager* getInstance();
        virtual ~TerrainManager();

        void createSegment(vector3df pos, bool empty=false, bool noextra=false, bool param=true);
		ISceneNode * createCustomSegment(vector3df pos, core::stringc model);
		ISceneNode * createCustomTile(vector3df pos, core::stringc model);

		void setEmptyTileVisible(bool visible);
		void setEmptyTileGridScale(f32 scale); //used for the scale of the texture for the grid
 
        std::string getHashCode(vector3df pos);

        f32 getHeightAt(vector3df pos);
		f32 getVerticeHeight(vector3df pos);

		void paintVegetation(App::MousePick mousePick, bool erase);

        void saveToXML(TiXmlElement* parentElement);
        bool loadFromXML(TiXmlElement* parentElement);

		void transformSegments(App::MousePick mousePick, f32 radius,f32 radius2, f32 strength,  bool norecalc=false);
		void transformSegmentsToValue(App::MousePick mousePick, f32 radius, f32 radius2, f32 strength, f32 value,  bool norecalc=false);

		void recalculate();

		stringc getTileMeshName();
		void setTileMeshName(stringc name);
		f32 getTileMeshSize();
		void setTileMeshSize(f32 newsize);

		f32 getScale();
		void setScale(f32 newsize);

		stringc getTerrainTexture(u32 layer);
		void setTerrainTexture(u32 layer, stringc name);

		void setVisible(bool visible);
        void clean();

        void showDebugData(bool show);

		void drawBrush(bool useray = true);
		void drawBrushCircle(vector3df position, f32 radius, int step,bool drawlines=false, bool useray=true);
		void drawBrushCircleSmooth(vector3df position, f32 radius, int step,bool drawlines=false, bool useray=true);

		void update();

		TerrainTile* getSegment(vector3df pos);
        TerrainTile* getSegment(std::string hashCode);
        ISceneNode* getEmptySegment(vector3df pos);
        void removeEmptySegment(vector3df pos, bool force=false);
        void createEmptySegment(vector3df pos);
		void createEmptySegmentMatrix(u32 x, u32 y);
		void removeSegment(vector3df pos, bool custom = false);
		void deleteTaggedSegment();

		void rotateLeft(core::vector3df pos);
		void rotateRight(core::vector3df pos);

		inline bool isParametric(){return parametric;}

		typedef std::map<std::string, TerrainTile*> TerrainMap;
		typedef std::pair<std::string, TerrainTile*> TerrainMapPair;

		typedef std::map<std::string, ISceneNode*> TerrainEmptySegmentsMap;
		typedef std::pair<std::string, ISceneNode*> TerrainEmptySegmentsMapPair;

    protected:
		

    private:
        f32 scale;
		bool parametric;

		// Tile tagged for deletion
		TerrainTile* tileTagged;
		vector3df posTagged;

        TerrainMap terrainMap;
        TerrainEmptySegmentsMap terrainEmptySegmentsMap;

		vector<TerrainTile*> customTiles;

        f32 transformSegmentsRadius;
        f32 transformSegmentsStrength;

		f32	tilemeshsize;
		stringc tilemesh;
		stringc terraintexture0;
		stringc terraintexture1;
		stringc terraintexture2;
		stringc terraintexture3;
		stringc terraintexture4;

        TerrainManager();
		u32 timer; // Used for time differential on the brush
		bool needrecalc; // Flag to tell the tile need to be recalculated
		u32 lastbrushtime; //time taken to redraw the brush the last time
		u16 brushstep; 
		f32 empty_texture_scale;
        
};

#endif // TERRAINMANAGER_H
