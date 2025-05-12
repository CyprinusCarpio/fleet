#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Scheme_Choice.H>
#include <FL/fl_draw.H>
#include <FL/fl_show_colormap.H>  // TEST: added by Albrecht-S

#include <iostream>

#include "FLE/Fle_Dock_Host.hpp"
#include "FLE/Fle_Events.hpp"
#include "FLE/Fle_Orientable_Flex.hpp"
#include "FLE/Fle_Toolbar.hpp"
#include "FLE/Fle_Listview.hpp"
#include "FLE/Fle_Listview_Item.hpp"
#include "FLE/Fle_Accordion.hpp"
#include "FLE/Fle_Accordion_Group.hpp"

#define FLE_BOXTYPES_BEGIN FL_FREE_BOXTYPE
#include "FLE/Fle_Schemes.hpp"

#include "FLE/Fle_Colors.hpp"

static const char* ikona_xpm[] = {
"16 16 8 1",
" 	c None",
".	c #000000",
"+	c #99966A",
"@	c #9C996C",
"#	c #74714F",
"$	c #767350",
"%	c #9D9A6D",
"&	c #B0AE84",
"                ",
" .......        ",
" .+@@@+.        ",
" .#$$$#.        ",
" .............  ",
" .@@@@@@@@@@@@. ",
" .@@@@@@@@@@@@. ",
" .@@@@@@@@@@@@. ",
" .@@@@@@@@@@@@. ",
" .%%%%%%%%%%%%. ",
" .&&&&&&&&&&&&. ",
" .&&&&&&&&&&&&. ",
" .%%%%%%%%%%%%. ",
" .++++++++++++. ",
"  ............  ",
"                " };
Fl_Pixmap* img = nullptr;

