#include "debug.hpp"
#if defined(DEBUG) and defined(DEBUG_OUT_SSTR)
	std::stringstream debug_str;
#endif
#if defined(INFO) and defined(INFO_OUT_SSTR)
	std::stringstream info_str;
	std::mutex info_mutex;
#endif
