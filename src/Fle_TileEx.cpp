#include <FLE/Fle_TileEx.hpp>

#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include <algorithm>

#include <cassert>

// LOT = left or top
// BOR = bottom or right

struct Edge 
{
    EdgeType type;
    int position, start, end;
    std::vector<int> LOTWidgets;
    std::vector<int> BORWidgets;
};

class EdgeComparator
{
public:
    bool operator()(const Edge* a, const Edge* b) const
    {
        return a->position < b->position;
    }
};

class EdgeList
{
public:
    std::vector<Edge> m_edges;
    Edge* m_leftEdge;
    Edge* m_topEdge;
    Edge* m_rightEdge;
    Edge* m_bottomEdge;

    EdgeList() {}

    void add_edge(Edge edge)
    {
        for(Edge& e : m_edges)
        {
            if(e.position == edge.position && e.type == edge.type &&
               e.start <= edge.end && edge.start <= e.end)
            {
                e.start = std::min(e.start, edge.start);
                e.end = std::max(e.end, edge.end);

                return;
            }
        }

        m_edges.push_back(edge);
    }

    void consolidate_edges()
    {
        for(int i = 0; i < m_edges.size(); i++)
        {
            Edge& e = m_edges[i];
            for(int j = i + 1; j < m_edges.size(); j++)
            {
                Edge& f = m_edges[j];
                if(e.position == f.position && e.type == f.type &&
                   e.start <= f.end && f.start <= e.end)
                {
                    e.start = std::min(e.start, f.start);
                    e.end = std::max(e.end, f.end);
                    m_edges.erase(m_edges.begin() + j);
                    j--;
                }
            }
        }
    }
};

struct WidgetExtraData
{
    int minW;
    int minH;
    int maxW;
    int maxH;
    int prefW;
    int prefH;

    WidgetExtraData() 
    {
        minW = 0;
        minH = 0;
        maxW = 0x7FFFFFFF;
        maxH = 0x7FFFFFFF;
        prefW = 25;
        prefH = 25;
    }
};

Fle_TileEx::Fle_TileEx(int X, int Y, int W, int H, const char *L) : Fl_Group(X, Y, W, H, L) 
{
    m_edges = new EdgeList();
    m_needToFindEdges = true;
}

Fle_TileEx::~Fle_TileEx() 
{
    delete m_edges;
}

void Fle_TileEx::find_edges()
{
    m_edges->m_edges.clear();
    m_edges->m_leftEdge = nullptr;
    m_edges->m_topEdge = nullptr;
    m_edges->m_rightEdge = nullptr;
    m_edges->m_bottomEdge = nullptr;

    for(int i = 0; i < children(); i++)
    {
        Fl_Widget* W = child(i);

        m_edges->add_edge({EdgeType::VERTICAL, W->x() + W->w(), W->y(), W->y() + W->h()});
        m_edges->add_edge({EdgeType::HORIZONTAL, W->y() + W->h(), W->x(), W->x() + W->w()});
    }

    m_edges->consolidate_edges();

    m_edges->m_edges.push_back(Edge{EdgeType::VERTICAL, 0, 0, h()});
    m_edges->m_edges.push_back(Edge{EdgeType::HORIZONTAL, 0, 0, w()});

    m_edges->m_leftEdge = &m_edges->m_edges[m_edges->m_edges.size() - 2];
    m_edges->m_topEdge = &m_edges->m_edges[m_edges->m_edges.size() - 1];

    for(Edge& e : m_edges->m_edges)
    {
        if(e.type == EdgeType::VERTICAL && e.position != 0 && e.start == 0 && e.end == h())
        {
            m_edges->m_rightEdge = &e;
        }
        else if(e.type == EdgeType::HORIZONTAL && e.position != 0 && e.start == 0 && e.end == w())
        {
            m_edges->m_bottomEdge = &e;
        }
    }

    edges_populate_widgets();
}

Edge* Fle_TileEx::find_edge_at(int x, int y, EdgeType type)
{;
    for(Edge& e : m_edges->m_edges)
    {
        if(type != EdgeType::ANY && e.type != type) continue;

        if(e.type == EdgeType::VERTICAL && e.position == x && y >= e.start && y <= e.end)
        {
            return &e;
        }
        else if(e.type == EdgeType::HORIZONTAL && e.position == y && x >= e.start && x <= e.end)
        {
            return &e;
        }
    }
    return nullptr;
}

