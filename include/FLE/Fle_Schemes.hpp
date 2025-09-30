#ifndef FLE_SCHEMES_H
#define FLE_SCHEMES_H

#include <FL/Fl.H>
#include <FL/Fl_Choice.H>

#ifndef FLE_BOXTYPES_BEGIN
#define FLE_BOXTYPES_BEGIN FL_FREE_BOXTYPE
#endif

#define FLE_SCHEME1_UP_BOX          (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 1)
#define FLE_SCHEME1_DOWN_BOX        (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 2)
#define FLE_SCHEME1_UP_FRAME        (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 3)
#define FLE_SCHEME1_DOWN_FRAME      (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 4)
#define FLE_SCHEME1_THIN_UP_BOX     (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 5)
#define FLE_SCHEME1_THIN_DOWN_BOX   (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 6)
#define FLE_SCHEME1_THIN_UP_FRAME   (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 7)
#define FLE_SCHEME1_THIN_DOWN_FRAME (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 8)

#define FLE_SCHEME2_UP_BOX          (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 9)
#define FLE_SCHEME2_DOWN_BOX        (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 10)
#define FLE_SCHEME2_UP_FRAME        (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 11)
#define FLE_SCHEME2_DOWN_FRAME      (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 12)
#define FLE_SCHEME2_THIN_UP_BOX     (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 13)
#define FLE_SCHEME2_THIN_DOWN_BOX   (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 14)
#define FLE_SCHEME2_THIN_UP_FRAME   (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 15)
#define FLE_SCHEME2_THIN_DOWN_FRAME (Fl_Boxtype)(FLE_BOXTYPES_BEGIN + 16)

/// \class Fle_Scheme_Choice
/// \brief A scheme choice widget.
///
/// In FLTK, a scheme is just a set of box types
/// and frame types. FLEET provides new schemes and
/// a choice widget to select them. As of FLTK 1.4,
/// FLEET's scheme implementation is not entirely
/// compatible with FLTK's and other custom schemes.
/// It is hoped that this will be addressed in the
/// upcoming FLTK 1.5.
class Fle_Scheme_Choice : public Fl_Choice
{
public:
	Fle_Scheme_Choice(int X, int Y, int W, int H, const char* l);
	~Fle_Scheme_Choice();
};

void fle_set_scheme(const char* scheme);
const char* fle_get_scheme();

void fle_scheme1_up_box_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme1_down_box_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme1_up_frame_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme1_down_frame_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme1_thin_up_box_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme1_thin_down_box_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme1_thin_up_frame_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme1_thin_down_frame_draw(int x, int y, int w, int h, Fl_Color c);

void fle_scheme2_up_box_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme2_down_box_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme2_up_frame_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme2_down_frame_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme2_thin_up_box_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme2_thin_down_box_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme2_thin_up_frame_draw(int x, int y, int w, int h, Fl_Color c);
void fle_scheme2_thin_down_frame_draw(int x, int y, int w, int h, Fl_Color c);

#endif