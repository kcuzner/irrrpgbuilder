#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include <vector>
#include <irrlicht.h>
#include "../App.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


enum EVENT_TYPE
{
    EVENT_KEY_PRESSED
};


class EventReceiver : public IEventReceiver
{
    public:
        static EventReceiver* getInstance();

        bool isMousePressed(int mb);
        bool isKeyPressed(int key);

        void flushKeys();
        void flushMouse();

        bool OnEvent(const SEvent& event);

        virtual ~EventReceiver();
	
		
    protected:
    private:
        EventReceiver();
		
		bool mouse[EMIE_COUNT];
		bool keys[KEY_KEY_CODES_COUNT];
				
};

#endif // EVENTRECEIVER_H
