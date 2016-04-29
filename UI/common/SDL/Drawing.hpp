#ifndef _CSURFACE_H_
#define _CSURFACE_H_
 
namespace ng {

namespace Drawing {
	
	void SetResolution( int w, int h );
	void Init();
	void Rect(int x, int y, int w, int h, unsigned int color);
	void FillRect(int x, int y, int w, int h, unsigned int color);
	void FillCircle(int x, int y, float radius, unsigned int color);
	void Circle(int x, int y, float radius, unsigned int color);
	void Line(int xA, int yA, int xB, int yB, unsigned int color);
	void VLine(int x1, int y1, int y2, unsigned int color);
}

}
#endif
