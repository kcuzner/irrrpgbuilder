#include "App.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#ifdef _IRR_WINDOWS_
    #include <direct.h>
#endif

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

rect<s32> myRect(s32 x, s32 y, s32 w, s32 h)
{
    return rect<s32>(x,y,x+w,y+h);
}

App::App()
{
    device = createDevice( video::EDT_OPENGL, dimension2d<u32>(632, 480), 32, false, false, false, 0);
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
    device->setEventReceiver(this);

    device->setWindowCaption(L"IRBPackMaker");

    this->setupGUI();
}

App::~App()
{
    device->drop();
}

void App::addFile()
{
    CGUIFileSelector* selector = new CGUIFileSelector(L"File Selector", guienv, guienv->getRootGUIElement(), OF_ADD_FILE, CGUIFileSelector::EFST_OPEN_DIALOG);
    if (selector)
    {
        selector->setCustomFileIcon(driver->getTexture("../media/file.png"));
        selector->setCustomDirectoryIcon(driver->getTexture("../media/folder.png"));
        selector->drop();
    }
}

bool App::OnEvent(const SEvent& event)
{
    if (event.EventType == EET_GUI_EVENT)
    {
        s32 id = event.GUIEvent.Caller->getID();

        switch(event.GUIEvent.EventType)
        {
            case EGET_BUTTON_CLICKED:
                switch(id)
                {
                    case BT_NEW:
                        this->cleanWorkspace();
                        break;
                    case BT_LOAD:
                        this->openPack();
                        break;
                    case BT_SAVE:
                        this->savePack();
                        break;
                    case BT_ADD:
                        this->addFile();
                        break;
                    case BT_EXTRACT:
                        this->extractFiles();
                        break;
                    case BT_REMOVE:
                        if(lbFiles->getItemCount() > 0 && lbFiles->getSelected()!=-1)
                        {
                            //realFileNames.erase(realFileNames.begin() + lbFiles->getSelected());
                            filesContent[lbFiles->getSelected()].clear();
                            filesContent.erase(filesContent.begin() + lbFiles->getSelected());
                            lbFiles->removeItem(lbFiles->getSelected());

                            this->printMemory();
                        }
                        break;
                    case BT_CLOSE:
                        runVar = false;
                        break;
                }
                break;
            case EGET_FILE_SELECTED:
                CGUIFileSelector* dialog = (CGUIFileSelector*)event.GUIEvent.Caller;
                s32 id = event.GUIEvent.Caller->getID();

                core::stringc fileName = dialog->getFileName();
                fileName = device->getFileSystem()->getAbsolutePath(fileName);

                switch(id)
                {
                    case OF_LOAD_PACK:
                        if (dialog->getDialogType() == CGUIFileSelector::EFST_OPEN_DIALOG)
                        {
                            this->cleanWorkspace();

                            fileName = device->getFileSystem()->getAbsolutePath(fileName);

                            cout << "READ FILE : " << fileName.c_str() << endl;

                            IReadFile* pReadPack = device->getFileSystem()->createAndOpenFile(fileName);

                            int ptotalfiles = 0;

                            pReadPack->read(&ptotalfiles,sizeof(int));
                            cout << "Total of files in pack: " << ptotalfiles << endl;

                            for(int i=0;i<ptotalfiles;i++)
                            {
                                int pfsize = 0;
                                pReadPack->read(&pfsize,sizeof(int));

                                char pfilename[200];

                                pReadPack->read(&pfilename,200);
                                stringc pFileNameStr = pfilename;
                                pFileNameStr.trim();

                                cout << "File[" << pFileNameStr.c_str() << "] size:" << pfsize << endl;

                                lbFiles->addItem(stringw(pFileNameStr).c_str());
                                vector<char> pFileContent;

                                for(int j=0;j<pfsize;j++)
                                {
                                    char pcontent;
                                    pReadPack->read(&pcontent,1);
                                    //cout << pcontent;
                                    pFileContent.push_back(pcontent);
                                }

                                filesContent.push_back(pFileContent);
                            }

                            this->printMemory();

                            this->setCurrentPack(fileName);
                        }
                        break;
                    case OF_ADD_FILE:
                        if (dialog->getDialogType() == CGUIFileSelector::EFST_OPEN_DIALOG)
                        {
                            bool fileAlreadyExist = false;

                            fileName = device->getFileSystem()->getAbsolutePath(fileName);

                            for(int i=0;i<lbFiles->getItemCount();i++)
                            {
                                if( stringc(lbFiles->getListItem(i)) == stringc(device->getFileSystem()->getFileBasename(fileName,true)) )
                                    fileAlreadyExist = true;
                            }

                            if(!fileAlreadyExist)
                            {
                                lbFiles->addItem( stringw(device->getFileSystem()->getFileBasename(fileName,true)).c_str() );
                                //realFileNames.push_back(device->getFileSystem()->getFileBasename(fileName,true));
                                vector<char> currentFileContent;

                                //read file...
                                IReadFile* pReadFile = device->getFileSystem()->createAndOpenFile(fileName);
                                for(int pReadCounter=0;pReadCounter<pReadFile->getSize();pReadCounter++)
                                {
                                    char c;
                                    pReadFile->read(&c,1);
                                    currentFileContent.push_back(c);
                                }

                                //store file in the vector.
                                filesContent.push_back(currentFileContent);

                                this->printMemory();
                            }
                            else
                                guienv->addMessageBox(L"Error",L"The pack can't have two files with the same name!");
                        }
                        break;
                    case OF_SAVE_PACK:
                        {
                            fileName = device->getFileSystem()->getAbsolutePath(fileName);

                            cout << "SAVE FILE : " << fileName.c_str() << endl;
                            IWriteFile* file = device->getFileSystem()->createAndWriteFile(fileName,false);

                            //write the number of files in pack
                            int ptotalfiles = filesContent.size();
                            file->write(&ptotalfiles,sizeof(int));

                            //write all files to the pack
                            for(int i = 0;i<filesContent.size(); i++)
                            {
                                stringc cFileName = stringc(lbFiles->getListItem(i));
                                cFileName = device->getFileSystem()->getFileBasename(cFileName,true);
                                vector<char> cFileContent = filesContent[i];

                                cout << "F:" << cFileName.c_str() << endl;

                                //write current file size
                                int pfsize = cFileContent.size();
                                file->write(&pfsize,sizeof(int));

                                //write current filename with 200 chars
                                char cfilename[200] = {' '};
                                stringc pfilename = cFileName;
                                for(int i=0;i<pfilename.size();i++) cfilename[i] = pfilename[i];

                                file->write(cfilename,200);

                                //now a simple process to read one byte from the real file and write it to the pack
                                for(int j=0;j<cFileContent.size();j++)
                                {
                                    char c = cFileContent[j];
                                    file->write(&c,1);
                                }
                            }

                            file->drop();

                        }
                        break;

                    case OF_EXTRACT_FILES:
                        {
                            fileName = device->getFileSystem()->getAbsolutePath(fileName);

                            #ifdef _IRR_WINDOWS_
                                if(fileName[fileName.size()-1] != '\\') fileName.append('\\');///TODO: test this on windows platform
                            #else
                                if(fileName[fileName.size()-1] != '/') fileName.append('/');
                            #endif

                            stringc newFolder = fileName;
                            newFolder.append("ExtractedPack_");
                            srand(time(0));
                            newFolder.append(stringc(rand()));

                            #ifdef _IRR_WINDOWS_
                                newFolder.append('\\');
                                mkdir(newFolder.c_str());
                            #else
                                newFolder.append('/');
                                mkdir(newFolder.c_str(),0777);
                            #endif

                            cout << "MAKE_DIR:" << newFolder.c_str() << endl;


                            //write all files to the folder
                            for(int i = 0;i<filesContent.size(); i++)
                            {
                                stringc cFileName = stringc(lbFiles->getListItem(i));
                                cFileName = device->getFileSystem()->getFileBasename(cFileName,true);
                                vector<char> cFileContent = filesContent[i];

                                stringc diskFile = newFolder;
                                diskFile.append(cFileName);
                                cout << "EXTRACT FILE:" << diskFile.c_str() << endl;

                                IWriteFile* dFile = device->getFileSystem()->createAndWriteFile(diskFile.c_str(),false);

                                for(int j=0;j<cFileContent.size();j++)
                                {
                                    char c = cFileContent[j];
                                    dFile->write(&c,1);
                                }

                                dFile->drop();
                            }

                            stringc msg = "Pack Extracted Sucessful to : ";
                            msg.append(newFolder);

                            guienv->addMessageBox(L"Extract Pack",stringw(msg).c_str(),true);
                        }
                        break;
                }

                return true;
                break;
        }
    }

    return false;
}

