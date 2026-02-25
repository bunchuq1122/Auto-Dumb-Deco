#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

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
			int maxID = 300;

			int id = minID + rand() % (maxID - minID + 1);
			float offsetX = 0;
            float offsetY = 0;
			if (Mod::get()->getSettingValue<bool>("random_offset")) {
				float offsetX = rand() % 80 - 40;
            	float offsetY = rand() % 80 - 40;
			}

            GameObject* newObj = nullptr;

			for (int i = 0; i < 10 && !newObj; i++) {
				int id = minID + rand() % (maxID - minID + 1);

				newObj = editor->createObject(id, {
					pos.x + offsetX,
					pos.y + offsetY
				});
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