"""Contains routines for printing protocol messages in xml format.
   Based off of google.protobuf.text_format"""

from xml.dom.minidom import Document

from google.protobuf import descriptor
from google.protobuf.text_encoding import CEscape

__all__ = [ 'MessageToXML', 'MessageToDOM', 
            'CreateXmlMessage', 'CreateXmlField', 'CreateXmlFieldValue', 
            'Merge']

def MessageToXml(message, *vargs, **kwargs):
    """ Builds an xml string from the message"""
    document = MessageToDOM(message)
    return document.toxml(*vargs,**kwargs)


def MessageToDOM(message):
    """ Builds a DOM object from the message"""
    doc = Document()
    document_root = doc.createElement(message.DESCRIPTOR.name)
    doc.appendChild(document_root)

    CreateXmlMessage(message, doc, document_root)
    return doc

def CreateXmlMessage(message, doc, element):
    for field, value in message.ListFields():
        if field.label == descriptor.FieldDescriptor.LABEL_REPEATED:
            for sub_element in value:
                CreateXmlField(field, sub_element, doc, element)
        else:
            CreateXmlField(field, value, doc, element)
    return

def CreateXmlField(field, value, doc, element):
    """Print a single field name/value pair.  For repeated fields, the value
    should be a single element."""
    if field.is_extension:
        if (field.containing_type.GetOptions().message_set_wire_format and
            field.type == descriptor.FieldDescriptor.TYPE_MESSAGE and
            field.message_type == field.extension_scope and
            field.label == descriptor.FieldDescriptor.LABEL_OPTIONAL):
            
            field_element = doc.createElement(field.message_type.full_name)
            CreateXmlFieldValue(field, value, doc, field_element)
            element.appendChild(field_element)   
        else:
            field_element = doc.createElement(field.full_name)
            CreateXmlFieldValue(field, value, doc, field_element)
            element.appendChild(field_element)   
    elif field.type == descriptor.FieldDescriptor.TYPE_GROUP:
        # For groups, use the capitalized name.
        field_element = doc.createElement(field.message_type.full_name)
        CreateXmlFieldValue(field, value, doc, field_element)
        element.appendChild(field_element)   
    else:
        field_element = doc.createElement(field.name)
        CreateXmlFieldValue(field, value, doc, field_element)
        element.appendChild(field_element)   
        
    return

def CreateXmlFieldValue(field, value, doc, element):
    if field.cpp_type == descriptor.FieldDescriptor.CPPTYPE_MESSAGE:
        CreateXmlMessage(value,doc,element)
    elif field.cpp_type == descriptor.FieldDescriptor.CPPTYPE_ENUM:
        # TM wonder if enum types should be element attributes
        field_value = doc.createTextNode(str(field.enum_type.values_by_number[value].name))
        element.appendChild(field_value)
    elif field.cpp_type == descriptor.FieldDescriptor.CPPTYPE_STRING:
        # should this be escaped?
#        field_value = doc.createTextNode(str(_CEscape(value)))
        field_value = doc.createTextNode(value)
        element.appendChild(field_value)
        pass
    elif field.cpp_type == descriptor.FieldDescriptor.CPPTYPE_BOOL:
        # lower cased boolean names, to match string output
        if value:
            field_value = doc.createTextNode("true")
            element.appendChild(field_value)
        else:
            field_value = doc.createTextNode("false")
            element.appendChild(field_value)
    else:
        field_value = doc.createTextNode(str(value))
        element.appendChild(field_value)

    return

def Merge(xml, message):
  """Merges an xml representation of a protocol message into a message object.

  Args:
    text: Message xml representation.
    message: A protocol buffer message to merge into.

  Raises:
    ParseError: On xml parsing problems.
  """
  raise NotImplementedError("Protobuf message creation from xml messages not implemented.")
