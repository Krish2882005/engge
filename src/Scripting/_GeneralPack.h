#pragma once
#include <string>
#include "squirrel.h"
#include "Camera.h"
#include "Cutscene.h"
#include "Dialog/DialogManager.h"

namespace ng
{
class _GeneralPack : public Pack
{
private:
    static Engine *g_pEngine;
    static unsigned int g_CRCTab[256];

private:
    void addTo(ScriptEngine &engine) const override
    {
        g_pEngine = &engine.getEngine();
        init_crc32();
        engine.registerGlobalFunction(activeVerb, "activeVerb");
        engine.registerGlobalFunction(arrayShuffle, "arrayShuffle");
        engine.registerGlobalFunction(assetExists, "assetExists");
        engine.registerGlobalFunction(cameraAt, "cameraAt");
        engine.registerGlobalFunction(cameraPos, "cameraPos");
        engine.registerGlobalFunction(cameraBounds, "cameraBounds");
        engine.registerGlobalFunction(cameraFollow, "cameraFollow");
        engine.registerGlobalFunction(cameraInRoom, "cameraInRoom");
        engine.registerGlobalFunction(cameraPanTo, "cameraPanTo");
        engine.registerGlobalFunction(cutscene, "cutscene");
        engine.registerGlobalFunction(cutsceneOverride, "cutsceneOverride");
        engine.registerGlobalFunction(distance, "distance");
        engine.registerGlobalFunction(findScreenPosition, "findScreenPosition");
        engine.registerGlobalFunction(frameCounter, "frameCounter");
        engine.registerGlobalFunction(in_array, "in_array");
        engine.registerGlobalFunction(incutscene, "incutscene");
        engine.registerGlobalFunction(indialog, "indialog");
        engine.registerGlobalFunction(integer, "int");
        engine.registerGlobalFunction(is_array, "is_array");
        engine.registerGlobalFunction(is_function, "is_function");
        engine.registerGlobalFunction(loadArray, "loadArray");
        engine.registerGlobalFunction(markAchievement, "markAchievement");
        engine.registerGlobalFunction(markProgress, "markProgress");
        engine.registerGlobalFunction(markStat, "markStat");
        engine.registerGlobalFunction(random, "random");
        engine.registerGlobalFunction(randomFrom, "randomfrom");
        engine.registerGlobalFunction(randomOdds, "randomOdds");
        engine.registerGlobalFunction(randomOdds, "randomodds");
        engine.registerGlobalFunction(randomseed, "randomseed");
        engine.registerGlobalFunction(refreshUI, "refreshUI");
        engine.registerGlobalFunction(screenSize, "screenSize");
        engine.registerGlobalFunction(setVerb, "setVerb");
        engine.registerGlobalFunction(startDialog, "startDialog");
        engine.registerGlobalFunction(stopSentence, "stopSentence");
        engine.registerGlobalFunction(strcount, "strcount");
        engine.registerGlobalFunction(strcrc, "strcrc");
        engine.registerGlobalFunction(strfind, "strfind");
        engine.registerGlobalFunction(strfirst, "strfirst");
        engine.registerGlobalFunction(strlast, "strlast");
        engine.registerGlobalFunction(strlines, "strlines");
        engine.registerGlobalFunction(strreplace, "strreplace");
        engine.registerGlobalFunction(strsplit, "strsplit");
        engine.registerGlobalFunction(translate, "translate");
    }

    static SQInteger activeVerb(HSQUIRRELVM v)
    {
        const auto& pVerb = g_pEngine->getActiveVerb();
        if(!pVerb)
        {
            sq_pushinteger(v, 0);
            return 1;
        }

        sq_pushinteger(v, pVerb->id);
        return 1;
    }

