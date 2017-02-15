#ifndef NG_CANVAS_HPP
#define NG_CANVAS_HPP

#include "../../Control.hpp"
#include "../../common/Texture.hpp"
namespace ng {
#define MAX_LAYERS 4
class Canvas : public Control {
	private:
		// SDL_Surface* m_drawing;
		Texture layers[MAX_LAYERS];
		// Uint32 tex_drawing;
		bool m_is_mouseDown;
		bool m_is_readonly;
		void onPositionChange();
		int pixel_size;
		
		int pixel_color;
		int grid_color;
		int last_x, last_y;
		bool align_to_grid;
		bool display_grid;
		bool maketex;
		
		void Render( Point pos, bool isSelected );
		void OnMouseDown( int mX, int mY, MouseButton which_button );
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseUp( int mX, int mY, MouseButton button );
		void OnLostFocus();
		void OnSetStyle(std::string& style, std::string& value);
	public:
		Canvas();
		~Canvas();
		
		Point point;
		Canvas* Clone();
		
		void RefreshTexture();
		void PutPixel(int x, int y);
		void SetPixelSize(int size);
		
		void SetBackgroundColor(int color);
		void SetAlignToGrid(bool align) { align_to_grid = align; }
		void DisplayGrid( bool grid ) { display_grid = grid; }
		void SetPixelColor(int color) { pixel_color = color; }
		// SDL_Surface* GetDrawingSurface() { return m_drawing; }
		Texture& GetLayer(int layer=0) { return layers[layer]; }
		void Clear(int color);
		void SetReadOnly( bool isReadOnly ) { m_is_readonly = isReadOnly; }
		

};

}
#endif
