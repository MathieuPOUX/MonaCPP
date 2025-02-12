/*
This file is a part of MonaSolutions Copyright 2017
mathieu.poux[a]gmail.com
jammetthomas[a]gmail.com

This program is free software: you can redistribute it and/or
modify it under the terms of the the Mozilla Public License v2.0.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Mozilla Public License v. 2.0 received along this program for more
details (or else see http://mozilla.org/MPL/2.0/).

*/

#pragma once

#include "Mona/Mona.h"
#include "Mona/Threading/Signal.h"
#if !defined(_WIN32)
  #include <signal.h>
#endif

namespace Mona {


struct TerminateSignal : virtual Object {
	TerminateSignal();
	// return true if the event has been set
	bool wait(uint32_t millisec = 0);
	void set();

private:
#if defined(_WIN32)
	Signal					_terminate;
#else
	sigset_t				_signalSet;
#endif
};




} // namespace Mona
