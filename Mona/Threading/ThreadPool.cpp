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

#include "Mona/Threading/ThreadPool.h"

using namespace std;


namespace Mona {

void ThreadPool::init(uint16_t threads, Thread::Priority priority) {
	_threads.resize(_size = threads ? threads : Thread::ProcessorCount());
	for (uint16_t i = 0; i < _size; ++i)
		_threads[i].set(priority);
}

uint16_t ThreadPool::join() {
	uint16_t count(0);
	for (Unique<ThreadQueue>& pThread : _threads) {
		if (!pThread->running())
			continue;
		++count;
		pThread->stop();
	}
	return count;
}

} // namespace Mona