    static SQInteger arrayShuffle(HSQUIRRELVM v)
    {
        HSQOBJECT array;
        sq_resetobject(&array);
        if (SQ_FAILED(sq_getstackobj(v, 2, &array)))
        {
            return sq_throwerror(v, "Failed to get array");
        }

        std::vector<HSQOBJECT> objs;
        sq_pushobject(v, array);
        sq_pushnull(v); //null iterator
        while (SQ_SUCCEEDED(sq_next(v, -2)))
        {
            HSQOBJECT obj;
            sq_getstackobj(v, -1, &obj);
            objs.push_back(obj);
            sq_pop(v, 2); //pops key and val before the nex iteration
        }
        sq_pop(v, 1); //pops the null iterator

        sq_newarray(v, 0);
        while (!objs.empty())
        {
            auto index = int_rand(0, objs.size() - 1);
            sq_pushobject(v, objs[index]);
            objs.erase(objs.begin() + index);
            sq_arrayappend(v, -2);
        }

        return 1;
    }

    static SQInteger assetExists(HSQUIRRELVM v)
    {
        const SQChar *filename = nullptr;
        if (SQ_FAILED(sq_getstring(v, 2, &filename)))
        {
            return sq_throwerror(v, "failed to get filename");
        }
        sq_pushbool(v, g_pEngine->getSettings().hasEntry(filename) ? SQTrue : SQFalse);
        return 1;
    }

    static SQInteger distance(HSQUIRRELVM v)
    {
        if (sq_gettype(v, 2) == OT_INTEGER)
        {
            SQInteger num1;
            if (SQ_FAILED(sq_getinteger(v, 2, &num1)))
            {
                return sq_throwerror(v, "failed to get num1");
            }
            SQInteger num2;
            if (SQ_FAILED(sq_getinteger(v, 3, &num2)))
            {
                return sq_throwerror(v, "failed to get num2");
            }
            auto d = std::abs(num1 - num2);
            sq_pushinteger(v, d);
            return 1;
        }
        auto obj1 = ScriptEngine::getEntity(v, 2);
        if (!obj1)
        {
            return sq_throwerror(v, "failed to get object1 or actor1");
        }
        auto obj2 = ScriptEngine::getEntity(v, 3);
        if (!obj2)
        {
            return sq_throwerror(v, "failed to get object2 or actor2");
        }
        auto pos1 = obj1->getRealPosition();
        auto pos2 = obj1->getRealPosition();
        auto dx = pos1.x - pos2.x;
        auto dy = pos1.y - pos2.y;
        auto d = std::sqrt(dx * dx + dy * dy);
        sq_pushfloat(v, d);
        return 1;
    }

    static SQInteger findScreenPosition(HSQUIRRELVM v)
    {
        SQInteger verb;
        sf::Vector2f pos;
        if(sq_gettype(v, 2) == OT_INTEGER)
        {
            if (SQ_FAILED(sq_getinteger(v, 2, &verb)))
            {
                return sq_throwerror(v, _SC("failed to get verb"));
            }
            pos = g_pEngine->findScreenPosition(static_cast<int>(verb));
        }
        else
        {
            auto actor = ScriptEngine::getActor(v, 2);
            if (!actor)
            {
                return sq_throwerror(v, _SC("failed to get actor"));
            }
            pos = actor->getRealPosition() - g_pEngine->getCamera().getAt();
        }
        ScriptEngine::push(v, pos);
        return 1;
    }

    static SQInteger frameCounter(HSQUIRRELVM v)
    {
        sq_pushinteger(v, g_pEngine->getFrameCounter());
        return 1;
    }

    static SQInteger in_array(HSQUIRRELVM v)
    {
        HSQOBJECT obj;
        sq_resetobject(&obj);
        if (SQ_FAILED(sq_getstackobj(v, 2, &obj)))
        {
            return sq_throwerror(v, "Failed to get object");
        }
        HSQOBJECT array;
        sq_resetobject(&array);
        if (SQ_FAILED(sq_getstackobj(v, 3, &array)))
        {
            return sq_throwerror(v, "Failed to get array");
        }

        std::vector<HSQOBJECT> objs;

        sq_pushobject(v, array);
        sq_pushnull(v); //null iterator
        while (SQ_SUCCEEDED(sq_next(v, -2)))
        {
            HSQOBJECT tmp;
            sq_getstackobj(v, -1, &tmp);
            objs.push_back(tmp);
            sq_pop(v, 2); //pops key and val before the nex iteration
        }
        sq_pop(v, 1); //pops the null iterator

        for (auto &&o : objs)
        {
            sq_pushobject(v, obj);
            sq_pushobject(v, o);
            if (sq_cmp(v) == 0)
            {
                sq_pop(v, 2);
                sq_pushinteger(v, 1);
                return 1;
            }
            sq_pop(v, 2);
        }

        sq_pushinteger(v, 0);
        return 1;
    }