void Fle_TileEx::edges_populate_widgets()
{
    for(Edge& e : m_edges->m_edges)
    {
        e.LOTWidgets.clear();
        e.BORWidgets.clear();

        for(int i = 0; i < children(); i++)
        {
            Fl_Widget* W = child(i);
            if(e.type == EdgeType::VERTICAL && W->y() + 1 <= e.end && e.start <= W->y() + W->h() - 1)
            {
                if(e.position == W->x() + W->w())
                {
                    e.LOTWidgets.push_back(i);
                }
                else if(e.position == W->x())
                {
                    e.BORWidgets.push_back(i);
                }
            }
            else if(e.type == EdgeType::HORIZONTAL && W->x() + 1 <= e.end && e.start <= W->x() + W->w() - 1)
            {
                if(e.position == W->y() + W->h())
                {
                    e.LOTWidgets.push_back(i);
                }
                else if(e.position == W->y())
                {
                    e.BORWidgets.push_back(i);
                }
            }
        }
    }
}

Edge* Fle_TileEx::get_prev_edge(Edge* edge)
{
    Fl_Widget* smallest = nullptr;

    for(int i : edge->LOTWidgets)
    {
        Fl_Widget* W = child(i);

        if(edge->type == EdgeType::VERTICAL)
        {
            if(smallest == nullptr || W->w() < smallest->w())
            {
                smallest = W;
            }
        }
        else
        {
            if(smallest == nullptr || W->h() < smallest->h())
            {
                smallest = W;
            }
        }
    }

    if(smallest != nullptr)
    {
        return find_edge_at(smallest->x(), smallest->y(), edge->type);
    }

    return nullptr;
}

Edge* Fle_TileEx::get_next_edge(Edge* edge)
{
    if(edge == m_edges->m_rightEdge || edge == m_edges->m_bottomEdge)
    {
        return nullptr;
    }

    Fl_Widget* smallest = nullptr;

    for(int i : edge->BORWidgets)
    {
        Fl_Widget* W = child(i);

        if(edge->type == EdgeType::VERTICAL)
        {
            if(smallest == nullptr || W->w() < smallest->w())
            {
                smallest = W;
            }
        }
        else
        {
            if(smallest == nullptr || W->h() < smallest->h())
            {
                smallest = W;
            }
        }
    }

    if(smallest != nullptr)
    {
        return find_edge_at(smallest->x() + (edge->type == EdgeType::VERTICAL ? smallest->w() : 0), smallest->y() + (edge->type == EdgeType::HORIZONTAL ? smallest->h() : 0), edge->type);
    }

    return nullptr;
}

void Fle_TileEx::get_max_changes_for_edge(Edge* edge, bool resizingWindow, int& maxShrinkLeftOrTop, int& maxGrowLeftOrTop, int& maxShrinkRightOrBottom, int& maxGrowRightOrBottom)
{
    maxShrinkLeftOrTop = 0x7FFFFFFF;
    maxGrowLeftOrTop = 0x7FFFFFFF;
    maxShrinkRightOrBottom = 0x7FFFFFFF;
    maxGrowRightOrBottom = 0x7FFFFFFF;

    for(int i : edge->LOTWidgets)
    {
        Fl_Widget* W = child(i);

        if(edge->type == EdgeType::VERTICAL)
        {
            maxShrinkLeftOrTop = std::min(maxShrinkLeftOrTop, W->w() - m_extraData[i].minW);
            maxGrowLeftOrTop = std::min(maxGrowLeftOrTop, resizingWindow ? m_extraData[i].prefW - W->w() : m_extraData[i].maxW - W->w());
        }
        else
        {
            maxShrinkLeftOrTop = std::min(maxShrinkLeftOrTop, W->h() - m_extraData[i].minH);
            maxGrowLeftOrTop = std::min(maxGrowLeftOrTop, resizingWindow ? m_extraData[i].prefH - W->h() : m_extraData[i].maxH - W->h());
        }
    }
    for(int i : edge->BORWidgets)
    {
        Fl_Widget* W = child(i);

        if(edge->type == EdgeType::VERTICAL)
        {
            maxShrinkRightOrBottom = std::min(maxShrinkRightOrBottom, W->w() - m_extraData[i].minW);
            maxGrowRightOrBottom = std::min(maxGrowRightOrBottom, resizingWindow ? m_extraData[i].prefW - W->w() : m_extraData[i].maxW - W->w());
        }
        else
        {
            maxShrinkRightOrBottom = std::min(maxShrinkRightOrBottom, W->h() - m_extraData[i].minH);
            maxGrowRightOrBottom = std::min(maxGrowRightOrBottom, resizingWindow ? m_extraData[i].prefH - W->h() : m_extraData[i].maxH - W->h());
        }
    }
    // Right or bottom edge
    if(edge->BORWidgets.empty())
    {
        maxGrowLeftOrTop = 0x7FFFFFFF;
    }
    // Left or top edge
    else if(edge->LOTWidgets.empty())
    {
        maxShrinkLeftOrTop = 0;
        maxGrowLeftOrTop = 0;
    }
    // Next edge is right or bottom, and BOR widgets
    // may be above max size, which messes us up
    // max grow calculation
    if(maxGrowRightOrBottom < 0) maxGrowRightOrBottom = 0;
}

