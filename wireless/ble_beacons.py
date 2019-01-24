"""
.. module:: ble_beacons

***********
BLE Beacons
***********

This module implements utility functions to implement and scan iBeacons and Eddystone beacons.

Refer to BLE examples for usage.


================
Module Functions
================


    """


########### EddyStone

import urlparse

EDDY_UID = 0x00
EDDY_URL = 0x10
EDDY_LTM = 0x20

# Eddystone Header
# 0x03,  // Length of Service List
# 0x03,  // Param: Service List
# 0xAA, 0xFE,  // Eddystone ID
# 0x0E,  // Length of Service Data
# 0x16,  // Service Data
# 0xAA, 0xFE, // Eddystone ID
_eddy_header = b'\x02\x01\x06\x03\x03\xAA\xFE\x0E\x16\xAA\xFE'

#https://github.com/google/eddystone/tree/master/eddystone-url
_eddy_suffixes = [
".com/",
".org/",
".edu/",
".net/",
".info/",
".biz/",
".gov/",
".com",
".org",
".edu",
".net",
".info",
".biz",
".gov"
]

def _encode_power(txpower):
    if txpower>=0:
        return txpower&0x1f
    else:
        return (256+txpower)&0xff

def _decode_power(txpower):
    if txpower<=0x1f:
        return txpower
    else:
        return txpower-256

def eddy_encode_tlm(battery, temperature, count, time):
    """
.. function:: eddy_encode_tlm(battery, temperature, count, time)

    Return a bytearray representing an encoded Eddystone payload of type tlm (not encrypted).

    According to the specifications:

    * :samp:`battery` is the battery charge in mV
    * :samp:`temperature` is the beacon temperature in degrees. If None is give, temperature is set to -128
    * :samp:`count` is the pdu count, the number of advertised packets
    * :samp:`time` is the time in seconds since beacon power up


    """
    #build packet
    packet = bytearray()
    packet.extend(_eddy_header)    # add header
    packet.append(0x20)            # add frame (TLM type)
    packet.append(0x00)            # version


    # add battery
    packet.append((battery>>8)&0xff)
    packet.append(battery&0xff)

    # add temperature
    if temperature is None:
        packet.append(0x80)
        packet.append(0)
    else:
        print(temperature)
        ipart = int(temperature)
        print(ipart)
        fpart = int((temperature-ipart)*256)
        print(fpart)
        packet.append(ipart)
        packet.append(fpart)

    # add count
    packet.append((count>>24)&0xff)
    packet.append((count>>16)&0xff)
    packet.append((count>>8)&0xff)
    packet.append(count&0xff)

    # add time
    time = int(time*10) # in 0.1s increments
    packet.append((time>>24)&0xff)
    packet.append((time>>16)&0xff)
    packet.append((time>>8)&0xff)
    packet.append(time&0xff)

    #patch header length
    packet[7]=len(packet)-8
    return packet


def eddy_encode_uid(namespace,instance,txpower):
    """
.. function:: eddy_encode_uid(namespace, instance, txpower)

    Return a bytearray representing an encoded Eddystone payload of type uid (not encrypted).

    According to the specifications:

    * :samp:`namespace` is a sequence of bytes of maximum length 10 (if less, it is zero padded to the right) representing the namespace of the uid
    * :samp:`instance` is a sequence of bytes of maximum lengtth 6 (if less, it is zero padded to the right) representing the instance of the uid
    * :samp:`txpower` is the power calibration measurement of the beacon (used to calculate distances)

    """
    # convert tx power
    tx = _encode_power(txpower)

    #build packet
    packet = bytearray()
    packet.extend(_eddy_header)    # add header
    packet.append(0x00)            # add frame (UID type)
    packet.append(tx)              # tx power
    # add namespace
    if len(namespace)>=10:
        packet.extend(namespace[0:10])
    else:
        packet.extend(namespace)
        packet.extend(b'\x00'*(10-len(namespace)))
    # add instance
    if len(instance)>=6:
        packet.extend(instance[0:6])
    else:
        packet.extend(instance)
        packet.extend(b'\x00'*(6-len(instance)))
    # fill last two bytes with 0
    packet.extend(b'\x00\x00')
    #patch header length
    packet[7]=len(packet)-8
    return packet



def eddy_encode_url(url,txpower):
    """
.. function:: eddy_encode_url(url, txpower)

    Return a bytearray representing an encoded Eddystone payload of type url (not encrypted).

    According to the specifications:

    * :samp:`url` is a string representing an URL to be encoded in the Eddystone format
    * :samp:`txpower` is the power calibration measurement of the beacon (used to calculate distances)

    """
    # convert tx power
    tx = _encode_power(txpower)

    # parse url
    scheme, netloc, path, query, fragment = urlparse.parse(url)

    # find prefix
    prefix = -1
    if scheme == "http":
        if netloc.startswith("www."):
            prefix = 0
            netloc = netloc[4:]
        else:
            prefix = 2
    elif scheme == "https":
        if netloc.startswith("www."):
            prefix = 1
            netloc = netloc[4:]
        else:
            prefix = 3

    #find suffix
    suffix = -1
    if path:
        ntl=netloc+path[0]
    else:
        ntl=netloc
    for i,ext in enumerate(_eddy_suffixes):
        if ntl.endswith(ext):
            suffix=i
            if path:
                netloc=netloc[:-(len(ext)-1)]
                path = path[1:]
            else:
                netloc=netloc[:-(len(ext))]
            break
    #build packet
    packet = bytearray()
    packet.extend(_eddy_header)    # add header
    packet.append(0x10)            # add frame (URL type)
    packet.append(tx)              # tx power
    if prefix>=0:
        packet.append(prefix)
    else:
        packet.extend(scheme)
    packet.extend(netloc)
    if suffix>=0:
        packet.append(suffix)
    packet.extend(path)
    packet.extend(query)
    packet.extend(fragment)

    #patch header length
    packet[7]=len(packet)-8

    return packet


