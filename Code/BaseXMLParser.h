#ifndef BaseXMLParser_h
#define BaseXMLParser_h

#include "tinyxml2.h"

using namespace tinyxml2;

class BaseXMLParser
{
public:

  BaseXMLParser()
  {
    _document = new XMLDocument();
  }

  ~BaseXMLParser()
  {
    delete _document;
    _document = NULL;
  }

  void LoadXML(const char* filename)
  { _document->LoadFile(filename); }

  void Parse(const char* xml)
  { _document->Parse(xml); }

  void Print()
  { _document->Print(); } 

  const char* CStr()
  {
    XMLPrinter printer;
    _document->Print( &printer );
    return printer.CStr();
  }

protected:

  XMLNode* DocumentFirstChildElement(const char* value = 0)
  {
    if (!_document)
    {
      return NULL;
    }
    return _document->FirstChildElement(value);
  }
 
private: 
  XMLDocument* _document;
};

#endif
