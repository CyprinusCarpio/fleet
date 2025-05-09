#include <FLE/Fle_Accordion_Group.hpp>

#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>

Fle_Accordion_Group::Fle_Accordion_Group(Fl_Widget* C, int minH, int maxH, const char* l): Fl_Group(0, 0, 0, 20, l), m_expandButton(0, 0, 16, 16, "@-42>")
{
    box(FL_FLAT_BOX);
    labeltype(FL_NO_LABEL);
    m_open = false;
    m_child = C;
    m_minH = minH;
    m_maxH = maxH;
    
    add(m_child);
    m_child->hide();
    end();

    m_expandButton.callback(Fle_Accordion_Group::expand_button_cb, (void*)this);
}

Fle_Accordion_Group::~Fle_Accordion_Group()
{
}

void Fle_Accordion_Group::expand_button_cb(Fl_Widget* w, void* d)
{
    if(((Fle_Accordion_Group*)d)->is_open())
        ((Fle_Accordion_Group*)d)->close();
    else
        ((Fle_Accordion_Group*)d)->open();
}

void Fle_Accordion_Group::draw()
{
    Fl_Group::draw();
    
    fl_draw_box(FL_UP_FRAME, x(), y(), w(), 20, FL_BACKGROUND_COLOR);
    fl_color(labelcolor());
    fl_draw(label(), x() + 5, y(), w(), 20, FL_ALIGN_LEFT);
}

void Fle_Accordion_Group::open()
{
    m_open = true;
    m_expandButton.label("@-48>");
    resize(x(), y(), w(), m_minH + 20);
    m_child->show();
    m_child->resize(x(), y() + 20, w(), m_minH);

    if(get_accordion()->single_open())
        get_accordion()->close_all(this);

    get_accordion()->fit_pack();

    if(when())
    {
        do_callback(FL_REASON_OPENED);
    }
}

void Fle_Accordion_Group::close()
{
    m_open = false;
    m_expandButton.label("@-42>");

    resize(x(), y(), w(), 20);

    m_child->hide();

    get_accordion()->fit_pack();

    if(when())
    {
        do_callback(FL_REASON_CLOSED);
    }
}

void Fle_Accordion_Group::resize(int X, int Y, int W, int H)
{
    Fl_Widget::resize(X, Y, W, H);
    
    if(m_child->visible())
        m_child->resize(X, Y + 20, W, H - 20);

    m_expandButton.resize(X + W - 16, Y + 2, 16, 16);
}

int Fle_Accordion_Group::handle(int event)
{
    bool eventInsideDrag = m_open && Fl::event_inside(x(), y() + h() - 5, w(), 5);
	static int offsetY = 0;
	int ey = Fl::event_y();

    if(event == FL_ENTER)
    {
        return 1;
    }
    if(event == FL_MOVE)
    {
        static bool resizeCursorSet = false;
        if(!resizeCursorSet && eventInsideDrag)
        {
            window()->cursor(FL_CURSOR_NS);
            resizeCursorSet = true;
        }
        else if(resizeCursorSet && !eventInsideDrag)
        {
            window()->cursor(FL_CURSOR_DEFAULT);
            resizeCursorSet = false;
        }
    }
    if(event == FL_PUSH && eventInsideDrag)
    {
        offsetY = ey;
        return 1;
    }
    if(event == FL_DRAG)
    {
        int dy = ey - offsetY;
        if(dy != 0 && h() + dy >= m_minH && h() + dy <= m_maxH)
        {
            resize(x(), y(), w(), h() + dy);
            offsetY = ey;
            get_accordion()->fit_pack();
            parent()->redraw();
            return 1;
        }
    }
    if(event == FL_RELEASE)
    {
        offsetY = 0;
    }

    return Fl_Group::handle(event);
}