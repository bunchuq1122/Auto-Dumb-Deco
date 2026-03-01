#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GJSpriteColor.hpp>

using namespace geode::prelude;

#include <ctime>
#include <unordered_set>

$on_mod(Loaded) {
    srand(static_cast<unsigned>(time(nullptr)));
}

class $modify(decorator, LevelEditorLayer) {
    bool init(GJGameLevel* level, bool noUI) {
		if (!LevelEditorLayer::init(level, noUI)) return false;

        auto spr = CCSprite::createWithSpriteFrameName("GJ_paintBtn_001.png");
        spr->setScale(0.8f);

        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(decorator::onRandonDeco)
        );

		auto mn = CCMenu::create();
		auto modId = Mod::get()->getID();
		mn->setID(modId+"/decorate");
		mn->setZOrder(100);

        btn->setPosition({136.f, -62.f});
        mn->addChild(btn);
		this->addChild(mn);

        this->schedule(schedule_selector(decorator::loop),0.01f);

		return true;
    }

    void loop(float dt) {
        auto editorUI = this->m_editorUI;
        auto menu = editorUI->getChildByID("editor-buttons-menu");
        auto anyBtn = menu->getChildByID("copy-paste-button");
        this->getChildByID(Mod::get()->getID()+"/decorate")->setVisible(anyBtn->isVisible());
    }

    bool checkObj(GameObject* obj) { 
        if (Mod::get()->getSettingValue<bool>("true_random")) return false;
        if (!obj) return true;
        if (obj->m_isTrigger) return true;
        return false;
    }

    // please make m_isPortal , m_isPad robtop
    bool checkObjByID(int id) {
        if (Mod::get()->getSettingValue<bool>("true_random")) return false;
        static const std::unordered_set<int> blockedIDs = {
            1829,1817,1818,1819,1814,1815,1816,18111,1812,1859,1751,1755,1736,1735,1734,1710,1709,1708,1707,1706,1705,1704,1700,
            1620,1619,1616,1615,1613,1612,1611,1595,1585,1586,1584,1520,
            1330,1331,1329,1334,1346,1347,
            1268,1049,1022,915,901,900,747,745,744,743,741,740,
            718,717,680,679,678,677,676,675,660,
            399,398,397,
            287,286,
            221,203,202,201,200,
            188,187,186,185,184,183,
            142,141,140,
            111,105,104,101,99,98,
            89,88,84,59,58,57,56,55,
            47,46,45,
            36,36,35,33,32,31,
            22,23,24,25,26,27,28,29,30,
            13,12,11,10
        };

        return blockedIDs.contains(id);
    }

    void onRandonDeco(CCObject*) {
        std::vector<GameObject*> createdObjects;
        
        auto editor = this->m_editorUI;
        auto selected = editor->getSelectedObjects();

        if (!selected || selected->count() == 0) {
            FLAlertLayer::create("Error", "Select objects first.", "OK")->show();
            return;
        }

        for (auto obj : CCArrayExt<GameObject*>(selected)) {

            auto pos = obj->getPosition();

            int minID = 1;
			int maxID = 1911;

			int id = minID + rand() % (maxID - minID + 1);
			float offsetX = 0;
            float offsetY = 0;
			if (Mod::get()->getSettingValue<bool>("random_offset")) {
				float offsetX = rand() % 80 - 40;
            	float offsetY = rand() % 80 - 40;
			}

            GameObject* newObj = nullptr;

            int attempts = 0;

            while (!newObj && attempts < 100) {
                attempts++;

                int id = minID + rand() % (maxID - minID + 1);

                if (checkObjByID(id))
                    continue;

                auto temp = GameObject::createWithKey(id);
                if (!temp) continue;

                if (checkObj(temp))
                    continue;

                newObj = editor->createObject(id, {pos.x, pos.y});
                createdObjects.push_back(newObj);
            }

            if (newObj && Mod::get()->getSettingValue<bool>("random_rotation")) { 
                newObj->setRotation(rand() % 360);
            }
            if (newObj && Mod::get()->getSettingValue<bool>("random_scale")) {
                newObj->setScale(
                    0.5f + static_cast<float>(rand()) / RAND_MAX * 2.f
                );
            }
			newObj->m_editorLayer= Mod::get()->getSettingValue<int>("Layer");
            
            auto randomCOlorRange = Mod::get()->getSettingValue<int>("random_color_range");
            if (Mod::get()->getSettingValue<bool>("random_color")) {
                newObj->m_baseColor->m_colorID = rand() % (randomCOlorRange);
            }
        }
        if (this->m_editorUI) {

            this->m_editorUI->onDeselectAll(nullptr);

            for (auto obj : createdObjects) {
                this->m_editorUI->m_selectedObjects->addObject(obj);
                obj->m_isSelected = true;
            }
        
            this->m_editorUI->updateButtons();
        }
    }
};