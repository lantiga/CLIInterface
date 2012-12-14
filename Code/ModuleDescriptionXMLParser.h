#ifndef ModuleDescriptionXMLParser_h
#define ModuleDescriptionXMLParser_h

#include "BaseXMLParser.h"
#include "tinyxml2.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace tinyxml2;

class ModuleDescriptionXMLParser : public BaseXMLParser
{
public:

  ModuleDescriptionXMLParser()
  {
    _group = NULL;
    _parameter = NULL;
    _typeDescriptionMap["boolean"] = "bool";
    _typeDescriptionMap["integer"] = "int";
    _typeDescriptionMap["float"] = "float";
    _typeDescriptionMap["double"] = "double";
    // TODO: complete
  }

  ~ModuleDescriptionXMLParser()
  {
  }

  const char* GetModuleInfo(const char* tag)
  {
    XMLElement* node = this->DocumentFirstChildElement("executable")->FirstChildElement(tag);
    if (!node)
    {
      return "";
    }
    return node->GetText();
  }

  int VisitFirstParameterGroup()
  { 
    _group = this->DocumentFirstChildElement("executable")->FirstChildElement("parameters"); 
    return _group ? 1 : 0;
  }

  int VisitNextParameterGroup()
  { 
    if (!_group)
    {
      return 0;
    }
    _group = _group->NextSiblingElement("parameters");
    return _group ? 1 : 0;
  }

  bool ParameterGroupValid()
  { 
    return _group != NULL; 
  }

  bool ParameterValid()
  { 
    return _parameter != NULL; 
  }

  bool IsGroupAdvanced()
  { 
    return _group->Attribute("advanced","true"); 
  }

  int VisitFirstParameterInGroup(const char* value = 0)
  {
    if (!_group)
    {
      return 0;
    }
    XMLElement* node = _group->FirstChildElement(value);
    while (!this->IsParameterNode(node->Value()))
    {
      node = node->NextSiblingElement();
      if (!node)
      {
        break;
      }
    }
    _parameter = node;
    return node ? 1 : 0;
  }

  int VisitNextParameterInGroup(const char* value = 0)
  {
    if (!_group)
    {
      return 0;
    }
    if (!_parameter)
    {
      return 0;
    }
    XMLElement* node = _parameter->NextSiblingElement(value);
    if (!node)
    {
      _parameter = node;
      return 0;
    }
    while (!this->IsParameterNode(node->Value()))
    {
      node = node->NextSiblingElement();
      if (!node)
      {
        break;
      }
    }
    _parameter = node;
    return node ? 1 : 0;
  }

  int VisitParameterByName(const char* name)
  {
    int done = 0;
    this->VisitFirstParameterGroup();
    this->VisitFirstParameterInGroup();
    while (!done)
    {
      const char* value = this->GetParameterTag("name");
      if (value)
      {
        if (strcmp(value,name) == 0)
        {
          return 1;
        }
      }
      if (!this->VisitNextParameterInGroup())
      {
        if (!this->VisitNextParameterGroup())
        {
          break;
        }
      }
    }
    return 0;
  }

  const char* GetGroupTag(const char* tag)
  {
    return this->GetChildText(_group,tag);
  }

  const char* GetGroupAttribute(const char* attribute)
  {
    return this->GetAttributeText(_group,attribute);
  }

  bool GetParameterTagAsBool(const char* tag, const char* subtag = 0)
  {
    const char* value = this->GetParameterTag(tag,subtag);
    if (!value)
    {
      return false;
    }
    if (strcmp(value,"true") == 0 || strcmp(value,"1") == 0)
    {
      return true;
    }
    else if (strcmp(value,"false") == 0 || strcmp(value,"0") == 0)
    {
      return false;
    }
    return false;
  }

  int GetParameterTagAsInt(const char* tag, const char* subtag = 0)
  {
    const char* value = this->GetParameterTag(tag,subtag);
    if (!value)
    {
      return 0;
    }
    return atoi(value);
  }

  float GetParameterTagAsFloat(const char* tag, const char* subtag = 0)
  {
    const char* value = this->GetParameterTag(tag,subtag);
    if (!value)
    {
      return 0.0f;
    }
    return atof(value);
  }

  double GetParameterTagAsDouble(const char* tag, const char* subtag = 0)
  {
    const char* value = this->GetParameterTag(tag,subtag);
    if (!value)
    {
      return 0.0;
    }
    return atof(value);
  }

