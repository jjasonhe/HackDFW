#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <iostream>
#include <fstream>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "SDL_image.h"

#include "main.h"
#include "View.h"
#include "Views.h"
#include "SocialNetworks/XFacebook.h"
//#include "SocialNetworks/XGooglePlus.h"
#include "Bluetooth/XBluetooth.h"
#include "HTTP/XHTTP.h"
//#include "Location/XLocation.h"
#include "Notifications/XNotification.h"

SDL_Window *window;
SDL_Renderer *renderer;
std::deque<std::shared_ptr<View> > views;
std::deque<std::shared_ptr<Overlay> > overlays;
#ifdef __ANDROID_API__
    JNIEnv* env;
    jobject activity;
#endif
const char* pref_path;
EventController viewController;
EventController overlayController;
std::string uid;
std::string xClassPath = "com/myapp/game/MyGame";
Json::Value flights;
std::string start;
std::string dest;
int lastmove;

size_t write_data(void *ptr, size_t size, size_t nmemb, void* stream){
    size_t written;
    written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}

Json::Value codeFromString(std::string name){
    HTTPRequest req("http://airportcode.riobard.com/search");
    req.addURI("q", name);
    req.addURI("fmt", "JSON");
    req.sendRequest((std::string(pref_path)+"/airport.json").c_str());

    Json::Reader reader;
    std::ifstream d(std::string(pref_path)+"/airport.json");
    Json::Value retValue, loadValue;
    reader.parse(d, loadValue);

    for(int i=0; i<loadValue.size(); i++){
        retValue[i] = loadValue[i]["code"];
    }

    return retValue;
}

int loadAirport(void* data){
    Json::Value dests = codeFromString(dest), source = codeFromString(start);

    HTTPRequest req("");
}

SDL_Texture* loadImage(const char* path){
    SDL_Surface* tSurf = IMG_Load(path);

    if(!tSurf) return nullptr;

    SDL_Texture* tText = SDL_CreateTextureFromSurface(renderer, tSurf);
    SDL_FreeSurface(tSurf);

    return tText;
}

bool enclosedPoint(SDL_Point &point, SDL_Rect &rect){
	bool ret = true;
	ret &= abs(point.x - (rect.x + rect.w/2))*2 < rect.w/2;
	ret &= abs(point.y - (rect.y + rect.h/2))*2 < rect.h/2;
	return ret;
}

template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

#ifdef __ANDROID_API__
    std::string getUID(){
        jclass  activityClass = env->GetObjectClass(activity);

        jmethodID  mid_getContentResolver =env->GetMethodID(activityClass,"getContentResolver","()Landroid/content/ContentResolver;");

        jobject contentObj =  env->CallObjectMethod(activity, mid_getContentResolver);

        if(contentObj == NULL)
            return "method 1 null";

        jclass secClass=env->FindClass("android/provider/Settings$Secure");

        if(secClass == NULL)
            return "class 2 null";

        jmethodID secMid = env->GetStaticMethodID(secClass,"getString","(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;");

        if(secMid == NULL)
            return "method 2 null";

        jstring jStringParam = env->NewStringUTF("android_id");
        jstring jandroid_id = (jstring) env->CallStaticObjectMethod(secClass,secMid,contentObj,jStringParam);
        env->DeleteLocalRef(jStringParam);

        if(jandroid_id == NULL)
            return "android id null";

        const char *nativeString = env->GetStringUTFChars(jandroid_id, JNI_FALSE);

        return std::string(nativeString);
    }
#else
    std::string getUID(){
        return to_string(rand());
    }
#endif //__ANDROID_API__

std::string url_encode(std::string value){
    for (size_t pos = value.find(' ');
         pos != std::string::npos;
         pos = value.find(' ', pos))
    {
        value.replace(pos, 1, "%20");
    }
    return value;
}

using namespace std;

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    if(SDL_CreateWindowAndRenderer(0, 0, 0, &window, &renderer) < 0)
        exit(2);

	TTF_Init();

#ifdef __ANDROID_API__
    env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    activity = static_cast<jobject>(SDL_AndroidGetActivity());

    pref_path = SDL_AndroidGetInternalStoragePath();
#else
    pref_path = SDL_GetPrefPath("myapp", "game");
#endif

    uid = getUID();

    views.push_back(make_shared<WelcomeView>(&viewController));

	int millis = SDL_GetTicks();
	while(!views.empty()){
        if(!views[0]->activated) views[0]->activate();
        bool cont = true;
        while(cont){
            SDL_RenderClear(renderer);
            millis = SDL_GetTicks();
            SDL_Event event;
            while(SDL_PollEvent(&event)){
                if(!overlays.empty()){
                    if(!overlayController.process(event)) viewController.process(event);
                }
                else viewController.process(event);
            }
            cont &= views[0]->updateWorld();
            cont &= views[0]->drawWorld();
            if(!overlays.empty()){
                bool contb = false;
                if(!overlays[0]->activated)
                    overlays[0]->activate();
                contb |= overlays[0]->updateWorld();
                contb |= overlays[0]->drawWorld();
                if(contb){
                    overlays[0]->deactivate();
                    overlays.pop_front();
                }
            }
            SDL_RenderPresent(renderer);
        }
        if(views[0]->activated){
            views[0]->deactivate();
        }
        else views[0]->deactivate();
        views.pop_front();
        for(unsigned int i = 0; i<overlays.size(); i++){
            overlays[i]->deactivate();
        }
        overlays.clear();
        overlayController.clearEvents();
        viewController.clearEvents();
	}

	SDL_DestroyRenderer(renderer);
	renderer = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;

	TTF_Quit();

    exit(0);
}
