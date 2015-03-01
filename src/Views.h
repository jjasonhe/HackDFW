#ifndef _VIEWS_H_
#define _VIEWS_H_

#include "View.h"
#include "main.h"
#include "SDL.h"
#include "GUI.h"

class LoadingView : public View {
public:
    int w,h;
	SDL_Texture* screen;
	int begtime;
	Sprite loadingWheel;

	LoadingView(EventController* controller);

    virtual bool activate();
    virtual bool updateWorld();
    virtual bool drawWorld();
    virtual bool deactivate();

    virtual ~LoadingView(){
        if(screen) SDL_DestroyTexture(screen);
        screen = nullptr;
    }
};

class WelcomeView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;

public:
    WelcomeView(EventController* controller);
    ~WelcomeView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class FlightView : public View{
protected:
    EventController* myController;
    SelectionBox submit, depFrom, destination, departureDate, returnDate, price, nonstop;
    SDL_Texture* screen;

public:
    FlightView(EventController* controller);
    ~FlightView();

    HList cardList;

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class InfoView : public View{
protected:
    EventController* myController;
    SelectionBox fctext, icon, departTime, arrivalTime, layover, timezones, flightNumbers;
    SDL_Texture* screen;

public:
    InfoView(EventController* controller);
    ~InfoView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class CurLocationView : public View{
protected:
    EventController* myController;
    InputBox textBox;
    SDL_Texture* screen;

public:
    CurLocationView(EventController* myController);
    ~CurLocationView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class DestLocationView : public View{
protected:
    EventController* myController;
    InputBox textBox;
    SDL_Texture* screen;
    SelectionBox submit;
    std::vector<RadioButton> buttons;
    int lastSel;
    SDL_Texture* selected;
    SDL_Texture* unselected;

public:
    DestLocationView(EventController* myController);
    ~DestLocationView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class TimeSpentView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;
    std::vector<SelectionBox> boxes;

public:
    TimeSpentView(EventController* myController);
    ~TimeSpentView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

#endif // _VIEWS_H_