    static SQInteger incutscene(HSQUIRRELVM v)
    {
        sq_pushinteger(v, g_pEngine->inCutscene() ? 1 : 0);
        return 1;
    }

    static SQInteger indialog(HSQUIRRELVM v)
    {
        sq_pushinteger(v, static_cast<SQInteger>(g_pEngine->getDialogManager().getState()));
        return 1;
    }

    static SQInteger integer(HSQUIRRELVM v)
    {
        SQFloat f = 0;
        if (SQ_FAILED(sq_getfloat(v, 2, &f)))
        {
            return sq_throwerror(v, "Failed to get float value");
        }
        sq_pushinteger(v, (SQInteger)f);
        return 1;
    }

    static SQInteger is_array(HSQUIRRELVM v)
    {
        sq_pushbool(v, sq_gettype(v, 2) == OT_ARRAY ? SQTrue : SQFalse);
        return 1;
    }

    static SQInteger is_function(HSQUIRRELVM v)
    {
        auto type = sq_gettype(v, 2);
        sq_pushbool(v, type == OT_CLOSURE || type == OT_NATIVECLOSURE ? SQTrue : SQFalse);
        return 1;
    }

    static SQInteger loadArray(HSQUIRRELVM v)
    {
        const SQChar *filename;
        if (SQ_FAILED(sq_getstring(v, 2, &filename)))
        {
            return sq_throwerror(v, "Failed to get filename");
        }
        std::vector<char> buffer;
        g_pEngine->getSettings().readEntry(filename, buffer);
        GGPackBufferStream input(buffer);
        std::string line;

        sq_newarray(v, 0);
        while (getLine(input, line))
        {
            sq_pushstring(v, line.data(), -1);
            sq_arrayappend(v, -2);
        }
        return 1;
    }

    static SQInteger strsplit(HSQUIRRELVM v)
    {
        const SQChar *text;
        if (SQ_FAILED(sq_getstring(v, 2, &text)))
        {
            return sq_throwerror(v, "Failed to get text");
        }

        const SQChar *delimiter;
        if (SQ_FAILED(sq_getstring(v, 3, &delimiter)))
        {
            return sq_throwerror(v, "Failed to get delimiter");
        }

        sq_newarray(v, 0);
        std::string token;
        std::istringstream tokenStream(text);
        while (std::getline(tokenStream, token, delimiter[0]))
        {
            sq_pushstring(v, token.data(), -1);
            sq_arrayappend(v, -2);
        }
        return 1;
    }

    static SQInteger cameraAt(HSQUIRRELVM v)
    {
        auto screen = g_pEngine->getWindow().getView().getSize();
        SQInteger x, y;
        auto numArgs = sq_gettop(v) - 1;
        if (numArgs == 2)
        {
            if (SQ_FAILED(sq_getinteger(v, 2, &x)))
            {
                return sq_throwerror(v, _SC("failed to get x"));
            }
            if (SQ_FAILED(sq_getinteger(v, 3, &y)))
            {
                return sq_throwerror(v, _SC("failed to get y"));
            }
            g_pEngine->getCamera().at(sf::Vector2f(x - screen.x / 2.f, y - screen.y / 2.f));
        }
        else
        {
            auto spot = ScriptEngine::getObject(v, 2);

            auto result = g_pEngine->setRoom(spot->getRoom());
            if (SQ_FAILED(result))
            {
                return result;
            }

            auto pos = spot->getRealPosition();
            g_pEngine->getCamera().at(sf::Vector2f(pos.x - screen.x / 2.f, pos.y - screen.y / 2.f));
        }
        return 0;
    }

