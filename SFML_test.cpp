#include <SFML/Graphics.hpp>
#include <iostream>

#include "UI/Gui.hpp"
#include "UI/Widget.hpp"
#include "UI/controls/Button.hpp"

#include "UI/controls/RadioButton.hpp"
#include "UI/controls/Label.hpp"
#include "UI/controls/ScrollBar.hpp"
#include "UI/controls/SFML/TextBox.hpp"
#include "UI/controls/SFML/ListBox.hpp"
#include "UI/controls/TrackBar.hpp"
#include "UI/controls/ComboBox.hpp"
#include "UI/controls/GridContainer.hpp"
#include "UI/controls/Container.hpp"
#include "UI/controls/TabContainer.hpp"
#include "UI/controls/Dialog.hpp"

usi namespace std;

int main() {
	sf::ContextSettis settis(8, 8, 0, 2, 1);
	sf::RenderWindow window;
	window.create(sf::VideoMode(800, 600), "My window", sf::Style::Default, settis);
	
	
	sf::Font fnt;
	if( !fnt.loadFromFile("/usr/share/fonts/TTF/DroidSans.ttf") ) {
		cout << "error" << endl;
	}
	sf::Text t1;
	t1.setPosition(5.f, 5.f);
	t1.setCharacterSize(12u);
	t1.setStri(" ghfghgf ");
	t1.setFont( fnt );
	
	window.setFramerateLimit(100);
	
	GuiEngine gui;
	Button* button = new Button();
	button->SetText(" hehe ");
	button->SetRect( 50, 50, 100, 50 );
	gui.AddControl( button );
	
	RadioButton* radioButton = new RadioButton;
	radioButton->SetText("test");
	radioButton->SetRect( 50, 150, 100, 50 );
	radioButton->SetGroup( 1 );
	gui.AddControl( radioButton );
	
	RadioButton* radioButton1 = new RadioButton;
	radioButton1->SetText("test");
	radioButton1->SetRect( 50, 220, 100, 50 );
	radioButton1->SetGroup( 1 );
	gui.AddControl( radioButton1 );
	
	RadioButton* radioButton2 = new RadioButton;
	radioButton2->SetText("test");
	radioButton2->SetRect( 50, 290, 100, 50 );
	radioButton2->SetGroup( 1 );
	gui.AddControl( radioButton2 );
	
	
	Label* lbl = new Label;
	lbl->SetRect( 50, 350, 100, 50 );
	lbl->SetFont( "default", 13 );
	lbl->SetText("label test\nlo");
	gui.AddControl( lbl );
	
	ScrollBar* sb = new ScrollBar;
	sb->SetRect( 200, 100, 300, 50 );
	sb->SetVertical( false );
	gui.AddControl( sb );
	
	TextBox* tb = new TextBox();
	tb->SetRect( 200, 200, 150, 60 );
	tb->SetText(" hello ");
	gui.AddControl( tb );
	
	TrackBar* tbar = new TrackBar();
	tbar->SetRect( 200, 290, 150, 60 );
	tbar->SetValue( 0 );
	gui.AddControl( tbar );
	
	Combobox* cbox = new Combobox();
	cbox->SetRect( 200, 370, 150, 30 );
	cbox->SetTextEditableMode( true );
	cbox->AddItem( "gfghh" );
	cbox->AddItem( "jhjhgjhg" );
	gui.AddControl( cbox );
	
	Combobox* cbox1 = new Combobox();
	cbox1->SetRect( 200, 370, 150, 30 );
	//~ 1cbox->SetTextEditableMode( false );
	cbox1->AddItem( "hghg" );
	cbox1->AddItem( "jhjhg" );
	//gui.AddControl( cbox );
	
	Combobox* cbox2 = new Combobox();
	cbox2->SetRect( 200, 370, 150, 30 );
	//~ cbox1->SetTextEditableMode( false );
	cbox2->AddItem( "hehe" );
	cbox2->AddItem( "haha" );
	//gui.AddControl( cbox1 );
	
	Combobox* cbox3 = new Combobox();
	cbox3->SetRect( 200, 370, 150, 30 );
	cbox3->SetTextEditableMode( true );
	cbox3->AddItem( "tyyrt" );
	cbox3->AddItem( "jhgjgh" );
	cbox3->AddItem( "jhgjghhjg" );
	cbox3->AddItem( "jhgjgh5656" );
	cbox3->AddItem( "tyyrt" );
	cbox3->AddItem( "jhgjgh" );
	cbox3->AddItem( "jhgjghhjg" );
	cbox3->AddItem( "jhgjgh5656" );
	//gui.AddControl( cbox2 );
	
	Listbox* slb = new Listbox();
	slb->SetRect( 600, 100, 150, 100 );
	slb->AddItem( "rewtrtre" );
	slb->AddItem("ytytr");
	gui.AddControl(slb);
	
	
	GridContainer* sgc = new GridContainer();
	sgc->SetRect( 600, 250, 200, 150 );
	sgc->SetGrid( 2, 3 );
	sgc->AddItem( cbox1 );
	sgc->AddItem( cbox2 );
	sgc->AddItem( cbox3 );
	gui.AddControl( sgc );
	
	// ----- testi stencil container --------
	Combobox* cbox12 = new Combobox();
	cbox12->SetRect( 0, 0, 150, 30 );
	//~ 1cbox->SetTextEditableMode( false );
	cbox12->AddItem( "test" );
	cbox12->AddItem( "gfgfdgfd" );
	gui.AddControl( cbox );
	
	Combobox* cbox22 = new Combobox();
	cbox22->SetRect( 0, 40, 150, 30 );
	//~ cbox1->SetTextEditableMode( false );
	cbox22->AddItem( "hehe" );
	cbox22->AddItem( "haha" );
	//gui.AddControl( cbox1 );
	
	Combobox* cbox32 = new Combobox();
	cbox32->SetRect( 0, 80, 150, 30 );
	cbox32->SetTextEditableMode( true );
	cbox32->AddItem( "tyyrt" );
	cbox32->AddItem( "jhgjgh" );
	cbox32->AddItem( "jhgjghhjg" );
	cbox32->AddItem( "jhgjgh5656" );
	cbox32->AddItem( "tyyrt" );
	cbox32->AddItem( "jhgjgh" );
	cbox32->AddItem( "jhgjghhjg" );
	cbox32->AddItem( "jhgjgh5656" );
	
	Combobox* cbox33 = new Combobox();
	cbox33->SetRect( 0, 150, 150, 30 );
	cbox33->SetTextEditableMode( true );
	cbox33->AddItem( "tyyrt" );
	cbox33->AddItem( "jhgjgh" );
	cbox33->AddItem( "jhgjghhjg" );
	cbox33->AddItem( "jhgjgh5656" );
	cbox33->AddItem( "tyyrt" );
	cbox33->AddItem( "jhgjgh" );
	cbox33->AddItem( "jhgjghhjg" );
	cbox33->AddItem( "jhgjgh5656" );
	
	Container* sc2 = new Container();
	sc2->SetId(10);
	sc2->SetRect( 375, 250, 100, 150 );
	//~ sc2->SetGrid( 2, 3 );
	sc2->AddItem( cbox12 );
	sc2->AddItem( cbox22 );
	sc2->AddItem( cbox32 );
	sc2->AddItem( cbox33 );
	gui.AddControl( sc2 );
	
	TabContainer* st = new TabContainer();
	st->SetRect( 0,0,220,40 );
	st->AddTab("hehehe");
	st->AddTab("trolollo");
	st->AddTab("hey :D");
	st->AddTab("543534");
	st->SetZIndex(1);
	// gui.AddControl( st );
	
	
	TabContainer* st1 = new TabContainer();
	st1->SetRect( 30,40,220,40 );
	st1->AddTab("hehehe");
	st1->AddTab("trolollo");
	st1->AddTab("hey :D");
	st1->AddTab("trolollo");
	
	Widget* widget = new Widget();
	
	widget->SetRect( 100, 450, 300, 100 );
	// gui.AddControl(widget);
	widget->SetId(5);
	Dialog* widget1 = new Dialog();
	widget1->SetId(6);
	widget1->AddControl(st);
	widget1->AddControl(st1);
	widget1->SetRect( 10, 0, 300, 100 );
	widget->AddControl(widget1);
	gui.AddControl(widget);
	widget->LockWidget(true);
	widget->LockWidget(false);
	// -------------------------------------------
	
	//+
	sf::View guiView = sf::View(window.getDefaultView());
		
	// run the program as lo as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
			if(event.type == sf::Event::Closed) {
				window.close();
			} else if(event.type == sf::Event::KeyPressed) {
				if( event.key.code == sf::Keyboard::Key::Escape or
					event.key.code == sf::Keyboard::Key::Q)
					window.close();
			}
			gui.OnEvent(event);
        }
        //+
        window.setView(guiView);
       
		window.clear(sf::Color::Black);
        //window.draw(t1);
        window.draw(gui);
        window.display();
    }
    
	return 0;
}
