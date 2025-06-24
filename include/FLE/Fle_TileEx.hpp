#ifndef FLE_TILEEX_H
#define FLE_TILEEX_H

#include <FL/Fl_Group.H>

#include <vector>

class WidgetExtraData;
struct Edge;

class Fle_TileEx : public Fl_Group
{
    std::vector<WidgetExtraData> m_extraData;

    Edge find_edge_at(int X, int Y);
    void get_widgets_at_edge(Edge& edge);
    void get_max_changes_for_edge(Edge& edge, bool userDrag, int& maxShrinkLeftOrTop, int& maxGrowLeftOrTop, int& maxShrinkRightOrBottom, int& maxGrowRightOrBottom, int recurse = 0);
    Edge get_next_viable_edge(Edge& edge, int delta, bool forShrink, bool userDrag = true);
    void propagate_resize(Edge& edge, int& delta, bool userDrag = true);

protected:
    int on_insert(Fl_Widget*, int) override;
    int on_move(int, int) override;
    void on_remove(int) override;
    
public:
    Fle_TileEx(int X, int Y, int W, int H, const char *L = 0);

    int handle(int e) override;
    void resize(int X, int Y, int W, int H) override;

    void get_min_size(int index, int& w, int& h);
    void get_max_size(int index, int& w, int& h);
    void get_preferred_size(int index, int& w, int& h);

    void size_range(int index, int minW, int minH, int maxW, int maxH, int preferredW, int preferredH);
    void size_range(Fl_Widget* widget, int minW, int minH, int maxW, int maxH, int preferredW, int preferredH);
};

#endif