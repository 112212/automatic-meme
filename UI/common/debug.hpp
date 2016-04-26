#ifndef _DEBUG_HPP
#define _DEBUG_HPP



/*
	debug_str
	breakpoint
	breakpoint2
	debug(str)
	info(str)
	flush_dbg()
	flush_info()
	info_lock
	info_unlock
*/
#define DEBUG
#define INFO
#define INFO_OUT_SSTR
//#define DEBUG_OUT_SSTR



	#ifdef DEBUG
		#include <iostream>
		#include <thread>
		#include <chrono>
		
		using std::cout;
		using std::endl;

		#ifndef DEBUG_OUT_SSTR
			#define debug(str) cout << str << endl
			#define flush_dbg
		#else
			#include <sstream>
			extern std::stringstream debug_str;
			#define debug(str) debug_str << str << endl
			#define flush_dbg debug_str.clear();
		#endif

		#define breakpoint std::this_thread::sleep_for(std::chrono::seconds(1));
	#else

		#define debug(str)
		#define breakpoint 
		#define NDEBUG // disable assert
	#endif

	#include <cassert>
	
#endif

#ifdef INFO
	#include <thread>
		
	#define breakpoint2 std::this_thread::sleep_for(std::chrono::seconds(1));
	#ifndef INFO_OUT_SSTR
		#define info(str) cout << str << endl
		#define flush_info()
		#define info_lock
		#define info_unlock
	#else
		#include <thread>
		#include <sstream>
		#include <mutex>
		extern std::mutex info_mutex;
		extern std::stringstream info_str;
		#define info_lock info_mutex.lock()
		#define info_unlock info_mutex.unlock()
		#define info(str) info_lock;info_str << str << endl;info_unlock
		#define flush_info() info_lock;info_str.str("");info_unlock
	#endif
#endif