Fl_Tree* make_tree()
{
    Fl_Tree* tree = new Fl_Tree(10, 10, 380, 280);

    // The following is Llama-3.3-70b-Instruct's take on a
    // typical Linux filesystem
    
    // Create root directory (invisible)
    Fl_Tree_Item* root = tree->root();

    // Create top-level directories
    Fl_Tree_Item* bin = tree->add(root, "bin"); bin->usericon(img);
    Fl_Tree_Item* boot = tree->add(root, "boot"); boot->usericon(img);
    Fl_Tree_Item* dev = tree->add(root, "dev"); dev->usericon(img);
    Fl_Tree_Item* etc = tree->add(root, "etc"); etc->usericon(img);
    Fl_Tree_Item* home = tree->add(root, "home"); home->usericon(img);
    Fl_Tree_Item* lib = tree->add(root, "lib"); lib->usericon(img);
    Fl_Tree_Item* lib32 = tree->add(root, "lib32"); lib32->usericon(img); // Added
    Fl_Tree_Item* lib64 = tree->add(root, "lib64"); lib64->usericon(img);
    Fl_Tree_Item* media = tree->add(root, "media"); media->usericon(img);
    Fl_Tree_Item* mnt = tree->add(root, "mnt"); mnt->usericon(img);
    Fl_Tree_Item* opt = tree->add(root, "opt"); opt->usericon(img);
    Fl_Tree_Item* proc = tree->add(root, "proc"); proc->usericon(img);
    Fl_Tree_Item* rootDir = tree->add(root, "root"); rootDir->usericon(img);
    Fl_Tree_Item* run = tree->add(root, "run"); run->usericon(img);
    Fl_Tree_Item* sbin = tree->add(root, "sbin"); sbin->usericon(img);
    Fl_Tree_Item* srv = tree->add(root, "srv"); srv->usericon(img);
    Fl_Tree_Item* sys = tree->add(root, "sys"); sys->usericon(img);
    Fl_Tree_Item* tmp = tree->add(root, "tmp"); tmp->usericon(img);
    Fl_Tree_Item* usr = tree->add(root, "usr"); usr->usericon(img);
    Fl_Tree_Item* var = tree->add(root, "var"); var->usericon(img);

    Fl_Tree_Item* homeUser = tree->add(home, "user"); homeUser->usericon(img); // Example user
    Fl_Tree_Item* homeUserConfig = tree->add(homeUser, ".config"); homeUserConfig->usericon(img); // Hidden config
    Fl_Tree_Item* homeUserCache = tree->add(homeUser, ".cache"); homeUserCache->usericon(img); // Hidden cache

    // Subdirectories of etc
    Fl_Tree_Item* etcNetwork = tree->add(etc, "network"); etcNetwork->usericon(img);
    Fl_Tree_Item* etcSsh = tree->add(etc, "ssh"); etcSsh->usericon(img);
    Fl_Tree_Item* etcX11 = tree->add(etc, "X11"); etcX11->usericon(img);
    Fl_Tree_Item* etcInitd = tree->add(etc, "init.d"); etcInitd->usericon(img);
    Fl_Tree_Item* etcSysconfig = tree->add(etc, "sysconfig"); etcSysconfig->usericon(img);
    Fl_Tree_Item* etcModprobeD = tree->add(etc, "modprobe.d"); etcModprobeD->usericon(img); // Added
    Fl_Tree_Item* etcSecurity = tree->add(etc, "security"); etcSecurity->usericon(img); // Added


    Fl_Tree_Item* devPts = tree->add(dev, "pts"); devPts->usericon(img); // Added
    Fl_Tree_Item* devShm = tree->add(dev, "shm"); devShm->usericon(img); // Added

    // Subdirectories of usr
    Fl_Tree_Item* usrBin = tree->add(usr, "bin"); usrBin->usericon(img);
    Fl_Tree_Item* usrInclude = tree->add(usr, "include"); usrInclude->usericon(img); // Added
    Fl_Tree_Item* usrLib = tree->add(usr, "lib"); usrLib->usericon(img);
    Fl_Tree_Item* usrLib32 = tree->add(usr, "lib32"); usrLib32->usericon(img); // Added
    Fl_Tree_Item* usrLib64 = tree->add(usr, "lib64"); usrLib64->usericon(img); // Added
    Fl_Tree_Item* usrLocal = tree->add(usr, "local"); usrLocal->usericon(img);
    Fl_Tree_Item* usrSbin = tree->add(usr, "sbin"); usrSbin->usericon(img); // Added
    Fl_Tree_Item* usrShare = tree->add(usr, "share"); usrShare->usericon(img);

    // Subdirectories of media
    Fl_Tree_Item* mediaCdrom = tree->add(media, "cdrom"); mediaCdrom->usericon(img); // Added
    Fl_Tree_Item* mediaUsb = tree->add(media, "usb"); mediaUsb->usericon(img); // Added

    // Subdirectories of usr/local
    Fl_Tree_Item* usrLocalBin = tree->add(usrLocal, "bin"); usrLocalBin->usericon(img);
    Fl_Tree_Item* usrLocalEtc = tree->add(usrLocal, "etc"); usrLocalEtc->usericon(img); // Added
    Fl_Tree_Item* usrLocalLib = tree->add(usrLocal, "lib"); usrLocalLib->usericon(img);
    Fl_Tree_Item* usrLocalSbin = tree->add(usrLocal, "sbin"); usrLocalSbin->usericon(img); // Added
    Fl_Tree_Item* usrLocalShare = tree->add(usrLocal, "share"); usrLocalShare->usericon(img);

    // Subdirectories of var
    Fl_Tree_Item* varLog = tree->add(var, "log"); varLog->usericon(img); // Added
    Fl_Tree_Item* varTmp = tree->add(var, "tmp"); varTmp->usericon(img); // Added
    Fl_Tree_Item* varRun = tree->add(var, "run"); varRun->usericon(img); // Added
    Fl_Tree_Item* varLib = tree->add(var, "lib"); varLib->usericon(img); // Added
    Fl_Tree_Item* varCache = tree->add(var, "cache"); varCache->usericon(img); // Added

    tree->showroot(false);
    tree->connectorstyle(FL_TREE_CONNECTOR_SOLID);
    tree->select("/etc/network/");

    return tree;
}