    static SQInteger cameraBounds(HSQUIRRELVM v)
    {
        SQInteger xMin, xMax, yMin, yMax;
        if (SQ_FAILED(sq_getinteger(v, 2, &xMin)))
        {
            return sq_throwerror(v, _SC("failed to get xMin"));
        }
        if (SQ_FAILED(sq_getinteger(v, 3, &xMax)))
        {
            return sq_throwerror(v, _SC("failed to get xMax"));
        }
        if (SQ_FAILED(sq_getinteger(v, 4, &yMin)))
        {
            return sq_throwerror(v, _SC("failed to get yMin"));
        }
        if (SQ_FAILED(sq_getinteger(v, 5, &yMax)))
        {
            return sq_throwerror(v, _SC("failed to get yMax"));
        }
        g_pEngine->getCamera().setBounds(sf::IntRect(xMin, yMin, xMax - xMin, yMax - yMin));
        return 0;
    }

    static SQInteger cameraFollow(HSQUIRRELVM v)
    {
        auto *pActor = ScriptEngine::getActor(v, 2);
        g_pEngine->follow(pActor);
        return 0;
    }

    static SQInteger cameraPanTo(HSQUIRRELVM v)
    {
        SQInteger x, y, interpolation{0};
        auto screen = g_pEngine->getWindow().getView().getSize();
        SQFloat t;
        if (sq_gettype(v, 2) == OT_TABLE)
        {
            auto *pEntity = ScriptEngine::getEntity(v, 2);
            if (!pEntity)
            {
                return sq_throwerror(v, _SC("failed to get actor/object"));
            }
            x = static_cast<int>(pEntity->getRealPosition().x);
            y = static_cast<int>(pEntity->getRealPosition().y);
            if (SQ_FAILED(sq_getfloat(v, 3, &t)))
            {
                return sq_throwerror(v, _SC("failed to get time"));
            }
        }
        else
        {
            if (SQ_FAILED(sq_getinteger(v, 2, &x)))
            {
                return sq_throwerror(v, _SC("failed to get x"));
            }
            if (SQ_FAILED(sq_getinteger(v, 3, &y)))
            {
                return sq_throwerror(v, _SC("failed to get y"));
            }
            if (SQ_FAILED(sq_getfloat(v, 4, &t)))
            {
                return sq_throwerror(v, _SC("failed to get time"));
            }
            if (SQ_FAILED(sq_getinteger(v, 5, &interpolation)))
            {
                interpolation = 0;
            }
        }

        g_pEngine->getCamera().panTo(sf::Vector2f(x - screen.x / 2.f, y - screen.y / 2.f), sf::seconds(t), (InterpolationMethod)interpolation);
        return 0;
    }

    static SQInteger cameraPos(HSQUIRRELVM v)
    {
        auto pos = g_pEngine->getCamera().getAt();
        ScriptEngine::push(v, pos);
        return 1;
    }

    static SQInteger cutscene(HSQUIRRELVM v)
    {
        auto numArgs = sq_gettop(v);
        HSQOBJECT env_obj;
        sq_resetobject(&env_obj);
        if (SQ_FAILED(sq_getstackobj(v, 1, &env_obj)))
        {
            return sq_throwerror(v, _SC("Couldn't get environment from stack"));
        }

        // create thread and store it on the stack
        sq_newthread(v, 1024);
        HSQOBJECT threadObj;
        sq_resetobject(&threadObj);
        if (SQ_FAILED(sq_getstackobj(v, -1, &threadObj)))
        {
            return sq_throwerror(v, _SC("Couldn't get coroutine thread from stack"));
        }

        // get the cutscene  closure
        HSQOBJECT closureObj;
        sq_resetobject(&closureObj);
        if (SQ_FAILED(sq_getstackobj(v, 2, &closureObj)))
        {
            return sq_throwerror(v, _SC("failed to get cutscene closure"));
        }

        // get the cutscene override closure
        HSQOBJECT closureCutsceneOverrideObj;
        sq_resetobject(&closureCutsceneOverrideObj);
        if (numArgs == 3)
        {
            if (SQ_FAILED(sq_getstackobj(v, 3, &closureCutsceneOverrideObj)))
            {
                return sq_throwerror(v, _SC("failed to get cutscene override closure"));
            }
        }

        auto scene = std::make_unique<Cutscene>(*g_pEngine, v, threadObj, closureObj, closureCutsceneOverrideObj, env_obj);
        g_pEngine->cutscene(std::move(scene));

        return sq_suspendvm(v);
    }

