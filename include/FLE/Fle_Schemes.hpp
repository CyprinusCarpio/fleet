#ifndef FLE_SCHEMES_H
#define FLE_SCHEMES_H

#include <FL/Fl.H>

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

void fle_define_schemes();
void fle_set_scheme1();
void fle_set_scheme2();

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