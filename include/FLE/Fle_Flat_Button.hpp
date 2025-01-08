#ifndef Fle_FLAT_BUTTON_H
#define Fle_FLAT_BUTTON_H

#include <FL/Fl_Button.H>

class Fle_Flat_Button : public Fl_Button
{
	bool m_mouseOver = false;

protected:
	void draw() override;

public:
	Fle_Flat_Button(int X, int Y, int W, int H, const char* L);

	int handle(int e) override;
};

#endif