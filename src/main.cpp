#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

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
            menu_selector(decorator::onRandomDecor)
        );

		auto mn = CCMenu::create();
		auto modId = Mod::get()->getID();
		mn->setID(modId+"/decorate");
		mn->setZOrder(100);

        btn->setPosition({156.f, -42.f});
        mn->addChild(btn);
		this->addChild(mn);

		return true;
    }

    void onPlaytest() {
        LevelEditorLayer::onPlaytest();
        auto modId = Mod::get()->getID();
		auto menu = this->getChildByID(modId+"/decorate");
        menu->setVisible(false);
    }

    void onStopPlaytest() {
        LevelEditorLayer::onStopPlaytest();
        auto modId = Mod::get()->getID();
		auto menu = this->getChildByID(modId+"/decorate");
        menu->setVisible(true);
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
            1268,1049,915,747,745,744,743,741,740,
            718,717,680,679,678,677,676,675,660,
            399,398,397,
            287,286,
            221,203,202,201,200,
            188,187,186,185,184,183,
            141,140,
            111,105,104,101,99,98,
            89,88,84,59,58,57,56,55,
            47,46,45,
            36,36,35,33,32,31,
            22,23,24,25,26,27,28,29,30,
            13,12,11,10
        };

        return blockedIDs.contains(id);
    }

    void onRandomDecor(CCObject*) {
        
        auto editor = EditorUI::get();
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
            }

            if (newObj && Mod::get()->getSettingValue<bool>("random_rotation")) {
                newObj->setScale(
                    0.5f + static_cast<float>(rand()) / RAND_MAX * 2.f
                );
                newObj->setRotation(rand() % 360);
            }
			newObj->m_editorLayer= Mod::get()->getSettingValue<int>("Layer");
        }
    }
};