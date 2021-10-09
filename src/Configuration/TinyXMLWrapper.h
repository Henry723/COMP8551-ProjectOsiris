#include "tinyxml2.h"
#include <iostream>

using namespace std;

class TinyXMLWrapper
{
private:
	tinyxml2::XMLDocument doc;
public:

	TinyXMLWrapper(const char* fileName);
	void PrintDocument();
	void test();
	const char* getAttribute(const char* name, const char* attribute);
};