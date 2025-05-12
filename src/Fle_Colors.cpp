#include <FLE/Fle_Colors.hpp>

#include <array>
#include <vector>
#include <cstring>
#include <algorithm>

using uint8 = unsigned char;

int  g_currentColors = 0;
std::vector <Fle_Colors_Choice*> g_colorChoices;
static std::array<Fl_Color, 256> default_color_map;

static void reload_color_map() 
{
    static bool init = false;
    if (!init) 
    {
        for (unsigned int i = 0; i < 256; i++) 
        {
            default_color_map[i] = Fl::get_color(i);
        }
        init = true;
    }
    
    for (unsigned int i = 0; i < 256; i++) 
    {
        Fl::set_color(i, default_color_map[i]);
    }
}

constexpr uint8 gray_ramp(uint8 dark, uint8 light, uint8 n)
{
    return static_cast<uint8>(
        dark + ((light - dark) * n + 11) / 23);
}

constexpr uint8 gray_ramp_inv(uint8 light, uint8 dark, uint8 n)
{
    return static_cast<uint8>(
        light - ((light - dark) * n + 11) / 23);
}

static void make_dark_ramp(uint8 light, uint8 dark) 
{
    for (unsigned int i = 32; i < 56; i++) 
    {
        uint8 v = gray_ramp_inv(light, dark, i - 32);
        Fl::set_color(i, v, v, v);
    }
}

static void make_light_ramp(uint8 dark, uint8 light) 
{
    for (unsigned int i = 32; i < 56; i++) 
    {
        uint8 v = gray_ramp(dark, light, i - 32);
        Fl::set_color(i, v, v, v);
    }
}

constexpr uint8 cube_chan(uint8 i,  uint8 steps, uint8 max) 
{
    return static_cast<uint8>(
        (i * max + (steps - 1) / 2) / (steps - 1));
}

static void make_color_cube(uint8 cube_max) 
{
    for (unsigned int i = 56; i < 256; i++) 
    {
        uint8 n = i - 56;
        uint8 b = n / (5 * 8);
        uint8 r = (n / 8) % 5;
        uint8 g = n % 8;
        Fl::set_color(i, cube_chan(r, 5, cube_max), cube_chan(g, 8, cube_max), cube_chan(b, 5, cube_max));
    }
}

static void prep_theme(uint8 gray32, uint8 gray55, uint8 cube_max) 
{
    reload_color_map();
    if (gray32 > gray55) 
    {
        make_dark_ramp(gray32, gray55);
    } else 
    {
        make_light_ramp(gray32, gray55);
    }
    make_color_cube(cube_max);
}

void fle_colors_choice_cb(Fl_Widget* w, void* data)
{
    switch ((std::size_t)data)
    {
    case 0:
        fle_set_colors("default");
        break;
    case 1:
        fle_set_colors("light");
        break;
    case 2:
        fle_set_colors("dark1");
        break;
    case 3:
        fle_set_colors("dark2");
        break;
    case 4:
        fle_set_colors("tan");
        break;
    case 5:
        fle_set_colors("dark_tan");
        break;
    case 6:
        fle_set_colors("marine");
        break;
    case 7:
        fle_set_colors("blueish");
        break;
    case 8:
        fle_set_colors("nord");
        break;
    case 9:
        fle_set_colors("high_contrast");
        break;
    case 10:
        fle_set_colors("forest");
        break;
    case 11:
        fle_set_colors("purple_dusk");
        break;
    case 12:
        fle_set_colors("solarized_light");
        break;
    case 13:
        fle_set_colors("solarized_dark");
        break;
    case 14:
        fle_set_colors("monokai");
        break;
    case 15:
        fle_set_colors("gruvbox_light");
        break;
    case 16:
        fle_set_colors("gruvbox_dark");
        break;
    case 17:
        fle_set_colors("dracula");
        break;
    case 18:
        fle_set_colors("oceanic_next");
        break;
    case 19:
        fle_set_colors("minimalist");
        break;
    case 20:
        fle_set_colors("autumn");
        break;
    case 21:
        fle_set_colors("cyberpunk");
        break;
    case 22:
        fle_set_colors("material_dark");
        break;
    case 23:
        fle_set_colors("mint");
        break;
    case 24:
        fle_set_colors("vintage");
        break;
    case 25:
        fle_set_colors("gray");
        break;
    }
}

