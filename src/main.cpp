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
bool loading=false;
Json::Value flightValues;
Json::Value weatherValues;
Json::Value flightValue;
std::string numDays;

size_t write_data(void *ptr, size_t size, size_t nmemb, void* stream){
    size_t written;
    written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}

std::string space_to_underscore(std::string value){
    for (size_t pos = value.find(' ');
         pos != std::string::npos;
         pos = value.find(' ', pos))
    {
        value.replace(pos, 1, "_");
    }
    return value;
}


Json::Value cityFromQuery(std::string query){
    HTTPRequest req("http://autocomplete.wunderground.com/aq");
    req.addURI("query", query.substr(0,10));
    req.sendRequest((std::string(pref_path)+"/city.json").c_str());

    Json::Reader reader;
    std::ifstream d(std::string(pref_path)+"/city.json");
    Json::Value retValue;
    reader.parse(d, retValue);

    return retValue;
}

int loadWeather(std::string xlocation){
    loading = true;
    Json::Value location = cityFromQuery(xlocation);
    std::string tLocation = location["RESULTS"][0]["name"].asString();
    int pos = tLocation.find(', ');
    SDL_Log("String: %s", tLocation.c_str());
    SDL_Log("Length: %d, Pos: %d", tLocation.size(), pos);
    std::string city = tLocation.substr(0, pos-1);
    std::string state = tLocation.substr(pos+1, std::string::npos);

    HTTPRequest req(std::string("http://api.wunderground.com/api/b5a7e5f9aa745e7a/forecast/q/")
                                + state + '/' + space_to_underscore(city) + ".json");
    req.sendRequest((std::string(pref_path) + "/weather.json").c_str());

    Json::Reader reader;
    std::ifstream d(std::string(pref_path)+"/weather.json");
    reader.parse(d, weatherValues);
    loading = false;
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

std::string cityFromCode(std::string code){
    HTTPRequest req(std::string("http://airportcode.riobard.com/airport/") + code);
    req.addURI("fmt", "JSON");
    req.sendRequest((std::string(pref_path)+"/airport.json").c_str());

    Json::Reader reader;
    std::ifstream d(std::string(pref_path)+"/airport.json");
    Json::Value retValue, loadValue;
    reader.parse(d, loadValue);

    return loadValue["location"].asString() + " (" + code + ')';
}

int loadFlights(void* data){
    loading = true;
    Json::Value source = codeFromString(start);
    int k=0;
    for(int i=0; i<source.size(); i++){
        HTTPRequest req("https://api.test.sabre.com/v1/shop/flights/fares");
        req.addURI("origin", source[i].asString());
        req.addURI("earliestdeparturedate", "2015-03-02");
        req.addURI("latestdeparturedate", "2015-03-06");
        req.addURI("lengthofstay", numDays);
        req.addURI("theme", dest);
        req.addURI("topdestinations", Json::valueToString(15/source.size()));
        req.addURI("pointofsalecountry", "US");
        req.setHeader("Authorization: Bearer Shared/IDL:IceSess\/SessMgr:1\.0.IDL/Common/!ICESMS\/ACPCRTD!ICESMSLB\/CRT.LB!-0123456789012345678!123456!0!ABCDEFGHIJKLM!E2E-1");
        req.sendRequest((std::string(pref_path)+"/flights.json").c_str());

        Json::Reader reader;
        Json::Value tempValue;
        std::ifstream d(std::string(pref_path)+"/flights.json");
        reader.parse(d, tempValue);
        for(int j=0; j<tempValue["FareInfo"].size(); j++){
            flightValues["FareInfo"][k] = tempValue["FareInfo"][j];
            flightValues["FareInfo"][k++]["OriginLocation"] = tempValue["OriginLocation"];
        }
    }
    loading=false;
    return 0;
}

int loadFlightDetails(void* data){
    loading = true;

    Json::Value flight = flightValues["FareInfo"][(int)data];

    HTTPRequest req(flight["Links"][0]["href"].asString());
    req.setHeader("Authorization: Bearer Shared/IDL:IceSess\/SessMgr:1\.0.IDL/Common/!ICESMS\/ACPCRTD!ICESMSLB\/CRT.LB!-0123456789012345678!123456!0!ABCDEFGHIJKLM!E2E-1");
    req.sendRequest((std::string(pref_path)+"/flight.json").c_str());

    Json::Reader reader;
    Json::Value tempValue;
    std::ifstream d(std::string(pref_path)+"/flight.json");
    reader.parse(d, flightValue);

    loading = false;
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

SDL_Texture* loadURLImage(std::string url, std::string cacheFile){
    if(!fileExists(cacheFile)){
        HTTPRequest req(url);
        req.sendRequest(cacheFile.c_str());
    }
    SDL_Surface* t = IMG_Load_RW(SDL_RWFromFile(cacheFile.c_str(), "rb"), 1);
    SDL_Texture* t2 = SDL_CreateTextureFromSurface(renderer, t);
    SDL_FreeSurface(t);

    return t2;
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
        if(loading) views.push_front(std::make_shared<LoadingView>(&viewController));
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
