#ifndef _GUI_H_
#define _GUI_H_

#include <vector>

#include "SDL.h"
#include "SDL_ttf.h"

#include "main.h"

class GUIElement{
public:
	SDL_Rect position;

	virtual bool draw() = 0;
};

class Sprite : public GUIElement{
public:
	SDL_Texture* texture;
	float angle, scale;

	virtual bool draw();

	virtual ~Sprite(){
        if(texture) SDL_DestroyTexture(texture);
        texture = nullptr;
    }
};

class InputBox : public GUIElement{
public:
	TTF_Font *font;
	std::string text;
	std::string composition;
	SDL_Color background, textcolor;
	bool active;
	SDL_Texture* box;

	InputBox()
    : background{0xFF, 0xFF, 0xFF, 0xFF}, textcolor{0x00, 0x00, 0x00, 0xFF}, active(false)
	{
	    int w, h;
        SDL_GetWindowSize(window, &w, &h);
	    font = TTF_OpenFont(std::string("bold.ttf").c_str(), w/22.5f);
    }

	~InputBox() {
		TTF_CloseFont(font);
		font = nullptr;
    }

	virtual bool draw();
};

class SelectionBox : public GUIElement{
public:
    SDL_Color boxColor, textColor;
    TTF_Font *font;
    std::string text;
    bool selected, drawBox, centered, right;
    SDL_Texture *box;
    SDL_Rect textPos;

    SelectionBox()
    : boxColor{0xA0, 0xA0, 0xA0, 0xFF}, textColor{0xFF, 0xFF, 0xFF, 0xFF},
    font(nullptr), text(""), selected(false), box(nullptr), drawBox(true), centered(false), textPos{0,0,0,0}
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
    }

    virtual ~SelectionBox() {
        //if(font) TTF_CloseFont(font);
        //if(box) SDL_DestroyTexture(box);
        //box = nullptr;
        //font = nullptr;
    }

    virtual bool draw();
};

class RadioButton : public SelectionBox{
protected:
    SelectionBox radioBox;
    SelectionBox textBox;

public:
    SDL_Texture* radio;

    bool draw();
};

class FlightCard : public SelectionBox{
protected:
    SelectionBox dateBox;
    SelectionBox retDateBox;
    SelectionBox destBox;
    SelectionBox nonStopBox;
    SelectionBox priceBox;
    SelectionBox depFromBox;

public:
    std::string dest;
    std::string date;
    std::string retDate;
    std::string depFrom;
    int nonStop;
    int price;

    bool draw();
};

class HList : public GUIElement{
protected:

public:
    int select, lastmove;
    std::vector<FlightCard*> elements;
    float accel[2], vel[2];
    bool moveElement, begin, infinitescroll, snap;

    HList() : select(0), accel{}, vel{}, moveElement(true), begin(true), infinitescroll(false), snap(true) {}

    virtual bool draw();
    virtual bool moveElements(int aax, int aay);

    virtual ~HList(){
        for(auto& s : elements) delete s;
    }
};

class VList : public HList{
    SDL_Texture *tText;

public:
    VList() {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        tText = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    }

    virtual bool draw();
    virtual bool moveElements(int aax, int aay);
};
#endif // _GUI_H_
