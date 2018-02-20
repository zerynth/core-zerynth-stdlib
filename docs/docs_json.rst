.. module:: json

****
JSON
****

This module define functions to serialize and deserialize objects to and from `JSON <http://json.org>`_ format.
The deserialization of objects is performed using a wrapped version of the awesome and lighting fast `JSMN library  <http://zserge.com/jsmn.html>`_

    
.. function:: dumps(obj)

    Returns a bytearray containing the JSON representation of *obj*.

    Raises ``JSONError`` when *obj* contains non serializable objects.

    
.. function:: loads(data)

    Returns the object represented in JSON format inside the byte sequence *data*.

    Raises ``JSONError`` when *data* contains bad JSON.

    