    static SQInteger cutsceneOverride(HSQUIRRELVM v)
    {
        g_pEngine->cutsceneOverride();
        return 0;
    }

    static SQInteger random(HSQUIRRELVM v)
    {
        if (sq_gettype(v, 2) == OT_INTEGER)
        {
            SQInteger min = 0;
            SQInteger max = 0;
            sq_getinteger(v, 2, &min);
            sq_getinteger(v, 3, &max);
            auto value = int_rand(min, max);
            sq_pushinteger(v, value);

            return 1;
        }
        {
            SQFloat min = 0;
            SQFloat max = 0;
            sq_getfloat(v, 2, &min);
            sq_getfloat(v, 3, &max);
            auto value = float_rand(min, max);
            sq_pushfloat(v, value);
            return 1;
        }
    }

    static SQInteger randomOdds(HSQUIRRELVM v)
    {
        SQFloat value = 0;
        if (SQ_FAILED(sq_getfloat(v, 2, &value)))
        {
            return sq_throwerror(v, _SC("failed to get value"));
        }
        auto rnd = float_rand(0, 1);
        sq_pushbool(v, static_cast<SQBool>(rnd <= value));
        return 1;
    }

    static SQInteger randomseed(HSQUIRRELVM v)
    {
        error("TODO: randomseed: not implemented");
        return 0;
    }

    static SQInteger randomFrom(HSQUIRRELVM v)
    {
        if (sq_gettype(v, 2) == OT_ARRAY)
        {
            HSQOBJECT obj;
            sq_resetobject(&obj);

            auto size = sq_getsize(v, 2);
            auto index = int_rand(0, size - 1);

            int i = 0;
            sq_push(v, 2);  // array
            sq_pushnull(v); //null iterator
            while (SQ_SUCCEEDED(sq_next(v, -2)))
            {
                sq_getstackobj(v, -1, &obj);
                sq_pop(v, 2); //pops key and val before the nex iteration
                if(index == i++)
                    break;
            }
            sq_pop(v, 2); //pops the null iterator and array

            sq_pushobject(v, obj);
            return 1;
        }
        auto size = sq_gettop(v);
        auto index = int_rand(0, size - 2);
        sq_push(v, 2 + index);
        return 1;
    }

    static SQInteger refreshUI(HSQUIRRELVM v)
    {
        error("TODO: refreshUI: not implemented");
        return 0;
    }

    static SQInteger cameraInRoom(HSQUIRRELVM v)
    {
        Room *pRoom = ScriptEngine::getRoom(v, 2);
        if (!pRoom)
        {
            return sq_throwerror(v, _SC("failed to get room"));
        }
        return g_pEngine->setRoom(pRoom);
    }

    static SQInteger markAchievement(HSQUIRRELVM v)
    {
        error("TODO: markAchievement: not implemented");
        return 0;
    }

    static SQInteger markProgress(HSQUIRRELVM v)
    {
        error("TODO: markProgress: not implemented");
        return 0;
    }

    static SQInteger markStat(HSQUIRRELVM v)
    {
        error("TODO: markStat: not implemented");
        return 0;
    }

    static SQInteger screenSize(HSQUIRRELVM v)
    {
        auto screen = g_pEngine->getWindow().getView().getSize();
        ScriptEngine::push(v, screen);
        return 1;
    }

