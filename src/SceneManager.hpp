#pragma once
#include "headers.h"

struct SceneManager
{
private:
  string filename;
  bool newScene;

public:
  bool isNewScene()
  {
    return this->newScene;
  }
  string getScene()
  {
    this->newScene = false;
    return this->filename;
  }
  void setScene(const string _filename)
  {
    this->newScene = true;
    this->filename = _filename;
  }

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
  SceneManager() : filename(""), newScene(false) {}
  ~SceneManager() = default;
};