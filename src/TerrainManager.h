#ifndef TERRAINMANAGER_H
#define TERRAINMANAGER_H

#include <map>
#include "TerrainTile.h"
#include "App.h"

typedef std::map<std::string, TerrainTile*> TerrainMap;
typedef std::pair<std::string, TerrainTile*> TerrainMapPair;

typedef std::map<std::string, ISceneNode*> TerrainEmptySegmentsMap;
typedef std::pair<std::string, ISceneNode*> TerrainEmptySegmentsMapPair;

class TerrainManager
{
    public:
        static TerrainManager* getInstance();
        virtual ~TerrainManager();

        void createSegment(vector3df pos);
        f32 getScale();

        std::string getHashCode(vector3df pos);

        f32 getHeightAt(vector3df pos);

        void paintVegetation(MousePick mousePick, bool erase);

        void saveToXML(TiXmlElement* parentElement);
        bool loadFromXML(TiXmlElement* parentElement);

        void transformSegments(MousePick mousePick, f32 radius, f32 strength);
        void transformSegmentsToZero(MousePick mousePick, f32 radius, f32 strength);

		stringc getTileMesh();
		void setTileMesh(stringc name);

		stringc getTerrainTexture(u32 layer);
		void setTerrainTexture(u32 layer, stringc name);

		int getTileMeshSize();
		void setTileMeshSize(int newsize);

        void clean();

        void showDebugData(bool show);

    protected:
    private:
        static const f32 scale;

        TerrainMap terrainMap;
        TerrainEmptySegmentsMap terrainEmptySegmentsMap;

        f32 transformSegmentsRadius;
        f32 transformSegmentsStrength;

		int	tilemeshsize;
		stringc tilemesh;
		stringc terraintexture1;
		stringc terraintexture2;
		stringc terraintexture3;
		stringc terraintexture4;

        TerrainManager();
        TerrainTile* getSegment(vector3df pos);
        TerrainTile* getSegment(std::string hashCode);
        ISceneNode* getEmptySegment(vector3df pos);
        void removeEmptySegment(vector3df pos);
        void createEmptySegment(vector3df pos);
};

#endif // TERRAINMANAGER_H
