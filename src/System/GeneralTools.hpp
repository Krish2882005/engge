#pragma once
#include <ngf/Graphics/ImGuiExtensions.h>
#include <imgui.h>
#include "engge/Dialog/DialogManager.hpp"

namespace ng {
class GeneralTools {
public:
  explicit GeneralTools(Engine &engine, bool &textureVisible, bool &consoleVisible, bool &showGlobalsTable)
      : m_engine(engine), m_textureVisible(textureVisible), m_consoleVisible(consoleVisible),
        m_showGlobalsTable(showGlobalsTable) {}

  void render() {
    std::stringstream s;
    s << "Stack: " << sq_gettop(ScriptEngine::getVm());
    std::vector<std::string> stack;
    getStack(stack);
    ImGui::Combo(s.str().c_str(),
                 &m_selectedStack,
                 _DebugControls::stringGetter,
                 static_cast<void *>(&stack),
                 stack.size());
    ImGui::Text("In cutscene: %s", m_engine.inCutscene() ? "yes" : "no");
    auto dialogState = m_engine.getDialogManager().getState();
    ImGui::Text("In dialog: %s",
                ((dialogState == DialogManagerState::Active)
                 ? "yes"
                 : (dialogState == DialogManagerState::WaitingForChoice ? "waiting for choice" : "no")));

    ImGui::Separator();
    auto effects = "None\0In\0Out\0Wobble\0";
    ImGui::Combo("Effect", (int *) &m_fadeEffect, effects);
    ImGui::DragFloat("Duration", &m_fadeDuration, 0.1f, 0.f, 10.f);
    if (ImGui::Button("Fade")) {
      m_engine.fadeTo((FadeEffect) m_fadeEffect, ngf::TimeSpan::seconds(m_fadeDuration));
    }
    ImGui::Separator();

    auto gameSpeedFactor = m_engine.getPreferences().getUserPreference(PreferenceNames::EnggeGameSpeedFactor,
                                                                       PreferenceDefaultValues::EnggeGameSpeedFactor);
    if (ImGui::SliderFloat("Game speed factor", &gameSpeedFactor, 0.f, 5.f)) {
      m_engine.getPreferences().setUserPreference(PreferenceNames::EnggeGameSpeedFactor, gameSpeedFactor);
    }
    ImGui::Checkbox("Show cursor position", &_DebugFeatures::showCursorPosition);
    ImGui::Checkbox("Show hovered object", &_DebugFeatures::showHoveredObject);
    ImGui::Checkbox("Textures", &m_textureVisible);
    ImGui::Checkbox("Console", &m_consoleVisible);
    ImGui::SameLine();
    if (ImGui::SmallButton("Globals...")) {
      m_showGlobalsTable = true;
    }
  }

private:
  void getStack(std::vector<std::string> &stack) {
    HSQOBJECT obj;
    auto size = static_cast<int>(sq_gettop(ScriptEngine::getVm()));
    for (int i = 1; i <= size; ++i) {
      auto type = sq_gettype(ScriptEngine::getVm(), -i);
      sq_getstackobj(ScriptEngine::getVm(), -i, &obj);
      std::ostringstream s;
      s << "#" << i << ": ";
      switch (type) {
      case OT_NULL:s << "null";
        break;
      case OT_INTEGER:s << sq_objtointeger(&obj);
        break;
      case OT_FLOAT:s << sq_objtofloat(&obj);
        break;
      case OT_BOOL:s << (sq_objtobool(&obj) == SQTrue ? "true" : "false");
        break;
      case OT_USERPOINTER: {
        s << "userpointer";
        break;
      }
      case OT_STRING:s << sq_objtostring(&obj);
        break;
      case OT_TABLE: {
        int id;
        if (ScriptEngine::rawGet(obj, "_id", id)) {
          if (EntityManager::isActor(id)) {
            s << "actor";
          } else if (EntityManager::isRoom(id)) {
            s << "room";
          } else if (EntityManager::isObject(id)) {
            s << "object";
          } else if (EntityManager::isLight(id)) {
            s << "light";
          } else if (EntityManager::isSound(id)) {
            s << "sound";
          } else if (EntityManager::isThread(id)) {
            s << "thread table";
          } else {
            s << "table";
          }
        } else {
          s << "table";
        }
      }
        break;
      case OT_ARRAY:s << "array";
        break;
      case OT_CLOSURE: {
        s << "closure: ";
        auto pName = _closure(obj)->_function->_name;
        auto pSourcename = _closure(obj)->_function->_sourcename;
        auto line = _closure(obj)->_function->_lineinfos->_line;
        s << (pName._type != OT_NULL ? _stringval(pName) : "null");
        s << ' ' << (pSourcename._type != OT_NULL ? _stringval(pSourcename) : "-null");
        s << " [" << line << ']';
        break;
      }
      case OT_NATIVECLOSURE:s << "native closure";
        break;
      case OT_GENERATOR:s << "generator";
        break;
      case OT_USERDATA:s << "user data";
        break;
      case OT_THREAD: {
        s << "thread";
        auto pThread = EntityManager::getThreadFromVm(_thread(obj));
        if (pThread) {
          s << " " << pThread->getName();
        }
        break;
      }
      case OT_INSTANCE:s << "instance";
        break;
      case OT_WEAKREF:s << "weak ref";
        break;
      default:s << "?";
        break;
      }
      stack.push_back(s.str());
    }
  }

private:
  Engine &m_engine;
  int m_selectedStack{0};
  bool &m_textureVisible;
  bool &m_consoleVisible;
  bool &m_showGlobalsTable;
  int m_fadeEffect{0};
  float m_fadeDuration{3.f};
};
}