Fle_Colors_Choice::Fle_Colors_Choice(int X, int Y, int W, int H, const char* l) : Fl_Choice(X, Y, W, H, l)
{
    add("default", "", fle_colors_choice_cb, (void*)0);
    add("light", "", fle_colors_choice_cb, (void*)1);
    add("dark1", "", fle_colors_choice_cb, (void*)2);
    add("dark2", "", fle_colors_choice_cb, (void*)3);
    add("tan", "", fle_colors_choice_cb, (void*)4);
    add("dark_tan", "", fle_colors_choice_cb, (void*)5);
    add("marine", "", fle_colors_choice_cb, (void*)6);
    add("blueish", "", fle_colors_choice_cb, (void*)7);
    add("nord", "", fle_colors_choice_cb, (void*)8);
    add("high_contrast", "", fle_colors_choice_cb, (void*)9);
    add("forest", "", fle_colors_choice_cb, (void*)10);
    add("purple_dusk", "", fle_colors_choice_cb, (void*)11);
    add("solarized_light", "", fle_colors_choice_cb, (void*)12);
    add("solarized_dark", "", fle_colors_choice_cb, (void*)13);
    add("monokai", "", fle_colors_choice_cb, (void*)14);
    add("gruvbox_light", "", fle_colors_choice_cb, (void*)15);
    add("gruvbox_dark", "", fle_colors_choice_cb, (void*)16);
    add("dracula", "", fle_colors_choice_cb, (void*)17);
    add("oceanic_next", "", fle_colors_choice_cb, (void*)18);
    add("minimalist", "", fle_colors_choice_cb, (void*)19);
    add("autumn", "", fle_colors_choice_cb, (void*)20);
    add("cyberpunk", "", fle_colors_choice_cb, (void*)21);
    add("material_dark", "", fle_colors_choice_cb, (void*)22);
    add("mint", "", fle_colors_choice_cb, (void*)23);
    add("vintage", "", fle_colors_choice_cb, (void*)24);
    add("gray", "", fle_colors_choice_cb, (void*)25);

    value(g_currentColors);

    g_colorChoices.push_back(this);
}

Fle_Colors_Choice::~Fle_Colors_Choice()
{
    g_colorChoices.erase(std::find(g_colorChoices.begin(), g_colorChoices.end(), this));
}