void Fle_TileEx::propagate_resize(Edge* edge, int& delta, bool resizingWindow, Edge* previousEdge)
{
    if(edge == nullptr) return;
    //std::cout << "propagate resize " << edge->position << " " << delta << std::endl;
    assert(edge->position >= 0);

    int maxShrinkLeftOrTop, maxGrowLeftOrTop, maxShrinkRightOrBottom, maxGrowRightOrBottom;
    get_max_changes_for_edge(edge, resizingWindow, maxShrinkLeftOrTop, maxGrowLeftOrTop, maxShrinkRightOrBottom, maxGrowRightOrBottom);

    int maxChange;
    bool maxSizeHit = false;
    if(delta > 0)
    {
        if(maxGrowLeftOrTop < maxShrinkRightOrBottom)
        {
            maxChange = maxGrowLeftOrTop;
            maxSizeHit = true;
        }
        else
        {
            maxChange = maxShrinkRightOrBottom;
        }
    }
    else
    {
        if(maxShrinkLeftOrTop < maxGrowRightOrBottom)
        {
            maxChange = maxShrinkLeftOrTop;
        }
        else
        {
            maxChange = maxGrowRightOrBottom;
            maxSizeHit = true;
        }
    }
    int change = std::min(std::abs(delta), maxChange);
    
    if(delta < 0) change = -change;
    delta -= change;
    
    if(change != 0)
    {
        edge_change(edge, change, !resizingWindow);
    }

    if(delta != 0)
    {
        int oldDelta = delta;
        Edge* nextToProcess = maxSizeHit != delta < 0 ? get_prev_edge(edge) : get_next_edge(edge);
        if(nextToProcess == previousEdge) return;
        propagate_resize(nextToProcess, delta, resizingWindow, edge);
        oldDelta -= delta;
        propagate_resize(edge, oldDelta, resizingWindow, edge);
    }
}

