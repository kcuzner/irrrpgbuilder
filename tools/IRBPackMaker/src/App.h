#ifndef APP_H
#define APP_H

#include <irrlicht.h>
#include "CGUIFileSelector.h"
#include <iostream>
#include <vector>

using namespace std;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


class App  : public IEventReceiver
{
    public:
        App();
        virtual ~App();

        void run();

    protected:
    private:
        IrrlichtDevice *device;
        IVideoDriver* driver;
        ISceneManager* smgr;
        IGUIEnvironment* guienv;

        enum BT_ID{
            BT_NEW = 1,
            BT_SAVE = 2,
            BT_LOAD = 3,
            BT_CLOSE = 4,
            BT_ADD = 5,
            BT_REMOVE = 6,
            BT_EXTRACT = 7,
        };

        enum OF_ID{
            OF_LOAD_PACK = 101,
            OF_ADD_FILE = 102,
            OF_SAVE_PACK = 103,
            OF_EXTRACT_FILES = 104,
        };

        bool runVar;

        //Main Buttons
        IGUIButton* btNew;
        IGUIButton* btSave;
        IGUIButton* btLoad;
        IGUIButton* btClose;

        //File operations
        IGUIButton* btAdd;
        IGUIButton* btRemove;

        IGUIButton* btExtract;

        //list of files
        IGUIListBox* lbFiles;
        //vector<stringc> realFileNames;
        vector<vector<char> > filesContent;

        void addFile();
        virtual bool OnEvent(const SEvent& event);
        void openPack();
        void removeFile();
        void extractFiles();
        bool savePack();
        void setCurrentPack(stringc file);
        void setupGUI();

        void cleanWorkspace();

        //debug only..
        void printMemory();
};

#endif // APP_H
