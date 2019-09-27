# simple-gui-engine

# Features:
* Every control can contain other controls (every control is widget)
* Easily extendable and hackable
* Control each control have common basic styles that can be applied
 * color, backgroundcolor, backgroundimage, bordercolor, hoverbordercolor, hoverbackgroundcolor,
   tooltip, hoverimage
* XML layouting, styling and basic scripting that executes on events
* Basic scripting support in XML
* relative layouting
* Tiled Image (animation)
* Tooltip
* UTF-8 encoding
* Dragging
* Effects (which can be combined)
 >  Fade In/Out, Move, Resize, AutoFade
* Animated image (so far implemented are animated pngs, or tiled animation, but can be extended)
* Color input for label, terminal and textbox
> ^rwill be red^gwill be green
* Styling
* Playing sound
* Easy to use event callbacks


## Abstraction

Abstractions allow possibility for easily porting this library for use
in any environment. All of the abstractions are optional. So for example it
can work without user input, so to draw controls on the screen without
any user interaction.

* Screen Drawing Abstraction
* Input Abstraction
* FileSystem Abstraction
* Image Abstraction
* Sound Abstraction
* Font Abstraction
* Speaker Abstraction
* Possible to register file handlers  
> for example handler for .jpg images, for .png images, animated .png,
> animated .gif. Or maybe mp3 sound handler, ogg sound handler,
> font rendering .ttf, font rendering from .bmp, and so on ...

# Controls:
* Button
* Canvas
* CheckBox
* ComboBox
* Container
* Dialog
* Form
* Graph
* Label
* ListBox
* ProgressBar
* RadioButton
* ScrollBar
* TabContainer
* Terminal
* TextBox (also multiline and textwrapping support)
* TrackBar
* WidgetMover
* WidgetResizer  


# Backends:

Its possible to choose which backend to use with cmake or given makefile.

* SDL2 + OpenGL
* Pure SDL2
* X11
* win32
