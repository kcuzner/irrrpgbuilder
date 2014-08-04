#ifndef XML_MANAGER_H
#define XML_MANAGER_H

#include "App.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class xmldata
{
	public:

		static xmldata * getInstance();
		core::stringw winconvert(core::stringw str);
		void loadBlock(IrrlichtDevice * device, core::stringc file );
		void loadSet(IrrlichtDevice * device);
		void loadLang(IrrlichtDevice * device);
		
	

	private:
		xmldata();
		~xmldata();

		void loadLangFile(core::stringc  filename);
		IrrlichtDevice * device;
};

#endif // XML_MANAGER_H