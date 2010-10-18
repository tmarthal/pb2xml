pb2xml
-------------

The pb2xml project is designed to output Google protobuf messages to xml.

**Python**

The python message protobuffer bindings can be created using the `--python_out` flag in the protobuf compiler.

Then the py.xml_format class can be used to generate xml output from generated protobuf message objects.

Assume in the following example that the `addressbook_pb2` object has been generated using the `addressbook.proto` definition.

> ` # Create the python message object `  
> ` > person = addressbook_pb2.Person()`  
> ` # (add various components of the person object)`  
> ` > person.id = "100" `  
> ` > ... `  
> ` # Create a string of xml with an utf-8 encoding attribute`  
> ` > string_output = xml_format.MessageToXml(person,encoding="utf-8")`  
> ` # Then any string based functions can be applied to the string`  
> ` > print string_output `  
> ` <?xml version="1.0" encoding="utf-8"?><Person> ... </Person>`
> ` # Can also get the DOM object directly`  
> ` > dom =  xml_format.MessageToDOM(person) `  
> ` # and then all dom related functions can be used`  
> ` > print xml_format.MessageToDOM(person).toprettyxml(indent="  ")`  
> `<?xml version="1.0" ?> `  
> `<Person>`  
> `... `  
> `</Person>`  



A `addressbook.proto` sample protobuf definition is in the `test/proto` directory.

**C++**

TBD



