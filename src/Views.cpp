#include <algorithm>
#include <memory>
#include <sstream>

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
    //views.push_back(std::make_shared<CurLocationView>(&viewController));
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
    if(dest.empty()) return false;
}

bool compareDest(FlightCard* a, FlightCard* b){
    return a->dest < b->dest;
}

bool compareDepartDate(FlightCard* a, FlightCard* b){
    return a->date < b->date;
}

bool comparePriceNonStop(FlightCard* a, FlightCard* b){
    return a->nonStop < b->nonStop;
}

bool comparePrice(FlightCard* a, FlightCard* b){
    return a->price < b->price;
}

FlightView::FlightView(EventController* controller)
: myController(controller), screen(loadImage("screen4.png"))
{
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    destination.position = {0, 3*h/8, w/3, h/8};
    destination.box = loadImage("Destination.png");
    departureDate.position = {0, h/2, w/3, h/8};
    departureDate.box = loadImage("departureDate.png");
    returnDate.position = {0, 5*h/8, w/3, h/8};
    returnDate.box = loadImage("returnDate.png");
    price.position = {0, 3*h/4, w/3, h/8};
    price.box = loadImage("regularPrice.png");
    nonstop.position = {0, 7*h/8, w/3, h/8};
    nonstop.box = loadImage("nonStop.png");
}

FlightView::~FlightView(){
    SDL_DestroyTexture(screen);
}

bool FlightView::activate(){
    if(flightValues.size()) SDL_DetachThread(SDL_CreateThread(loadFlightDetails, "LoadFlightDetails", 0));
    myEvents.push_back(std::make_shared<QuitKeyEventProcessor>(myController, this));

    int w,h;
    SDL_GetWindowSize(window, &w, &h);

    TTF_Font* tFont = TTF_OpenFont("Font.otf", h/32);
    for(int i=0; i<flightValues["FareInfo"].size(); i++){
        FlightCard* tCard = new FlightCard;
        tCard->font = tFont;
        tCard->position = {w/3 + i*w, 3*h/8, 2*w/3, 5*h/8};
        tCard->dest = cityFromCode(flightValues["FareInfo"][i]["DestinationLocation"].asString());
        std::string tDate = flightValues["FareInfo"][i]["DepartureDateTime"].asString();
        int pos = tDate.find("T");
        tCard->date = tDate.substr(0, pos);
        std::string tDate2 = flightValues["FareInfo"][i]["ReturnDateTime"].asString();
        int pos2 = tDate2.find("T");
        tCard->retDate = tDate2.substr(0, pos2);
        std::istringstream d(flightValues["FareInfo"][i]["LowestNonStopFare"].asString());
        int tInt;
        d >> tInt;
        tCard->nonStop = tInt;
        std::istringstream e(flightValues["FareInfo"][i]["LowestFare"].asString());
        e >> tInt;
        tCard->price = tInt;
        cardList.elements.push_back(tCard);
    }
    SDL_Rect tRect = {w/3, 3*h/8, 2*w/3, 5*h/8};
    myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &destination));
    myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &departureDate));
    myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &returnDate));
    myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &price));
    myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &nonstop));
    myEvents.push_back(std::make_shared<FMotionEventProcessor>(myController, this, tRect));

    //views.push_back(std::make_shared<WelcomeView>(myController));
}

bool FlightView::updateWorld(){
   // if(cardList.select >=0 && cardList.elements[cardList.select]->selected){
   //     return false;
        //Go to next screen
    //}
    if(departureDate.selected){
        std::sort(cardList.elements.begin(), cardList.elements.end(), compareDepartDate);
        cardList.select = 0;
        departureDate.selected = false;
    }
    else if(destination.selected){
        std::sort(cardList.elements.begin(), cardList.elements.end(), compareDest);
        cardList.select = 0;
        destination.selected = false;
    }
    else if(returnDate.selected){
        std::sort(cardList.elements.begin(), cardList.elements.end(), compareDest);
        cardList.select = 0;
        returnDate.selected = false;
    }
    else if(price.selected){
        std::sort(cardList.elements.begin(), cardList.elements.end(), comparePrice);
        cardList.select = 0;
        price.selected = false;
    }
    else if(nonstop.selected){
        std::sort(cardList.elements.begin(), cardList.elements.end(), comparePriceNonStop);
        cardList.select = 0;
        nonstop.selected = false;
    }

    return !done;

}

bool FlightView::drawWorld(){
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);
    cardList.draw();

    destination.draw();
    departureDate.draw();
    returnDate.draw();
    price.draw();
    nonstop.draw();

    return !done;
}

bool FlightView::deactivate(){
    for(auto& e : myEvents) e->deactivate();
}

TimeSpentView::TimeSpentView(EventController* controller)
: myController(controller), screen(loadImage("screen2.png"))
{
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    textBox.position = {w/8, h/2, 3*w/4, h/16};
    textBox.box = loadImage("textBox.png");
    textBox.font = TTF_OpenFont("Font.otf", h/32);
}

TimeSpentView::~TimeSpentView(){
    SDL_DestroyTexture(screen);
}

bool TimeSpentView::activate(){
    views.push_back(std::make_shared<FlightView>(myController));

    SelectionBox tBox;
    tBox.position = {5*w/24, 541*h/1280, w/4, 7*h/50};
    tBox.box = "1.png";
    boxes.push_back(tBox);

    myEvents.push_back(std::make_shared<QuitKeyEventProcessor>(myController, this));
    myEvents.push_back(std::make_shared<SwipeDownEventProcesor>(myController, this));
}

bool TimeSpentView::updateWorld(){
    for(int i=0; i<5; i++)
        if(boxes[i].selected)
            return false;

    return !done;
}

bool TimeSpentView::drawWorld(){
    SDL_RenderCopy(renderer,screen,nullptr,nullptr);
    for(auto& b : boxes) b.draw();

    return !done;
}

bool TimeSpentView::deactivate(){
    for(int i=0; i<5; i++)
        if(boxes[i].selected)
            numDays = Json::valueToString(i);

    SDL_DetachThread(SDL_CreateThread(loadFlights, "LoadFlights", nullptr));
}