void App::openPack()
{
    CGUIFileSelector* selector = new CGUIFileSelector(L"Load Pack", guienv, guienv->getRootGUIElement(), OF_LOAD_PACK, CGUIFileSelector::EFST_OPEN_DIALOG);
    if (selector)
    {
        selector->setCustomFileIcon(driver->getTexture("../media/file.png"));
        selector->setCustomDirectoryIcon(driver->getTexture("../media/folder.png"));
        selector->addFileFilter(L"IrrRPG Builder Pack", L"irbpk", driver->getTexture("../media/filepack.png"));
        selector->drop();
    }
}

bool App::savePack()
{
    CGUIFileSelector* selector = new CGUIFileSelector(L"Save Pack", guienv, guienv->getRootGUIElement(), OF_SAVE_PACK, CGUIFileSelector::EFST_SAVE_DIALOG);
    if (selector)
    {
        selector->setCustomFileIcon(driver->getTexture("../media/file.png"));
        selector->setCustomDirectoryIcon(driver->getTexture("../media/folder.png"));
        selector->addFileFilter(L"IrrRPG Builder Pack", L"irbpk", driver->getTexture("../media/filepack.png"));
        selector->drop();
    }
}

void App::setCurrentPack(stringc file)
{
    cout << file.c_str() << endl;
}

