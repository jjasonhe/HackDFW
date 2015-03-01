#ifndef _EVENTS_H_
#define _EVENTS_H_ 1

#include "main.h"
#include "Views.h"

class QuitKeyEventProcessor : public EventProcessor{
protected:
	View* myView;

public:
	QuitKeyEventProcessor(EventController* controller, View* that)
	: EventProcessor(controller, SDL_KEYDOWN), myView(that)
	{}

	virtual bool process(SDL_Event &event) {
		if(event.key.keysym.sym == SDLK_AC_BACK){
			myView->done = true;
			return true;
		}
	}
};

class QuitEventProcessor : public EventProcessor{
private:
	LoadingView* myView;
public:
	QuitEventProcessor(EventController* controller, LoadingView* that)
	: EventProcessor(controller, SDL_QUIT), myView(that)
	{}

	virtual bool process(SDL_Event &event) { return myView->done = true;}
};

extern int lastmove;

class SelFDownEventProcesor : public EventProcessor{
protected:
	SelectionBox *myBox;
	SDL_Point lastcoord;

public:
	SelFDownEventProcesor(EventController* controller, SelectionBox *other)
	: EventProcessor(controller, SDL_FINGERUP), myBox(other), lastcoord{0,0}
	{ myController->registerEvent(this, SDL_FINGERDOWN); }

	virtual bool process(SDL_Event &event){
		int w,h;
		SDL_GetWindowSize(window, &w, &h);

		SDL_Point point = {static_cast<int>(event.tfinger.x*w), static_cast<int>(event.tfinger.y*h)};
		if(event.type == SDL_FINGERDOWN) lastcoord = point;
		else if (enclosedPoint(point, myBox->position) &&
				(lastcoord.x - point.x)*(lastcoord.x - point.x) + (lastcoord.y - point.y)*(lastcoord.y - point.y) < w*w/2500){
			myBox->selected = !myBox->selected;
			lastmove = SDL_GetTicks();
        }
        return true;
	}
};

class SwipeDownEventProcesor : public EventProcessor{
protected:
	View *myView;
	SDL_Point lastcoord;

public:
	SwipeDownEventProcesor(EventController* controller, View *other)
	: EventProcessor(controller, SDL_FINGERUP), myView(other), lastcoord{0,0}
	{ myController->registerEvent(this, SDL_FINGERDOWN); }

	virtual bool process(SDL_Event &event){
		int w,h;
		SDL_GetWindowSize(window, &w, &h);

		SDL_Point point = {static_cast<int>(event.tfinger.x*w), static_cast<int>(event.tfinger.y*h)};
		if(event.type == SDL_FINGERDOWN) lastcoord = point;
		else if ((lastcoord.x - point.x) > w/4){
			myView->done = true;
        }
        return true;
	}
};

class SelSwipeEventProcesor : public EventProcessor{
protected:
	SelectionBox *myBox;
	SDL_Point lastcoord;

public:
	SelSwipeEventProcesor(EventController* controller, SelectionBox *other)
	: EventProcessor(controller, SDL_FINGERUP), myBox(other), lastcoord{0,0}
	{ myController->registerEvent(this, SDL_FINGERDOWN); }

	virtual bool process(SDL_Event &event){
		int w,h;
		SDL_GetWindowSize(window, &w, &h);

		SDL_Point point = {static_cast<int>(event.tfinger.x*w), static_cast<int>(event.tfinger.y*h)};
		if(event.type == SDL_FINGERDOWN) lastcoord = point;
		else if (enclosedPoint(point, myBox->position) &&
				(lastcoord.x - point.x)*(lastcoord.x - point.x) + (lastcoord.y - point.y)*(lastcoord.y - point.y) > w*w/2500){
			myBox->selected = !myBox->selected;
			lastmove = SDL_GetTicks();
        }
        return true;
	}
};

class FMotionEventProcessor : public EventProcessor{
protected:
	FlightView* myView;
	int wait;
	int countA;
	SDL_Rect screen;

public:
	FMotionEventProcessor(EventController* controller, FlightView* that, SDL_Rect selArea)
	: EventProcessor(controller, SDL_FINGERMOTION), myView(that), wait(256), countA(0), screen(selArea)
	{}

	virtual bool process(SDL_Event &event){
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		SDL_Point xy = {static_cast<int>(event.tfinger.x*w), static_cast<int>(event.tfinger.y*h)};
        if(enclosedPoint(xy, screen)) myView->cardList.moveElements(-3*w*event.tfinger.dx, 0);
		return true;
	}
};

class FVMotionEventProcessor : public EventProcessor{
protected:
	VList* myList;
	int wait,countA;

public:
	FVMotionEventProcessor(EventController* controller, VList* that)
	: EventProcessor(controller, SDL_FINGERMOTION), myList(that), wait(256), countA(0)
	{}

	virtual bool process(SDL_Event &event){
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		if(fabs(event.tfinger.dx) > 0.0005){
			myList->moveElements(0, event.tfinger.dy*w);
		}
		return true;
	}
};

class InputEventProcessor : public EventProcessor{
protected:
	InputBox* myBox;

public:
	InputEventProcessor(EventController* controller, InputBox* that)
	: EventProcessor(controller, SDL_TEXTINPUT), myBox(that)
	{}

	virtual bool process(SDL_Event &event) {
		if(!myBox->active) return false;
		myBox->text += event.text.text;
		myBox->composition = std::string();
		return true;
	}
};

class EditEventProcessor : public EventProcessor{
protected:
	InputBox* myBox;

public:
	EditEventProcessor(EventController* controller, InputBox* that)
	: EventProcessor(controller, SDL_TEXTEDITING), myBox(that)
	{}

	virtual bool process(SDL_Event &event) {
		if(!myBox->active) return false;
		myBox->composition = std::string(event.edit.text);
		return true;
	}
};

class InFDownEventProcesor : public EventProcessor{
protected:
	InputBox *myBox;

public:
	InFDownEventProcesor(EventController* controller, InputBox* that)
	: EventProcessor(controller, SDL_FINGERUP), myBox(that)
	{}

	virtual bool process(SDL_Event &event){
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		SDL_Point point{static_cast<int>(event.tfinger.x*w), static_cast<int>(event.tfinger.y*h)};
		if (enclosedPoint(point, myBox->position)){
			if(SDL_IsTextInputActive()){
				myBox->active = false;
				SDL_StopTextInput();
			}
			else{
				SDL_StartTextInput();
				myBox->active = true;
			}
		}
		else{
			myBox->active = false;
			//if(SDL_IsTextInputActive()) SDL_StopTextInput();
		}
		return true;
	}
};

class InKeyEventProcessor : public EventProcessor{
protected:
	InputBox* myBox;

public:
	InKeyEventProcessor(EventController* controller, InputBox* that)
	: EventProcessor(controller, SDL_KEYDOWN), myBox(that)
	{}

	virtual bool process(SDL_Event &event){
		if(!myBox->active) return false;
		switch(event.key.keysym.sym){
		case SDLK_BACKSPACE:
			if(myBox->composition.length()) myBox->composition.erase(myBox->composition.end());
			else if(myBox->text.length()) myBox->text.erase(myBox->text.end());
			break;
		case SDLK_RETURN:
			SDL_StopTextInput();
			myBox->active = false;
			break;
		}
		return true;
	}
};

#endif // _EVENTS_H_