class FlatMenuBar : public Fl_Menu_Bar
{
public:
    FlatMenuBar(int X, int Y, int W, int H, const char* l) : Fl_Menu_Bar(X, Y, W, H, l) {};

protected:
    void draw() override
    {
        fl_push_clip(x() + 3, y() + 3, w() - 6, h() - 6);
        Fl_Menu_Bar::draw();
        fl_pop_clip();
    }
};

class SchemeTest : public Fl_Box
{
public:
    SchemeTest(int X, int Y, int W, int H, const char* l) : Fl_Box(X, Y, W, H, l)
    {
        box(FL_DOWN_BOX);
        color(FL_BACKGROUND2_COLOR);
    }

protected:
    void draw() override
    {
        Fl_Box::draw();

        /*fl_draw_box(FLE_SCHEME1_UP_BOX, x() + 20, y() + 20, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME1_DOWN_BOX, x() + 20, y() + 50, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME1_UP_FRAME, x() + 90, y() + 20, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME1_DOWN_FRAME, x() + 90, y() + 50, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME1_THIN_UP_BOX, x() + 20, y() + 20 + 60, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME1_THIN_DOWN_BOX, x() + 20, y() + 50 + 60, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME1_THIN_UP_FRAME, x() + 90, y() + 20 + 60, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME1_THIN_DOWN_FRAME, x() + 90, y() + 50 + 60, 64, 26, FL_BACKGROUND_COLOR);

        fl_draw_box(FLE_SCHEME2_UP_BOX, x() + 20, 150 +  y() + 20, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME2_DOWN_BOX, x() + 20, 150 + y() + 50, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME2_UP_FRAME, x() + 90, 150 + y() + 20, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME2_DOWN_FRAME, x() + 90, 150 + y() + 50, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME2_THIN_UP_BOX, x() + 20, 150 + y() + 20 + 60, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME2_THIN_DOWN_BOX, x() + 20, 150 + y() + 50 + 60, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME2_THIN_UP_FRAME, x() + 90, 150 + y() + 20 + 60, 64, 26, FL_BACKGROUND_COLOR);
        fl_draw_box(FLE_SCHEME2_THIN_DOWN_FRAME, x() + 90, 150 + y() + 50 + 60, 64, 26, FL_BACKGROUND_COLOR);*/
    }
};

void min_size_cb(Fle_Dock_Host* host, int W, int H)
{
    //std::cout << "min_size_cb " << W << " " << H << std::endl;
    
    host->window()->size_range(W, H);
}
Fle_Dock_Group* vertGroup1;

void showFoldersCb(Fl_Widget* w, void* d)
{
    Fle_Dock_Group* parent = (Fle_Dock_Group*)w->parent()->parent();
    Fle_Dock_Host* dh = parent->get_host();
    if (vertGroup1->hidden())
    {
        vertGroup1->show_group();
    }
    else
        vertGroup1->hide_group();
}

