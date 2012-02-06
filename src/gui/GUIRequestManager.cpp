#include "GUIRequestManager.h"

GUIRequestManager::GUIRequestManager()
{

	selector = NULL;
	filedevice = NULL;
	guienv = NULL;	
	filename=L"";
}

GUIRequestManager::~GUIRequestManager()
{

}

GUIRequestManager* GUIRequestManager::getInstance()
{
    static GUIRequestManager *instance = 0;
    if (!instance) instance = new GUIRequestManager();
    return instance;
}
 
void GUIRequestManager::FileSelector()
{
	iscomplete=false;
	if (filedevice)
	{
		filedevice->closeDevice();
		filedevice->drop();
		filedevice=NULL;
	}
	// Create the new window
	if (!filedevice)
	{
		filedevice = irr::createDevice(video::EDT_BURNINGSVIDEO, irr::core::dimension2d<u32>(640,400), 32, false,false,false);
		filedevice->setResizable(true);
		filedevice->setWindowCaption(L"IRB: Load a project file...");
		
	} 
	
	guienv=filedevice->getGUIEnvironment();
	irr::video::IVideoDriver* driver =filedevice->getVideoDriver();
	
	
	// Set up the environment
	for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
    {
            video::SColor col = guienv->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
            col.setAlpha(230);
            guienv->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
    }

	// Fake office style skin colors
	guienv->getSkin()->setColor(EGDC_3D_SHADOW,video::SColor(200,140,178,226));
	guienv->getSkin()->setColor(EGDC_3D_FACE,video::SColor(200,204,227,248));
	guienv->getSkin()->setColor(EGDC_WINDOW,video::SColor(255,220,220,220));

	irr::gui::IGUIFont * Font = guienv->getFont("../media/fonts/Arial10.xml");
    if (Font)
	{	
		Font->setKerningWidth(-1);
		guienv->getSkin()->setFont(Font);
		Font->drop();
	}

	// Create the file selector
	selector =	new CGUIFileSelector(L"File Selector", filedevice->getGUIEnvironment(), filedevice->getGUIEnvironment()->getRootGUIElement(), 1, CGUIFileSelector::EFST_OPEN_DIALOG);
	selector->setCustomFileIcon(driver->getTexture("../media/art/file.png"));
    selector->setCustomDirectoryIcon(driver->getTexture("../media/art/folder.png"));
	selector->addFileFilter(L"IRB Project files", L"xml", driver->getTexture("../media/art/wma.png"));
	update();
}

void GUIRequestManager::update()
{
	if (filedevice->isWindowFocused())
	{
		if (selector->isComplete())
		{
			printf("Set the filename...\n");
			filename=(irr::core::stringw)selector->getFileName();
			printf("Set the filename to %s\n",((stringc)filename.c_str()));
			iscomplete=true;
			//selector->drop();
			printf("Will close the device now...\n");
			filedevice->minimizeWindow();
			filedevice->clearSystemMessages();
			filedevice->closeDevice();
			filedevice->drop();
			filedevice=irr::createDevice(video::EDT_NULL);
			filedevice->run();
			
			//filedevice->drop();
			printf("device closed...\n");
			selector->drop();
		}
		else
		{
			printf("Gui update...\n");
			filedevice->run();
			filedevice->getVideoDriver()->beginScene(true, true, SColor(0,200,200,200));
			filedevice->getSceneManager()->drawAll();
			filedevice->getGUIEnvironment()->drawAll();
			filedevice->getVideoDriver()->endScene();
		}

	}

}
