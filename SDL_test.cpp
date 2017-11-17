#include "UI/Gui.hpp"
#include "UI/AllControls.hpp"
#include "UI/TiledImage.hpp"
#include "UI/TiledFont.hpp"
#include "UI/managers/Images.hpp"
#include "UI/backend/SDL/SDLBackend.hpp"
#include "UI/managers/Effects.hpp"

ng::Gui gui;
#include <iostream>

using namespace std;
using namespace ng;

void test(Control* test) {
	cout << "clicked\n";
}

GUIFUNC(hehe) {
	cout << args.control->GetId() << endl;
	for(auto &v : args.cmd_args) {
		cout << "argv: " << v << "( " << v.size() << ") " << endl;
	}
	cout << endl;
}


GUIFUNC(position) {
	const Rect& r = args.control->GetRect();
	cout << "control " << args.control->GetId() << " is at " << r.x << ", " << r.y << endl;
	if(!args.control->GetEffect("Move")) {
		std::cout << "no effect\n";
		ng::Effect* ef = new ng::Effects::Move(Point(std::stoi(args.cmd_args[0]),std::stoi(args.cmd_args[1])), 0.4);
		ng::Effect* ef2 = new ng::Effects::Resize(Size(100,100), 0.5);
		args.control->AddEffect(ef);
		args.control->AddEffect(ef2);
	} else {
		std::cout << "have effect\n";
	}
}

GUIFUNC(say) {
	if(args.cmd_args.empty()) return;
	string l;
	for(auto s : args.cmd_args) {
		l += s + " ";
	}
	cout << "control " << args.control->GetId() << " says: " << l << endl;
}

/*
class MyEffect : public Effect {
	public:
		MyEffect() : Effect("MyEffect") {}
		void PreRender() {
			// Drawing().SetMaxAlpha(0.5);
			cout << name << " pre rendering control: " << control->GetId() << "\n";
		}
		void PostRender() {
			cout << name << " post rendering control: " << control->GetId() << "\n";
		}
};
*/

int main() {
	int sizeX = 1280;
	int sizeY = 900;

	ng::SDLBackend::SetupSDLWindow("SDL test", 100, 100, sizeX, sizeY);
	
	
	gui = Gui(sizeX, sizeY);
	gui.SetDefaultFont("/usr/share/fonts/TTF/Ubuntu-B.ttf", 10);
	// ng::Fonts::PutFont("hehe1", new ng::TiledFont( ng::Images::GetImage("lazyfont.png"), Size(39,55)), 13 );
	ng::Fonts::PutFont("hehe1", new ng::TiledFont( ng::Images::GetImage("data/lazyfont.png"), Size(39,55), 0x00ffff), 10 );
	
	gui.LoadXml("data/gui-test.xml");
	// gui.LoadXml("basic.xml");
	gui.SetTooltipDelay(0.5);
	ng::SDLBackend::SetSDLBackend(&gui);
	
	gui.AddFunction("clear_canvas", [](Args& args) {
		Canvas *c = (Canvas*)gui.GetControlById("canvas2");
		c->Clear(0);
	});
	
	
	
	// gui.GetCursor().SetCursorImage("Hand.png", Point(-50,-37));
	ng::Image* curs = new ng::TiledImage( ng::Images::GetImage( "data/AppStarting.png" ), Size(128,128) );
	gui.GetCursor().SetCursorImage( CursorType::pointer, curs, Point(-50,-38) );
	// ng::Image* curs_input = ng::Images::GetImage("Hand.png");
	// gui.GetCursor().SetCursorImage( CursorType::textinput, curs_input, Point(-50,-38) );
	
	gui.HideOSCursor();
	gui.GetCursor().SetCursorImage( CursorType::textinput, "data/Hand.png", Point(-50,-38) );
	curs->SetAnimated(true);
	curs->SetAnimationSpeed(10);

	gui.SetTooltipDelay(1.0);
	// gui.GetCursor().SetCursorImage( curs, Point(0,0) );
	
	
	
	
	// gui.GetCursor().SetSize(400,400);
	
	// curs->SetTile(13);
	
	
	
	gui.OnEvent("curs", "change", [&](Args& args) {
		int val = static_cast<ng::TrackBar*>(args.control)->GetValue();
		gui.GetCursor().SetSize(val,val);
	});
	
	gui.OnEvent("put_here", "drag", [](Args args) {
		cout << "drag evt\n";
	});
	
	gui.OnEvent("combo", "drag", [&](Args& args) {
		cout << "drag combo\n";
		ng::Widget* wgt = gui.GetSelectedWidget();
		cout << "drop on: " << wgt->GetId() << "\n";
		if(wgt->GetId() == "put_here" || wgt->GetId() == "or_here") {
			args.control->Unattach();
			wgt->AddControl(args.control);
			args.control->SetDraggable(false);
		}
	});
	
	auto lbfps = gui.Get<ng::Label>("fps");
	gui.OnFpsChange([&](uint32_t fps) {
		lbfps->SetText("fps: " + std::to_string(fps));
	});
	
	// gui.Get<ng::ComboBox>("combo")->AddEffect(new ng::Effects::AutoFade());
	
	gui.OnEvent("term", "enter", [](Args& args) {
		Terminal* t = static_cast<Terminal*>(args.control);
		
		t->AppendLog(t->GetLastCommand());
	});
	
	
	ng::SDLBackend::SDLMainLoop(&gui);
	ng::SDLBackend::CloseBackend();
}
