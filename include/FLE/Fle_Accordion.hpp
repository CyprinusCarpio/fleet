#ifndef FLE_ACCORDION_H
#define FLE_ACCORDION_H

#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>

class Fle_Accordion_Group;
class Fle_Accordion: public Fl_Scroll
{
    bool m_singleOpen;
    Fl_Pack m_pack;

public:
    Fle_Accordion(int X, int Y, int W, int H, const char* l);
    
    void fit_pack();
    void add_group(Fle_Accordion_Group* group);
    void insert_group(Fle_Accordion_Group* group, int index);
    void remove_group(Fle_Accordion_Group* group);
    void remove_group(int index);

    void close_all(Fle_Accordion_Group* except);

    void single_open(bool v) { m_singleOpen = v; }
    bool single_open() const { return m_singleOpen; }

    void resize(int X, int Y, int W, int H) override;
};

#endif