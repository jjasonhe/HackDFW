#ifndef _VIEWS_H_
#define _VIEWS_H_

#include "View.h"
#include "main.h"

class LoadingView : public View {
public:
	EventController* myController;
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
public:
    WelcomeView();
    ~WelcomeView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class FlightView : public View{
public:
    FlightView();
    ~FlightView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class InfoView : public View{
    InfoView();
    ~InfoView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

#endif // _VIEWS_H_
