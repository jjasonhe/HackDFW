#include "Views.h"
#include "Events.h"
#include "main.h"
#include "SDL_image.h"
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
    temp = IMG_Load(file);
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
	: begtime(0), loadingWheel(LoadSprite("loadingWheel.png", renderer))
{
    screen = loadImage("loadingScreen.png");
    SDL_GetWindowSize(window, &w, &h);
    loadingWheel.position = {7*w/16, 3*h/4, w/8, w/8};
}

bool LoadingView::activate(){
    views.push_back(std::make_shared<CurLocationView>(&viewController));
    begtime = SDL_GetTicks();
    activated = true;
}

bool LoadingView::deactivate(){
    activated = false;
}

bool LoadingView::drawWorld(){
    if(!screen) return false;
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);
    loadingWheel.draw();

    return !done;
}

bool LoadingView::updateWorld(){
    if(!activated) return false;
    SDL_Delay(15);
    static int counting;
    if(SDL_GetTicks() - begtime > 2500) return false;
    if (!counting) loadingWheel.angle += 30.f;
    counting = ++counting % 5;
    return !done;
}

WelcomeView::WelcomeView(EventController* controller)
: myController(controller), screen(loadImage("screen.png"))
{
}

WelcomeView::~WelcomeView(){
    SDL_DestroyTexture(screen);
}

bool WelcomeView::activate(){
    views.push_back(std::make_shared<LoadingView>(myController));
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

CurLocationView::CurLocationView(EventController* controller)
: myController(controller), screen(loadImage("screen2.png"))
{
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    textBox.position = {w/8, h/2, 3*w/4, h/16};
    textBox.box = loadImage("textBox.png");
    textBox.font = TTF_OpenFont("Font.otf", h/32);
}

CurLocationView::~CurLocationView(){
    SDL_DestroyTexture(screen);
}

bool CurLocationView::activate(){
    views.push_back(std::make_shared<DestLocationView>(myController));

    myEvents.push_back(std::make_shared<InputEventProcessor>(myController, &textBox));
    myEvents.push_back(std::make_shared<EditEventProcessor>(myController, &textBox));
    myEvents.push_back(std::make_shared<InFDownEventProcesor>(myController, &textBox));
    myEvents.push_back(std::make_shared<InKeyEventProcessor>(myController, &textBox));
    myEvents.push_back(std::make_shared<QuitKeyEventProcessor>(myController, this));
}

bool CurLocationView::updateWorld(){
    return !done;
}

bool CurLocationView::drawWorld(){
    SDL_RenderCopy(renderer,screen,nullptr,nullptr);
    textBox.draw();

    return !done;
}

bool CurLocationView::deactivate(){
    SDL_StopTextInput();
    start = textBox.text + textBox.composition;
}

DestLocationView::DestLocationView(EventController* controller)
: myController(controller), screen(loadImage("screen3.png"))
{
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    textBox.position = {w/8, h/2, 5*w/8, h/16};
    textBox.box = loadImage("textBox.png");
    textBox.font = TTF_OpenFont("Font.otf", h/32);
    plusBox.box = loadImage("plus.png");
    plusBox.position = {3*w/4, h/2, w/8, h/16};
}

DestLocationView::~DestLocationView(){
    SDL_DestroyTexture(screen);
}

bool DestLocationView::activate(){
    myEvents.push_back(std::make_shared<InputEventProcessor>(myController, &textBox));
    myEvents.push_back(std::make_shared<EditEventProcessor>(myController, &textBox));
    myEvents.push_back(std::make_shared<InFDownEventProcesor>(myController, &textBox));
    myEvents.push_back(std::make_shared<InKeyEventProcessor>(myController, &textBox));
    myEvents.push_back(std::make_shared<QuitKeyEventProcessor>(myController, this));
    myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &plusBox));
}

bool DestLocationView::updateWorld(){
    return !done;
}

bool DestLocationView::drawWorld(){
    SDL_RenderCopy(renderer,screen,nullptr,nullptr);
    textBox.draw();
    plusBox.draw();

    return !done;
}

bool DestLocationView::deactivate(){
    SDL_StopTextInput();
    dest = textBox.text + textBox.composition;
}
