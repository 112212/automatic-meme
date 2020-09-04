
##############################
### default configuration ####
##############################
backend := sdl2
platform := linux
USE_SDL2_image := n
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


compiler=$(CXX)-g++
linker=$(CXX)-ar
.PHONY: dirs

debug:=false

build := build-$(backend)
flags :=  -Wfatal-errors

ifeq ($(debug),true)
	flags += -g
endif

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


backend := sdl2
backend_use := $(backend)

ifeq ($(backend_use),win32)
	CXX := i686-w64-mingw32-
	# CXX := x86_64-w64-mingw32-
endif

############ SDL2 ###############
$(shell mkdir -p $(build); echo "" > $(build)/config.h)



ifneq (, $(filter sdl2 emscripten, $(backend_use)))

	#### EMSCRIPTEN
	ifeq (${backend_use},emscripten)
		compiler:=em++
		linker:=emar
		test := sdl_test.html
# 		-fsanitize=address  -fno-sanitize-address-use-after-scope
		inc += -s USE_SDL=2 -s USE_LIBPNG -g4
		
		link +=  -s USE_SDL=2  --preload-file data \
			-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 \
			-s DISABLE_EXCEPTION_CATCHING=0 \
			-s ASSERTIONS=2  \
			-g4  \
			-s INITIAL_MEMORY=130MB \
			-s ALLOW_MEMORY_GROWTH -s USE_LIBPNG \
			-s DISABLE_EXCEPTION_THROWING=0 \
			-s WASM=1

		#### SDL2 TTF
		ifeq (${USE_SDL2_ttf},y)
			link += -s USE_SDL_TTF=2
			inc += -s USE_SDL_TTF=2 
			backend_cpp += $(backend_dir)/SDL_TTF_Font.cpp
			s:=$(shell echo "#define USE_SDL2_ttf" >> $(build)/config.h)
		endif
		
		#### SDL2 IMAGE
		ifeq (${USE_SDL2_image},y)
			inc += -s USE_SDL_IMAGE=2
			link += -s USE_SDL_IMAGE=2
			s:=$(shell echo "#define USE_SDL2_image" >> $(build)/config.h)
		endif
		
		#### PTHREAD
		ifeq (${USE_PTHREAD},y)
			link += -pthread
			s:=$(shell echo "#define USE_PTHREAD" >> $(build)/config.h)
		endif
		
		###### LIBPNG ######
		ifeq (${USE_LIBPNG},y)
			link += -s USE_LIBPNG
			inc += -s USE_LIBPNG
# 			cpp +=	$(extensions_dir)/Image_libpng.cpp
# 			s:=$(shell echo "#define USE_LIBPNG" >> $(build)/config.h)
		endif
		#####################
		
	else
		
		test := sdl_test
		inc += $(shell /usr/bin/sdl2-config --cflags)
		link +=  $(shell /usr/bin/sdl2-config --libs)
		
		#### SDL2 IMAGE
		ifeq (${USE_SDL2_image},y)
			link += -lSDL2_image
			s:=$(shell echo "#define USE_SDL2_image" >> $(build)/config.h)
		endif
		
		#### PTHREAD
		ifeq (${USE_PTHREAD},y)
			link += -pthread
			s:=$(shell echo "#define USE_PTHREAD" >> $(build)/config.h)
		endif
		
		#### SDL2 TTF
		ifeq (${USE_SDL2_ttf},y)
			link += -lSDL2_ttf
			backend_cpp += $(backend_dir)/SDL_TTF_Font.cpp
			s:=$(shell echo "#define USE_SDL2_ttf" >> $(build)/config.h)
		endif
		
		#### LIBSNDFILE
		ifeq (${USE_LIBSNDFILE},y)
			link += -lsndfile
			backend_cpp += $(extensions_dir)/SoundLibSnd.cpp
			s:=$(shell echo "#define USE_LIBSNDFILE" >> $(build)/config.h)
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

	#### OPENGL
	ifeq (${USE_OPENGL},y)
		backend_cpp += $(backend_dir)/SDLOpenGLScreen.cpp
		link += -lGL -lGLEW
		s:=$(shell echo "#define USE_OPENGL" >> $(build)/config.h)
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
	s:=$(shell echo "#define USE_LIBPNG" >> $(build)/config.h)
endif
#####################

###### LIBBMP ######
ifeq (${USE_LIBBMP},y)
	# cpp += lib/libbmp/libbmp.cpp
	cpp += lib/bmpread/bmpread.cpp
	cpp += $(extensions_dir)/Image_libbmp.cpp
	# link += lib/libbmp/libbmp.o
	s:=$(shell echo "#define USE_LIBBMP" >> $(build)/config.h)
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
	cpp += $(backend_cpp) tests/gui-test.cpp
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
DEP = $(obj:%.o=%.d)
-include $(DEP)

######### TEST ##########
test_obj := $(addprefix $(build)/,$(patsubst %.cpp,%.o,$(test_cpp)))
test: dirs test_exe

build_dirs += tests/
test_exe: $(obj) $(test_obj)
	$(compiler) $^ -o $(exe) $(link) $(CFLAGS)
#########################


########## LIB ##########
libout := libngui-$(backend).a
lib_make: defs :=
lib_make: $(obj)
	$(linker) r $(libout) $^
lib: dirs lib_make
#########################

link_flags:
	@echo ${link}

all:
	make lib
	make


clean:
	@echo project cleaned
	@rm -f libngui-$(backend).a *_test *_test.exe
	@rm -rf build-$(backend)
	
dirs:
	@mkdir -p $(addprefix $(build)/,$(build_dirs))
	

$(build)/%.o: %.cpp
	$(compiler) -c $< -MMD -o $@ -std=c++14 $(flags) $(defs) $(inc) $(CFLAGS)