Fle_Dock_Group* make_toolbar(Fle_Dock_Host* dh)
{
    Fle_Dock_Group* tbg = new Fle_Dock_Group(dh, 7, "Toolbar", FLE_DOCK_NO_HOR_LABEL | FLE_DOCK_DETACHABLE | FLE_DOCK_FLEXIBLE, FLE_DOCK_TOP, FLE_DOCK_BOTTOM, 270, 26, true);
    Fle_Toolbar* tb = new Fle_Toolbar(0, 0, 0, 0, Fl_Flex::HORIZONTAL);

    Fle_Flat_Button* btn1 = new Fle_Flat_Button(0, 0, 50, 24, "@<- Back");
    Fle_Flat_Button* btn2 = new Fle_Flat_Button(50, 0, 24, 24, "@->");
    Fle_Flat_Button* btn3 = new Fle_Flat_Button(24 + 50, 0, 24, 24, "@8->");
    Fle_Flat_Button* btn4 = new Fle_Flat_Button(24 * 2 + 50 + 4, 0, 75, 24, "@search Search");
    Fle_Flat_Button* btn5 = new Fle_Flat_Button(24 * 2 + 50 + 75 + 4, 0, 75, 24, "@fileopen Folders");
    btn5->callback(showFoldersCb);

    tb->add_tool(btn1);
    tb->add_tool(btn2);
    tb->add_tool(btn3);
    tb->add_separator();
    tb->add_tool(btn4);
    tb->add_tool(btn5);

    tbg->add_band_widget(tb);

    return tbg;
}

const int* layout = nullptr;

void saveLayoutCb(Fl_Widget* w, void* d)
{
    Fle_Dock_Group* parent = (Fle_Dock_Group*)w->parent()->parent();
    Fle_Dock_Host* dh = parent->get_host();
    int size;
    if (layout) delete[] layout;
    layout = dh->save_layout(size);

    /*for (int i = 0; i < size; i++)
    {
        std::cout << layout[i] << ", ";
    }
    std::cout << std::endl;*/
}

void loadLayoutCb(Fl_Widget* w, void* d)
{
    if (!layout) return;

    Fle_Dock_Group* parent = (Fle_Dock_Group*)w->parent()->parent();
    Fle_Dock_Host* dh = parent->get_host();
    
    dh->load_layout(layout);
}

Fle_Listview* listview;

void view_cb(Fl_Widget* w, void* d)
{
    if (d == (void*)0) listview->set_display_mode(FLE_LISTVIEW_DISPLAY_ICONS);
    if (d == (void*)1) listview->set_display_mode(FLE_LISTVIEW_DISPLAY_SMALL_ICONS);
    if (d == (void*)2) listview->set_display_mode(FLE_LISTVIEW_DISPLAY_LIST);
    if (d == (void*)3) listview->set_display_mode(FLE_LISTVIEW_DISPLAY_DETAILS);
    if (d == (void*)4) listview->set_details_mode(FLE_LISTVIEW_DETAILS_NONE);
    if (d == (void*)5) listview->set_details_mode(FLE_LISTVIEW_DETAILS_LINES);
    if (d == (void*)6) listview->set_details_mode(FLE_LISTVIEW_DETAILS_HIGHLIGHT);
}

void colormap_cb(Fl_Widget *, void *v) {    // TEST: changed by Albrecht-S
    fl_show_colormap(fl_int(v));
}

Fle_Dock_Group* make_debug_toolbar(Fle_Dock_Host* dh)
{
    Fle_Dock_Group* tb = new Fle_Dock_Group(dh, 9, "Debug", FLE_DOCK_NO_HOR_LABEL | FLE_DOCK_DETACHABLE | FLE_DOCK_FLEXIBLE, FLE_DOCK_BOTTOM, FLE_DOCK_TOP, 270, 26, true);
    Fl_Group* tbg = new Fl_Group(0, 0, 85 * 5 + 2, 24);  // TEST: changed by Albrecht-S
    Fle_Flat_Button* btn1 = new Fle_Flat_Button(0, 0, 85, 24, "Save layout");
    btn1->callback(saveLayoutCb);
    Fle_Flat_Button* btn2 = new Fle_Flat_Button(85, 0, 85, 24, "Load layout");
    btn2->callback(loadLayoutCb);

    Fle_Scheme_Choice* schemeChoice = new Fle_Scheme_Choice(85 * 2, 0, 85, 24, "");
    Fle_Colors_Choice* colorsChoice = new Fle_Colors_Choice(85 * 3, 0, 85, 24, "");

    // TEST: added by Albrecht-S:
    Fl_Button* colorMap = new Fl_Button (85 * 4, 0, 85, 24, "colormap");
    colorMap->callback(colormap_cb, (void *)FL_YELLOW);
    colorMap->color(FL_YELLOW);
    colorMap->tooltip("show color map");  
    // disable the annoying focus rectangle in these small widgets
    schemeChoice->visible_focus(0);
    colorsChoice->visible_focus(0);
    colorMap->visible_focus(0);   
    Fl_Box* r = new Fl_Box(85 * 5, 0, 2, 24);   // TEST: changed by Albrecht-S

    tbg->resizable(r);
    tb->add_band_widget(tbg);

    return tb;
}