def eddy_decode_type(packet):
    """
.. function:: eddy_decode_type(packet)

    Given :samp:`packet` as a bytes or bytearray return the type of the Eddystone packet or raises :samp:`ValueError` if it is not an Eddystone packet.

    Return values are:

    * :samp:`EDDY_LTM` for ltm packets
    * :samp:`EDDY_UID` for uid packets
    * :samp:`EDDY_URL` for url packets

    """
    if packet.startswith(_eddy_header[0:7]):
        return packet[11]
    raise ValueError

def eddy_decode(packet):
    """
.. function:: eddy_decode(packet)

    Given :samp:`packet` as a bytes or bytearray return a tuple with the packet decoded fields or raises :samp:`ValueError` in case :samp:`packet` is not Eddystone.

    Return values are:

    * a tuple with namespace, instance and txpower for uid packets
    * a tuple with url and txpower for url packets
    * a tuple with battery, temperature, count and time for tlm packets

    """
    etype = packet[11]
    if etype==0:
        #UID
        tx = _decode_power(packet[12])
        namespace = packet[13:23]
        instance = packet[24:30]
        return namespace,instance,tx
    elif etype==0x10:
        #URL
        tx = _decode_power(packet[12])
        url = bytearray()
        prefix = packet[13]
        if prefix == 0:
            url.extend("http://www.")
        elif prefix == 1:
            url.extend("https://www.")
        elif prefix == 2:
            url.extend("http://")
        elif prefix == 3:
            url.extend("https://")
        else:
            url.append(prefix)
        for i in range(14,len(packet)):
            v = packet[i]
            if v<=len(_eddy_suffixes):
                url.extend(_eddy_suffixes[v])
            else:
                url.append(v)
        return str(url), tx
    elif etype==0x20:
        #LTM
        battery = (packet[13]<<8)+packet[14]
        temp = (packet[15]<<8)+packet[16]
        print(packet[15],packet[16])
        if temp==0x8000:
            temp = None
        else:
            if packet[15]>127:
                temp = packet[15]-128+(packet[16]/256)
            else:
                temp = packet[15]+(packet[16]/256)


        
        count = (packet[17]<<24)+(packet[18]<<16)+(packet[19]<<8)+packet[20]
        time = (packet[21]<<24)+(packet[22]<<16)+(packet[23]<<8)+packet[24]
        
        return battery,temp,count,time/10
    raise ValueError


########### iBeacon

_ibeacon_header = b'\x02\x01\x06\x1a\xff\x4c\x00\x02\x15'

def ibeacon_encode(uuid,major,minor,txpower,manufacturer=0x4c00):
    """
.. function:: ibeacon_encode(uuid, major, minor, txpower, manufacturer=0x4c00)

    Return a bytearray representing an encoded iBeacon payload.

    According to the specifications:

    * :samp:`uuid` is a 16 bytes unique identifier
    * :samp:`major` is the major version as integer
    * :samp:`minor` is the minor version as integer
    * :samp:`txpower` is the power calibration measurement of the beacon (used to calculate distances)
    * :samp:`manufacturer` is set to Apple, but can be changed if needed

    """
    if type(uuid)==PSTRING:
        #split uuid and convert to bytes
        uus = uuid.split("-")
        uus = "".join(uus)
        uuid = bytearray()
        for i in range(0,len(uus),2):
            bb = uus[i:i+2]
            uuid.append(int(bb,16))
    # pad or trim uuid
    if len(uuid)>16:
        uuid=uuid[0:16]
    else:
        uuid= uuid+(b'\x00'*(16-len(uuid)))
    # set power
    tx = _encode_power(txpower)

    # build packet
    packet = bytearray()
    packet.extend(_ibeacon_header)    # add header
    packet.extend(uuid)
    packet.append((major>>8)&0xff)
    packet.append((major)&0xff)
    packet.append((minor>>8)&0xff)
    packet.append((minor)&0xff)
    packet.append(tx)              # tx power

    #patch manufacturer
    packet[5]=(manufacturer>>8)&0xff
    packet[6]=(manufacturer)&0xff

    return packet

def ibeacon_decode(packet):
    """
.. function:: ibeacon_decode(packet)

    Given :samp:`packet` as a bytes or bytearray return a tuple with the packet decoded fields or raises :samp:`ValueError` in case :samp:`packet` is not iBeacon.

    Return value is a tuple with uuid, major, minor and txpower

    """
    if (not packet.startswith(_ibeacon_header)) or len(packet)!=30:
        # not an ibeacon
        raise ValueError
    uuid = packet[9:25]
    major = (packet[25]<<8) + packet[26]
    minor = (packet[27]<<8) + packet[28]
    tx = _decode_power(packet[29])

    return uuid, major, minor, tx