    static SQInteger setVerb(HSQUIRRELVM v)
    {
        SQInteger actorSlot;
        if (SQ_FAILED(sq_getinteger(v, 2, &actorSlot)))
        {
            return sq_throwerror(v, _SC("failed to get actor slot"));
        }
        SQInteger verbSlot;
        if (SQ_FAILED(sq_getinteger(v, 3, &verbSlot)))
        {
            return sq_throwerror(v, _SC("failed to get verb slot"));
        }
        HSQOBJECT table;
        if (SQ_FAILED(sq_getstackobj(v, 4, &table)))
        {
            return sq_throwerror(v, _SC("failed to get verb definitionTable"));
        }
        if (!sq_istable(table))
        {
            return sq_throwerror(v, _SC("failed to get verb definitionTable"));
        }

        sq_pushobject(v, table);
        // id
        sq_pushstring(v, _SC("verb"), -1);
        if (SQ_FAILED(sq_get(v, -2)))
        {
            sq_pop(v, 2);
            return sq_throwerror(v, _SC("failed to get verb"));
        }

        SQInteger id = 0;
        if (SQ_FAILED(sq_getinteger(v, -1, &id)))
        {
            sq_pop(v, 2);
            return sq_throwerror(v, _SC("failed to get verb"));
        }
        sq_pop(v, 1);

        // image
        sq_pushstring(v, _SC("image"), -1);
        if (SQ_FAILED(sq_get(v, -2)))
        {
            sq_pop(v, 2);
            return sq_throwerror(v, _SC("failed to get image"));
        }

        const SQChar *image = nullptr;
        if (SQ_FAILED(sq_getstring(v, -1, &image)))
        {
            sq_pop(v, 2);
            return sq_throwerror(v, _SC("failed to get image"));
        }
        sq_pop(v, 1);

        // text
        sq_pushstring(v, _SC("text"), -1);
        if (SQ_FAILED(sq_get(v, -2)))
        {
            sq_pop(v, 2);
            return sq_throwerror(v, _SC("failed to get text"));
        }

        const SQChar *text = nullptr;
        if (SQ_FAILED(sq_getstring(v, -1, &text)))
        {
            sq_pop(v, 2);
            return sq_throwerror(v, _SC("failed to get text"));
        }
        sq_pop(v, 1);

        // func
        sq_pushstring(v, _SC("func"), -1);
        if (SQ_FAILED(sq_get(v, -2)))
        {
            sq_pop(v, 2);
            return sq_throwerror(v, _SC("failed to get func"));
        }

        const SQChar *func = nullptr;
        if (SQ_FAILED(sq_getstring(v, -1, &func)))
        {
            sq_pop(v, 2);
            return sq_throwerror(v, _SC("failed to get func"));
        }
        sq_pop(v, 1);

        // key
        sq_pushstring(v, _SC("key"), -1);
        if (SQ_FAILED(sq_get(v, -2)))
        {
            sq_pop(v, 2);
            return sq_throwerror(v, _SC("failed to get key"));
        }

        const SQChar *key = nullptr;
        if (SQ_FAILED(sq_getstring(v, -1, &key)))
        {
            sq_pop(v, 2);
            return sq_throwerror(v, _SC("failed to get key"));
        }
        sq_pop(v, 2);

        VerbSlot slot;
        Verb verb;
        verb.id = id;
        verb.func = func;
        verb.image = image;
        verb.text = text;
        verb.key = key;
        g_pEngine->setVerb(actorSlot - 1, verbSlot, verb);
        return 0;
    }

    static SQInteger startDialog(HSQUIRRELVM v)
    {
        auto count = sq_gettop(v);
        const SQChar *dialog;
        if (SQ_FAILED(sq_getstring(v, 2, &dialog)))
        {
            return sq_throwerror(v, _SC("failed to get dialog"));
        }
        const SQChar *node = "start";
        if (count == 3)
        {
            sq_getstring(v, 3, &node);
        }
        g_pEngine->startDialog(dialog, node);
        return 0;
    }

    static SQInteger stopSentence(HSQUIRRELVM v)
    {
        g_pEngine->stopSentence();
        return 0;
    }

    static SQInteger strcount(HSQUIRRELVM v)
    {
        const SQChar *str1;
        if (SQ_FAILED(sq_getstring(v, 2, &str1)))
        {
            return sq_throwerror(v, _SC("failed to get string1"));
        }
        const SQChar *str2;
        if (SQ_FAILED(sq_getstring(v, 3, &str2)))
        {
            return sq_throwerror(v, _SC("failed to get string1"));
        }
        int count = 0;
        while((str1 = strstr(str1, str2)))
        {
            str1 += strlen(str2);
            ++count;
        }
        sq_pushinteger(v, static_cast<SQInteger>(count));
        return 1;
    }

    static void init_crc32() 
    {
        unsigned int i, j, c;
        for(i=0; i<256; i++)
        {
            for( c=i,j=0; j<8; j++ ) c=(c&1) ? (c>>1)^0xEDB88320L : (c>>1);
            g_CRCTab[i^0xFF] = c ^ 0xFF000000;
        }
    }