  // TODO: implement specialized return types as needed

  template<typename T>
  T GetParameterTagT(const char* tag, const char* subtag = 0);

  const char* GetParameterTag(const char* tag, const char* subtag = 0)
  {
    if (strcmp(tag,"constraints") == 0)
    {
      if (!_parameter)
      {
        return NULL;
      }
      XMLElement* child = _parameter->FirstChildElement(tag);
      return this->GetChildText(child,subtag);
    }
    return this->GetChildText(_parameter,tag);
  }

  const char* GetParameterTypeDescription()
  {
    if (!_parameter)
    {
      return NULL;
    }
    const char* tag = _parameter->Value();
    return this->GetTypeDescription(tag);
  }

  const char* GetParameterAttribute(const char* attribute)
  {
    return this->GetAttributeText(_parameter,attribute);
  }

  bool GetParameterAttributeAsBool(const char* attribute)
  {
    return this->GetAttributeAsBool(_parameter,attribute);
  }

  int GetParameterAttributeAsInt(const char* attribute)
  {
    return this->GetAttributeAsInt(_parameter,attribute);
  }

  float GetParameterAttributeAsFloat(const char* attribute)
  {
    return this->GetAttributeAsFloat(_parameter,attribute);
  }

  double GetParameterAttributeAsDouble(const char* attribute)
  {
    return this->GetAttributeAsDouble(_parameter,attribute);
  }

protected:

  const char* GetTypeDescription(const char* tag)
  {
    if (_typeDescriptionMap.count(tag) == 0)
    {
      return NULL;
    }
    return _typeDescriptionMap[tag].c_str();
  }

  bool IsParameterNode(const char* value)
  {
    // TODO: maybe more useful to specify what is not parameter
    const char* excluded_tags[] = {"label", "description", NULL};
    bool res = true;
    for (unsigned int i=0; excluded_tags[i]; i++)
    {
      if (strcmp(value,excluded_tags[i]) == 0)
      {
        res = false;
        break;
      }
    }
    return res; 
  }
 
  const char* GetChildText(XMLElement* node, const char* value)
  {
    if (!node)
    {
      return NULL;
    }
    XMLElement* child = node->FirstChildElement(value);
    return child ? child->GetText() : NULL;
  }
 
  const char* GetAttributeText(XMLElement* node, const char* name)
  {
    if (!node)
    {
      return NULL;
    }
    return node->Attribute(name);
  }

  bool GetAttributeAsBool(XMLElement* node, const char* name)
  {
    if (!node)
    {
      return NULL;
    }
    return node->BoolAttribute(name);
  }

  int GetAttributeAsInt(XMLElement* node, const char* name)
  {
    if (!node)
    {
      return NULL;
    }
    return node->IntAttribute(name);
  }

  float GetAttributeAsFloat(XMLElement* node, const char* name)
  {
    if (!node)
    {
      return NULL;
    }
    return node->FloatAttribute(name);
  }

  double GetAttributeAsDouble(XMLElement* node, const char* name)
  {
    if (!node)
    {
      return NULL;
    }
    return node->DoubleAttribute(name);
  }

private: 
  XMLElement* _group;
  XMLElement* _parameter;

  std::map<std::string,std::string> _typeDescriptionMap;
};

template<>
const char* ModuleDescriptionXMLParser::GetParameterTagT<const char*>(const char* tag, const char* subtag)
{
  return this->GetParameterTag(tag,subtag);
}

template<>
bool ModuleDescriptionXMLParser::GetParameterTagT<bool>(const char* tag, const char* subtag)
{
  return this->GetParameterTagAsBool(tag,subtag);
}

template<>
int ModuleDescriptionXMLParser::GetParameterTagT<int>(const char* tag, const char* subtag)
{
  return this->GetParameterTagAsInt(tag,subtag);
}

template<>
float ModuleDescriptionXMLParser::GetParameterTagT<float>(const char* tag, const char* subtag)
{
  return this->GetParameterTagAsFloat(tag,subtag);
}

template<>
double ModuleDescriptionXMLParser::GetParameterTagT<double>(const char* tag, const char* subtag)
{
  return this->GetParameterTagAsDouble(tag,subtag);
}

// TODO: implement further specializations as needed

#endif
