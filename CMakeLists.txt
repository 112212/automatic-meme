cmake_minimum_required(VERSION 3.10)
project("NG Gui Engine")

set(SOURCES
	./UI/backend/Speaker.cpp
	./UI/backend/Screen.cpp
	./UI/File.cpp
	./UI/Layout.cpp
	./UI/BasicImage.cpp
	./UI/Control.cpp
	./UI/managers/Effects.cpp
	./UI/managers/Images.cpp
	./UI/managers/ControlManager.cpp
	./UI/managers/Fonts.cpp
	./UI/managers/Sounds.cpp
	./UI/managers/ResourceManager.cpp
	./UI/Sound.cpp
	./UI/ColorString.cpp
	./UI/TiledImage.cpp
	./UI/Gui.cpp
	./UI/Font.cpp
	./UI/Image.cpp
	./UI/TiledFont.cpp
	./UI/Cursor.cpp
	./UI/Color.cpp
	./UI/common.cpp
	
	# controls
	./UI/controls/TrackBar.cpp
	./UI/controls/Container.cpp
	./UI/controls/ListBox.cpp
	./UI/controls/TabContainer.cpp
	./UI/controls/ScrollBar.cpp
	./UI/controls/ProgressBar.cpp
	./UI/controls/Label.cpp
	./UI/controls/WidgetMover.cpp
	./UI/controls/TextBox.cpp
	./UI/controls/ComboBox.cpp
	./UI/controls/CheckBox.cpp
	./UI/controls/Graph.cpp
	./UI/controls/Dialog.cpp
	./UI/controls/RadioButton.cpp
	./UI/controls/Canvas.cpp
	./UI/controls/WidgetResizer.cpp
	./UI/controls/Terminal.cpp
	./UI/controls/Button.cpp
	./UI/controls/ContextMenu.cpp
	)


# define backend
set(BACKEND "SDL2" CACHE STRING "Backend")
set_property(CACHE BACKEND PROPERTY STRINGS SDL2 win32 xlib xcb)


option(BACKEND_SDL2 "SDL2 Backend" ON)
option(BACKEND_WIN32 "Win32 Backend" OFF)

option(USE_PTHREAD "Input Mutex Locking" OFF)
option(USE_OPENGL "SDL2 OpenGL" OFF)
option(USE_SDL2_image "SDL2_image library" OFF)
option(USE_SDL2_ttf "SDL2_ttf library" ON)
option(USE_LIBSNDFILE "libsndfile library" OFF)
option(USE_LIBPNG "libpng library" OFF)


add_executable(sdl_test tests/gui-test.cpp)

target_link_libraries(sdl_test SDL2)

if(USE_PTHREAD)
	target_link_libraries(sdl_test pthread)
endif()

if(USE_LIBPNG)
	target_link_libraries(sdl_test png)
	set(SOURCES ${SOURCES} ./UI/backend/addons/Image_libpng.cpp)
endif()

if(USE_LIBSNDFILE)
	target_link_libraries(sdl_test sndfile)
	set(SOURCES ${SOURCES} ./UI/backend/addons/SoundLibSnd.cpp)
endif()

if(BACKEND EQUAL SDL2)
	set(SOURCES ${SOURCES} 
		./UI/backends/SDL2/SDL_Surface_Image.cpp
		./UI/backends/SDL2/Backend.cpp
		./UI/backends/SDL2/SDLSpeaker.cpp
		./UI/backends/SDL2/SDLScreen.cpp
		./UI/backends/SDL2/SDLSystem.cpp
		./UI/backends/SDL2/SDLSound.cpp
		./UI/backends/SDL2/RWOpsFromFile.cpp
	)
	
	if(USE_SDL2_image)
		target_link_libraries(sdl_test SDL2_image)
	endif()
	if(USE_SDL2_ttf)
		set(SOURCES ${SOURCES} ./UI/backends/SDL2/SDL_TTF_Font.cpp)
		target_link_libraries(sdl_test SDL2_ttf)
	endif()
	if(USE_OPENGL)
		set(SOURCES ${SOURCES} ./UI/backends/SDL2/SDLOpenGLScreen.cpp)
		target_link_libraries(sdl_test GL)
		target_link_libraries(sdl_test GLEW)
	endif()

elseif(BACKEND EQUAL win32)

	set(SOURCES ${SOURCES} 
		./UI/backends/win32/Backend.cpp
		./UI/backends/win32/Speaker.cpp
		./UI/backends/win32/Screen.cpp
		./UI/backends/win32/System.cpp
		./UI/backends/win32/Font.cpp
	)
	target_link_libraries(sdl_test msimg32)
	
elseif(BACKEND EQUAL xlib)

	set(SOURCES ${SOURCES} 
		./UI/backends/xlib/SDL_Surface_Image.cpp
		./UI/backends/xlib/Backend.cpp
		./UI/backends/xlib/Speaker.cpp
		./UI/backends/xlib/Screen.cpp
		./UI/backends/xlib/System.cpp
		./UI/backends/xlib/Font.cpp
	)

elseif(BACKEND EQUAL xcb)
	
	set(SOURCES ${SOURCES} 
		./UI/backends/xcb/SDL_Surface_Image.cpp
		./UI/backends/xcb/Backend.cpp
		./UI/backends/xcb/Speaker.cpp
		./UI/backends/xcb/Screen.cpp
		./UI/backends/xcb/System.cpp
		./UI/backends/xcb/Font.cpp
	)
	
endif()


configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )


include_directories("${PROJECT_BINARY_DIR}")

add_library(gui STATIC ${SOURCES})
target_include_directories(gui PRIVATE lib)
include_directories(/usr/include/SDL2)
target_link_libraries(sdl_test gui)



# find_package(OpenCV REQUIRED)
# target_link_libraries( sdl_test ${OpenCV_LIBS} )

# if (OPENCV_FOUND)
  # target_include_directories(sdl_test ${OPENCV_INCLUDE_DIRS})
  # target_link_libraries (sdl_test ${OPENCV_LIBRARIES})
# endif (OPENCV_FOUND)

