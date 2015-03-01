#include <cmath>

//#include "SDL2_gfxPrimitives.h"
#include "SDL_image.h"d
#include "SDL_ttf.h"

#include "GUI.h"
//#include "Font.h"

bool Sprite::draw(){
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    if(position.x + position.w < 0 || position.x > w) return false;

    SDL_Rect destRect = {position.x, position.y, static_cast<int>(scale*position.w), static_cast<int>(scale*position.h)};
    SDL_RenderCopyEx(renderer, texture, nullptr, &destRect, angle, nullptr, SDL_FLIP_NONE);
    return true;
}

bool InputBox::draw(){
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    if(position.x + position.w < 0 || position.x > w) return false;

    if(box)
        SDL_RenderCopy(renderer, box, nullptr, &position);
    else{
        SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
        SDL_RenderFillRect(renderer, &position);
    }

    if(font){
        SDL_Surface *surf = TTF_RenderText_Blended(font, (text + composition).c_str(), textcolor);
        if(surf){
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            if(texture){
                int iW, iH;
                SDL_QueryTexture(texture, nullptr, nullptr, &iW, &iH);
                SDL_Rect destRect = {position.x + position.w/16, position.y + (position.h - iH)/2, std::min(iW, position.w), std::min(iH, position.h)};
                SDL_Rect srcRect = {0, 0, std::min(iW, position.w), std::min(iH, position.h)};
                SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
                SDL_DestroyTexture(texture);
                texture = nullptr;
            }
        }
    }
}

bool SelectionBox::draw(){
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    if(position.x + position.w < 0 || position.x > w) return false;
    if(drawBox){
        if(box) SDL_RenderCopy(renderer, box, nullptr, &position);
        else{
            SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
            SDL_RenderFillRect(renderer, &position);
        }
    }
    bool tb = false;
    if (textPos.w == 0 && textPos.h == 0){
        textPos = position;
        tb = true;
    }
    if(font){
        SDL_Surface *surf = TTF_RenderText_Blended_Wrapped(font, text.c_str(), textColor, textPos.w-textPos.h/2);
        if(surf){
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            if(texture){
                int iW, iH;
                SDL_QueryTexture(texture, nullptr, nullptr, &iW, &iH);
                SDL_Rect destRect;
                //if(centered) destRect = {textPos.x + (textPos.w - iW)/2, textPos.y + (textPos.h - iH)/2, std::min(iW, textPos.w), std::min(iH, textPos.h)};
                //else if(right) destRect = {textPos.x + textPos.w - iW, textPos.y, iW, std::min(iH, textPos.h)};
                destRect = {textPos.x, textPos.y, std::min(iW, textPos.w), std::min(iH, textPos.h)};
                SDL_Rect srcRect = {0, 0, std::min(iW, textPos.w), std::min(iH, textPos.h)};
                SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
                SDL_DestroyTexture(texture);
                texture = nullptr;
            }
        }
    }
    if(tb){
        textPos.w = textPos.h = 0;
    }
}

bool RadioButton::draw(){
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    if(position.x + position.w < 0 || position.x > w) return false;
    radioBox.box = radio;
    radioBox.position = {position.x, position.y, position.h, position.h};
    textBox.text = text;
    if(!textBox.font) textBox.font = TTF_OpenFont("Font.otf", position.h/2);
    textBox.position = {position.x + position.w/4, position.y, 3*position.w/4, position.h};
    textBox.box = box;

    radioBox.draw();
    textBox.draw();
}

