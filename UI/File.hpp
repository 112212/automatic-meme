#ifndef NG_FILE_HPP
#define NG_FILE_HPP

#include <cstddef>
#include <string>

namespace ng {
enum {
	FILE_SEEK_SET = 0,
	FILE_SEEK_CUR = 1,
	FILE_SEEK_END = 2
};
class File {
	private:
	public:
		File();
		std::string path;
		// std::string GetPath() { return path; }
		virtual size_t Read(void* buffer, size_t size);
		virtual size_t Write(const void* buffer, size_t size);
		virtual size_t Seek(size_t offset, int seek_type);
		virtual void Close();
		size_t Tell() { return Seek(0, FILE_SEEK_CUR); }
		size_t Size() {
			size_t cur = Tell();
			size_t size = Seek(0, FILE_SEEK_END);
			Seek(cur, FILE_SEEK_SET);
			return size;
		}
};
}
#endif
