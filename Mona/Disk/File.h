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
#include "Mona/Disk/Path.h"
#include "Mona/Threading/Handler.h"

namespace Mona {

/**
File is a Path file with read and write operation,
it's performance oriented, supports UTF8 path, and can be used with IOFile to work in a asynchronous way */
struct File : virtual Object {
	typedef Event<void(Shared<std::string>& pBuffer, bool end)>	OnReaden;
	typedef Event<void(const Exception&)>						OnError;
	typedef Event<void(bool deletion)>							OnFlush;
	NULLABLE(!_loaded)

	/**
	Decoder offers to decode data in the reception thread when file is used with IOFile,
	If pBuffer is reseted, no onReaden is callen (data captured),
	If returns > 0 it continue reading operation (reads returned size) */
	struct Decoder : virtual Object {
		virtual uint32_t decode(Shared<std::string>& pBuffer, bool end) = 0;
		virtual void onRelease(File& file) {}
	};

	// A mode R+W has no sense at this system level, because there is just one reading/writing shared header (R and W position)
	enum Mode {
		MODE_READ = 0, // 0 value allow to test WRITE with a simple "if"
		MODE_WRITE, // allow to write file or delete file/folder
		MODE_APPEND, // append data to file
		MODE_DELETE // just deletion file/folder permission
	};
	File(const Path& path, Mode mode);
	~File();

	const Mode  mode;

	operator const Path&() const { return _path; }

	// properties
	const std::string&  path() const { return _path; }
	const std::string&	name() const { return _path.name(); }
	const std::string&	baseName() const { return _path.baseName(); }
	const std::string&	extension() const { return _path.extension(); }
	const std::string&  parent() const { return _path.parent(); }
	bool				isAbsolute() const { return _path.isAbsolute(); }

	bool		exists(bool refresh = false) const { return _path.exists(refresh); }
	uint64_t		size(bool refresh = false) const;
	int64_t		lastAccess(bool refresh = false) const { return _path.lastAccess(refresh); }
	int64_t		lastChange(bool refresh = false) const { return _path.lastChange(refresh); }

	bool		loaded() const { return _loaded; }

	uint64_t		readen() const { return _readen; }
	uint64_t		written() const { return _written; }

	uint64_t		queueing() const;

	// File operation
	/**
	Load the file, as expensive as a FileSystem::GetAttributes!
	If reading error => Ex::Permission || Ex::Unfound || Ex::Intern */
	virtual bool		load(Exception& ex); // virtual to allow to detect loading error with IOFile (see HTTPFileSender sample)
	/**
	If reading error returns -1 => Ex::System::File || Ex::Permission */
	int					read(Exception& ex, void* data, uint32_t size);
	/**
	If writing error => Ex::System::File || Ex::Permission */
	bool				write(Exception& ex, const void* data, uint32_t size);
	/**
	If deletion error => Ex::System::File || Ex::Permission
	/!\ One time deleted no more write operation is possible */
	bool				erase(Exception& ex);
	/**
	Create file or folder */
	bool				create(Exception& ex) { return write(ex, NULL, 0); }

	void				reset(uint64_t position = 0);

private:
	Path				_path;
	volatile bool		_loaded;
	std::atomic<uint64_t>	_readen;
	std::atomic<uint64_t>	_written;
#if defined(_WIN32)
	HANDLE				_handle;
#else
	long				_handle;
#endif

	//// Used by IOFile /////////////////////
	Decoder*					_pDecoder;
	bool						_externDecoder;
	OnReaden					_onReaden;
	OnFlush						_onFlush;
	OnError						_onError;

	std::atomic<uint64_t>			_queueing;
	std::atomic<uint32_t>			_flushing;
	uint16_t						_ioTrack;
	uint16_t						_decodingTrack;
	const Handler*				_pHandler; // to diminue size of Action+Handle
	friend struct IOFile;
};


} // namespace Mona
