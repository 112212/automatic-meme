
##############################
### default configuration ####
##############################
backend := sdl2
platform := linux
USE_SDL2_image := y
USE_SDL2_ttf := y
USE_OPENGL := n
USE_LIBSNDFILE := n
USE_LIBBMP := y
USE_LIBPNG := y
USE_PTHREAD := n
###############################

# CORE
cpp = \
	UI/Gui.cpp \
	UI/Control.cpp \
	UI/Layout.cpp \
	UI/common.cpp \
	UI/Font.cpp \
	UI/Color.cpp \
	UI/ColorString.cpp \
	UI/Cursor.cpp \
	UI/Image.cpp \
	UI/BasicImage.cpp \
	UI/Sound.cpp \
	UI/File.cpp \
	UI/TiledImage.cpp \
	UI/TiledFont.cpp \
	UI/managers/ControlManager.cpp \
	UI/managers/Fonts.cpp \
	UI/managers/Images.cpp \
	UI/managers/Sounds.cpp \
	UI/managers/Effects.cpp \
	UI/managers/ResourceManager.cpp \
	UI/backend/Speaker.cpp \
	UI/backend/Screen.cpp \

exe := 
link := 
defs := 
build_dirs = UI/ UI/managers UI/backend UI/extensions/ lib/libbmp lib/bmpread


.PHONY: dirs


build := build
flags := -g -Wfatal-errors
inc = -Ilib -I$(build)

CXX := 
CFLAGS := 

# obj = $(addprefix $(build)/, $(patsubst %.cpp,%.o,$(cpp)))

controls_path := UI/controls



# CONTROLS
# all controls which contains only controls with .cpp code
control_names := Button 	    \
				RadioButton 	\
				ComboBox 		\
				CheckBox		\
				Label           \
				ScrollBar       \
				TextBox         \
				ListBox         \
				TrackBar        \
				Container       \
				Dialog			\
				Terminal		\
				Canvas			\
				WidgetMover		\
				WidgetResizer	\
				ProgressBar		\
				TabContainer    \
				Graph    		\
				ContextMenu		\




build_dirs += $(controls_path)
core_cpp := $(cpp) $(addsuffix .cpp, $(addprefix $(controls_path)/, $(control_names)))
cpp += $(core_cpp)
test_cpp :=
s :=

backends_dir := UI/backends
extensions_dir := UI/extensions

ifeq ($(backend_use),win32)
	CXX := i686-w64-mingw32-
	# CXX := x86_64-w64-mingw32-
	build := build_win
endif


############ SDL2 ###############
$(shell mkdir -p build; echo "" > $(build)/config.h)

backend := sdl2
backend_use := $(backend)

ifneq (, $(filter sdl2 emscripten, $(backend_use)))

	ifeq (${backend_use},emscripten)
		CXX := em-
		test := sdl_test.html
		inc += -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_IMAGE=2
		link +=  -s USE_SDL=2 -s USE_SDL_TTF=2 --embed-file data -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s DISABLE_EXCEPTION_CATCHING=0 -s ASSERTIONS=2 -s USE_SDL_IMAGE=2
	else
		
		test := sdl_test
		inc += $(shell /usr/local/bin/sdl2-config --cflags) -I/usr/include/SDL2
		link +=  $(shell /usr/local/bin/sdl2-config --libs)
		
		ifeq (${USE_SDL2_image},y)
			link += -lSDL2_image
			s:=$(shell echo "\#define USE_SDL2_image" >> $(build)/config.h)
		endif
		
		ifeq (${USE_PTHREAD},y)
			link += -pthread
			s:=$(shell echo "\#define USE_PTHREAD" >> $(build)/config.h)
		endif
				
		ifeq (${USE_SDL2_ttf},y)
			link += -lSDL2_ttf
			backend_cpp += $(backend_dir)/SDL_TTF_Font.cpp
			s:=$(shell echo "\#define USE_SDL2_ttf" >> $(build)/config.h)
		endif

		ifeq (${USE_LIBSNDFILE},y)
			link += -lsndfile
			backend_cpp += $(extensions_dir)/SoundLibSnd.cpp
			s:=$(shell echo "\#define USE_LIBSNDFILE" >> $(build)/config.h)
		endif
	endif
	backend_use := sdl2
	
	
	backend_dir=$(backends_dir)/SDL2
	build_dirs += $(backend_dir)
	backend_cpp += \
			$(backend_dir)/SDL_Surface_Image.cpp \
			$(backend_dir)/SDLSound.cpp \
			$(backend_dir)/SDLSpeaker.cpp \
			$(backend_dir)/Backend.cpp \
			$(backend_dir)/SDLSystem.cpp \
			$(backend_dir)/RWOpsFromFile.cpp

	ifeq (${USE_OPENGL},y)
		backend_cpp += $(backend_dir)/SDLOpenGLScreen.cpp
		link += -lGL -lGLEW
		s:=$(shell echo "\#define USE_OPENGL" >> $(build)/config.h)
	else
		backend_cpp += $(backend_dir)/SDLScreen.cpp
	endif


	exe := $(test)
	test_cpp := tests/gui-test.cpp

	#test_cpp := tests/wut.cpp
	cpp += $(backend_cpp)
	backend_use:=SDL2
