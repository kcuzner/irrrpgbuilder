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

class GUIConfigWindow {
    public:
        GUIConfigWindow(IrrlichtDevice* device);
        virtual ~GUIConfigWindow();

        void showWindow();
		inline void setID(u32 id) {cfgWindow->setID(id);}
		inline u32 getID() {return cfgWindow->getID();}

    protected:
    private:
        IrrlichtDevice* device;

		IGUITab* tabSystem;
		IGUITab* tabScenary;
		IGUITab* tabExperimental;
		IGUITab* tabPlayerApp;

        IGUIButton* btOK;
        IGUIButton* btCancel;

        IGUIWindow* cfgWindow;

        IGUIComboBox* resolutionList;
        vector<vector2di> vModes;

        IGUICheckBox* cbFullscreen;
        IGUICheckBox* cbResizeable;
		IGUICheckBox* cbVSync;
		IGUICheckBox* cbAntialias;
		IGUICheckBox* cbSilouette;

		IGUICheckBox* cbXEffect;

        IGUIComboBox* languageList;
		IGUIComboBox* models;	

		IGUIEditBox* ebDefaultProject;
		IGUIEditBox* ebLogoImage;
        vector<stringc> languageListVector;

        IGUIEditBox* ebTerrainMesh;
		IGUIEditBox* ebTerrainDensity;
        IGUIEditBox* ebTerrainScale;
        IGUIEditBox* ebTerrainL0;
        IGUIEditBox* ebTerrainL1;
        IGUIEditBox* ebTerrainL2;
        IGUIEditBox* ebTerrainL3;
		IGUIEditBox* ebTerrainL4;

        IGUIEditBox* ebOceanMesh;
        IGUIEditBox* ebOceanNormalMap;
        IGUIEditBox* ebOceanReflection;

        void populateLanguageList();
        void populateResolutionList();

        void loadActualSeetings();
        void saveNewSeetings();

        rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);

		core::stringc mapname;
		f32 terraindensity;
};

#endif // GUICONFIGWINDOW_H
