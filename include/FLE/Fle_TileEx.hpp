#ifndef FLE_TILEEX_H
#define FLE_TILEEX_H

#include <FL/Fl_Group.H>

#include <vector>
#include <set>

struct WidgetExtraData;
struct Edge;
class EdgeList;
enum class EdgeType {ANY, VERTICAL, HORIZONTAL};

class Fle_TileEx : public Fl_Group
{
    std::vector<WidgetExtraData> m_extraData;
    EdgeList* m_edges;
    bool m_needToFindEdges;
    
    void find_edges();
    Edge* find_edge_at(int x, int y, EdgeType type = EdgeType::ANY);
    void edges_populate_widgets();
    Edge* get_prev_edge(Edge* edge);
    Edge* get_next_edge(Edge* edge);
    void get_max_changes_for_edge(Edge* edge, bool resizingWindow, int& maxShrinkLeftOrTop, int& maxGrowLeftOrTop, int& maxShrinkRightOrBottom, int& maxGrowRightOrBottom);
    void propagate_resize(Edge* edge, int& delta, bool resizingWindow, Edge* previousEdge = nullptr);
    void propagate_grow(Edge* edge, int delta, std::set<Edge*>& processedEdges, bool correction = false);
    void edge_change(Edge* edge, int delta, bool updatePrefSizes);

protected:
    int on_insert(Fl_Widget*, int) override;
    int on_move(int, int) override;
    void on_remove(int) override;

    void draw() override;
    
public:
    Fle_TileEx(int X, int Y, int W, int H, const char *L = 0);
    ~Fle_TileEx();

    int handle(int e) override;
    void resize(int X, int Y, int W, int H) override;

    void get_min_size(int index, int& w, int& h);
    void get_max_size(int index, int& w, int& h);
    void get_preferred_size(int index, int& w, int& h);

    void size_range(int index, int minW, int minH, int maxW, int maxH, int preferredW, int preferredH);
    void size_range(Fl_Widget* widget, int minW, int minH, int maxW, int maxH, int preferredW, int preferredH);
};

#endif