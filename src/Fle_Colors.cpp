#include <FLE/Fle_Colors.hpp>

#include <array>
#include <vector>
#include <string>
#include <algorithm>

using uint8 = unsigned char;

struct Fle_Color_Theme
{
    std::string name;
    
    unsigned char gray32;
    unsigned char gray55;
    unsigned char cube_max;

    unsigned char backgroundR;
    unsigned char backgroundG;
    unsigned char backgroundB;

    unsigned char background2R;
    unsigned char background2G;
    unsigned char background2B;

    unsigned char foregroundR;
    unsigned char foregroundG;
    unsigned char foregroundB;

    unsigned char selectionR;
    unsigned char selectionG;
    unsigned char selectionB;
};

void(*g_colorsChangedCB)(void*) = nullptr;
void* g_colorsChangedCBData = nullptr;

int  g_currentColors = 0;
std::vector<Fle_Colors_Choice*> g_colorChoices;
std::vector<Fle_Color_Theme> g_colorThemes =
{
    {"default", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {"light", 40, 200, 255, 235, 235, 235, 255, 255, 255, 55, 55, 55, 0, 120, 180},
    {"dark1", 110, 40, 110, 55, 55, 55, 75, 75, 75, 235, 235, 235, 0, 120, 180},
    {"dark2", 180, 40, 110, 35, 35, 40, 26, 26, 30, 235, 235, 235, 0, 120, 180},
    {"tan", 40, 200, 255, 195, 195, 181, 243, 243, 243, 55, 55, 55, 0, 0, 175},
    {"dark_tan", 180, 40, 180, 165, 165, 151, 223, 223, 223, 55, 55, 55, 0, 0, 175},
    {"marine", 40, 200, 255, 136, 192, 184, 200, 224, 216, 55, 55, 55, 0, 0, 128},
    {"blueish", 40, 200, 255, 210, 213, 215, 255, 255, 255, 55, 55, 55, 0, 0, 128},
    {"nord", 180, 40, 110, 41, 46, 57, 59, 66, 82, 235, 235, 235, 0, 120, 180},
    {"gray", 40, 200, 255, 192, 192, 192, 255, 255, 255, 0, 0, 0, 0, 0, 128},

    {"high_contrast", 180, 40, 110, 0, 0, 0, 20, 20, 20, 255, 255, 255, 0, 120, 255},
    {"forest", 180, 40, 110, 34, 51, 34, 46, 79, 46, 200, 200, 200, 64, 224, 208},
    {"purple_dusk", 180, 40, 110, 48, 25, 52, 72, 50, 72, 220, 220, 220, 255, 105, 180},
    {"solarized_light", 40, 200, 255, 253, 246, 227, 238, 232, 213, 101, 123, 131, 38, 139, 210},
    {"solarized_dark", 180, 40, 110, 0, 43, 54, 7, 54, 66, 131, 148, 150, 211, 54, 130},
    {"monokai", 180, 40, 110, 39, 40, 34, 51, 52, 46, 249, 249, 249, 152, 159, 177},
    {"gruvbox_light", 40, 200, 255, 251, 237, 193, 235, 219, 178, 56, 52, 46, 69, 133, 137},
    {"gruvbox_dark", 110, 40, 110, 40, 40, 40, 60, 60, 60, 220, 208, 184, 131, 165, 163},
    {"dracula", 180, 40, 110, 40, 42, 54, 68, 71, 90, 248, 248, 242, 189, 147, 249},
    {"oceanic_next", 180, 40, 110, 45, 52, 54, 60, 68, 70, 220, 220, 220, 99, 184, 219},
    {"minimalist", 40, 200, 255, 240, 240, 240, 230, 230, 230, 50, 50, 50, 100, 149, 237},
    {"autumn", 40, 200, 255, 245, 245, 220, 230, 230, 200, 80, 50, 10, 255, 165, 0},
    {"cyberpunk", 180, 40, 110, 30, 30, 75, 20, 20, 50, 0, 255, 0, 255, 0, 255},
    {"material_dark", 180, 40, 110, 28, 28, 28, 40, 40, 40, 255, 255, 255, 0, 122, 255},
    {"mint", 40, 200, 255, 200, 240, 200, 180, 220, 180, 50, 100, 50, 0, 255, 0},
    {"vintage", 40, 200, 255, 240, 230, 200, 220, 210, 180, 80, 50, 30, 255, 215, 0},
};

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
    fle_set_colors(g_colorThemes[(std::size_t)data].name.c_str());
}

Fle_Colors_Choice::Fle_Colors_Choice(int X, int Y, int W, int H, const char* l) : Fl_Choice(X, Y, W, H, l)
{
    for(size_t i = 0; i < g_colorThemes.size(); i++)
    {
        add(g_colorThemes[i].name.c_str(), "", fle_colors_choice_cb, (void*)i);
    }

    value(g_currentColors);

    g_colorChoices.push_back(this);
}

Fle_Colors_Choice::~Fle_Colors_Choice()
{
    g_colorChoices.erase(std::find(g_colorChoices.begin(), g_colorChoices.end(), this));
}

void fle_set_colors(const char* colors)
{
    if(strcmp(colors, "default") == 0)
    {
        reload_color_map();
        g_currentColors = 0;
    }
    else for (size_t i = 0; i < g_colorThemes.size(); ++i)
    {
        if (strcmp(g_colorThemes[i].name.c_str(), colors) == 0)
        {
            prep_theme(g_colorThemes[i].gray32, g_colorThemes[i].gray55, g_colorThemes[i].cube_max);
            Fl::background(g_colorThemes[i].backgroundR, g_colorThemes[i].backgroundG, g_colorThemes[i].backgroundB);
            Fl::background2(g_colorThemes[i].background2R, g_colorThemes[i].background2G, g_colorThemes[i].background2B);
            Fl::foreground(g_colorThemes[i].foregroundR, g_colorThemes[i].foregroundG, g_colorThemes[i].foregroundB);
            Fl::set_color(FL_SELECTION_COLOR, g_colorThemes[i].selectionR, g_colorThemes[i].selectionG, g_colorThemes[i].selectionB);
            g_currentColors = static_cast<int>(i);
            break;
        }
    }

    for (auto it = g_colorChoices.begin(); it != g_colorChoices.end(); ++it)
    {
        (*it)->value(g_currentColors);
    }

    if(g_colorsChangedCB)
        g_colorsChangedCB(g_colorsChangedCBData);

    Fl::redraw();
}

void fle_colors_changed_callback(void(*cb)(void*), void* data)
{
	g_colorsChangedCB = cb;
	g_colorsChangedCBData = data;
}

const char* fle_get_colors()
{
    return g_colorThemes[g_currentColors].name.c_str();
}