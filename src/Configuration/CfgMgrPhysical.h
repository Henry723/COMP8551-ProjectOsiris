//============================================================================
// Configuration Manager - Physical Class Interface
//
// Defines Physical Layer Class for the Configuration Manager subsystem.
//
// The Configuration Manager Subsystem purpose is to read external source(s)
// information containing game objects data backand forth through 4 layers.
// The four layers and their purpose are defined as :
//
//  Layer 1. Application : Implements the API between its clients and the
//                         link between the presentation layers.It hides
//                         from clients any management or access for
//                         configuration. Configuration access is with the
//                         use of game objects of a type IDand a given
//                         instance of type.
//        2. Presentation : Implements the link between layer 1 and 3 by
//                          unwrapping app game objects to the data layer
//                          format or wrapping data layer data into usable
//                          layer 1 app objects.
//        3. Data : Implements the link between layer 2 and 4 containing a
//                  raw data format of game object collections.
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

using namespace std;

class CCfgMgrPhysical
{
private:
	tinyxml2::XMLDocument doc;
public:

	CCfgMgrPhysical(const char* fileName);
	void PrintDocument();
	void LoadObjects(EntityManager& em);

	//Component loaders
	Model3D GetModel3DComponent(tinyxml2::XMLElement* data);
	Transform GetTransformComponent(tinyxml2::XMLElement* data);
	Rigidbody GetRigidbodyComponent(tinyxml2::XMLElement* data);

	//Vector parsers
	glm::vec2 ParseVec2(const char* data);
	glm::vec3 ParseVec3(const char* data);
	glm::vec4 ParseVec4(const char* data);
};

#endif /* defined(__ConfigurationManager__CfgMgrPhysical__) */
