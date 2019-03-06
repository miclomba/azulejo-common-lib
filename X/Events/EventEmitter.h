#ifndef events_event_emitter_h
#define events_event_emitter_h

#include "config.h"

namespace events
{

class EVENTS_DLL_EXPORT EventEmitter
{
public:
	EventEmitter();
	virtual ~EventEmitter();
};

} // end namespace events

#endif // events_event_emitter_h
