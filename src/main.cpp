#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/LevelSettingsLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/StartPosObject.hpp>

using namespace geode::prelude;

struct GlobalCache {
    StartPosObject* sp = nullptr;
    Speed pinnedSpeed;
	int pinnedMode;
	bool isMini;
	bool isDual;
	bool isDisabled;
};

inline GlobalCache& getCache() {
    static GlobalCache s_cache;
    return s_cache;
}

class $modify(pinSp, LevelSettingsLayer) {
	bool init(LevelSettingsObject* object, LevelEditorLayer* layer) {
		if (!LevelSettingsLayer::init(object, layer)) return false;

		auto& cache = getCache();

		auto mn = CCMenu::create();
		mn->setID(Mod::get()->getID() + "/pin");
		auto btn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("pin"),
			this,
			menu_selector(pinSp::pinSet)
		);
		mn->setPosition({457.f,277.f});
		mn->setContentSize({50.f,70.f});
		mn->addChild(btn);
		mn->setTouchPriority(-600);
		this->addChild(mn);
		return true;
	}

	void pinSet(CCObject*) {
		auto& cache = getCache();

		if (!cache.sp) {
			return;
		}

		cache.pinnedSpeed = cache.sp->m_startSettings->m_startSpeed;
		cache.pinnedMode = cache.sp->m_startSettings->m_startMode;
		cache.isMini = cache.sp->m_startSettings->m_startMini;
		cache.isDual = cache.sp->m_startSettings->m_startDual;
		cache.isDisabled = cache.sp->m_startSettings->m_disableStartPos;
	}
};

class $modify(editorUI, EditorUI) {
	void selectObject(GameObject* obj, bool ignoreFilter) {
        if (obj->m_isStartPos) {
            getCache().sp = typeinfo_cast<StartPosObject*>(obj);
        }

        EditorUI::selectObject(obj, ignoreFilter);
    }

	GameObject* createObject(int id, CCPoint pos) {
        auto obj = EditorUI::createObject(id, pos);

        if (!obj) return obj;

        auto& cache = getCache();

        if (obj->m_isStartPos) {
			auto sp = typeinfo_cast<StartPosObject*>(obj);
            sp->m_startSettings->m_startSpeed = cache.pinnedSpeed;
			sp->m_startSettings->m_startMode = cache.pinnedMode;
			sp->m_startSettings->m_startMini = cache.isMini;
			sp->m_startSettings->m_startDual = cache.isDual;
			sp->m_startSettings->m_disableStartPos = cache.isDisabled;
        }
		
        return obj;
    }
};