class Test_Listview_Item : public Fle_Listview_Item
{
    int m_sizeKB;
    std::string m_owner;

protected:
    bool is_greater(Fle_Listview_Item* other, int property) override
    {
        Test_Listview_Item* o = (Test_Listview_Item*)other;
        if (property == 0) return m_sizeKB > o->m_sizeKB;
        if (property == 1) return m_owner > o->m_owner;

        return Fle_Listview_Item::is_greater(other, property);
    }

    void draw_property(int i, int X, int Y, int W, int H) override
    {
        fl_color(textcolor());
        std::string s;
        if(i == 0)
        {
            s = std::to_string(m_sizeKB);
        }
        else if (i == 1)
        {
            s = m_owner;
        }
        fl_draw(s.c_str(), X, Y, W, H, FL_ALIGN_LEFT);
    }

public:

    Test_Listview_Item(const char* l, int kb, std::string o) : Fle_Listview_Item(l)
    {
        m_sizeKB = kb;
        m_owner = std::move(o);
    }
};

void listview_cb(Fl_Widget* w, void* d)
{
    Fle_Listview* lv = (Fle_Listview*)w;

    std::string callbackReason;

    Fle_Listview_Reason reason = (Fle_Listview_Reason)Fl::callback_reason();

    switch (reason)
    {
    case FLE_LISTVIEW_REASON_SELECTED:
		callbackReason = "selected";
		break;
	case FLE_LISTVIEW_REASON_DESELECTED:
		callbackReason = "deselected";
		break;
	case FLE_LISTVIEW_REASON_RESELECTED:
		callbackReason = "reselected";
		break;
    case FLE_LISTVIEW_REASON_ADDED:
		callbackReason = "added";
		break;
	case FLE_LISTVIEW_REASON_REMOVED:
		callbackReason = "removed";
		break;
    case FLE_LISTVIEW_REASON_DND_START:
        std::cout << "dnd start\n";
        Fl::copy("test", 4);
        return;
		break;
    case FLE_LISTVIEW_REASON_DND_END:
		std::cout << "dnd end\n";
        if(lv->get_item_drag_at(Fl::event_x(), Fl::event_y()) != nullptr)
            std::cout << "dragged on item\n";

        Fle_Listview_Item * item = new Test_Listview_Item(Fl::event_text(), 4, "Drag");
        item->bgcolor(FL_RED);
        lv->add_item(item);
		return;
		break;
    }

    std::cout << "Item " << lv->get_callback_item()->get_name() << " " << callbackReason << std::endl;
}