bool FlightCard::draw(){
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    if(position.x + position.w < 0 || position.x > w) return false;
    SDL_SetRenderDrawColor(renderer, 0x18, 0x5C, 0x83, 0xFF);
    SDL_RenderFillRect(renderer, &position);

    depFromBox.position = {position.x, position.y, position.w, position.h/6};
    depFromBox.text = depFrom;
    depFromBox.font = font;
    depFromBox.drawBox = false;
    depFromBox.draw();

    destBox.position = {position.x, position.y + position.h/6, position.w, position.h/6};
    destBox.text = dest;
    destBox.font = font;
    destBox.drawBox = false;
    destBox.draw();

    dateBox.position = {position.x, position.y + position.h/3, position.w, position.h/5};
    dateBox.text = date;
    dateBox.font = font;
    dateBox.drawBox = false;
    dateBox.draw();

    retDateBox.position = {position.x, position.y + position.h/2, position.w, position.h/5};
    retDateBox.text = retDate;
    retDateBox.font = font;
    retDateBox.drawBox = false;
    retDateBox.draw();

    priceBox.position = {position.x, position.y + 2*position.h/3, position.w, position.h/5};
    priceBox.text = Json::valueToString(price);
    priceBox.font = font;
    priceBox.drawBox = false;
    priceBox.draw();

    nonStopBox.position = {position.x, position.y + 5*position.h/6, position.w, position.h/5};
    nonStopBox.text = Json::valueToString(nonStop);
    if(nonStop == 0) nonStopBox.text = "N/A";
    nonStopBox.font = font;
    nonStopBox.drawBox = false;
    nonStopBox.draw();
}

template<class T>
int sgn(T num){
    if (num > 0)
        return 1;
    if (num < 0)
        return -1;
    return 0;
}

bool HList::draw(){
    if(elements.empty()) return false;
    moveElements(0, 0);
    for(auto& e : elements)
        e->draw();
}

bool HList::moveElements(int aax, int aay){
    if(elements.empty()) return false;
    if(infinitescroll) select %= elements.size();
    else select = (select < 0) ? 0 : ((select >= elements.size()) ? (elements.size() - 1) : select);
    int w , h;
    SDL_GetWindowSize(window, &w, &h);
    int s = abs(elements[select]->position.x - 2*w/3 + elements[select]->position.w/2);
    if(moveElement && !begin && snap){
        int t = sgn(elements[select]->position.x - 2*w/3 + elements[select]->position.w/2)*std::min(12*(int)sqrt(s), s);
        int w , h;
        SDL_GetWindowSize(window, &w, &h);
        if(!elements.empty()){
            elements[0]->position.x -= t;
            for(int i=1; i<elements.size(); i++){
                elements[i]->position.x = elements[i-1]->position.x + elements[i-1]->position.w + w/8;
            }
        }
        if(!s) moveElement = false;
        lastmove = SDL_GetTicks();
    } else if(elements.size() > 1){
        if(SDL_GetTicks() - lastmove > 512){
            if(!elements.empty()){
                elements[0]->position.x -= aax;
                for(int i=1; i<elements.size(); i++){
                    elements[i]->position.x = elements[i-1]->position.x + elements[i-1]->position.w + w/8;
                }
            }
        }
        if(s > elements[select]->position.w/2){
            elements[select]->selected = false;
            select -= sgn(elements[select]->position.x);
            moveElement = true;
            begin = false;
        }
    }
}

//IMPLEMENT
bool VList::moveElements(int aax, int aay){
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    if (accel[1]) accel[1] -= vel[1]/4 + 2*accel[1]/3;
    accel[1] += aay;
    vel[1] += accel[1];
    if(elements.size() <= 6) return false;
    else if(elements[0]->position.y+ vel[1] > h/4 + h/32) vel[1] = h/4 + h/32 - elements[0]->position.y;
    else if(elements[elements.size() - 1]->position.y + elements[elements.size() - 1]->position.h + vel[1] < h) vel[1] = h - elements[elements.size() - 1]->position.y - elements[elements.size() - 1]->position.h;
    elements[0]->position.y += vel[1];
    for(int i=1; i<elements.size(); i++){
        elements[i]->position.y = elements[i-1]->position.y + elements[i-1]->position.h;
    }

}

bool VList::draw(){
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    moveElements(0, 0);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    if(info.flags&SDL_RENDERER_TARGETTEXTURE){
        SDL_SetRenderTarget(renderer, tText);
        SDL_SetRenderDrawColor(renderer, 0xD8, 0xD8, 0xD8, 0xFF);
        SDL_RenderFillRect(renderer,nullptr);
    }
    for(auto& e : elements)
        e->draw();
    if(info.flags&SDL_RENDERER_TARGETTEXTURE){
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_Rect tRect = {0, h/4 + h/32, w, 3*h/4};
        SDL_RenderCopy(renderer, tText, &tRect, &tRect);
    }
}
