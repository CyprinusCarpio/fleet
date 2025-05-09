#ifndef FLE_COLORS_H
#define FLE_COLORS_H

#include <FL/Fl.H>
#include <FL/Fl_Choice.H>

/// \class Fle_Colors_Choice
/// \brief A color theme choice widget.
class Fle_Colors_Choice : public Fl_Choice
{
public:
	Fle_Colors_Choice(int X, int Y, int W, int H, const char* l);
	~Fle_Colors_Choice();
};

void fle_set_colors(const char* colors);

#endif 