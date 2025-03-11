#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Scheme_Choice.H>

#include <iostream>

#include "FLE/Fle_Dock_Host.hpp"
#include "FLE/Fle_Events.hpp"
#include "FLE/Fle_Orientable_Flex.hpp"
#include "FLE/Fle_Toolbar.hpp"
#include "FLE/Fle_Listview.hpp"
#include "FLE/Fle_Listview_Item.hpp"

#define FLE_BOXTYPES_BEGIN FL_FREE_BOXTYPE
#include "FLE/Fle_Schemes.hpp"

static char* ikona_xpm[] = {
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

void listview_cb(Fl_Widget* w, void* d)
{
    if (d == (void*)0) listview->set_display_mode(FLE_LISTVIEW_DISPLAY_ICONS);
    if (d == (void*)1) listview->set_display_mode(FLE_LISTVIEW_DISPLAY_SMALL_ICONS);
    if (d == (void*)2) listview->set_display_mode(FLE_LISTVIEW_DISPLAY_LIST);
    if (d == (void*)3) listview->set_display_mode(FLE_LISTVIEW_DISPLAY_DETAILS);
}

Fle_Dock_Group* make_debug_toolbar(Fle_Dock_Host* dh)
{
    Fle_Dock_Group* tb = new Fle_Dock_Group(dh, 9, "Debug", FLE_DOCK_NO_HOR_LABEL | FLE_DOCK_DETACHABLE | FLE_DOCK_FLEXIBLE, FLE_DOCK_BOTTOM, FLE_DOCK_TOP, 270, 26, true);
    Fl_Group* tbg = new Fl_Group(0, 0, 85 * 3 + 2, 24);
    Fle_Flat_Button* btn1 = new Fle_Flat_Button(0, 0, 85, 24, "Save layout");
    btn1->callback(saveLayoutCb);
    Fle_Flat_Button* btn2 = new Fle_Flat_Button(85, 0, 85, 24, "Load layout");
    btn2->callback(loadLayoutCb);

    Fl_Scheme_Choice* schemeChoice = new Fl_Scheme_Choice(85 * 2, 0, 85, 24, "");

    Fl_Box* r = new Fl_Box(85 * 3, 0, 2, 24);
    tbg->resizable(r);

    tb->add_band_widget(tbg);

    return tb;
}

int main(int argc, char** argv)
{
    img = new Fl_Pixmap(ikona_xpm);
    fle_define_schemes();

#ifndef WIN32
    // make it look "nice" and modernish
    Fl::scheme("oxy");
    Fl::background(235, 235, 235);
    Fl::background2(255, 255, 255);
    Fl::foreground(55, 55, 55);
    Fl::set_color(FL_SELECTION_COLOR, 0, 120, 180);
#endif
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
    //listview->single_selection(true);

    {
        Fle_Listview_Item* item;
        item = new Fle_Listview_Item("report_data_v1.csv"); listview->add_item(item);
        item = new Fle_Listview_Item("image_001.png"); listview->add_item(item);
        item = new Fle_Listview_Item("config.ini"); listview->add_item(item);
        item = new Fle_Listview_Item("backup_20231225.zip"); listview->add_item(item);
        item = new Fle_Listview_Item("document_draft.docx"); listview->add_item(item);
        item = new Fle_Listview_Item("audio_track.mp3"); listview->add_item(item);
        item = new Fle_Listview_Item("video_clip.mp4"); listview->add_item(item);
        item = new Fle_Listview_Item("script.py"); listview->add_item(item);
        item = new Fle_Listview_Item("database_dump.sql"); listview->add_item(item);
        item = new Fle_Listview_Item("texture_02.jpg"); listview->add_item(item);
        item = new Fle_Listview_Item("settings.json"); listview->add_item(item);
        item = new Fle_Listview_Item("readme.md"); listview->add_item(item);
        item = new Fle_Listview_Item("project_proposal.pdf"); listview->add_item(item);
        item = new Fle_Listview_Item("source_code.cpp"); listview->add_item(item);
        item = new Fle_Listview_Item("level_data.dat"); listview->add_item(item);
        item = new Fle_Listview_Item("user_profile.xml"); listview->add_item(item);
        item = new Fle_Listview_Item("animation_01.gif"); listview->add_item(item);
        item = new Fle_Listview_Item("temp_file.tmp"); listview->add_item(item);
        item = new Fle_Listview_Item("error_log_01.txt"); listview->add_item(item);
        item = new Fle_Listview_Item("system_report.html"); listview->add_item(item);
        item = new Fle_Listview_Item("model_weights.bin"); listview->add_item(item);
        item = new Fle_Listview_Item("game_save.sav"); listview->add_item(item);
        item = new Fle_Listview_Item("resource_pack.zip"); listview->add_item(item);
        item = new Fle_Listview_Item("translation_en.txt"); listview->add_item(item);
        item = new Fle_Listview_Item("carp.obj"); listview->add_item(item);
        item = new Fle_Listview_Item("shader_code.glsl"); listview->add_item(item);
        item = new Fle_Listview_Item("test_results.log"); listview->add_item(item);
        item = new Fle_Listview_Item("input_data.in"); listview->add_item(item);
        item = new Fle_Listview_Item("output_file.out"); listview->add_item(item);
        item = new Fle_Listview_Item("archive_001.tar.gz"); listview->add_item(item);
        item = new Fle_Listview_Item("website_index.php"); listview->add_item(item);
        item = new Fle_Listview_Item("font_file.ttf"); listview->add_item(item);
        item = new Fle_Listview_Item("custom_style.css"); listview->add_item(item);
        item = new Fle_Listview_Item("plugin_01.dll"); listview->add_item(item);
        item = new Fle_Listview_Item("map_data.json"); listview->add_item(item);
        item = new Fle_Listview_Item("calibration_data.cal"); listview->add_item(item);
        item = new Fle_Listview_Item("firmware_update.bin"); listview->add_item(item);
        item = new Fle_Listview_Item("license_agreement.txt"); listview->add_item(item);
        item = new Fle_Listview_Item("3d_model.obj"); listview->add_item(item);
        item = new Fle_Listview_Item("particle_effect.pfx"); listview->add_item(item);
        item = new Fle_Listview_Item("key_bindings.cfg"); listview->add_item(item);
        item = new Fle_Listview_Item("material_library.mat"); listview->add_item(item);
        item = new Fle_Listview_Item("animation_data.anim"); listview->add_item(item);
        item = new Fle_Listview_Item("character_sheet.chr"); listview->add_item(item);
        item = new Fle_Listview_Item("patch_file.patch"); listview->add_item(item);
        item = new Fle_Listview_Item("voice_recording.wav"); listview->add_item(item);
        item = new Fle_Listview_Item("network_config.net"); listview->add_item(item);
        item = new Fle_Listview_Item("ui_layout.ui"); listview->add_item(item);
        item = new Fle_Listview_Item("compressed_data.lz"); listview->add_item(item);
        item = new Fle_Listview_Item("encrypted_file.enc"); listview->add_item(item);
        item = new Fle_Listview_Item("metadata.meta"); listview->add_item(item);
        item = new Fle_Listview_Item("thumbnail.thumb"); listview->add_item(item);
        item = new Fle_Listview_Item("backup_02_20240101.tar"); listview->add_item(item);
        item = new Fle_Listview_Item("debug_log_002.txt"); listview->add_item(item);
        item = new Fle_Listview_Item("data_export.jsonl"); listview->add_item(item);
        item = new Fle_Listview_Item("render_output.exr"); listview->add_item(item);
        item = new Fle_Listview_Item("physics_data.phys"); listview->add_item(item);
        item = new Fle_Listview_Item("collision_mesh.cmesh"); listview->add_item(item);
        item = new Fle_Listview_Item("dialogue_tree.dtree"); listview->add_item(item);
        item = new Fle_Listview_Item("environment_map.hdr"); listview->add_item(item);
        item = new Fle_Listview_Item("light_profile.lprof"); listview->add_item(item);
        item = new Fle_Listview_Item("ai_model.onnx"); listview->add_item(item);
        item = new Fle_Listview_Item("timeline_data.tline"); listview->add_item(item);
        item = new Fle_Listview_Item("voxel_data.vox"); listview->add_item(item);
        item = new Fle_Listview_Item("heightmap.hmap"); listview->add_item(item);
        item = new Fle_Listview_Item("spline_data.spl"); listview->add_item(item);
        item = new Fle_Listview_Item("procedural_texture.ptx"); listview->add_item(item);
        item = new Fle_Listview_Item("behavior_tree.bt"); listview->add_item(item);
        item = new Fle_Listview_Item("navigation_mesh.nav"); listview->add_item(item);
        item = new Fle_Listview_Item("localization_data.loc"); listview->add_item(item);
        item = new Fle_Listview_Item("custom_filter.flt"); listview->add_item(item);
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

    Fle_Dock_Group* vertGroup5 = new Fle_Dock_Group(dh, 3, "Sth", FLE_DOCK_DETACHABLE | FLE_DOCK_FLEXIBLE, FLE_DOCK_LEFT, FLE_DOCK_RIGHT, 75, 180, false);
    Fl_Box* sthBox1 = new Fl_Box(0, 0, 0, 0, "Something");
    sthBox1->box(FL_DOWN_BOX);
    sthBox1->color(FL_BACKGROUND2_COLOR);
    vertGroup5->add_band_widget(sthBox1);

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
    menu->add("View/Big icons", 0, listview_cb, (void*)0);
    menu->add("View/Small icons", 0, listview_cb, (void*)1);
    menu->add("View/List", 0, listview_cb, (void*)2);
    menu->add("View/Details", 0, listview_cb, (void*)3);
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

#ifdef WIN32
    win->show(argc, argv);
#else
    win->show();
#endif

    //fle_set_scheme2();

    return Fl::run();
}