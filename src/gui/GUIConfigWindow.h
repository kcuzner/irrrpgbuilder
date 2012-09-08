#ifndef GUICONFIGWINDOW_H
#define GUICONFIGWINDOW_H

#include <irrlicht.h>
#include <vector>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

class GUIConfigWindow
{
    public:
        GUIConfigWindow(IrrlichtDevice* device);
        virtual ~GUIConfigWindow();

        void showWindow();

    protected:
    private:
        IrrlichtDevice* device;
        IGUIButton* btOK;
        IGUIButton* btCancel;

        IGUIWindow* cfgWindow;

        IGUIComboBox* resolutionList;
        vector<vector2di> vModes;

        IGUICheckBox* cbFullscreen;
        IGUICheckBox* cbResizeable;

        IGUIComboBox* languageList;
        vector<stringc> languageListVector;

        IGUIEditBox* ebTerrainMesh;
        IGUIEditBox* ebTerrainScale;
        IGUIEditBox* ebTerrainL0;
        IGUIEditBox* ebTerrainL1;
        IGUIEditBox* ebTerrainL2;
        IGUIEditBox* ebTerrainL3;

        IGUIEditBox* ebOceanMesh;
        IGUIEditBox* ebOceanNormalMap;
        IGUIEditBox* ebOceanReflection;

        void populateLanguageList();
        void populateResolutionList();

        void loadActualSeetings();
        void saveNewSeetings();

        rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);
};

#endif // GUICONFIGWINDOW_H
