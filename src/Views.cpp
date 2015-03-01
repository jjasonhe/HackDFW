#include <algorithm>
#include <memory>

#include "Views.h"
#include "Events.h"
#include "main.h"
#include "SDL_image.h"

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
    if(!loading) return false;
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
    views.push_back(std::make_shared<CurLocationView>(myController));
    myEvents.push_back(std::make_shared<SwipeDownEventProcesor>(myController, this));
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
    myEvents.push_back(std::make_shared<SwipeDownEventProcesor>(myController, this));
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
: myController(controller), screen(loadImage("screen3.png")), lastSel(-1), unselected(loadImage("radioUnselected.png")),
  selected(loadImage("radioSelected.png"))
{
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
	RadioButton tRButton;
	tRButton.radio = unselected;
	tRButton.position = {w/8, h/2, 3*w/4, h/16};
	tRButton.box = loadImage("Beaches.png");
	buttons.push_back(tRButton);
	tRButton.position = {w/8, 9*h/16, 3*w/4, h/16};
	tRButton.box = loadImage("Historic.png");
	buttons.push_back(tRButton);
	tRButton.position = {w/8, 10*h/16, 3*w/4, h/16};
	tRButton.box = loadImage("Theme-Park.png");
	buttons.push_back(tRButton);
	tRButton.position = {w/8, 11*h/16, 3*w/4, h/16};
	tRButton.box = loadImage("Skiing.png");
	buttons.push_back(tRButton);
	tRButton.position = {w/8, 12*h/16, 3*w/4, h/16};
	tRButton.box = loadImage("Outdoors.png");
	buttons.push_back(tRButton);
	tRButton.position = {w/8, 13*h/16, 3*w/4, h/16};
	tRButton.box = loadImage("Gambling.png");
	buttons.push_back(tRButton);
	tRButton.position = {w/8, 14*h/16, 3*w/4, h/16};
	tRButton.box = loadImage("Romantic.png");
	buttons.push_back(tRButton);
	submit.box = loadImage("submit.png");
	submit.position = {w/8, 15*h/16, 3*w/4, h/16};
}

DestLocationView::~DestLocationView(){
    SDL_DestroyTexture(screen);
    SDL_DestroyTexture(selected);
    SDL_DestroyTexture(unselected);
}

bool DestLocationView::activate(){
    myEvents.push_back(std::make_shared<InputEventProcessor>(myController, &textBox));
    myEvents.push_back(std::make_shared<EditEventProcessor>(myController, &textBox));
    myEvents.push_back(std::make_shared<InFDownEventProcesor>(myController, &textBox));
    myEvents.push_back(std::make_shared<InKeyEventProcessor>(myController, &textBox));
    myEvents.push_back(std::make_shared<QuitKeyEventProcessor>(myController, this));
    for(auto& r : buttons){
        myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &r));
    }
    myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &submit));
    myEvents.push_back(std::make_shared<SwipeDownEventProcesor>(myController, this));
}

bool DestLocationView::updateWorld(){
    bool sel = false;
    for(int i=0; i<buttons.size(); i++){
        if (buttons[i].selected){
            if(lastSel == -1){
                lastSel = i;
                //SDL_DestroyTexture(buttons[i].radio);
                buttons[i].radio = selected;
            }
            else if(lastSel != i){
                buttons[lastSel].selected = false;
                //SDL_DestroyTexture(buttons[lastSel].radio);
                buttons[lastSel].radio = unselected;
                //SDL_DestroyTexture(buttons[i].radio);
                buttons[i].radio = selected;
                lastSel = i;
            }
            sel = true;
        }
        else{
            buttons[i].radio = unselected;
        }
    }

    return !(done || (sel && submit.selected));
}

bool DestLocationView::drawWorld(){
    SDL_RenderCopy(renderer,screen,nullptr,nullptr);
    submit.draw();
    for(auto& r : buttons) {
        r.draw();
    }


    return !done;
}

bool DestLocationView::deactivate(){
    SDL_StopTextInput();
    for(int i=0; i<buttons.size(); i++)
		if(buttons[i].selected){
			switch(i){
			case 0:
				dest = "BEACH";
				break;
			case 1:
				dest = "HISTORIC";
				break;
			case 2:
				dest = "THEME-PARK";
				break;
			case 3:
				dest = "SKIING";
				break;
			case 4:
				dest = "OUTDOORS";
				break;
			case 5:
				dest = "GAMBLING";
				break;
			case 6:
				dest = "ROMANTIC";
				break;
			}
		}
}

bool compareDest(FlightCard a, FlightCard b){
    return a.dest < b.dest;
}

bool compareDepartDate(FlightCard a, FlightCard b){
    return a.date < b.date;
}

bool comparePriceNonStop(FlightCard a, FlightCard b){
    return a.nonStop < b.nonStop;
}

bool comparePrice(FlightCard a, FlightCard b){
    return a.price < b.price;
}
