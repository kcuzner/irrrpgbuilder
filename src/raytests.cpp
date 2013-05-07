#include "raytests.h"

raytest::raytest()
{
	device=NULL;
	driver=NULL;
	active=false;
}

// Add a new ray to test inside the
void raytest::addRay(core::line3df ray)
{
	if (device && active)
	{
		lines.push_back(ray);
		device->getLogger()->log("added 1 line to the test");
	}

}

// Remove all the rays from the test
void raytest::clearAll()
{
	if (device)
	{
		lines.clear();
	}
}

// Update and draw the rays at each update
void raytest::update()
{
	if (device)
	{
		// Prepare the transformations and set the material
		driver->setTransform(video::ETS_WORLD, core::matrix4());
		video::SMaterial m;
        m.Lighting = false;
        driver->setMaterial(m);

		if (lines.size()>0)
		{
			for (int i=0 ; i<(int)lines.size() ; i++)
			{
				driver->draw3DLine(lines[i].start, lines[i].end, video::SColor(128,255,45,45)); //Red	
			}
			
		}
	}
}

void raytest::init(irr::IrrlichtDevice *device)
{
	this->device = device;
	driver=device->getVideoDriver();
}