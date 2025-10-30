#include <FLE/Fle_Property_Sheet.hpp>

#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>

Fle_Property_Sheet::Fle_Property_Sheet(int X, int Y, int W, int H, const char* l) : Fl_Grid(X, Y, W, H, l)
{
    m_nameColumnWidth = 0;
    m_heightSum = 0;
    layout(1, 2, 1, 1);
    col_weight(0, 0);
    show_grid(true, FL_INACTIVE_COLOR);
    box(FL_DOWN_BOX);
    color(FL_BACKGROUND2_COLOR);

    end();
}

void Fle_Property_Sheet::draw()
{
    Fl_Grid::draw();

    // Draw over the line below last widget
    fl_push_clip(x(), y() + m_heightSum + rows() + 2, w(), h() - m_heightSum - rows() - 5);
    fl_draw_box(box(), x(), y(), w(), h(), color());
    fl_pop_clip();
}

void Fle_Property_Sheet::add_property_widget(Fl_Widget* w, const char* name)
{
    int row = rows() - 1;
    int span = 1;

    layout(row + 2, 2, 1, 1);

    if(strcmp(name, "") != 0)
    {
	    int lx = 0, ly = 0;
        fl_font(labelfont(), labelsize());
	    fl_measure(name, lx, ly);
        lx += 10;

        if(lx > m_nameColumnWidth)
        {
            m_nameColumnWidth = lx;
            col_width(0, m_nameColumnWidth);
        }

        Fl_Box* nameBox = new Fl_Box(0, 0, 0, 0, name);
        nameBox->box(FL_NO_BOX);
        nameBox->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        nameBox->labelsize(labelsize());
        nameBox->labelfont(labelfont());
        nameBox->labelcolor(labelcolor());

        add(nameBox);
        widget(nameBox, row, 0);
    }
    else
    {
        span = 2;
    }

    add(w);
    widget(w, row, span == 2 ? 0 : 1, 1, span);
    row_height(row, w->h());
    row_weight(row, 0);

    m_heightSum += w->h();
}