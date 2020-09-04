#include "../UI/Gui.hpp"
#include "../UI/AllControls.hpp"
#include "../UI/TiledImage.hpp"
#include "../UI/TiledFont.hpp"
#include "../UI/managers/Images.hpp"
#include "../UI/backend/SDL2/SDLBackend.hpp"
#include "../UI/managers/Effects.hpp"

ng::Gui gui;
#include <iostream>
#include <thread>
#include <unistd.h>

//~ #include <opencv2/opencv.hpp>

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
		{"test0"},
		{"test1", {
			{"tt0", {
				{"tt1"},
				{"tt2"}
			}},
			{"tt1", {
				{"tst1"},
				{"tst2"}
			}}
		}},
		{"test2"},
		{"test3"},
		{"test4"}
	});
	cm->SetId("context menu\n");
	gui.AddControl(cm);
	cm->Show(0);
}

int main() {
	int sizeX = 1800;
	int sizeY = 900;

	ng::SDLBackend::SetupSDLWindow("SDL test", 100, 100, sizeX, sizeY);
	std::cout << "size of control: " << sizeof(Control) << "\n";
	
	gui = Gui(sizeX, sizeY);
	ng::SDLBackend::SetSDLBackend(&gui);
	
	try {
		gui.LoadXml("data/gui-test.xml");
	} catch (std::exception e) {
		return 0;
	}
	// gui.LoadXml("data/basic.xml");
	gui.SetTooltipDelay(0.5);
	
	
	gui.AddFunction("clear_canvas", [](Args& args) {
		Canvas *c = gui.Get<Canvas>("canvas");
		c->Clear(0);
	});

	// gui.GetCursor().SetCursorImage("Hand.png", Point(-50,-37));
	// ng::Image* curs_input = ng::Images::GetImage("Hand.png");
	// gui.GetCursor().SetCursorImage( CursorType::textinput, curs_input, Point(-50,-38) );
	
	// ng::Image* curs = new ng::TiledImage( ng::Images::GetImage( "data/AppStarting.png" ), Size(128,128) );
	// curs->SetAnimated(true);
	// curs->SetAnimationSpeed(10);
	// gui.GetCursor().SetCursorImage( CursorType::pointer, curs, Point(-50,-38) );
	
	// gui.HideOSCursor();
	// gui.GetCursor().SetCursorImage( CursorType::textinput, "data/Hand.png", Point(-50,-38) );
	
	// gui.GetCursor().SetCursorImage( CursorType::pointer, "data/Hand.png", Point(-50,-38) );

	gui.SetTooltipDelay(1.0);
	
	// gui.GetCursor().SetCursorImage( curs, Point(0,0) );
	
	gui.AddFunction("changetheme", [&](Args& a) {
		auto *c = (ComboBox*)a.control;
		gui.DisableAllStyles();
		gui.EnableStyleGroup(c->GetText());
		gui.ForEachControl([](Control* c) {
			c->ApplyStyle();
		});
	});
	
	gui.EnableStyleGroup("awesome_looks");
	gui.ForEachControl([](Control* c) {
		c->ApplyStyle();
	});
	

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
	
	/*
	cv::VideoCapture vcap;
	cv::VideoCapture vcap2;
	cv::Mat image(cv::Size(640,480), CV_8UC3);
	cv::Mat image2(cv::Size(640,480), CV_8UC3);
	vcap.set(CV_CAP_PROP_MODE, CV_CAP_MODE_RGB);
	// vcap2.set(CV_CAP_PROP_MODE, CV_CAP_MODE_RGB);
	*/
	
	const std::string videoStreamAddress = "/dev/video0";
	// const std::string videoStreamAddress2 = "udp://224.0.1.2:5004";
	const std::string videoStreamAddress2 = "udp://192.168.1.100:5004";
	// const std::string videoStreamAddress = "/home/nikola/Downloads/Ip Man (2008) - 1080p/Ip.Man.2008.1080p.BluRay.x264.VPPV.mp4";
	// const std::string videoStreamAddress = "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov";
	
	//open the video stream and make sure it's opened

	bool quit =false;
	// Canvas *canv2 = gui.Get<Canvas>("canvas2");
	// Canvas *canv3 = gui.Get<Canvas>("canvas3");
	// Canvas *canv4 = gui.Get<Canvas>("canvas4");
	// auto &cnv2 = canv3->GetLayer(0);
	// auto &cnv3 = canv4->GetLayer(0);
	// cnv2.Resize(640,480);
	
	/*
	std::thread t([&]() {
		if(!vcap2.open(videoStreamAddress2)) {
				std::cout << "Error opening video stream or file" << std::endl;
				return -1;
			}
		while(!quit) {
			
			if(vcap2.read(image2)) {
					for(int y = 0; y < image2.rows; y++) {
						// const uint32_t* p = (uint32_t*)image2.ptr<uint32_t>(y);
						const uint8_t* p = (uint8_t*)image2.ptr<uint32_t>(y);
						for(int x = 0; x < image2.cols; x++) {
							// cnv.Pixel(ng::Point(x,y), Color(image.at<uint8_t>(y,x,0), image.at<uint8_t>(y,x,1), image.at<uint8_t>(y,x,2)).GetUint32());
							cnv2.Pixel(ng::Point(x,y),  Color(p[2+x*3], p[1+x*3], p[0+x*3]).GetUint32());
						}
					}
					cnv2.Refresh();
				}
		}
	});
	t.detach();
	*/
	
	/*
	std::thread t2([&]() {
		 
		// cnv.Resize(641,481);
		 if(!vcap.open(videoStreamAddress)) {
			std::cout << "Error opening video stream or file" << std::endl;
			return -1;
		}

		while(!quit) {
			auto &cnv = canv2->GetLayer(0);
			if(vcap.read(image)) {
			
				// std::cout << "cols: " << image.cols << ", " << image.rows << "\n";
				for(int y = 0; y < image.rows; y++) {
					const uint8_t* p = (uint8_t*)image.ptr<uint32_t>(y);
					for(int x = 0; x < image.cols; x++) {
						// cnv.Pixel(ng::Point(x,y), Color(image.at<uint8_t>(y,x,0), image.at<uint8_t>(y,x,1), image.at<uint8_t>(y,x,2)).GetUint32());
						cnv.Pixel(ng::Point(x,y),  Color(p[2+x*3], 0, 0).GetUint32());
						cnv2.Pixel(ng::Point(x,y),  Color(0, p[1+x*3], 0).GetUint32());
						// cnv3.Pixel(ng::Point(x,y),  Color(0, 0, p[0+x*3]).GetUint32());
						cnv3.Pixel(ng::Point(x,y),  Color(p[2+x*3], p[1+x*3], p[0+x*3]).GetUint32());
					}
				}
				cnv.Refresh();
				cnv2.Refresh();
				cnv3.Refresh();
			}
			
		}
	});
	t2.detach();
	*/
	

	auto tbox = gui.Get<ng::TextBox>("coords");
	gui.OnRender([&]() {
		Point pt = gui.GetCursor().GetPosition();
		tbox->SetText(std::to_string(pt.x) + " " + std::to_string(pt.y));
	});
	
	auto lbfps = gui.Get<ng::Label>("fps");
	gui.OnFpsChange([&](uint32_t fps) {
		static int val = 0;
		if(lbfps) {
			lbfps->SetText("fps: " + std::to_string(fps));
		}
		// std::cout << "fps: " << fps << "\n";
		// gui.Get<ProgressBar>("progress")->SetValue(val++);
	});
	
	
	ng::SDLBackend::SDLMainLoop(&gui);
	ng::SDLBackend::CloseBackend();
	quit = true;
	// std::this_thread::sleep_for(std::chrono::milliseconds(
	usleep(1000);
}
