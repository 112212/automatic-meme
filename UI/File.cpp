#include "File.hpp"

namespace ng {
	File::File() {}
	
	size_t File::Read(void* buffer, size_t size) {return 0;}
	size_t File::Write(const void* buffer, size_t size) {return 0;}
	size_t File::Seek(size_t offset, int seek_type) {return 0;}
	void File::Close() {}
}
