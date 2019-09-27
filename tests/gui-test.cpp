#include "../UI/Gui.hpp"
#include "../UI/AllControls.hpp"
#include "../UI/TiledImage.hpp"
#include "../UI/TiledFont.hpp"
#include "../UI/managers/Images.hpp"
#include "../UI/managers/Effects.hpp"
#include "../UI/backends/default/Backend.hpp"

#include <iostream>
#include <thread>
#include <unistd.h>

ng::Gui gui;

using namespace std;
using namespace ng;

void test(Control* test) {
	cout << "clicked\n";
}

GUIFUNC(hehe) {
	cout << args.control->GetId() << endl;
	for(auto &v : args.cmd_args) {
		cout << "argv: " << v << " (" << v.size() << ") " << endl;
	}
	cout << endl;
}


GUIFUNC(position) {
	const Rect& r = args.control->GetRect();
	cout << "POSITION evt control " << args.control->GetId() << " is at " << r.x << ", " << r.y << endl;
	if(!args.control->GetEffect("Move")) {
		std::cout << "no effect\n";
		ng::Effect* ef = new ng::Effects::Move(Point(700,300), 0.5);
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

GUIFUNC(show_context) {
	ng::ContextMenu *cm = new ng::ContextMenu({
		ng::MenuItem("test0"),
		ng::MenuItem("test1", {
			ng::MenuItem("tt0", {
				{"tt1"},
				{"tt2"}
			}),
			ng::MenuItem("tt1", {
				{"tst1"},
				{"tst2"}
			})
		}),
		ng::MenuItem("test2"),
		ng::MenuItem("test3"),
		ng::MenuItem("test4")
	});
	cm->SetId("context menu\n");
	gui.AddControl(cm, false);
	cm->Show(0);
}

MAIN
{  
	int sizeX = 1800;
	int sizeY = 900;
	
	std::cout << "setting up window\n";
	ng::DefaultBackend::SetupWindow("Gui-test", 100, 100, sizeX, sizeY);
	std::cout << "size of control: " << sizeof(Control) << "\n";
	
	std::cout << "setting backend\n";
	gui = Gui(sizeX, sizeY);
	ng::DefaultBackend::SetBackend(&gui);
	
	std::cout << "loading xml\n";
	try {
		gui.LoadXml("data/gui-test.xml");
		// gui.LoadXml("data/basic.xml");
	} catch (std::exception e) {
		std::cout << "Fail to load gui-test.xml" << "\n";
		return 0;
	}
	
	gui.AddFunction("clear_canvas", [](Args& args) {
		Canvas *c = gui.Get<Canvas>("canvas");
		c->Clear(0);
	});
	
	gui.SetTooltipDelay(1.0);
	
	std::cout << "xml loaded\n";
	
	// ---- theme
	gui.AddFunction("changetheme", [&](Args& a) {
		auto *c = (ComboBox*)a.control;
		gui.DisableAllStyles();
		gui.EnableStyleGroup(c->GetText());
		gui.ForEachControl([](Control* c) {
			c->ApplyStyle();
		});
	});
	
	// gui.EnableStyleGroup("white");
	gui.EnableStyleGroup("awesome_looks");
	gui.ForEachControl([](Control* c) {
		c->ApplyStyle();
	});
	// --------

	gui.AddFunction("lbox", [&](Args& args) {
		std::cout << "sel: " << static_cast<ng::ListBox*>(args.control)->GetText() << "\n";
	});
	
	gui.OnEvent("curs", "change", [&](Args& args) {
		int val = static_cast<ng::TrackBar*>(args.control)->GetValue();
		gui.GetCursor().SetSize(val,val);
	});
	
	gui.OnEvent("put_here", "drag", [](Args args) {
		cout << "drag evt\n";
	});
	
	gui.OnEvent("combo", "drag", [&](Args& args) {
		cout << "drag combo\n";
		ng::Control* wgt = gui.GetSelectedControl();
		cout << "drop on: " << wgt->GetId() << "\n";
		if(wgt->GetId() == "put_here" || wgt->GetId() == "or_here") {
			args.control->Unattach();
			wgt->AddControl(args.control);
			args.control->SetDraggable(false);
		}
	});
	
	gui.OnEvent("term", "enter", [](Args& args) {
		Terminal* t = static_cast<Terminal*>(args.control);
		
		t->AppendLog(t->GetLastCommand());
	});
	
	bool quit = false;
	auto tbox = gui.Get<ng::TextBox>("coords");
	
	auto lbfps = gui.Get<ng::Label>("fps");
	gui.OnFpsChange([&](uint32_t fps) {
		static int val = 0;
		if(lbfps) {
			lbfps->SetText("fps: " + std::to_string(fps));
		}
	});
	
	auto graph = gui.Get<ng::Graph>("graph");
	// int s = graph->CreateSignal("hehe");
	// int s1 = graph->CreateSignal("haha", Color::Green);
	// int s2 = graph->CreateSignal("red", Color::Red);
	// graph->AddPoints(s, {{1,2}, {5,2},{20,10}});
	// graph->AddPoints(s1, {{1,2}, {5,2},{20,10}});
	// graph->AddPoints(s2, {{1,2}, {5,2},{20,10}});
	
	double t=25;
	gui.OnRender([&]() {
		Point pt = gui.GetCursor().GetCursor();
		// tbox->SetText(std::to_string(pt.x) + " " + std::to_string(pt.y));
		// graph->AddPoints(s, {{t,1}});
		t+=2;
	});
	
	std::cout << "starting main loop\n";
	ng::DefaultBackend::MainLoop(&gui);
	quit = true;
	usleep(1000);
}