void Fle_TileEx::propagate_grow(Edge* edge, int delta, std::set<Edge*>& processedEdges, bool correction)
{
    if(edge == nullptr) return;
    if(!correction && processedEdges.count(edge) > 0) return;
    // the correction bool is to prevent further propagation during a correction AFTER propagation for a given edge
    assert(edge->position >= 0);

    int maxShrinkLeftOrTop, maxGrowLeftOrTop, maxShrinkRightOrBottom, maxGrowRightOrBottom;
    get_max_changes_for_edge(edge, true, maxShrinkLeftOrTop, maxGrowLeftOrTop, maxShrinkRightOrBottom, maxGrowRightOrBottom);

    int maxChange;
    bool maxSizeHit = false;
    if(delta > 0)
    {
        if(maxGrowLeftOrTop < maxShrinkRightOrBottom)
        {
            maxChange = maxGrowLeftOrTop;
            maxSizeHit = true;
        }
        else
        {
            maxChange = maxShrinkRightOrBottom;
        }
    }
    else
    {
        if(maxShrinkLeftOrTop < maxGrowRightOrBottom)
        {
            maxChange = maxShrinkLeftOrTop;
        }
        else
        {
            maxChange = maxGrowRightOrBottom;
            maxSizeHit = true;
        }
    }
    int change = std::min(std::abs(delta), maxChange);
    
    if(delta < 0) change = -change;
    delta -= change;
    
    if(change != 0)
    {
        edge_change(edge, change, false);

        processedEdges.insert(edge);
    }

    if(!correction && delta != 0)
    {
        std::set<Edge*, EdgeComparator> edgesToGo;

        if(edge->type == EdgeType::VERTICAL)
        {
            for(int i : edge->BORWidgets)
            {
                Fl_Widget* W = child(i);
                Edge* next = find_edge_at(W->x() + W->w(), W->y(), edge->type);
                if(next)
                    edgesToGo.insert(next);
            }
        }
        else
        {
            for(int i : edge->BORWidgets)
            {
                Fl_Widget* W = child(i);
                Edge* next = find_edge_at(W->x(), W->y() + W->h(), edge->type);
                if(next)
                    edgesToGo.insert(next);
            }
        }

        for(Edge* e : edgesToGo)
        {
            propagate_grow(e, delta, processedEdges);
        }

        // The propagation may have freed up some space
        // "propagate" one last time
        propagate_grow(edge, delta, processedEdges, true);
    }
}

void Fle_TileEx::edge_change(Edge* edge, int delta, bool updatePrefSizes)
{
    for(Edge& e : m_edges->m_edges)
    {
        if(e.type != edge->type && e.position >= edge->start && e.position <= edge->end)
        {
            if(e.start == edge->position)
            {
                e.start += delta;
            }
            else if(e.end == edge->position)
            {
                e.end += delta;
            }
        }
    }
    edge->position += delta;
    if(edge->type == EdgeType::VERTICAL)
    {
        for(int i : edge->LOTWidgets)
        {
            Fl_Widget* W = child(i);
            W->resize(W->x(), W->y(), W->w() + delta, W->h());
            if(updatePrefSizes)
            {
                m_extraData[i].prefW = W->w();
            }
        }
        for(int i : edge->BORWidgets)
        {
            Fl_Widget* W = child(i);
            W->resize(W->x() + delta, W->y(), W->w() - delta, W->h());
            if(updatePrefSizes)
            {
                m_extraData[i].prefW = W->w();
            }
        }
    }
    else
    {
        for(int i : edge->LOTWidgets)
        {
            Fl_Widget* W = child(i);
            W->resize(W->x(), W->y(), W->w(), W->h() + delta);
            if(updatePrefSizes)
            {
                m_extraData[i].prefH = W->h();
            }
        }
        for(int i : edge->BORWidgets)
        {
            Fl_Widget* W = child(i);
            W->resize(W->x(), W->y() + delta, W->w(), W->h() - delta);
            if(updatePrefSizes)
            {
                m_extraData[i].prefH = W->h();
            }
        }
    }
}

int Fle_TileEx::handle(int event)
{
    static int ex, ey;
    static Edge* edge = nullptr;

    if(event == FL_ENTER)
    {
        return 1;
    }
    else if(event == FL_MOVE)
    {
        Edge* e = find_edge_at(Fl::event_x(), Fl::event_y());
        if(e != nullptr)
        {
            window()->cursor(e->type == EdgeType::VERTICAL ? FL_CURSOR_WE : FL_CURSOR_NS);
            return 1;
        }
        else
        {
            window()->cursor(FL_CURSOR_DEFAULT);
            return 1;
        }
    }
    else if(event == FL_PUSH && edge == nullptr)
    {
        ex = Fl::event_x();
        ey = Fl::event_y();
        edge = find_edge_at(Fl::event_x(), Fl::event_y());
        return 1;
    }
    else if(event == FL_RELEASE)
    {
        edge = nullptr;
        return 1;
    }
    else if(event == FL_DRAG && edge != nullptr)
    {
        int delta = Fl::event_y() - ey;
        if(edge->type == EdgeType::VERTICAL)
            delta = Fl::event_x() - ex;
        
        propagate_resize(edge, delta, false);
        redraw();
        if(delta == 0)
        {
            edges_populate_widgets();
            ex = Fl::event_x();
            ey = Fl::event_y();
        }
        return 1;
    }
    return Fl_Group::handle(event);
}

