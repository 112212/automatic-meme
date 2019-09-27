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


option(USE_SDL2_BACKEND "SDL2 Backend" ON)
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

if(USE_SDL2_BACKEND)
	set(SOURCES ${SOURCES} 
	./UI/backends/SDL2/SDL_Surface_Image.cpp
	./UI/backends/SDL2/Backend.cpp
	./UI/backends/SDL2/SDLSpeaker.cpp
	./UI/backends/SDL2/SDLScreen.cpp
	./UI/backends/SDL2/SDLSystem.cpp
	./UI/backends/SDL2/SDLSound.cpp
	./UI/backends/SDL2/RWOpsFromFile.cpp)
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
endif()


configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
 
 
 # add_custom_command(TARGET sdl_test
	# POST_BUILD
	# COMMAND echo "#ifdef NG_DEFAULT_BACKEND_HPP" >> $(build)/config.h
	# COMMAND echo "#include \"../UI/backends/$(backend_use)/Backend.hpp\"" >> $(build)/config.h
	# COMMAND echo "namespace ng {" >> $(build)/config.h
	# COMMAND echo "namespace DefaultBackend = ng::$(backend_use)Backend;" >> $(build)/config.h
	# COMMAND echo "}" >> $(build)/config.h
	# COMMAND echo "#endif" >> $(build)/config.h
	# )

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