    static unsigned int crc32_update(unsigned int x, char c)
    { 
        return g_CRCTab[(x^c)&0xFF] ^ (x>>8);
    }

    static SQInteger strcrc(HSQUIRRELVM v)
    {
        const SQChar *str;
        if (SQ_FAILED(sq_getstring(v, 2, &str)))
        {
            return sq_throwerror(v, _SC("failed to get string"));
        }
        unsigned int x;
        for(x=0; *str++; x = crc32_update(x, str[-1]));
        sq_pushinteger(v, x);
        return 1;
    }

    static SQInteger strfind(HSQUIRRELVM v)
    {
        const SQChar *str1;
        if (SQ_FAILED(sq_getstring(v, 2, &str1)))
        {
            return sq_throwerror(v, _SC("failed to get string1"));
        }
        const SQChar *str2;
        if (SQ_FAILED(sq_getstring(v, 3, &str2)))
        {
            return sq_throwerror(v, _SC("failed to get string1"));
        }
        auto p = std::strstr(str1, str2);
        if (p == nullptr)
        {
            sq_pushinteger(v, -1);
        }
        else
        {
            sq_pushinteger(v, p - str1);
        }
        return 1;
    }

    static SQInteger strfirst(HSQUIRRELVM v)
    {
        const SQChar *str;
        if (SQ_FAILED(sq_getstring(v, 2, &str)))
        {
            return sq_throwerror(v, _SC("failed to get string"));
        }
        if(strlen(str)>0)
        {
            const SQChar s[2]{str[0],'\0'};
            sq_pushstring(v, s, 1);
            return 1;
        }
        sq_pushnull(v);
        return 1;
    }

    static SQInteger strlast(HSQUIRRELVM v)
    {
        const SQChar *str;
        if (SQ_FAILED(sq_getstring(v, 2, &str)))
        {
            return sq_throwerror(v, _SC("failed to get string"));
        }
        auto len = strlen(str);
        if(len > 0)
        {
            const SQChar s[2]{str[len-1],'\0'};
            sq_pushstring(v, s, 1);
            return 1;
        }
        sq_pushnull(v);
        return 1;
    }

    static SQInteger strlines(HSQUIRRELVM v)
    {
        const SQChar *text;
        if (SQ_FAILED(sq_getstring(v, 2, &text)))
        {
            return sq_throwerror(v, _SC("failed to get text"));
        }
        std::istringstream is(text);
        sq_newarray(v, 0);
        std::string line;
        while (std::getline(is, line))
        {
            sq_pushstring(v, line.data(), -1);
            sq_arrayappend(v, -2);
        }
        return 1;
    }

    static SQInteger strreplace(HSQUIRRELVM v)
    {
        const SQChar *input;
        const SQChar *search;
        const SQChar *replace;
        if (SQ_FAILED(sq_getstring(v, 2, &input)))
        {
            return sq_throwerror(v, _SC("failed to get input"));
        }
        if (SQ_FAILED(sq_getstring(v, 3, &search)))
        {
            return sq_throwerror(v, _SC("failed to get search"));
        }
        if (SQ_FAILED(sq_getstring(v, 4, &replace)))
        {
            return sq_throwerror(v, _SC("failed to get replace"));
        }
        std::string strInput(input);
        std::string strSearch(search);
        std::string strReplace(replace);
        replaceAll(strInput, strSearch, strReplace);
        sq_pushstring(v, strInput.data(), -1);
        return 1;
    }

    static SQInteger translate(HSQUIRRELVM v)
    {
        const SQChar *idText;
        if (SQ_FAILED(sq_getstring(v, 2, &idText)))
        {
            return sq_throwerror(v, _SC("failed to get idText"));
        }
        std::string s(idText);
        s = s.substr(1);
        auto id = std::strtol(s.c_str(), nullptr, 10);
        auto text = g_pEngine->getText(id);
        sq_pushstring(v, tostring(text).c_str(), -1);
        return 1;
    }
};

Engine *_GeneralPack::g_pEngine = nullptr;
unsigned int _GeneralPack::g_CRCTab[256] = {};

} // namespace ng
