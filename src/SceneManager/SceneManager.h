#pragma once
#include "../headers.h"
#include "../UI/UI.h"


struct SceneManager
{
    public:
        enum class TScene { none, mainMenu, level_1, level_2, gameOver, TotalScenes};

        bool isNewScene() { return m_isNewScene; }
        const TScene getScene() { return m_curScene; }
        const TScene assignNewScene();
        void setScene(const TScene newScene);
        TScene nextScene(void);
        string getSceneName() { return m_sceneName; }
        string getSceneLoadFile() { return m_sceneLoadFile; }

        // Forwards from system
        enum class TKeyPress { none, START, MENU, RESTART, TotalKeyPress };

        void keyPressNotification(TKeyPress keyPressed);

        static SceneManager& getInstance()
        {
            static SceneManager INSTANCE;
            return INSTANCE;
        }

        SceneManager(const SceneManager const&) = delete;
        SceneManager(const SceneManager&&) = delete;

        SceneManager& operator=(const SceneManager const&) = delete;
        SceneManager& operator=(const SceneManager&&) = delete;

    private:
        SceneManager();
        ~SceneManager() = default;

    private:
        const std::string c_sceneNameText[int(TScene::TotalScenes) + 1] = 
            { "", "Main Menu", "Level -1-", "Level -2-", "Game Over", "" };
        const std::string c_sceneLoadFileText[int(TScene::TotalScenes) + 1] =
            { "", "", "./src/Level01.xml", "./src/Level02.xml", "", "" };
        TScene m_curScene;
        TScene m_newScene;
        string m_sceneName;
        string m_sceneLoadFile;
        bool m_isNewScene;
};