#include "Views.h"
#include "Events.h"
#include <memory>

Sprite LoadSprite(const char* file, SDL_Renderer* renderer)
{
	Sprite result;
	result.texture = NULL;
	result.position.w = 0;
	result.position.h = 0;
	result.position.x = 0;
	result.position.y = 0;
	result.scale = 1.f;
	result.angle = 0.f;

    SDL_Surface* temp;

    /* Load the sprite image */
    temp = SDL_LoadBMP(file);
    if (temp == NULL)
	{
        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
        return result;
    }
    result.position.w = temp->w;
    result.position.h = temp->h;

    /* Create texture from the image */
    result.texture = SDL_CreateTextureFromSurface(renderer, temp);
    if (!result.texture) {
        fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(temp);
        return result;
    }
    SDL_FreeSurface(temp);

    return result;
}

LoadingView::LoadingView(EventController* controller)
	: begtime(0), loadingWheel(LoadSprite("loading.png", renderer))
{
    screen = loadImage("screen.bmp");
    SDL_GetWindowSize(window, &w, &h);
    loadingWheel.position = {7*w/16, 3*h/4, w/8, w/8};
}

bool LoadingView::activate(){
    activated = true;
}

bool LoadingView::deactivate(){
    activated = false;
}

bool LoadingView::drawWorld(){
    if(!activated) return false;
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);

    loadingWheel.draw();

    return !done;
}

bool LoadingView::updateWorld(){
    if(!activated) return false;
    SDL_Delay(15);
    static int counting;
    if (!counting) loadingWheel.angle += 30.f;
    counting = ++counting % 5;
    return !done;
}

WelcomeView::WelcomeView(EventController* controller)
: myController(controller), screen(loadImage("screen.png"))
{
}

WelcomeView::~WelcomeView(){
}

bool WelcomeView::activate(){
    myEvents.push_back(std::make_shared<QuitKeyEventProcessor>(myController, this));
}

bool WelcomeView::updateWorld(){
    return !done;
}

bool WelcomeView::drawWorld(){
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);
    return !done;
}

bool WelcomeView::deactivate(){
}