void fle_set_colors(const char* colors)
{
    reload_color_map();
    if (strcmp("light", colors) == 0)
    {
        prep_theme(40, 200, 255);
        Fl::background(235, 235, 235);
        Fl::background2(255, 255, 255);
        Fl::foreground(55, 55, 55);
        Fl::set_color(FL_SELECTION_COLOR, 0, 120, 180);
        g_currentColors = 1;
    }
    else if (strcmp("dark1", colors) == 0)
    {
        prep_theme(110, 40, 110);
        Fl::background(55, 55, 55);
        Fl::background2(75, 75, 75);
        Fl::foreground(235, 235, 235);
        Fl::set_color(FL_SELECTION_COLOR, 0, 120, 180);
        g_currentColors = 2;
    }
    else if (strcmp("dark2", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(35, 35, 40);
        Fl::background2(26, 26, 30);
        Fl::foreground(235, 235, 235);
        Fl::set_color(FL_SELECTION_COLOR, 0, 120, 180);
        g_currentColors = 3;
    }
    else if (strcmp("tan", colors) == 0)
    {
        prep_theme(40, 200, 255);
        Fl::background(195, 195, 181);
        Fl::background2(243, 243, 243);
        Fl::foreground(55, 55, 55);
        Fl::set_color(FL_SELECTION_COLOR, 0, 0, 175);
        g_currentColors = 4;
    }
    else if (strcmp("dark_tan", colors) == 0)
    {
        prep_theme(180, 40, 180);
        Fl::background(165, 165, 151);
        Fl::background2(223, 223, 223);
        Fl::foreground(55, 55, 55);
        Fl::set_color(FL_SELECTION_COLOR, 0, 0, 175);
        g_currentColors = 5;
    }
    else if (strcmp("marine", colors) == 0)
    {
        prep_theme(40, 200, 255);
        Fl::background(136, 192, 184);
        Fl::background2(200, 224, 216);
        Fl::foreground(55, 55, 55);
        Fl::set_color(FL_SELECTION_COLOR, 0, 0, 128);
        g_currentColors = 6;
    }
    else if (strcmp("blueish", colors) == 0)
    {
        prep_theme(40, 200, 255);
        Fl::background(210, 213, 215);
        Fl::background2(255, 255, 255);
        Fl::foreground(55, 55, 55);
        Fl::set_color(FL_SELECTION_COLOR, 0, 0, 128);
        g_currentColors = 7;
    }
    else if (strcmp("nord", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(41, 46, 57);
        Fl::background2(59, 66, 82);
        Fl::foreground(235, 235, 235);
        Fl::set_color(FL_SELECTION_COLOR, 0, 120, 180);
        g_currentColors = 8;
    }
    else if (strcmp("high_contrast", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(0, 0, 0);
        Fl::background2(20, 20, 20);
        Fl::foreground(255, 255, 255);
        Fl::set_color(FL_SELECTION_COLOR, 0, 120, 255);
        g_currentColors = 9;
    }
    else if (strcmp("forest", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(34, 51, 34);
        Fl::background2(46, 79, 46);
        Fl::foreground(200, 200, 200);
        Fl::set_color(FL_SELECTION_COLOR, 64, 224, 208);
        g_currentColors = 10;
    }
    else if (strcmp("purple_dusk", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(48, 25, 52);
        Fl::background2(72, 50, 72);
        Fl::foreground(220, 220, 220);
        Fl::set_color(FL_SELECTION_COLOR, 255, 105, 180);
        g_currentColors = 11;
    }
    else if (strcmp("solarized_light", colors) == 0)
    {
        prep_theme(40, 200, 255);
        Fl::background(253, 246, 227);
        Fl::background2(238, 232, 213);
        Fl::foreground(101, 123, 131);
        Fl::set_color(FL_SELECTION_COLOR, 38, 139, 210);
        g_currentColors = 12;
    }
    else if (strcmp("solarized_dark", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(0, 43, 54);
        Fl::background2(7, 54, 66);
        Fl::foreground(131, 148, 150);
        Fl::set_color(FL_SELECTION_COLOR, 211, 54, 130);
        g_currentColors = 13;
    }
    else if (strcmp("monokai", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(39, 40, 34);
        Fl::background2(51, 52, 46);
        Fl::foreground(249, 249, 249);
        Fl::set_color(FL_SELECTION_COLOR, 152, 159, 177);
        g_currentColors = 14;
    }
    else if (strcmp("gruvbox_light", colors) == 0)
    {
        prep_theme(40, 200, 255);
        Fl::background(251, 237, 193);
        Fl::background2(235, 219, 178);
        Fl::foreground(56, 52, 46);
        Fl::set_color(FL_SELECTION_COLOR, 69, 133, 137);
        g_currentColors = 15;
    }
    else if (strcmp("gruvbox_dark", colors) == 0)
    {
        prep_theme(110, 40, 110);
        Fl::background(40, 40, 40);
        Fl::background2(60, 60, 60);
        Fl::foreground(220, 208, 184);
        Fl::set_color(FL_SELECTION_COLOR, 131, 165, 163);
        g_currentColors = 16;
    }
    else if (strcmp("dracula", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(40, 42, 54);
        Fl::background2(68, 71, 90);
        Fl::foreground(248, 248, 242);
        Fl::set_color(FL_SELECTION_COLOR, 189, 147, 249);
        g_currentColors = 17;
    }
    else if (strcmp("oceanic_next", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(45, 52, 54);
        Fl::background2(60, 68, 70);
        Fl::foreground(220, 220, 220);
        Fl::set_color(FL_SELECTION_COLOR, 99, 184, 219);
        g_currentColors = 18;
    }
    else if (strcmp("minimalist", colors) == 0)
    {
        prep_theme(40, 200, 255);   
        Fl::background(240, 240, 240); 
        Fl::background2(230, 230, 230);
        Fl::foreground(50, 50, 50);
        Fl::set_color(FL_SELECTION_COLOR, 100, 149, 237);
        g_currentColors = 19;
    }
    else if (strcmp("autumn", colors) == 0)
    {
        prep_theme(40, 200, 255);
        Fl::background(245, 245, 220);
        Fl::background2(230, 230, 200);
        Fl::foreground(80, 50, 10);
        Fl::set_color(FL_SELECTION_COLOR, 255, 165, 0);
        g_currentColors = 20;
    }
    else if (strcmp("cyberpunk", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(30, 30, 75); 
        Fl::background2(20, 20, 50);
        Fl::foreground(0, 255, 0);
        Fl::set_color(FL_SELECTION_COLOR, 255, 0, 255);
        g_currentColors = 21;
    }
    else if (strcmp("material_dark", colors) == 0)
    {
        prep_theme(180, 40, 110);
        Fl::background(28, 28, 28);
        Fl::background2(40, 40, 40);
        Fl::foreground(255, 255, 255);
        Fl::set_color(FL_SELECTION_COLOR, 0, 122, 255);
        g_currentColors = 22;
    }
    else if (strcmp("mint", colors) == 0)
    {
        prep_theme(40, 200, 255);
        Fl::background(200, 240, 200); 
        Fl::background2(180, 220, 180);
        Fl::foreground(50, 100, 50);
        Fl::set_color(FL_SELECTION_COLOR, 0, 255, 0);
        g_currentColors = 23;
    }
    else if (strcmp("vintage", colors) == 0)
    {
        prep_theme(40, 200, 255);
        Fl::background(240, 230, 200);
        Fl::background2(220, 210, 180);
        Fl::foreground(80, 50, 30);
        Fl::set_color(FL_SELECTION_COLOR, 255, 215, 0);
        g_currentColors = 24;
    }
    else if (strcmp("gray", colors) == 0)
    {
        prep_theme(40, 200, 255);
        Fl::background(192, 192, 192);
        Fl::background2(255, 255, 255);
        Fl::foreground(0, 0, 0);
        Fl::set_color(FL_SELECTION_COLOR, 0, 0, 128);
        g_currentColors = 25;
    }
    else
    {
        g_currentColors = 0;
    }

    for (auto it = g_colorChoices.begin(); it != g_colorChoices.end(); ++it)
    {
        (*it)->value(g_currentColors);
    }

    Fl::redraw();
}