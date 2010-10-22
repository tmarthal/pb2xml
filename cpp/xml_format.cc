#include <sstream>
#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

#include "xml_format.h"


#define XML_TRUE_STRING "true"
#define XML_FALSE_STRING "false"

using namespace std;

namespace google {
namespace protobuf {



XmlFormat::Printer::Printer() {}
XmlFormat::Printer::~Printer() {}

void XmlFormat::Printer::PrintToXmlString(const Message& message,
                                        string* output) {
  GOOGLE_DCHECK(output) << "output specified is NULL";

  output->clear();

  // create the xml dom
  rapidxml::xml_document<> doc;

  MessageToDOM(message,&doc);

  std::stringstream ss;
  ss << doc;
  *output = ss.str();

}


void XmlFormat::Printer::MessageToDOM(const Message& message, rapidxml::xml_document<>* doc) {
	// xml version and encoding
	rapidxml::xml_node<>* xml_decl = doc->allocate_node(rapidxml::node_declaration);
	xml_decl->append_attribute(doc->allocate_attribute("version", "1.0"));
	xml_decl->append_attribute(doc->allocate_attribute("encoding", "utf-8"));
	doc->append_node(xml_decl);

	// the root node of the protobuf xml is the name of the protobuf container
	rapidxml::xml_node<> *root_node = doc->allocate_node(rapidxml::node_element, message.GetDescriptor()->name().c_str());
	doc->append_node(root_node);

	PrintXml(message, doc, root_node);
}

void XmlFormat::Printer::PrintXml(const Message& message,
								rapidxml::xml_document<>* doc,
								rapidxml::xml_node<>* node) {

	const Reflection* reflection = message.GetReflection();
	vector<const FieldDescriptor*> fields;
	reflection->ListFields(message, &fields);
	for (unsigned int i = 0; i < fields.size(); i++) {
		PrintXmlField(message,reflection, fields[i], doc, node);
	}

}


void XmlFormat::Printer::PrintXmlField(const Message& message,
                                     const Reflection* reflection,
                                     const FieldDescriptor* field,
                             		rapidxml::xml_document<>* doc,
                         			rapidxml::xml_node<>* node) {
	int count = 0;
	if (field->is_repeated()) {
		count = reflection->FieldSize(message, field);
	} else if (reflection->HasField(message, field)) {
		count = 1;
	}

	for (int j = 0; j < count; ++j) {
		// Write the field value.
		int field_index = j;
		if (!field->is_repeated()) {
			field_index = -1;
		}

		PrintXmlFieldValue(message, reflection, field, field_index, doc, node);
	}
}


string XmlFormat::Printer::GetXmlFieldName(const Message& message,
                                         const Reflection* reflection,
                                         const FieldDescriptor* field) {
	if (field->is_extension()) {
		// We special-case MessageSet elements for compatibility with proto1.
		if (field->containing_type()->options().message_set_wire_format()
				&& field->type() == FieldDescriptor::TYPE_MESSAGE
				&& field->is_optional()
				&& field->extension_scope() == field->message_type()) {
			return field->message_type()->full_name();
		} else {
			return field->full_name();
		}
	} else {
		if (field->type() == FieldDescriptor::TYPE_GROUP) {
			// Groups must be serialized with their original capitalization.
			return field->message_type()->name();
		} else {
			return field->name();
		}
	}
}

void XmlFormat::Printer::PrintXmlFieldValue(
    const Message& message,
    const Reflection* reflection,
    const FieldDescriptor* field,
    int field_index,
	rapidxml::xml_document<>* doc,
	rapidxml::xml_node<>* node) {

	GOOGLE_DCHECK(field->is_repeated() || (field_index == -1))
    		<< "field_index must be -1 for non-repeated fields";

	switch (field->cpp_type()) {

	//tm use the preprocessor to generate the numerical value cases
	// replace the google used string methods with using a stringstream
	#define OUTPUT_FIELD(CPPTYPE, METHOD, NUM_TYPE)                         \
      case FieldDescriptor::CPPTYPE_##CPPTYPE: {                              \
        NUM_TYPE value = field->is_repeated() ?                      \
          reflection->GetRepeated##METHOD(message, field, field_index) :     \
          reflection->Get##METHOD(message, field);                          \
        stringstream number_stream; \
	    number_stream << value; \
    	rapidxml::xml_node<> *string_node = doc->allocate_node(              \
    	  rapidxml::node_element,                                      \
    	  GetXmlFieldName(message, reflection, field).c_str(),         \
    	  number_stream.str().c_str());                                              \
    	node->append_node(string_node);                                      \
        break;                                                               \
      }

      OUTPUT_FIELD( INT32,  Int32, int32_t);
      OUTPUT_FIELD( INT64,  Int64, int64_t);
      OUTPUT_FIELD(UINT32, UInt32, uint32_t);
      OUTPUT_FIELD(UINT64, UInt64, uint64_t);
      OUTPUT_FIELD( FLOAT,  Float, float);
      OUTPUT_FIELD(DOUBLE, Double, double);
#undef OUTPUT_FIELD

	case FieldDescriptor::CPPTYPE_STRING: {
		string scratch;
        const string& value = field->is_repeated() ?
            reflection->GetRepeatedStringReference(
              message, field, field_index, &scratch) :
            reflection->GetStringReference(message, field, &scratch);

    	rapidxml::xml_node<> *string_node = doc->allocate_node(rapidxml::node_element,
    			GetXmlFieldName(message, reflection, field).c_str(),
    			value.c_str());
    	node->append_node(string_node);

        break;
    }

    case FieldDescriptor::CPPTYPE_BOOL: {
        if (field->is_repeated()) {
        	if (reflection->GetRepeatedBool(message, field, field_index)) {
        		rapidxml::xml_node<> *bool_node = doc->allocate_node(rapidxml::node_element,
        			GetXmlFieldName(message, reflection, field).c_str(),
        			XML_TRUE_STRING);
        		node->append_node(bool_node);
        	} else {
        		rapidxml::xml_node<> *bool_node = doc->allocate_node(rapidxml::node_element,
        			GetXmlFieldName(message, reflection, field).c_str(),
        			XML_FALSE_STRING);
        		node->append_node(bool_node);
        	}
        } else {
        	if (reflection->GetBool(message,field)) {
        		rapidxml::xml_node<> *bool_node = doc->allocate_node(rapidxml::node_element,
        			GetXmlFieldName(message, reflection, field).c_str(),
        			XML_TRUE_STRING);
        		node->append_node(bool_node);
        	} else {
        		rapidxml::xml_node<> *bool_node = doc->allocate_node(rapidxml::node_element,
        			GetXmlFieldName(message, reflection, field).c_str(),
        			XML_FALSE_STRING);
        		node->append_node(bool_node);
        	}
        }
        break;
    }

    case FieldDescriptor::CPPTYPE_ENUM: {
        string value = field->is_repeated() ?
          reflection->GetRepeatedEnum(message, field, field_index)->name() :
          reflection->GetEnum(message, field)->name();
    	rapidxml::xml_node<> *enum_node = doc->allocate_node(rapidxml::node_element,
    			GetXmlFieldName(message, reflection, field).c_str(),
    			value.c_str());
    	node->append_node(enum_node);
        break;
    }
    case FieldDescriptor::CPPTYPE_MESSAGE: {
    	// create the child node and recurse
    	rapidxml::xml_node<> *message_node = doc->allocate_node(rapidxml::node_element, field->name().c_str());
    	node->append_node(message_node);

    	PrintXml(field->is_repeated() ?
    	                  reflection->GetRepeatedMessage(message, field, field_index) :
    	                  reflection->GetMessage(message, field),
    	         doc, message_node);
        break;
    }
	}
}



/* static */ void XmlFormat::PrintToXmlString(
    const Message& message, string* output) {
  Printer().PrintToXmlString(message, output);
}

/* static */ void XmlFormat::MessageToDOM(
	const Message& message, rapidxml::xml_document<>* doc) {
	Printer().MessageToDOM(message, doc);
}


}  // namespace protobuf
}  // namespace google
