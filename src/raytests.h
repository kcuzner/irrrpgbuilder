#ifndef RAYTESTS_H
#define RAYTESTS_H

#include <irrlicht.h>
#include <vector>

using namespace irr;

class raytest
{
	public:
		raytest();
		void addRay(core::line3df ray, bool result);
		void clearAll();
		void update();
		void init(IrrlichtDevice* device);
		inline void enable(bool activate){active=activate;};

	private:

		video::IVideoDriver* driver;
		IrrlichtDevice* device;

		std::vector<core::line3df> linesbad;
		std::vector<core::line3df> linesgood;
		bool active;
		



};

#endif // RAYTESTS_H