//
// Based on the original text_format.h file in the google.protobuf library
//
// Utilities for printing and parsing protocol messages in a simple xml format.
//
#ifndef GOOGLE_PROTOBUF_XML_FORMAT_H__
#define GOOGLE_PROTOBUF_XML_FORMAT_H__

#include <string>
#include "rapidxml-1.13/rapidxml.hpp"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

namespace google {
namespace protobuf {

// This class implements protocol buffer xml format.
//
// This class is really a namespace that contains only static methods.
class LIBPROTOBUF_EXPORT XmlFormat {
 public:

  // Like MessageToDOM(), but outputs xml directly to a string.
  static void PrintToXmlString(const Message& message, string* output);

  // Like ...
  static void MessageToDOM(const Message& message, rapidxml::xml_document<>* doc);

  // Class for those users which require more fine-grained control over how
  // a protobuffer message is printed out.
  class LIBPROTOBUF_EXPORT Printer {
   public:
    Printer();
    ~Printer();

    // Like XmlFormat::PrintToXmlString
    void PrintToXmlString(const Message& message, string* output);

    // Like XmlFormat::MessageToDOM
    void MessageToDOM(const Message& message, rapidxml::xml_document<>* doc);


   private:
    // Internal Print method
    void PrintXml(const Message& message,
    		rapidxml::xml_document<>* doc,
			rapidxml::xml_node<>* node);

    // Print a single field.
    void PrintXmlField(const Message& message,
                    const Reflection* reflection,
                    const FieldDescriptor* field,
            		rapidxml::xml_document<>* doc,
        			rapidxml::xml_node<>* node);

    // Utility function to return the right name function based
    // on field type
    string GetXmlFieldName(const Message& message,
                        const Reflection* reflection,
                        const FieldDescriptor* field);

    // Outputs a textual representation of the value of the field supplied on
    // the message supplied or the default value if not set.
    void PrintXmlFieldValue(const Message& message,
                        const Reflection* reflection,
                        const FieldDescriptor* field,
                        int field_index,
                 		rapidxml::xml_document<>* doc,
             			rapidxml::xml_node<>* node);

  };


 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(XmlFormat);
};

}  // namespace protobuf

}  // namespace google
#endif  // GOOGLE_PROTOBUF_XML_FORMAT_H__

