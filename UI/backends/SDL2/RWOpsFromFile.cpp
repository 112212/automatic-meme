#include "../../File.hpp"
#include <SDL.h>
namespace ng {
static Sint64 mysizefunc(SDL_RWops * rw)
{
  File* f = (File*)rw->hidden.unknown.data1;
  return f->Size();;
}

static Sint64 myseekfunc(SDL_RWops *rw, Sint64 offset, int whence)
{
  File* f = (File*)rw->hidden.unknown.data1;
  return f->Seek(offset, whence);
}

static size_t myreadfunc(SDL_RWops *rw, void *ptr, size_t size, size_t maxnum)
{
  // SDL_memset(ptr,0,size*maxnum);
  File* f = (File*)rw->hidden.unknown.data1;
  return f->Read(ptr, size*maxnum) / size;
}

static size_t mywritefunc(SDL_RWops *rw, const void *ptr, size_t size, size_t num)
{
  // return num;
  File* f = (File*)rw->hidden.unknown.data1;
  return f->Write(ptr, size*num) / size;
}

static int myclosefunc(SDL_RWops *rw)
{
  // if(context->type != 0xdeadbeef)
  // {
    // return SDL_SetError("Wrong kind of SDL_RWops for myclosefunc()");
  // }

  // free(rw->hidden.unknown.data1);
  // SDL_FreeRW(context);
  File* f = (File*)rw->hidden.unknown.data1;
  f->Close();
  SDL_FreeRW(rw);
  return 0;
}

SDL_RWops* GetRWOps(File* file) {
	SDL_RWops* c = SDL_AllocRW();
	c->size =mysizefunc;
	c->seek =myseekfunc;
	c->read =myreadfunc;
	c->write=mywritefunc;
	c->close=myclosefunc;
	c->type = 0xdeadbeef;
	c->hidden.unknown.data1=file;
	return c;
}
}
