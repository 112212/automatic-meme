

build := build

cpp = \
	UI/Gui.cpp \
	UI/Control.cpp \
	UI/Widget.cpp \
	UI/common/cache.cpp \
	\
	UI/common/debug.cpp \
	UI/common/common.cpp \
	UI/common/Colors.cpp \
	UI/common/Fonts.cpp \
	\
	UI/XmlLoader.cpp \



exe := 
link := 
defs := 

.PHONY: dirs, sfml_test, sdl_test

build := build

flags := -g -O2 -Wfatal-errors
inc := -Ilib


obj := $(addprefix $(build)/, $(patsubst %.cpp,%.o,$(cpp)))

sfml_test_cpp := $(cpp) SFML_test.cpp \
		UI/common/SFML/Drawing.cpp \
		UI/controls/Button.cpp \
		UI/controls/RadioButton.cpp \
		UI/controls/ComboBox.cpp \
		UI/controls/Label.cpp \
		UI/controls/ScrollBar.cpp \
		UI/controls/SFML/TextBox.cpp \
		UI/controls/SFML/ListBox.cpp \
		UI/controls/TrackBar.cpp \
		UI/controls/GridContainer.cpp \
		UI/controls/Container.cpp \
		UI/controls/TabContainer.cpp \
		UI/controls/Dialog.cpp
		
sfml_test_obj := $(addprefix $(build)/, $(patsubst %.cpp,%.o,$(sfml_test_cpp)))
sfml_link := -lsfml-window -lsfml-graphics -lsfml-system -lGL
sfml_defs := -DUSE_SFML
sfml_test_exe := sfml_test

sfml_test_build: defs := -DUSE_SFML
sfml_test_build: $(sfml_test_obj)
	g++ $^ -o $@ $(sfml_link)
	
sfml_test: dirs sfml_test_build

sdl_test_cpp := $(cpp) SDL_test.cpp \
		UI/controls/Button.cpp \
		UI/controls/ScrollBar.cpp \
		UI/controls/Container.cpp \
		UI/controls/ComboBox.cpp \
		UI/controls/GridContainer.cpp \
		UI/controls/SDL/TextBox.cpp \
		UI/controls/SDL/RadioButton.cpp \
		UI/controls/SDL/ListBox.cpp \
		UI/controls/SDL/Label.cpp \
		UI/controls/SDL/TrackBar.cpp \
		UI/controls/SDL/Canvas.cpp \
		UI/controls/SDL/CheckBox.cpp \
		UI/controls/WidgetMover.cpp \
		UI/common/SDL/Drawing.cpp \
		
sdl_test_obj := $(addprefix $(build)/, $(patsubst %.cpp,%.o,$(sdl_test_cpp)))
sdl_link := -lSDL2 -lSDL2_ttf -lSDL2_gfx -lGL -lGLEW
sdl_test_exe := sdl_test

sdl_test_build: defs := -DUSE_SDL
sdl_test_build: 
sdl_test_build: $(sdl_test_obj)
	g++ $^ -o sdl_test $(sdl_link)

sdl_test: dirs sdl_test_build
	
all: dirs
	@echo choose target 'sfml_test' or 'sdl_test'

sdl_lib_make: defs := -DUSE_SDL
sdl_lib_make: $(sdl_test_obj)
	ar r libgui.a $^

sdl_lib: dirs sdl_lib_make

clean:
	@echo project cleaned
	@rm -f libgui.a
	@rm -f sdl_test
	@rm -f sfml_test
	@rm -rf build
	
dirs:
	@mkdir -p $(build)
	@mkdir -p $(build)/UI
	@mkdir -p $(build)/UI/common
	@mkdir -p $(build)/UI/common/SDL
	@mkdir -p $(build)/UI/common/SFML
	@mkdir -p $(build)/UI/controls
	@mkdir -p $(build)/UI/controls/SDL
	@mkdir -p $(build)/UI/controls/SFML
	
$(build)/%.o: %.cpp
	g++ -c $< -o $@ -std=c++11 $(flags) $(defs) $(inc)


