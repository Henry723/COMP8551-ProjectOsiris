//============================================================================
// Configuration Manager - Physical Class Interface
//
// Defines Physical Layer Class for the Configuration Manager subsystem.
//
// The Configuration Manager Subsystem purpose is to read external source(s)
// information containing game objects data or custom data back and forth
// through 2 layers. The two layers and their purpose are defined as:
//
//  Layer 1. Application : Implements the API between its clients and the
//                         link between the physical layer.
//        4. Physical : Implements the mechanism to retrieve or write data
//                      from sources presented in their correct formats.
//
//  This layer methodology assures that each layer is independent and could
//  change without affecting its adjacent layer providing the signatures
//  between the two remain the same.
//
//  NOTE: We are using XML as the physical protocol using TinyXML2 module
//        to do the work. So this is a wrapper as it should be. See the 
//        TinyXML2 module for appropriate credit of the work.
//
//  Created by Kristoffer Olsson
// 
//============================================================================

#ifndef __ConfigurationManager__CfgMgrPhysical__
#define __ConfigurationManager__CfgMgrPhysical__

#include "tinyxml2.h"
#include "../components.hpp"
#include <iostream>
#include <sstream>

using namespace std;

class CCfgMgrPhysical
{
private:
	tinyxml2::XMLDocument doc;
	ModelImporter* model_imp;
public:

	CCfgMgrPhysical(const char* fileName);
	~CCfgMgrPhysical();

	void PrintDocument();
	void LoadObjects(EntityManager& em);
	void LoadLevel(EntityManager& em);
	tinyxml2::XMLElement* FindObject(const char* name, tinyxml2::XMLElement* root);
	void CreateEntityAtPosition(tinyxml2::XMLElement* data, EntityManager& em, int x, int y);
	void elementtostring(tinyxml2::XMLElement* element);
	Transform CreateTransformAtPosition(tinyxml2::XMLElement* data, int x, int y);
	Rigidbody CreateRigidbodyAtPosition(tinyxml2::XMLElement* data, int x, int y);
	Health CreateHealth(tinyxml2::XMLElement* data);

	//Component loaders
	Models3D GetModels3DComponent(tinyxml2::XMLElement* data);
	Transform GetTransformComponent(tinyxml2::XMLElement* data);
	Rigidbody GetRigidbodyComponent(tinyxml2::XMLElement* data);
	CommandFlags CreateCommandFlags(tinyxml2::XMLElement* data);

	//Vector parsers
	glm::vec2 ParseVec2(const char* data);
	glm::vec3 ParseVec3(const char* data);
	glm::vec4 ParseVec4(const char* data);
};

#endif /* defined(__ConfigurationManager__CfgMgrPhysical__) */