void App::setupGUI()
{
    guienv->getSkin()->setFont(guienv->getFont("../media/packMakerFont.xml"));

    int x = 5;
    int btsize = 40;

    btNew = guienv->addButton(myRect(x,5,32,32),0,BT_NEW);
    btNew->setImage(driver->getTexture("../media/bt_new.png"));
    btNew->setToolTipText(L"New Pack");

    x+=btsize;

    btLoad = guienv->addButton(myRect(x,5,32,32),0,BT_LOAD);
    btLoad->setImage(driver->getTexture("../media/bt_load.png"));
    btLoad->setToolTipText(L"Load Pack");

    x+=btsize;

    btSave = guienv->addButton(myRect(x,5,32,32),0,BT_SAVE);
    btSave->setImage(driver->getTexture("../media/bt_save.png"));
    btSave->setToolTipText(L"Save Pack");

    x+=btsize + 15;

    btAdd = guienv->addButton(myRect(x,5,32,32),0,BT_ADD);
    btAdd->setImage(driver->getTexture("../media/bt_add.png"));
    btAdd->setToolTipText(L"Add File to Pack");

    x+=btsize;

    btRemove = guienv->addButton(myRect(x,5,32,32),0,BT_REMOVE);
    btRemove->setImage(driver->getTexture("../media/bt_remove.png"));
    btRemove->setToolTipText(L"Remove File From Pack");

    x += btsize;

    btExtract = guienv->addButton(myRect(x,5,32,32),0,BT_EXTRACT);
    btExtract->setImage(driver->getTexture("../media/bt_extract.png"));
    btExtract->setToolTipText(L"Extract Files From Pack to Hard Disk");


    btClose = guienv->addButton(myRect(640-47,5,32,32),0,BT_CLOSE);
    btClose->setImage(driver->getTexture("../media/bt_close.png"));
    btClose->setToolTipText(L"Close IRBPackMaker");

    lbFiles = guienv->addListBox(myRect(5,47,620,428),0,-1,true);
}

void App::run()
{
    runVar = true;

    while(device->run() && runVar)
    {
        driver->beginScene(true, true, SColor(255,100,100,100));

        smgr->drawAll();
        guienv->drawAll();

        driver->endScene();
    }
}

void App::cleanWorkspace()
{
    while(lbFiles->getItemCount() > 0)
    {
        filesContent[0].clear();
        filesContent.erase(filesContent.begin());
        lbFiles->removeItem(0);
    }
    this->printMemory();
}

void App::extractFiles()
{
    CGUIFileSelector* selector = new CGUIFileSelector(L"Extract Files", guienv, guienv->getRootGUIElement(), OF_EXTRACT_FILES, CGUIFileSelector::EFST_CHOOSE_FOLDER);
    if (selector)
    {
        selector->setCustomFileIcon(driver->getTexture("../media/file.png"));
        selector->setCustomDirectoryIcon(driver->getTexture("../media/folder.png"));
        selector->drop();
    }
}

void App::printMemory()
{
    return;//comment this line to debug

    cout << "_____________________________________________ MEMORY __" << endl;

    for(int iFiles=0;iFiles <filesContent.size();iFiles++)
    {
        cout << "FILE:" << stringc(lbFiles->getListItem(iFiles)).c_str() << endl;

        vector<char> fFile = filesContent[iFiles];

        for(int fContent=0;fContent<fFile.size();fContent++)
        {
            cout << fFile[fContent];
        }
    }

    cout << "_________________________________________ MEMORY END __" << endl;
}
