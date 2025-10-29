#include <FLE/Fle_Accordion.hpp>
#include <FLE/Fle_Accordion_Group.hpp>
Fle_Accordion::Fle_Accordion(int X, int Y, int W, int H, const char* l) : Fl_Scroll(X, Y, W, H, l), m_pack(X, Y, W, H, "")
{  
    m_pack.type(Fl_Pack::VERTICAL);
    m_singleOpen = false;
}

void Fle_Accordion::fit_pack()
{
    int sumH = 0;
    int scroll = yposition();
    scroll_to(0,0);
    for(int i = 0; i < m_pack.children(); i++)
    {
        Fle_Accordion_Group* c = (Fle_Accordion_Group*)m_pack.child(i);
        if(!c->visible()) continue;
        sumH += c->h();
    }

    if(sumH > h())
    {
        m_pack.resize(x(), y(), w() - Fl::scrollbar_size(), sumH);
        scroll_to(0, scroll);
    }
    else
    {
        m_pack.resize(x(), y(), w(), h());
    }
    
    redraw();
}

void Fle_Accordion::add_group(Fle_Accordion_Group* group)
{
    m_pack.add(group);
    fit_pack();
}

void Fle_Accordion::insert_group(Fle_Accordion_Group* group, int index)
{
    m_pack.insert(*group, index);
    fit_pack();
}

void Fle_Accordion::remove_group(Fle_Accordion_Group* group)
{
    m_pack.remove(group);
    fit_pack();
}

void Fle_Accordion::remove_group(int index)
{
    Fle_Accordion_Group* c = (Fle_Accordion_Group*)m_pack.child(index);
    m_pack.remove(c);
    fit_pack();

    delete c;
}

void Fle_Accordion::close_all(Fle_Accordion_Group* except)
{
    for(int i = 0; i < m_pack.children(); i++)
    {
        Fle_Accordion_Group* c = (Fle_Accordion_Group*)m_pack.child(i);
        if(c != except)
            c->close();
    }
}

void Fle_Accordion::resize(int X, int Y, int W, int H)
{
    Fl_Scroll::resize(X, Y, W, H);
    fit_pack();
}