endif
#################################

###### LIBPNG ######
ifeq (${USE_LIBPNG},y)
	link += -lpng -lz
	cpp +=	$(extensions_dir)/Image_libpng.cpp
	s:=$(shell echo "\#define USE_LIBPNG" >> $(build)/config.h)
endif
#####################

###### LIBBMP ######
ifeq (${USE_LIBBMP},y)
	# cpp += lib/libbmp/libbmp.cpp
	cpp += lib/bmpread/bmpread.cpp
	cpp += $(extensions_dir)/Image_libbmp.cpp
	# link += lib/libbmp/libbmp.o
	s:=$(shell echo "\#define USE_LIBBMP" >> $(build)/config.h)
endif
#####################

########## Xlib #################
ifeq ($(backend_use),xlib)
	backend_dir=$(backends_dir)/xlib
	build_dirs += $(backend_dir)
	backend_cpp += \
			$(backend_dir)/Speaker.cpp \
			$(backend_dir)/Screen.cpp \
			$(backend_dir)/Backend.cpp \
			$(backend_dir)/System.cpp \
			$(backend_dir)/Font.cpp
			
	link += -lX11
	exe := xlib_test
	test_cpp := tests/gui-test.cpp
	cpp += $(backend_cpp) 
endif
###############################

########## XCB #################
ifeq ($(backend_use),xcb)
	backend_dir=$(backends_dir)/xcb
	build_dirs += $(backend_dir)
	backend_cpp += \
			$(backend_dir)/Speaker.cpp \
			$(backend_dir)/Screen.cpp \
			$(backend_dir)/Backend.cpp \
			$(backend_dir)/System.cpp \
			$(backend_dir)/Font.cpp
			
	link += -lxcb -lxcb-image
	exe := xcb_test
	test_cpp := tests/gui-test.cpp
	cpp += $(backend_cpp)
endif
###############################

########## Win32 #################
ifeq ($(backend_use),win32)
	backend_dir=$(backends_dir)/win32
	build_dirs += $(backend_dir)
	backend_cpp := \
			$(backend_dir)/Speaker.cpp \
			$(backend_dir)/Screen.cpp \
			$(backend_dir)/Backend.cpp \
			$(backend_dir)/System.cpp \
			$(backend_dir)/Font.cpp

	link += -mwindows -static -s -lmsimg32
	# link += -mwindows -s
	exe := win32_test.exe
	cpp += $(backend_cpp) tests/win32_test.cpp
endif
###############################

# DefaultBackend
$(shell echo "#ifdef NG_DEFAULT_BACKEND_HPP" >> $(build)/config.h)
$(shell echo "#include \"../UI/backends/$(backend_use)/Backend.hpp\"" >> $(build)/config.h)
$(shell echo "namespace ng {" >> $(build)/config.h)
$(shell echo "namespace DefaultBackend = ng::$(backend_use)Backend;" >> $(build)/config.h)
$(shell echo "}" >> $(build)/config.h)
$(shell echo "#endif" >> $(build)/config.h)

#~ sdl_link += `pkg-config --libs opencv` -pthread


obj := $(addprefix $(build)/,$(patsubst %.cpp,%.o,$(cpp)))

######### TEST ##########
test_obj := $(addprefix $(build)/,$(patsubst %.cpp,%.o,$(test_cpp)))
test: dirs test_exe

build_dirs += tests/
test_exe: $(obj) $(test_obj)
	$(CXX)g++ $^ -o $(exe) $(link) $(CFLAGS)
#########################


########## LIB ##########
libout := libngui.a
lib_make: defs :=
lib_make: $(obj)
	$(CXX)ar r $(libout) $^
lib: dirs lib_make
#########################


clean:
	@echo project cleaned
	@rm -f libgui.a *_test *_test.exe
	@rm -rf build build_win
	
dirs:
	@mkdir -p $(addprefix $(build)/,$(build_dirs))
	
$(build)/%.o: %.cpp
	$(CXX)g++ -c $< -o $@ -std=c++14 $(flags) $(defs) $(inc) $(CFLAGS)


