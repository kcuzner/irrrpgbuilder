#include "raytests.h"

raytest::raytest()
{
	device=NULL;
	driver=NULL;
	active=false;
}

// Add a new ray to test inside the
void raytest::addRay(core::line3df ray, bool result)
{
	if (device && active)
	{
		if (result)
			linesgood.push_back(ray);
		else
			linesbad.push_back(ray);

	}

}

// Remove all the rays from the test
void raytest::clearAll()
{
	if (device)
	{
		linesbad.clear();
		linesgood.clear();
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

		if (linesbad.size()>0 || linesgood.size()>0)
		{
			for (int i=0 ; i<(int)linesbad.size() ; i++)
			{
				driver->draw3DLine(linesbad[i].start, linesbad[i].end, video::SColor(255,255,45,45)); //Red	
			}
			for (int i=0 ; i<(int)linesgood.size() ; i++)
			{
				driver->draw3DLine(linesgood[i].start, linesgood[i].end, video::SColor(255,145,255,45)); //Green	
			}
			
		}
	}
}

void raytest::init(irr::IrrlichtDevice *device)
{
	this->device = device;
	driver=device->getVideoDriver();
}