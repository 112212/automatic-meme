#include <SDL2/SDL.h>
// #include <SDL2/SDL_opel.h>
#include "UI/Gui.hpp"
#include "UI/Control.hpp"
#include "UI/controls/Button.hpp"
#include "UI/controls/ScrollBar.hpp"
#include "UI/controls/Container.hpp"
#include "UI/controls/ComboBox.hpp"
#include "UI/controls/GridContainer.hpp"
#include "UI/controls/TextBox.hpp"
#include "UI/controls/RadioButton.hpp"
#include "UI/controls/ListBox.hpp"
#include "UI/controls/Label.hpp"
#include "UI/controls/TrackBar.hpp"
#include "UI/controls/Canvas.hpp"
#include "UI/controls/CheckBox.hpp"
#include "UI/controls/WidgetMover.hpp"
#include "UI/XmlLoader.hpp"

#include <iostream>
using namespace std;
using namespace ng;
void test(Control* test) {
	cout << "clicked\n";
}

int main() {
	
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::string("Failed to initialize SDL: ") + SDL_GetError();
    }
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
	SDL_Window* win = SDL_CreateWindow("win name", 100, 100, 1280, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if(!win) {
		cout << "win error\n";
		SDL_Quit();
		return 1;
	}
	
	// SDL_GLContext glcontext = SDL_GL_CreateContext(win);
	// if (!glcontext) {
		// cout << "Couldn't create context: %s" << SDL_GetError() << endl;
		// SDL_DestroyRenderer(ren);
		// SDL_DestroyWindow(win);
		// SDL_Quit();
		// return 0;
	// }
	
	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if(!ren) {
		SDL_DestroyWindow(win);
	}
	

	int stencil;
	SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil);
	cout << "stencil: " << stencil << endl;
	
	
	GuiEngine gui;
	
	XmlLoader::LoadXml(gui, "gui-test.xml");
	
	gui.SubscribeEvent(5, EVENT_BUTTON_CLICK, [](Control* c){
		cout << "clicked at 1 \n";
	});
	gui.SubscribeEvent(55, EVENT_BUTTON_CLICK, [](Control* c){
		cout << "clicked at 3 \n";
	});
	gui.SubscribeEvent(56, EVENT_BUTTON_CLICK, [](Control* c){
		cout << "clicked at 4 \n";
	});
	
	gui.SubscribeEvent(2, EVENT_BUTTON_CLICK, [](Control* c){
		cout << "clicked at nikola\n";
		Button* b = (Button*)c;
		b->SetText("you clicked at me");
		b->SetRect(50,50,200, 50);
	});
	
	gui.SubscribeEvent(546, EVENT_TRACKBAR_CHANGE, [](Control *c) {
		cout << "trackbar chaed: " << ((TrackBar*)c)->GetValue() << endl;
	});
	
	gui.SubscribeEvent(20, EVENT_SCROLLBAR_CHANGE, [](Control* c) {
		ScrollBar* s = (ScrollBar*)c;
		cout << "value is: " << s->GetValue() << endl;
	});
	

	SDL_GL_SetSwapInterval(1);
	
	bool runni = true;
	while(runni) {
		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_KEYDOWN) {
				if(e.key.keysym.sym == 'q' || 
					e.key.keysym.sym == SDLK_ESCAPE) {
					runni = false;
				}
			}
			gui.OnEvent(e);
		}
		gui.Render(ren);
		SDL_RenderPresent(ren);
		 SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
	}
	
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
