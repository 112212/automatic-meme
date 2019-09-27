#ifndef NG_FILE_IFSTREAM
#define NG_FILE_IFSTREAM
#include "../File.hpp"
#include <fstream>
#include <iostream>
namespace ng {
class FileIFStream : public File {
	std::ifstream* f;
	
	public:
	FileIFStream(std::ifstream* f) {
		this->f = f;
	}
	
	size_t Read(void* buffer, size_t size) {
		f->read((char*)buffer,size);
		// std::cout << "read ok: " << size<< "\n";
		return size;
	}
	size_t Write(const void* buffer, size_t size) {
		// f->write((char*)buffer,size);
		return 0;
	}
	size_t Seek(size_t offset, int seek_type) {
		std::ios_base::seekdir sdir = std::ios_base::beg;
		if(seek_type == FILE_SEEK_CUR) {
			sdir = std::ios_base::cur;
		} else if(seek_type == FILE_SEEK_END) {
			sdir = std::ios_base::end;
		}
		f->seekg(offset, sdir);
		return f->tellg();
	}
	
	void Close() {
		delete f;
		delete this;
	}
};
}

#endif
