#ifndef FLE_PROPERTY_SHEET_H
#define FLE_PROPERTY_SHEET_H

#include <FL/Fl_Grid.H>

#include <string>

class Fle_Property_Sheet: public Fl_Grid
{
    int m_nameColumnWidth;
    int m_heightSum;

protected:
    void draw() override;

public:
    Fle_Property_Sheet(int X, int Y, int W, int H, const char* l = 0);

    void add_property_widget(Fl_Widget* w, const char* name);
};
#endif