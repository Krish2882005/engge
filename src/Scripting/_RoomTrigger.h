#pragma once
#include "squirrel.h"
#include "Engine.h"
#include "Trigger.h"

namespace ng
{
class _RoomTrigger : public Trigger
{
  public:
    _RoomTrigger(Engine &engine, HSQUIRRELVM vm, Object &object, HSQOBJECT inside, HSQOBJECT outside)
        : _engine(engine), _object(object), _v(vm), _inside(inside), _outside(outside)
    {
        sq_addref(_v, &inside);
        sq_addref(_v, &outside);
        sq_resetobject(&thread_obj);

        SQInteger top = sq_gettop(vm);
        sq_newthread(_v, 1024);
        if (SQ_FAILED(sq_getstackobj(_v, -1, &thread_obj)))
        {
            std::cerr << "Couldn't get coroutine thread from stack" << std::endl;
            return;
        }
        sq_addref(_v, &thread_obj);

        SQInteger nfreevars;
        sq_pushobject(_v, _inside);
        sq_getclosureinfo(_v, -1, &_insideParamsCount, &nfreevars);
        if (SQ_SUCCEEDED(sq_getclosurename(_v, -1)))
        {
            sq_getstring(_v, -1, &_insideName);
        }
        sq_pushobject(_v, _outside);
        sq_getclosureinfo(_v, -1, &_outsideParamsCount, &nfreevars);
        if (SQ_SUCCEEDED(sq_getclosurename(_v, -1)))
        {
            sq_getstring(_v, -1, &_outsideName);
        }
        sq_settop(_v, top);

        std::wcout << L"Add room trigger (" << _object.getName() << L")" << std::endl;
    }
    ~_RoomTrigger() override
    {
        std::wcout << L"Delete room trigger" << std::endl;
        sq_release(_v, &thread_obj);
        sq_release(_v, &_inside);
        sq_release(_v, &_outside);
    }

    HSQOBJECT &getInside() { return _inside; }
    HSQOBJECT &getOutside() { return _outside; }

  private:
    void trig() override
    {
        auto actor = _engine.getCurrentActor();
        if (!actor)
            return;

        auto inObjectHotspot = _object.getRealHotspot().contains((sf::Vector2i)actor->getPosition());
        if (!_isInside && inObjectHotspot)
        {
            _isInside = true;

            std::vector<HSQOBJECT> params;
            if (_insideParamsCount == 2)
            {
                params.push_back(_inside);
                params.push_back(_object.getTable());
                params.push_back(actor->getTable());
            }
            else
            {
                params.push_back(_inside);
                params.push_back(_object.getTable());
            }

            std::string name;
            name.append("inside");
            if (_insideName)
            {
                name.append(" ").append(_insideName);
            }
            callTrigger(params, name);
        }
        else if (_isInside && !inObjectHotspot)
        {
            _isInside = false;
            if (_outside._type != SQObjectType::OT_NULL)
            {
                std::vector<HSQOBJECT> params;
                if (_outsideParamsCount == 2)
                {
                    params.push_back(_outside);
                    params.push_back(_object.getTable());
                    params.push_back(actor->getTable());
                }
                else
                {
                    sq_pushobject(_v, _outside);
                    sq_pushobject(_v, _object.getTable());
                }

                std::string name;
                name.append("outside");
                if (_outsideName)
                {
                    name.append(" ").append(_outsideName);
                }
                callTrigger(params, "outside");
            }
        }
    }

    void callTrigger(std::vector<HSQOBJECT> &params, const std::string &name)
    {
        for (auto param : params)
        {
            sq_pushobject(thread_obj._unVal.pThread, param);
        }

        std::wcout << L"call room " << towstring(name) << L" trigger (" << _object.getName() << L")" << std::endl;
        _engine.addThread(thread_obj._unVal.pThread);
        if (SQ_FAILED(sq_call(thread_obj._unVal.pThread, params.size() - 1, SQFalse, SQTrue)))
        {
            std::cerr << "failed to call room " << name << " trigger" << std::endl;
            return;
        }
    }

  private:
    Engine &_engine;
    Object &_object;
    HSQUIRRELVM _v;
    HSQOBJECT _inside;
    HSQOBJECT _outside;
    HSQOBJECT thread_obj;
    bool _isInside{false};
    SQInteger _insideParamsCount{0};
    SQInteger _outsideParamsCount{0};
    const SQChar *_insideName{nullptr};
    const SQChar *_outsideName{nullptr};
};
} // namespace ng
