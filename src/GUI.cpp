#include <cmath>

//#include "SDL2_gfxPrimitives.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "GUI.h"
//#include "Font.h"

/* Adapted from SDL's testspriteminimal.c */
Sprite LoadSprite(const char* file, SDL_Renderer* renderer)
{
    Sprite result;
    result.texture = nullptr;
    result.position.w = 0;
    result.position.h = 0;
    result.position.x = 0;
    result.position.y = 0;
    result.scale = 1.f;
    result.angle = 0.f;

    SDL_Surface* tSurf = IMG_Load(file);
    if (tSurf == nullptr)
    {
        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
        return result;
    }
    result.position.w = tSurf->w;
    result.position.h = tSurf->h;

    result.texture = SDL_CreateTextureFromSurface(renderer, tSurf);
    if (!result.texture) {
        fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(tSurf);
        return result;
    }
    SDL_FreeSurface(tSurf);

    return result;
}

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

    SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
    SDL_RenderFillRect(renderer, &position);

    if(font){
        SDL_Surface *surf = TTF_RenderText_Blended(font, (text + composition).c_str(), textcolor);
        if(surf){
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            if(texture){
                int iW, iH;
                SDL_QueryTexture(texture, nullptr, nullptr, &iW, &iH);
                SDL_Rect destRect = {position.x, position.y, std::min(iW, position.w), std::min(iH, position.h)};
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
            SDL_RenderCopy(renderer, gradient, nullptr, &position);
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
                if(centered) destRect = {textPos.x + (textPos.w - iW)/2, textPos.y + (textPos.h - iH)/2, std::min(iW, textPos.w), std::min(iH, textPos.h)};
                else if(right) destRect = {textPos.x + textPos.w - iW, textPos.y, iW, std::min(iH, textPos.h)};
                else destRect = {textPos.x, textPos.y, std::min(iW, textPos.w), std::min(iH, textPos.h)};
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

bool Card::draw(){
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    if(position.x + position.w < 0 || position.x > w) return false;

    if(drawCard){
        if(border){
            SDL_SetRenderDrawColor(renderer, 212, 212, 212, boxColor.a);
            SDL_RenderFillRect(renderer, &position);
        }
        SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
        SDL_Rect pos2 = {position.x + position.w/50, position.y+position.w/50, 24*position.w/25, position.h - position.w/24};
        SDL_RenderFillRect(renderer, &pos2);
    }

    int tW = position.w/10;
    int tH = position.h/10;
    if(face){
        SDL_Rect destRect = {position.x + tW, position.y + tH, 3*h/32, 3*h/32};
        if(faceSize) destRect = {position.x + tW, position.y + tH, faceSize, faceSize};
        SDL_RenderCopy(renderer, gBorder, nullptr, &destRect);
        destRect = {position.x + tW + 3*h/392, position.y+tH+3*h/392, 3*h/32 - 3*h/196, 3*h/32 - 3*h/196};
        if(faceSize) destRect = {position.x + tW + faceSize/7, position.y + tH + faceSize/7, faceSize - faceSize*2/7, faceSize - faceSize*2/7};
        SDL_RenderCopy(renderer, face, nullptr, &destRect);
        if(faceSize) tW += faceSize;
        else tW += 3*h/32;
        if(faceSize) tH += faceSize/2 - h/45;
        else tH += 3*h/64 - h/45;
    }
    int positions = text.find("_____");
    int end;
    std::string backtext = text;
    if (positions != std::string::npos){
        text.erase(positions, 5);
        if(replaceText.empty()){
            text.insert(positions, "____________");
            end = positions + 12;
        }
        else{
            text.insert(positions, replaceText);
            end = positions + replaceText.size();
        }
    }
    if(font){
        int iH=0;
        textBack* bg = new textBack[3];
        SDL_Surface * surf;
        if(positions != std::string::npos){
            bg[0] = {SDL_Color{0,0,0,0}, 0, positions};
            bg[1] = {textColor, positions-1, end};
            if(!replaceText.empty()) bg[1].color = {0x87, 0x87, 0xD7, 0xFF};
            bg[2] = {SDL_Color{0,0,0,0}, end, text.size()};
            surf = TTF_RenderUTF8_Blended_WrappedColored(font, text.c_str(), textColor, 9*position.w/10-tW, bg, 3);
        }
        else
            surf = TTF_RenderUTF8_Blended_WrappedColored(font, text.c_str(), textColor, 9*position.w/10-tW, nullptr, 0);
        delete[] bg;
        if(surf){
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            if(texture){
                int iW;
                SDL_QueryTexture(texture, nullptr, nullptr, &iW, &iH);
                SDL_Rect destRect = {position.x+tW, position.y+tH, iW, std::min(iH, position.h)};
                SDL_Rect srcRect = {0, 0, iW, std::min(iH, position.h)};
                SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
                SDL_DestroyTexture(texture);
                texture = nullptr;
            }
        }
    }
    text = backtext;
}

bool Toggle::draw(){
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    if(position.x + position.w < 0 || position.x +position.w > w) return false;

    if(!back){
        SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
        SDL_RenderFillRect(renderer, &position);
    }

    SDL_QueryTexture(box, nullptr, nullptr, &w, &h);
    SDL_Rect srcRect = {selected ? 19*w/48 : 0, 0, 29*w/48, h};
    SDL_RenderCopy(renderer, box, &srcRect, &position);

    if(back) SDL_RenderCopy(renderer, back, nullptr, &position);
}

bool JudgePos::draw(){
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    if(position.x + position.w < 0 || position.x > w || !drawBox) return false;

    if(ourCard){
        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer, &info);
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        if(info.flags&SDL_RENDERER_TARGETTEXTURE){
            SDL_SetRenderTarget(renderer, renderTex);
        }
        SDL_Rect tRect = ourCard->position;
        ourCard->position.x = 0;
        ourCard->position.y = 0;
        ourCard->draw();
        ourCard->position = tRect;
        tRect = position;
        tRect.x = tRect.y = 0;
        SDL_SetTextureColorMod(fade, ourCard->boxColor.r, ourCard->boxColor.g, ourCard->boxColor.b);
        if(fade) SDL_RenderCopy(renderer, fade, nullptr, &tRect);
        if(info.flags&SDL_RENDERER_TARGETTEXTURE){
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_Rect destRect = {position.x, position.y, position.w, position.h};
            if(gBorder){
                SDL_RenderCopy(renderer, gBorder, nullptr, &destRect);
                destRect = {position.x + position.w/7, position.y + position.h/7, position.w - position.w*2/7, position.h - position.h*2/7};
            }
            SDL_RenderCopy(renderer, renderTex, &tRect, &destRect);
        }
    }
    else{
        SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
        //SDL_RenderFillRect(renderer, &position);
        SDL_RenderCopy(renderer, gBorder, nullptr, &position);
        if(font){
            //roundedBoxRGBA(renderer, position.x+position.w, position.y+position.h, position.x, position.y, position.h/8, boxColor.r, boxColor.g, boxColor.b, 0xFF);
            SDL_Surface *surf = TTF_RenderText_Blended(font, text.c_str(), textColor);
            if(surf){
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_FreeSurface(surf);
                if(texture){
                    int iW, iH;
                    SDL_QueryTexture(texture, nullptr, nullptr, &iW, &iH);
                    SDL_Rect destRect = {position.x + (position.w - iW)/2, position.y+(position.h - iH)/2, std::min(iW, position.w), std::min(iH, position.h)};
                    SDL_Rect srcRect = {0, 0, std::min(iW, position.w), std::min(iH, position.h)};
                    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
                    SDL_DestroyTexture(texture);
                    texture = nullptr;
                }
            }
        }
    }
}

bool Game::draw(){
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    if(position.y+position.h < 0 || position.y > h) return false;
    SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
    SDL_RenderDrawRect(renderer, &position);
    //rectangleRGBA(renderer, position.x+position.w, position.y+position.h, position.x, position.y, boxColor.r, boxColor.g, boxColor.b, 0xFF);
    //int cH = position.h + (position.y - h/4);
    //if(cH > position.h) cH = position.h;
    int cH = position.h;

    int tW=0, tH;
    if(face){
        tW = 3*position.h/4;
        tH = 3*position.h/4;
        int vtW, vtH;
        SDL_Rect destRect = {position.x, position.y, position.h, position.h};
        SDL_RenderCopy(renderer, wBorder, nullptr, &destRect);
        SDL_QueryTexture(face,nullptr,nullptr,&vtW, &vtH);
        destRect = {position.x + (position.h - tH)/2 , position.y + (position.h - tH)/2 + position.h - cH, tW, std::min(cH - position.h/4 > 0 ? cH - position.h/4 : 0, tH)};
        SDL_Rect srcRect = {0, (position.h - cH)*vtH/position.h, vtW, (vtH - (position.h - cH)*vtH/position.h)};
        SDL_RenderCopy(renderer, face, &srcRect, &destRect);
    }
    tW += position.w/15;
    tH = position.h/8;
    if(font && !text.empty()){
        int iH=10;
        SDL_Surface *surf = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), textColor, position.w-tW-position.w/15);
        if(surf){
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            if(texture){
                int iW;
                SDL_QueryTexture(texture, nullptr, nullptr, &iW, &iH);
                //SDL_Rect destRect = {position.x+(position.w - iW)/2, position.y+(position.h - iH)/2, iW, std::min(iH, position.h)};
                SDL_Rect destRect = {position.x + tW, position.y + tH, iW, std::min(iH, position.h)};
                SDL_Rect srcRect = {0, 0, std::min(iW, position.w-tW), std::min(iH, position.h)};
                SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
                SDL_DestroyTexture(texture);
                texture = nullptr;
            }
        }
    }

    if(ymove){
        SDL_QueryTexture(ymove, nullptr, nullptr, &tW, &tH);
        SDL_Rect destRect = {position.x + 63*position.w/64 - tW*position.h/3/tH, position.y + w/16, tW*position.h/3/tH, position.h/3};
        SDL_RenderCopy(renderer, ymove, nullptr, &destRect);
    }
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
    int s = abs(elements[select]->position.x - (w - elements[select]->position.w)/2);
    if(moveElement && !begin && snap){
        int t = sgn(elements[select]->position.x - (w - elements[select]->position.w)/2)*std::min(12*(int)sqrt(s), s);
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
