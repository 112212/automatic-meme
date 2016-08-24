#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
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
#include "UI/controls/Terminal.hpp"
#include "UI/controls/Form.hpp"

#include "UI/common/SDL/Drawing.hpp"

#include <sstream>
ng::GuiEngine gui;
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
    
    /*
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
	*/
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	int sizeX = 1280;
	int sizeY = 900;
	SDL_Window* win = SDL_CreateWindow("gui sdl test", 100, 100, sizeX, sizeY, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!win) {
		cout << "creating window error\n";
		SDL_Quit();
		return 1;
	}
	
	SDL_GLContext glcontext = SDL_GL_CreateContext(win);
	if (!glcontext) {
		cout << "Couldn't create context: %s" << SDL_GetError() << endl;
		// SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 0;
	}
	
	SDL_GL_MakeCurrent(win, glcontext);
	
	SDL_Renderer* ren = 0;
	
	glewExperimental = GL_TRUE; 
    glewInit();
	
    
	// SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	// if(!ren) {
		// SDL_DestroyWindow(win);
	// }
	

	int stencil;
	SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil);
	cout << "stencil: " << stencil << endl;
	
	// Drawing::Init();
	// Drawing::SetResolution( sizeX, sizeY );
	
	gui = GuiEngine(sizeX, sizeY);
	// gui.SetSize(sizeX, sizeY);
	gui.SetDefaultFont("/usr/share/fonts/TTF/Ubuntu-B.ttf");
	gui.LoadXml("gui-test.xml");
	
	gui.SubscribeEvent("form", Form::event::submit, [](Control* c) {
		cout << "submit\n";
	});
	// gui.SubscribeEvent("5", EVENT_BUTTON_CLICK, [](Control* c){
		// cout << "clicked at 1 \n";
	// });
	// gui.SubscribeEvent("55", EVENT_BUTTON_CLICK, [](Control* c){
		// cout << "clicked at 3 \n";
	// });
	// gui.SubscribeEvent("56", EVENT_BUTTON_CLICK, [](Control* c){
		// cout << "clicked at 4 \n";
	// });
	
	// gui.SubscribeEvent("2", EVENT_BUTTON_CLICK, [](Control* c){
		// cout << "clicked at nikola\n";
		// Button* b = (Button*)c;
		// b->SetText("you clicked at me");
		// b->SetRect(50,50,200, 50);
	// });
	
	// gui.SubscribeEvent("546", EVENT_TRACKBAR_CHANGE, [](Control *c) {
		// cout << "trackbar changed: " << ((TrackBar*)c)->GetValue() << endl;
	// });
	
	// gui.SubscribeEvent("20", EVENT_SCROLLBAR_CHANGE, [](Control* c) {
		// ScrollBar* s = (ScrollBar*)c;
		// cout << "value is: " << s->GetValue() << endl;
	// });
	
	// Drawing::SetResolution( sizeX, sizeY );
	// gui.SetSize(sizeX, sizeY);
	// Drawing::Init();
	
	/*
	TextBox* tb_debug = (TextBox*)gui.GetControlById("debug");
	//tb_debug->SetText("text");
	//tb_debug->SetRect(0, 0, 100, 50);
	
	Canvas* cv_minimap = (Canvas*)gui.GetControlById("minimap");
	// cv_minimap->SetBackgroundColor(0x00000000);
	cv_minimap->SetPixelColor(0x00ff0000);
	cv_minimap->PutPixel(1,1);
	cv_minimap->SetPixelColor(0xff00ff00);
	cv_minimap->PutPixel(2,2);
	*/
	
	// Button* btn = (Button*)ControlManager::CreateControl("button");
	// Anchor a = btn->GetAnchor();
	// a.sx = 300;
	// a.sy = 100;
	// a.isrelative = true;
	// btn->SetAnchor(a);
	// btn->SetText("test create button");
	// btn->SetId("36");
	// gui.AddControl(btn);
	
	// Widget* w1 = (Widget*)gui.GetControlById("w1");
	
	// std::stringstream s("<gui><button rect=\"0,150,100,100\" value=\"hehe\"/></gui>");
	// w1->LoadXml(s);
	
	// Terminal &t = *((Terminal*)gui.GetControlById("term"));
	// t.SubscribeEvent( Terminal::event::command, [](Control* c) {
		// Terminal* t = (Terminal*)c;
		// cout << "command: " << t->GetText() << endl;
	// });
	
	TextBox* tb = (TextBox*)gui.GetControlById("mojtbox");
	// tb->SetWordWrap(true);
	tb->SetTextWrap(true);
	// tb->SetTextWrap(false);
		
	gui.ApplyAnchoring();
	
	SDL_GL_SetSwapInterval(0);
	
	bool running = true;
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_KEYDOWN) {
				if(e.key.keysym.sym == 'q' || 
					e.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				}
			}
			gui.OnEvent(e);
		}
		Drawing::GetResolution(sizeX, sizeY);
		glViewport(0, 0, sizeX, sizeY);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
		gui.Render();
		SDL_GL_SwapWindow(win); 
	}
	
	// SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
