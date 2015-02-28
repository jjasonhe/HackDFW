#include "Views.h"

LoadingView::LoadingView(EventController* controller)
	: myController(controller), begtime(0), loadingWheel(LoadSprite("loading.png", renderer))
{
    screen = loadImage("screen.bmp");
    SDL_GetWindowSize(window, &w, &h);
    loadingWheel.position = {7*w/16, 3*h/4, w/8, w/8};
}

bool LoadingView::activate(){
    activated = true;
}

bool LoadingView::deactivate(){
    for(auto& a : myEvents) a->deactivate();
    activated = false;
}

bool LoadingView::drawWorld(){
    SDL_RenderClear(renderer);
    if(!activated) return false;
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);

    loadingWheel.draw();

    if(overlays.empty()) SDL_RenderPresent(renderer);

    return !done;
}

bool LoadingView::updateWorld(){
    if(!activated) return false;
    SDL_Delay(15);
    static int counting;
    if (!counting) loadingWheel.angle += 30.f;
    counting = ++counting % 5;
    if(!loading){
        //views.push_back(new GamesView(myController));
        done = true;
    }
    return !done;
}

WelcomeView::WelcomeView(){
}

WelcomeView::~WelcomeView(){
}

bool WelcomeView::activate(){
}

bool WelcomeView::updateWorld(){
}

bool WelcomeView::drawWorld(){
}

bool WelcomeView::deactivate{
}