void Fle_TileEx::resize(int X, int Y, int W, int H)
{
    int oldW = w();
    int oldH = h();

    Fl_Widget::resize(X, Y, W, H);

    if(m_edges->m_rightEdge == nullptr || m_edges->m_bottomEdge == nullptr)
    {
        find_edges();
    }

    if(oldW != W)
    {
        int delta = W - oldW;
        if(delta < 0)
        {
            propagate_resize(m_edges->m_rightEdge, delta, true);
        }
        else
        {
            std::set<Edge*> processedEdges;
            propagate_grow(m_edges->m_leftEdge, delta, processedEdges);

            int rightEdgePos = m_edges->m_rightEdge->position;

            if(rightEdgePos != W)
            {
                edge_change(m_edges->m_rightEdge, rightEdgePos - W, false);
            }
        }
    }
    if(oldH != H)
    {
        int delta = H - oldH;
        if(delta < 0)
        {
            propagate_resize(m_edges->m_bottomEdge, delta, true);
        }
        else
        {
            std::set<Edge*> processedEdges;
            propagate_grow(m_edges->m_topEdge, delta, processedEdges);

            int bottomEdgePos = m_edges->m_bottomEdge->position;

            if(bottomEdgePos != H)
            {
                edge_change(m_edges->m_bottomEdge, H- bottomEdgePos, false);
            }
        }
    }
}

void Fle_TileEx::get_min_size(int index, int &w, int &h)
{
    if(index >= 0 && index < m_extraData.size())
    {
        w = m_extraData[index].minW;
        h = m_extraData[index].minH;
    }
}

void Fle_TileEx::get_max_size(int index, int &w, int &h)
{
    if(index >= 0 && index < m_extraData.size())
    {
        w = m_extraData[index].maxW;
        h = m_extraData[index].maxH;
    }
}

void Fle_TileEx::get_preferred_size(int index, int &w, int &h)
{
    if(index >= 0 && index < m_extraData.size())
    {
        w = m_extraData[index].prefW;
        h = m_extraData[index].prefH;
    }
}

void Fle_TileEx::size_range(int index, int minW, int minH, int maxW, int maxH, int preferredW, int preferredH)
{
    if(index >= 0 && index < m_extraData.size())
    {
        m_extraData[index].minW = minW;
        m_extraData[index].minH = minH;
        m_extraData[index].maxW = maxW;
        m_extraData[index].maxH = maxH;
        m_extraData[index].prefW = preferredW;
        m_extraData[index].prefH = preferredH;
    }
}

void Fle_TileEx::size_range(Fl_Widget *widget, int minW, int minH, int maxW, int maxH, int preferredW, int preferredH)
{
    size_range(find(widget), minW, minH, maxW, maxH, preferredW, preferredH);
}

int Fle_TileEx::on_insert(Fl_Widget *widget, int index) 
{
    m_extraData.insert(m_extraData.begin() + index, WidgetExtraData());
    
    m_needToFindEdges = true;

    return index;
}

int Fle_TileEx::on_move(int index, int new_index) 
{
    std::swap(m_extraData[index], m_extraData[new_index]);

    m_needToFindEdges = true;

    return new_index;
}

void Fle_TileEx::on_remove(int index) 
{
    m_extraData.erase(m_extraData.begin() + index);

    m_needToFindEdges = true;
}

/*void Fle_TileEx::draw()
{
    if(m_needToFindEdges)
    {
        find_edges();
        m_needToFindEdges = false;
    }
    Fl_Group::draw();

    for(int i = 0; i < m_edges->m_edges.size(); i++)
    {
        Edge& e = m_edges->m_edges[i];
        fl_color(i * 9 + 56);
        if(e.type == EdgeType::VERTICAL)
        {
            fl_rectf(e.position - 2, e.start - 2, 4, e.end - e.start);
        }
        else
        {
            fl_rectf(e.start - 2, e.position - 2, e.end - e.start, 4);
        }
    }
}*/