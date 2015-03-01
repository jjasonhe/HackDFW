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
    text(""), selected(false), box(nullptr), drawBox(true), centered(false), textPos{0,0,0,0}
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        font = TTF_OpenFont(std::string("rimouski.ttf").c_str(), h/32);
    }

    ~SelectionBox() {
        TTF_CloseFont(font);
        if(box) SDL_DestroyTexture(box);
        box = nullptr;
        font = nullptr;
    }

    virtual bool draw();
};

class Card : public SelectionBox{
public:
    SDL_Texture* face, *gBorder;
    bool drawCard, border;
    std::string replaceText;
    int faceSize;

    Card() : face(nullptr), drawCard(true), border(true), gBorder(loadImage("gBorder.png")), faceSize(0) {}

    virtual bool draw();

    virtual ~Card(){
        if(font) TTF_CloseFont(font);
        if(box) SDL_DestroyTexture(box);
        if(face) SDL_DestroyTexture(face);
        box = nullptr;
        font = nullptr;
        face = nullptr;
    }
};

class Toggle : public SelectionBox{
public:
    std::string text2;
    SDL_Color toggleColor;
    SDL_Texture *back;

    Toggle() : text2(""), toggleColor{0x30, 0x30, 0xC0}, back(nullptr) {}

    virtual bool draw();

    virtual ~Toggle(){
        TTF_CloseFont(font);
        if(box) SDL_DestroyTexture(box);
        if(back) SDL_DestroyTexture(back);
        back = nullptr;
        box = nullptr;
        font = nullptr;
    }
};

class JudgePos : public SelectionBox{
public:
    SelectionBox* ourCard;
    SDL_Texture* renderTex, *fade, *gBorder;

    JudgePos() : ourCard(nullptr), fade(loadImage("fade.png")), gBorder(loadImage("gBorder.png")) {
        boxColor = {0x82, 0x82, 0x82, 0xFF};
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        renderTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h/8);
    }

    virtual bool draw();
};

class Game : public SelectionBox{
public:
    SDL_Texture *face, *ymove, *wBorder;
    int expiryTime, pos;

    Game() : expiryTime(0), face(nullptr), ymove(nullptr), wBorder(loadImage("wBorder.png")) {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        font = TTF_OpenFont(std::string("rimouski.ttf").c_str(), 9*h/256);
    }

    virtual bool draw();

    virtual ~Game(){
        TTF_CloseFont(font);
        if(box) SDL_DestroyTexture(box);
        if(face) SDL_DestroyTexture(face);
        face = nullptr;
        box = nullptr;
        font = nullptr;
    }
};

class RadioButton : public GUIElement{
protected:
    SelectionBox radioBox;
    SelectionBox textBox;

public:
    std::string text;
    SDL_Texture* radio;

    bool draw();
};

class HList : public GUIElement{
protected:

public:
    int select, lastmove;
    std::vector<SelectionBox*> elements;
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
