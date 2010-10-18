from pb2xml.gen.py import addressbook_pb2
from pb2xml.py import xml_format

person = addressbook_pb2.Person()
person.id = 1234
person.name = "John Doe"
person.email = "jdoe@example.com"
phone = person.phone.add()
phone.number = "555-4321"
phone.type = addressbook_pb2.Person.HOME
phone2 = person.phone.add()
phone2.number = "123-4567"
phone2.type =  addressbook_pb2.Person.MOBILE
person.awesome = True

#print person

print xml_format.MessageToXml(person,encoding="utf-8")

# create a dom then prettify it
#print xml_format.MessageToDOM(person).toprettyxml(indent="  ")