int main(int argc, char** argv)
{
    img = new Fl_Pixmap(ikona_xpm);

	Fl_Double_Window* win = new Fl_Double_Window(600, 600, "Fake File Manager Demo");

	Fle_Dock_Host* dh = new Fle_Dock_Host(0, 0, 600, 600, "Dock Host", FLE_DOCK_ALLDIRS);
    dh->set_min_size_callback(min_size_cb);
    //dh->set_preview_color(FL_MAGENTA);

    //Fl_Box* box = new Fl_Box(0, 0, 0, 0);
    //box->box(FL_DOWN_BOX);
    //box->color(FL_BACKGROUND2_COLOR);

	//dh->add_work_widget(box);

    //SchemeTest* st = new SchemeTest(0, 0, 0, 0, "");
    //dh->add_work_widget(st);

    listview = new Fle_Listview(0, 0, 0, 0, "");

    listview->callback(listview_cb);

    listview->set_property_widths({ 82, 82 });
    listview->set_property_order({ 1, 0 });
    listview->add_property_name("Size (KB)");
    listview->add_property_name("Owner");
    //listview->single_selection(true);

    {
        Test_Listview_Item* item;
        // LLama to the rescue again
        item = new Test_Listview_Item("model_2023.obj", 43, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("texture.png", 18, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("scene_2_14_2022.blend", 1024, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("script_01.py", 5, "root"); listview->add_item(item);
        item = new Test_Listview_Item("image_20210101.jpg", 512, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("doc_20230420.docx", 236, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("sound_effect.wav", 120, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("video_clip.mp4", 2048, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("report_q4_2022.pdf", 876, "root"); listview->add_item(item);
        item = new Test_Listview_Item("portrait.jpg", 300, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("level_design.txt", 12, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("icon_16x16.png", 3, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("project_plan_2023.xlsx", 458, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("backup_20221231.zip", 10240, "root"); listview->add_item(item);
        item = new Test_Listview_Item("certificate.p12", 6, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("manual_v3.2.pdf", 1500, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("source_code.tar.gz", 4096, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("avatar.jpg", 82, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("presentation_2023.pptx", 982, "root"); listview->add_item(item);
        item = new Test_Listview_Item("database.dump", 8192, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("invoice_20220101.pdf", 201, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("diagram.svg", 41, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("3d_model.stl", 1200, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("thesis_draft.docx", 1792, "root"); listview->add_item(item);
        item = new Test_Listview_Item("photo_album.zip", 3072, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("meeting_minutes_2023.docx", 56, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("icon_set.zip", 29, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("annual_report_2022.pdf", 2400, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("game_save.dat", 14, "root"); listview->add_item(item);
        item = new Test_Listview_Item("profile_picture.jpg", 91, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("chart.xlsx", 83, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("trailer.mp4", 4096, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("leaflet_v2.pdf", 192, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("source.tar", 6144, "root"); listview->add_item(item);
        item = new Test_Listview_Item("banner.png", 221, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("memo_20230210.docx", 17, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("font_pack.zip", 105, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("itinerary.pdf", 67, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("dataset.csv", 3008, "root"); listview->add_item(item);
        item = new Test_Listview_Item("screensaver.exe", 49, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("brochure_2023.pdf", 291, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("patch_note.txt", 7, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("project_timeline.gantt", 420, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("ebook.epub", 1024, "root"); listview->add_item(item);
        item = new Test_Listview_Item("image_gallery.zip", 2048, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("contract_20220101.pdf", 375, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("theme.css", 25, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("newsletter_jan2023.pdf", 138, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("backup_20221231.7z", 5120, "root"); listview->add_item(item);
        item = new Test_Listview_Item("portrait_2022.jpg", 119, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("spreadsheet_template.xlsx", 46, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("whitepaper_v2.pdf", 842, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("video_tutorial.mp4", 3072, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("game_trailer_2023.mp4", 4096, "root"); listview->add_item(item);
        item = new Test_Listview_Item("data_analysis.csv", 1892, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("press_release.docx", 63, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("icon_32x32.png", 4, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("brochure_design.ai", 921, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("database_schema.sql", 85, "root"); listview->add_item(item);
        item = new Test_Listview_Item("profile_picture_2023.jpg", 108, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("meeting_schedule.ics", 11, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("style_guide.pdf", 275, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("project_plan_v2.xlsx", 521, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("dataset_v2.csv", 3584, "root"); listview->add_item(item);
        item = new Test_Listview_Item("banner_ad.jpg", 251, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("minutes_of_meeting.docx", 39, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("update_patch.exe", 56, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("chart_template.pptx", 182, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("image_set.zip", 2048, "root"); listview->add_item(item);
        item = new Test_Listview_Item("thesis_final.docx", 1921, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("event_ticket.jpg", 93, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("soundtrack.mp3", 921, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("presentation_template.pptx", 182, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("database_backup.7z", 5120, "root"); listview->add_item(item);
        item = new Test_Listview_Item("product_catalog.xlsx", 46, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("video_tutorial_2023.mp4", 4096, "Fishy"); listview->add_item(item);
        item = new Test_Listview_Item("data_analysis_2023.csv", 1892, "CC"); listview->add_item(item);
        item = new Test_Listview_Item("press_release_2023.docx", 63, "2121"); listview->add_item(item);
        item = new Test_Listview_Item("icon_64x64.png", 8, "root"); listview->add_item(item);
        item = new Test_Listview_Item("brochure_design_2023.ai", 921, "Wald"); listview->add_item(item);
        item = new Test_Listview_Item("extremely_long_named_item_2021.txt", 921, "Wald"); listview->add_item(item);
    }

    dh->add_work_widget(listview);

	win->end();
	win->resizable(dh);

    vertGroup1 = new Fle_Dock_Group(dh, 1, "Folders", FLE_DOCK_DETACHABLE | FLE_DOCK_FLEXIBLE, FLE_DOCK_LEFT, FLE_DOCK_RIGHT, 120, 180, false);
    Fl_Tree* tree = make_tree();
	vertGroup1->add_band_widget(tree);

    Fle_Dock_Group* vertGroup4 = new Fle_Dock_Group(dh, 2, "Inflexible 1", FLE_DOCK_LOCKED | FLE_DOCK_DETACHABLE, FLE_DOCK_LEFT,  FLE_DOCK_RIGHT, 75, 180, false);
    Fl_Box* sthBox = new Fl_Box(0, 0, 0, 0, "Inflexible group 1");
    sthBox->box(FL_DOWN_BOX);
    sthBox->color(FL_BACKGROUND2_COLOR);
    vertGroup4->add_band_widget(sthBox);

    Fle_Dock_Group* vertGroup5 = new Fle_Dock_Group(dh, 3, "Accordion", FLE_DOCK_DETACHABLE | FLE_DOCK_FLEXIBLE, FLE_DOCK_LEFT, FLE_DOCK_RIGHT, 75, 180, false);
    Fle_Accordion* accordion = new Fle_Accordion(0, 0, 0, 0, "");
    

    for(int i = 0; i < 6; i++)
    {
        Fle_Accordion_Group* accordgroup = new Fle_Accordion_Group(new Fl_Box(0, 0, 0, 0, "Group"), 60, 90,  "Group");
        accordion->add_group(accordgroup);
        //accordgroup->open();
    }

    vertGroup5->add_band_widget(accordion);

    Fle_Dock_Group* vertGroup2 = new Fle_Dock_Group(dh, 4, "Favourites", FLE_DOCK_DETACHABLE | FLE_DOCK_FLEXIBLE, FLE_DOCK_LEFT, FLE_DOCK_RIGHT, 120, 180, false);
    Fl_Box* favs = new Fl_Box(0, 0, 0, 0, "");
    favs->box(FL_DOWN_BOX);
    favs->color(FL_BACKGROUND2_COLOR);
    vertGroup2->add_band_widget(favs);

    Fle_Dock_Group* vertGroup3 = new Fle_Dock_Group(dh, 5, "Inflexible 2", FLE_DOCK_DETACHABLE, FLE_DOCK_LEFT, FLE_DOCK_RIGHT, 220, 180, false);
    Fl_Box* large = new Fl_Box(0, 0, 0, 0, "Inflexible group 2");
    large->box(FL_DOWN_BOX);
    large->color(FL_BACKGROUND2_COLOR);
    vertGroup3->add_band_widget(large);

    Fle_Dock_Group* toolbar = new Fle_Dock_Group(dh, 6, "Menubar", FLE_DOCK_NO_HOR_LABEL | FLE_DOCK_FLEXIBLE, FLE_DOCK_TOP, FLE_DOCK_BOTTOM, 240, 26, true);
    FlatMenuBar* menu = new FlatMenuBar(0, 0, 0, 0, "");
    menu->add("File/Open");
    menu->add("File/Properties");
    menu->add("File/Quit");
    menu->add("Edit/Copy");
    menu->add("Edit/Paste");
    menu->add("Edit/Cut");
    menu->add("View/Big icons", 0, view_cb, (void*)0, FL_MENU_RADIO);
    menu->add("View/Small icons", 0, view_cb, (void*)1, FL_MENU_RADIO);
    menu->add("View/List", 0, view_cb, (void*)2, FL_MENU_RADIO);
    menu->add("View/Details", 0, view_cb, (void*)3, FL_MENU_RADIO | FL_MENU_DIVIDER | FL_MENU_VALUE);
    menu->add("View/None", 0, view_cb, (void*)4, FL_MENU_RADIO);
    menu->add("View/Display lines", 0, view_cb, (void*)5, FL_MENU_RADIO);
    menu->add("View/Display highlights", 0, view_cb, (void*)6, FL_MENU_RADIO | FL_MENU_VALUE);
    toolbar->add_band_widget(menu, 1, 2, 2, 2);

    make_toolbar(dh);

    Fle_Dock_Group* horGroup = new Fle_Dock_Group(dh, 8, "Address: ", FLE_DOCK_DETACHABLE | FLE_DOCK_FLEXIBLE, FLE_DOCK_TOP, FLE_DOCK_BOTTOM, 180, 26, true);
    Fl_Group* addressGroup = new Fl_Group(0, 0, 100, 30);
    Fl_Input* input = new Fl_Input(0, 0, 50, 30);
    input->append("/etc/network/");
    Fle_Flat_Button* btn = new Fle_Flat_Button(50, 0, 50, 30, "@redo  Go");
    addressGroup->end();
    addressGroup->resizable(input);
    horGroup->add_band_widget(addressGroup, 0, 1, 1, 1);

    make_debug_toolbar(dh);

    Fle_Dock_Group* tb = new Fle_Dock_Group(dh, 10, "Orientable Flex", FLE_DOCK_DETACHABLE | FLE_DOCK_TOOLBAR, FLE_DOCK_RIGHT, FLE_DOCK_ALLDIRS, 180, 60, true);

    Fle_Orientable_Flex* fof1 = new Fle_Orientable_Flex(Fl_Flex::HORIZONTAL);
    Fle_Orientable_Flex* fof2 = new Fle_Orientable_Flex(Fl_Flex::VERTICAL);
    for (int i = 0; i < 15; i += 2)
    {
        std::string* l = new std::string;
        *l = std::to_string(i);
        Fle_Flat_Button* nmb = new Fle_Flat_Button(0, 0, 0, 0, l->c_str());
        fof2->fixed(nmb, 25);
    }
    fof2->end();
    Fle_Orientable_Flex* fof3 = new Fle_Orientable_Flex(Fl_Flex::VERTICAL);
    for (int i = 1; i < 16; i += 2)
    {
        std::string* l = new std::string;
        *l = std::to_string(i);
        Fle_Flat_Button* nmb = new Fle_Flat_Button(0, 0, 0, 0, l->c_str());
        fof3->fixed(nmb, 25);
    }
    fof3->end();
    fof1->end();
    tb->add_band_widget(fof1);

    int s;
    layout = dh->save_layout(s);

    //setenv("LIBDECOR_FORCE_CSD", "1", 1);

    win->show(argc, argv);

    return Fl::run();
}