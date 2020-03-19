/*
{{ project }}
Copyright (C) {{ year }}  {{ organization }}

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef __VHAL_PINS__
#define __VHAL_PINS__


typedef void *PinPort;
typedef uint8_t PinPad;

#define MAKE_PIN(port,pad,prph) {port,pad, prph}
#define MAKE_PIN_CLASS(pidx,pdata0,pdata1,pdata2) {pidx,pdata0,pdata1,pdata2}
#define MAKE_VPIN(cls,pad) (((cls)<<8)|(pad))

typedef uint8_t PinStatus;


typedef struct _pininfo {
  uint8_t port;
  uint8_t pad;
  uint16_t prph;
} PinInfo;

typedef struct _pinclass {
  uint8_t pidx;
  uint8_t data0;
  uint8_t data1;
  uint8_t data2;
} PinClass;


#if defined(VM_IS_CUSTOM)
extern const PinPort _vhalpinports[];
extern uint16_t _vhalpinnums;
extern PinStatus _vhalpinstatus[];
extern PinInfo *_vhalpinmap;
extern uint8_t _vhalpinclasses;
extern PinClass *_vhalpinclass[];
extern uint8_t *_vhalpinclassnum;
#else
extern const PinPort _vhalpinports[];
extern const uint16_t _vhalpinnums;
extern PinStatus _vhalpinstatus[];
extern const PinInfo const _vhalpinmap[];
extern const uint8_t _vhalpinclasses;
extern const PinClass *const _vhalpinclass[];
extern const uint8_t _vhalpinclassnum[];
#endif

#define PIN_CLASS(vpin) (((vpin)&0xff00)>>8)
#define PIN_CLASS_PAD(vpin) (((vpin)&0x00ff))

#define PIN_CLASS_ID(vpin) ((PIN_CLASS(vpin)!=PINCLASS_DIGITAL) ? (_vhalpinclass[PIN_CLASS(vpin)][PIN_CLASS_PAD(vpin)].pidx):(PIN_CLASS_PAD(vpin)))

//never use with DIGITAL
#define PIN_CLASS_DATA0(vpin) _vhalpinclass[PIN_CLASS(vpin)][PIN_CLASS_PAD(vpin)].data0
#define PIN_CLASS_DATA1(vpin) _vhalpinclass[PIN_CLASS(vpin)][PIN_CLASS_PAD(vpin)].data1
#define PIN_CLASS_DATA2(vpin) _vhalpinclass[PIN_CLASS(vpin)][PIN_CLASS_PAD(vpin)].data2


#define _get_vhalpinmap() _vhalpinmap



#define PIN_PORT_NUMBER(vpin) (_get_vhalpinmap()[PIN_CLASS_ID(vpin)].port)
#define PIN_PORT(vpin) (_vhalpinports[_get_vhalpinmap()[PIN_CLASS_ID(vpin)].port])
#define PIN_PAD(vpin) _get_vhalpinmap()[PIN_CLASS_ID(vpin)].pad
#define PIN_PRPH(vpin)(_get_vhalpinmap()[PIN_CLASS_ID(vpin)].prph)
#define PIN_HAS_PRPH(vpin,prh) (PIN_PRPH(vpin)&(1<<(prh)))
#define PIN_CLASS_NUM(cls) _vhalpinclassnum[cls]
#define PIN_SET_TO_PRPH(vpin,prh) _vhalpinstatus[PIN_CLASS_ID(vpin)]=prh
#define PIN_GET_PRPH(vpin) _vhalpinstatus[PIN_CLASS_ID(vpin)]
#define PIN_FREE_OR_ASSIGNED_TO(vpin,prph) (PIN_GET_PRPH(vpin)==0xff || PIN_GET_PRPH(vpin)==prph)

#if !defined(VM_IS_CUSTOM)

#define VHAL_PORT_DECLARATIONS() \
  const uint8_t _vhalpinclassnum[] STORED = {\
                                             NUM_PINS,\
                                             sizeof(_analogclass) / sizeof(PinClass),\
                                             sizeof(_spiclass) / sizeof(PinClass),\
                                             sizeof(_i2cclass) / sizeof(PinClass),\
                                             sizeof(_pwmclass) / sizeof(PinClass),\
                                             sizeof(_icuclass) / sizeof(PinClass),\
                                             sizeof(_canclass) / sizeof(PinClass),\
                                             sizeof(_serclass) / sizeof(PinClass),\
                                             sizeof(_dacclass) / sizeof(PinClass),\
                                             sizeof(_ledclass) / sizeof(PinClass),\
                                             sizeof(_btnclass) / sizeof(PinClass),\
                                            };\
  const PinClass *const _vhalpinclass[] STORED = {\
                                                  NULL,\
                                                  _analogclass,\
                                                  _spiclass,\
                                                  _i2cclass,\
                                                  _pwmclass,\
                                                  _icuclass,\
                                                  _canclass,\
                                                  _serclass,\
                                                  _dacclass,\
                                                  _ledclass,\
                                                  _btnclass\
                                                 };\
  const uint8_t _vhalpinclasses = sizeof(_vhalpinclass) / sizeof(PinClass *)

#else

#define VHAL_PORT_DECLARATIONS() \
    PinClass *_vhalpinclass[] ={ \
                                                  NULL,\
                                                  NULL,\
                                                  NULL,\
                                                  NULL,\
                                                  NULL,\
                                                  NULL,\
                                                  NULL,\
                                                  NULL,\
                                                  NULL,\
                                                  NULL,\
                                                  NULL \
    };\
    uint8_t _vhalpinclasses = sizeof(_vhalpinclass) / sizeof(PinClass *); \
    uint16_t _vhalpinnums; \
    uint8_t *_vhalpinclassnum; \
    PinInfo *_vhalpinmap;

#endif //VM_IS_CUSTOM


#define VHAL_IS_VALID_VPIN(vpin) (((PIN_CLASS(vpin))<_vhalpinclasses)&&(PIN_CLASS_PAD(vpin)<_vhalpinclassnum[PIN_CLASS(vpin)]))


#define PINCLASS_DIGITAL    0x00
#define PINCLASS_ANALOG     0x01
#define PINCLASS_SPI        0x02
#define PINCLASS_I2C        0x03
#define PINCLASS_PWM        0x04
#define PINCLASS_ICU        0x05
#define PINCLASS_CAN        0x06
#define PINCLASS_SER        0x07
#define PINCLASS_DAC        0x08
//TODO: fill with other peripherals
#define PINCLASS_LED        0x09
#define PINCLASS_BTN        0x0A


//Peripherals that use GPIOs (0..0xF ,masked in pinstatus.prph)
#define PRPH_EXT    0x00
#define PRPH_ADC    0x01
#define PRPH_SPI    0x02
#define PRPH_I2C    0x03
#define PRPH_PWM    0x04
#define PRPH_ICU    0x05
#define PRPH_CAN    0x06
#define PRPH_SER    0x07
#define PRPH_DAC    0x08
#define PRPH_MAC    0x09
#define PRPH_SDC    0x0A
#define PRPH_USB    0x0B
#define PRPH_SD     0x0C
//Peripherals that don't use GPIOs
#define PRPH_HTM    0x10
#define PRPH_RTC    0x20
#define PRPH_CRYP   0x30
#define PRPH_FLASH  0x40
#define PRPH_NFO    0x50
#define PRPH_RNG    0x60
#define PRPH_DMA    0x70


#define MAKE_PRPH_ID(type,num) (((type)<<16)|(num))
#define PRPH_ID_TO_TYPE(id) (((id)&0xffff0000)>>16)
#define PRPH_ID_TO_NUM(id) (((id)&0xffff))

#define PRPHS(x) (1<<(x))

#define HAS_EXT PRPHS(PRPH_EXT)
#define HAS_ADC PRPHS(PRPH_ADC)
#define HAS_PWM PRPHS(PRPH_PWM)
#define HAS_ICU PRPHS(PRPH_ICU)
#define HAS_SER PRPHS(PRPH_SER)
#define HAS_I2C PRPHS(PRPH_I2C)
#define HAS_CAN PRPHS(PRPH_CAN)
#define HAS_DAC PRPHS(PRPH_DAC)
#define HAS_MAC PRPHS(PRPH_MAC)
#define HAS_SDC PRPHS(PRPH_SDC)
#define HAS_USB PRPHS(PRPH_USB)
#define HAS_SPI PRPHS(PRPH_SPI)
#define HAS_ATM (HAS_ICU|HAS_PWM)


#define PORT_A 0
#define PORT_B 1
#define PORT_C 2
#define PORT_D 3
#define PORT_E 4
#define PORT_F 5
#define PORT_G 6
#define PORT_H 7
#define PORT_I 8
#define PORT_J 9
#define PORT_K 10
#define PORT_L 11
#define PORT_M 12
#define PORT_N 13
#define PORT_O 14
#define PORT_P 15
#define PORT_Q 16
#define PORT_R 17
#define PORT_S 18
#define PORT_T 19
#define PORT_U 20
#define PORT_V 21
#define PORT_W 22
#define PORT_X 23
#define PORT_Y 24
#define PORT_Z 25


#if !defined(Z_VHAL_SKIP_PIN_DEFS)
#define D0  MAKE_VPIN(PINCLASS_DIGITAL,0)
#define D1  MAKE_VPIN(PINCLASS_DIGITAL,1)
#define D2  MAKE_VPIN(PINCLASS_DIGITAL,2)
#define D3  MAKE_VPIN(PINCLASS_DIGITAL,3)
#define D4  MAKE_VPIN(PINCLASS_DIGITAL,4)
#define D5  MAKE_VPIN(PINCLASS_DIGITAL,5)
#define D6  MAKE_VPIN(PINCLASS_DIGITAL,6)
#define D7  MAKE_VPIN(PINCLASS_DIGITAL,7)
#define D8  MAKE_VPIN(PINCLASS_DIGITAL,8)
#define D9  MAKE_VPIN(PINCLASS_DIGITAL,9)
#define D10  MAKE_VPIN(PINCLASS_DIGITAL,10)
#define D11  MAKE_VPIN(PINCLASS_DIGITAL,11)
#define D12  MAKE_VPIN(PINCLASS_DIGITAL,12)
#define D13  MAKE_VPIN(PINCLASS_DIGITAL,13)
#define D14  MAKE_VPIN(PINCLASS_DIGITAL,14)
#define D15  MAKE_VPIN(PINCLASS_DIGITAL,15)
#define D16  MAKE_VPIN(PINCLASS_DIGITAL,16)
#define D17  MAKE_VPIN(PINCLASS_DIGITAL,17)
#define D18  MAKE_VPIN(PINCLASS_DIGITAL,18)
#define D19  MAKE_VPIN(PINCLASS_DIGITAL,19)
#define D20  MAKE_VPIN(PINCLASS_DIGITAL,20)
#define D21  MAKE_VPIN(PINCLASS_DIGITAL,21)
#define D22  MAKE_VPIN(PINCLASS_DIGITAL,22)
#define D23  MAKE_VPIN(PINCLASS_DIGITAL,23)
#define D24  MAKE_VPIN(PINCLASS_DIGITAL,24)
#define D25  MAKE_VPIN(PINCLASS_DIGITAL,25)
#define D26  MAKE_VPIN(PINCLASS_DIGITAL,26)
#define D27  MAKE_VPIN(PINCLASS_DIGITAL,27)
#define D28  MAKE_VPIN(PINCLASS_DIGITAL,28)
#define D29  MAKE_VPIN(PINCLASS_DIGITAL,29)
#define D30  MAKE_VPIN(PINCLASS_DIGITAL,30)
#define D31  MAKE_VPIN(PINCLASS_DIGITAL,31)
#define D32  MAKE_VPIN(PINCLASS_DIGITAL,32)
#define D33  MAKE_VPIN(PINCLASS_DIGITAL,33)
#define D34  MAKE_VPIN(PINCLASS_DIGITAL,34)
#define D35  MAKE_VPIN(PINCLASS_DIGITAL,35)
#define D36  MAKE_VPIN(PINCLASS_DIGITAL,36)
#define D37  MAKE_VPIN(PINCLASS_DIGITAL,37)
#define D38  MAKE_VPIN(PINCLASS_DIGITAL,38)
#define D39  MAKE_VPIN(PINCLASS_DIGITAL,39)
#define D40  MAKE_VPIN(PINCLASS_DIGITAL,40)
#define D41  MAKE_VPIN(PINCLASS_DIGITAL,41)
#define D42  MAKE_VPIN(PINCLASS_DIGITAL,42)
#define D43  MAKE_VPIN(PINCLASS_DIGITAL,43)
#define D44  MAKE_VPIN(PINCLASS_DIGITAL,44)
#define D45  MAKE_VPIN(PINCLASS_DIGITAL,45)
#define D46  MAKE_VPIN(PINCLASS_DIGITAL,46)
#define D47  MAKE_VPIN(PINCLASS_DIGITAL,47)
#define D48  MAKE_VPIN(PINCLASS_DIGITAL,48)
#define D49  MAKE_VPIN(PINCLASS_DIGITAL,49)
#define D50  MAKE_VPIN(PINCLASS_DIGITAL,50)
#define D51  MAKE_VPIN(PINCLASS_DIGITAL,51)
#define D52  MAKE_VPIN(PINCLASS_DIGITAL,52)
#define D53  MAKE_VPIN(PINCLASS_DIGITAL,53)
#define D54  MAKE_VPIN(PINCLASS_DIGITAL,54)
#define D55  MAKE_VPIN(PINCLASS_DIGITAL,55)
#define D56  MAKE_VPIN(PINCLASS_DIGITAL,56)
#define D57  MAKE_VPIN(PINCLASS_DIGITAL,57)
#define D58  MAKE_VPIN(PINCLASS_DIGITAL,58)
#define D59  MAKE_VPIN(PINCLASS_DIGITAL,59)
#define D60  MAKE_VPIN(PINCLASS_DIGITAL,60)
#define D61  MAKE_VPIN(PINCLASS_DIGITAL,61)
#define D62  MAKE_VPIN(PINCLASS_DIGITAL,62)
#define D63  MAKE_VPIN(PINCLASS_DIGITAL,63)
#define D64  MAKE_VPIN(PINCLASS_DIGITAL,64)
#define D65  MAKE_VPIN(PINCLASS_DIGITAL,65)
#define D66  MAKE_VPIN(PINCLASS_DIGITAL,66)
#define D67  MAKE_VPIN(PINCLASS_DIGITAL,67)
#define D68  MAKE_VPIN(PINCLASS_DIGITAL,68)
#define D69  MAKE_VPIN(PINCLASS_DIGITAL,69)
#define D70  MAKE_VPIN(PINCLASS_DIGITAL,70)
#define D71  MAKE_VPIN(PINCLASS_DIGITAL,71)
#define D72  MAKE_VPIN(PINCLASS_DIGITAL,72)
#define D73  MAKE_VPIN(PINCLASS_DIGITAL,73)
#define D74  MAKE_VPIN(PINCLASS_DIGITAL,74)
#define D75  MAKE_VPIN(PINCLASS_DIGITAL,75)
#define D76  MAKE_VPIN(PINCLASS_DIGITAL,76)
#define D77  MAKE_VPIN(PINCLASS_DIGITAL,77)
#define D78  MAKE_VPIN(PINCLASS_DIGITAL,78)
#define D79  MAKE_VPIN(PINCLASS_DIGITAL,79)
#define D80  MAKE_VPIN(PINCLASS_DIGITAL,80)
#define D81  MAKE_VPIN(PINCLASS_DIGITAL,81)
#define D82  MAKE_VPIN(PINCLASS_DIGITAL,82)
#define D83  MAKE_VPIN(PINCLASS_DIGITAL,83)
#define D84  MAKE_VPIN(PINCLASS_DIGITAL,84)
#define D85  MAKE_VPIN(PINCLASS_DIGITAL,85)
#define D86  MAKE_VPIN(PINCLASS_DIGITAL,86)
#define D87  MAKE_VPIN(PINCLASS_DIGITAL,87)
#define D88  MAKE_VPIN(PINCLASS_DIGITAL,88)
#define D89  MAKE_VPIN(PINCLASS_DIGITAL,89)
#define D90  MAKE_VPIN(PINCLASS_DIGITAL,90)
#define D91  MAKE_VPIN(PINCLASS_DIGITAL,91)
#define D92  MAKE_VPIN(PINCLASS_DIGITAL,92)
#define D93  MAKE_VPIN(PINCLASS_DIGITAL,93)
#define D94  MAKE_VPIN(PINCLASS_DIGITAL,94)
#define D95  MAKE_VPIN(PINCLASS_DIGITAL,95)
#define D96  MAKE_VPIN(PINCLASS_DIGITAL,96)
#define D97  MAKE_VPIN(PINCLASS_DIGITAL,97)
#define D98  MAKE_VPIN(PINCLASS_DIGITAL,98)
#define D99  MAKE_VPIN(PINCLASS_DIGITAL,99)
#define D100 MAKE_VPIN(PINCLASS_DIGITAL,100)
#define D101 MAKE_VPIN(PINCLASS_DIGITAL,101)
#define D102 MAKE_VPIN(PINCLASS_DIGITAL,102)
#define D103 MAKE_VPIN(PINCLASS_DIGITAL,103)
#define D104 MAKE_VPIN(PINCLASS_DIGITAL,104)
#define D105 MAKE_VPIN(PINCLASS_DIGITAL,105)
#define D106 MAKE_VPIN(PINCLASS_DIGITAL,106)
#define D107 MAKE_VPIN(PINCLASS_DIGITAL,107)
#define D108 MAKE_VPIN(PINCLASS_DIGITAL,108)
#define D109 MAKE_VPIN(PINCLASS_DIGITAL,109)
#define D110 MAKE_VPIN(PINCLASS_DIGITAL,110)
#define D111 MAKE_VPIN(PINCLASS_DIGITAL,111)
#define D112 MAKE_VPIN(PINCLASS_DIGITAL,112)
#define D113 MAKE_VPIN(PINCLASS_DIGITAL,113)
#define D114 MAKE_VPIN(PINCLASS_DIGITAL,114)
#define D115 MAKE_VPIN(PINCLASS_DIGITAL,115)
#define D116 MAKE_VPIN(PINCLASS_DIGITAL,116)
#define D117 MAKE_VPIN(PINCLASS_DIGITAL,117)
#define D118 MAKE_VPIN(PINCLASS_DIGITAL,118)
#define D119 MAKE_VPIN(PINCLASS_DIGITAL,119)
#define D120 MAKE_VPIN(PINCLASS_DIGITAL,120)
#define D121 MAKE_VPIN(PINCLASS_DIGITAL,121)
#define D122 MAKE_VPIN(PINCLASS_DIGITAL,122)
#define D123 MAKE_VPIN(PINCLASS_DIGITAL,123)
#define D124 MAKE_VPIN(PINCLASS_DIGITAL,124)
#define D125 MAKE_VPIN(PINCLASS_DIGITAL,125)
#define D126 MAKE_VPIN(PINCLASS_DIGITAL,126)
#define D127 MAKE_VPIN(PINCLASS_DIGITAL,127)
#define D128 MAKE_VPIN(PINCLASS_DIGITAL,128)
#define D129 MAKE_VPIN(PINCLASS_DIGITAL,129)
#define D130 MAKE_VPIN(PINCLASS_DIGITAL,130)
#define D131 MAKE_VPIN(PINCLASS_DIGITAL,131)
#define D132 MAKE_VPIN(PINCLASS_DIGITAL,132)
#define D133 MAKE_VPIN(PINCLASS_DIGITAL,133)
#define D134 MAKE_VPIN(PINCLASS_DIGITAL,134)
#define D135 MAKE_VPIN(PINCLASS_DIGITAL,135)
#define D136 MAKE_VPIN(PINCLASS_DIGITAL,136)
#define D137 MAKE_VPIN(PINCLASS_DIGITAL,137)
#define D138 MAKE_VPIN(PINCLASS_DIGITAL,138)
#define D139 MAKE_VPIN(PINCLASS_DIGITAL,139)
#define D140 MAKE_VPIN(PINCLASS_DIGITAL,140)
#define D141 MAKE_VPIN(PINCLASS_DIGITAL,141)
#define D142 MAKE_VPIN(PINCLASS_DIGITAL,142)
#define D143 MAKE_VPIN(PINCLASS_DIGITAL,143)
#define D144 MAKE_VPIN(PINCLASS_DIGITAL,144)
#define D145 MAKE_VPIN(PINCLASS_DIGITAL,145)
#define D146 MAKE_VPIN(PINCLASS_DIGITAL,146)
#define D147 MAKE_VPIN(PINCLASS_DIGITAL,147)
#define D148 MAKE_VPIN(PINCLASS_DIGITAL,148)
#define D149 MAKE_VPIN(PINCLASS_DIGITAL,149)
#define D150 MAKE_VPIN(PINCLASS_DIGITAL,150)
#define D151 MAKE_VPIN(PINCLASS_DIGITAL,151)
#define D152 MAKE_VPIN(PINCLASS_DIGITAL,152)
#define D153 MAKE_VPIN(PINCLASS_DIGITAL,153)
#define D154 MAKE_VPIN(PINCLASS_DIGITAL,154)
#define D155 MAKE_VPIN(PINCLASS_DIGITAL,155)
#define D156 MAKE_VPIN(PINCLASS_DIGITAL,156)
#define D157 MAKE_VPIN(PINCLASS_DIGITAL,157)
#define D158 MAKE_VPIN(PINCLASS_DIGITAL,158)
#define D159 MAKE_VPIN(PINCLASS_DIGITAL,159)
#define D160 MAKE_VPIN(PINCLASS_DIGITAL,160)
#define D161 MAKE_VPIN(PINCLASS_DIGITAL,161)
#define D162 MAKE_VPIN(PINCLASS_DIGITAL,162)
#define D163 MAKE_VPIN(PINCLASS_DIGITAL,163)
#define D164 MAKE_VPIN(PINCLASS_DIGITAL,164)
#define D165 MAKE_VPIN(PINCLASS_DIGITAL,165)
#define D166 MAKE_VPIN(PINCLASS_DIGITAL,166)
#define D167 MAKE_VPIN(PINCLASS_DIGITAL,167)
#define D168 MAKE_VPIN(PINCLASS_DIGITAL,168)
#define D169 MAKE_VPIN(PINCLASS_DIGITAL,169)

#define A0 MAKE_VPIN(PINCLASS_ANALOG,0)
#define A1 MAKE_VPIN(PINCLASS_ANALOG,1)
#define A2 MAKE_VPIN(PINCLASS_ANALOG,2)
#define A3 MAKE_VPIN(PINCLASS_ANALOG,3)
#define A4 MAKE_VPIN(PINCLASS_ANALOG,4)
#define A5 MAKE_VPIN(PINCLASS_ANALOG,5)
#define A6 MAKE_VPIN(PINCLASS_ANALOG,6)
#define A7 MAKE_VPIN(PINCLASS_ANALOG,7)
#define A8 MAKE_VPIN(PINCLASS_ANALOG,8)
#define A9 MAKE_VPIN(PINCLASS_ANALOG,9)
#define A10 MAKE_VPIN(PINCLASS_ANALOG,10)
#define A11 MAKE_VPIN(PINCLASS_ANALOG,11)
#define A12 MAKE_VPIN(PINCLASS_ANALOG,12)
#define A13 MAKE_VPIN(PINCLASS_ANALOG,13)
#define A14 MAKE_VPIN(PINCLASS_ANALOG,14)
#define A15 MAKE_VPIN(PINCLASS_ANALOG,15)
#define A16 MAKE_VPIN(PINCLASS_ANALOG,16)
#define A17 MAKE_VPIN(PINCLASS_ANALOG,17)
#define A18 MAKE_VPIN(PINCLASS_ANALOG,18)
#define A19 MAKE_VPIN(PINCLASS_ANALOG,19)
#define A20 MAKE_VPIN(PINCLASS_ANALOG,20)
#define A21 MAKE_VPIN(PINCLASS_ANALOG,21)
#define A22 MAKE_VPIN(PINCLASS_ANALOG,22)
#define A23 MAKE_VPIN(PINCLASS_ANALOG,23)
#define A24 MAKE_VPIN(PINCLASS_ANALOG,24)
#define A25 MAKE_VPIN(PINCLASS_ANALOG,25)
#define A26 MAKE_VPIN(PINCLASS_ANALOG,26)
#define A27 MAKE_VPIN(PINCLASS_ANALOG,27)
#define A28 MAKE_VPIN(PINCLASS_ANALOG,28)
#define A29 MAKE_VPIN(PINCLASS_ANALOG,29)


#define BTN0 MAKE_VPIN(PINCLASS_BTN,0)
#define BTN1 MAKE_VPIN(PINCLASS_BTN,1)
#define BTN2 MAKE_VPIN(PINCLASS_BTN,2)
#define BTN3 MAKE_VPIN(PINCLASS_BTN,3)
#define BTN4 MAKE_VPIN(PINCLASS_BTN,4)
#define BTN5 MAKE_VPIN(PINCLASS_BTN,5)
#define BTN6 MAKE_VPIN(PINCLASS_BTN,6)
#define BTN7 MAKE_VPIN(PINCLASS_BTN,7)
#define BTN8 MAKE_VPIN(PINCLASS_BTN,8)
#define BTN9 MAKE_VPIN(PINCLASS_BTN,9)


#define LED0 MAKE_VPIN(PINCLASS_LED,0)
#define LED1 MAKE_VPIN(PINCLASS_LED,1)
#define LED2 MAKE_VPIN(PINCLASS_LED,2)
#define LED3 MAKE_VPIN(PINCLASS_LED,3)
#define LED4 MAKE_VPIN(PINCLASS_LED,4)
#define LED5 MAKE_VPIN(PINCLASS_LED,5)
#define LED6 MAKE_VPIN(PINCLASS_LED,6)
#define LED7 MAKE_VPIN(PINCLASS_LED,7)
#define LED8 MAKE_VPIN(PINCLASS_LED,8)
#define LED9 MAKE_VPIN(PINCLASS_LED,9)

#define PWM0  MAKE_VPIN(PINCLASS_PWM,0)
#define PWM1  MAKE_VPIN(PINCLASS_PWM,1)
#define PWM2  MAKE_VPIN(PINCLASS_PWM,2)
#define PWM3  MAKE_VPIN(PINCLASS_PWM,3)
#define PWM4  MAKE_VPIN(PINCLASS_PWM,4)
#define PWM5  MAKE_VPIN(PINCLASS_PWM,5)
#define PWM6  MAKE_VPIN(PINCLASS_PWM,6)
#define PWM7  MAKE_VPIN(PINCLASS_PWM,7)
#define PWM8  MAKE_VPIN(PINCLASS_PWM,8)
#define PWM9  MAKE_VPIN(PINCLASS_PWM,9)
#define PWM10  MAKE_VPIN(PINCLASS_PWM,10)
#define PWM11  MAKE_VPIN(PINCLASS_PWM,11)
#define PWM12  MAKE_VPIN(PINCLASS_PWM,12)
#define PWM13  MAKE_VPIN(PINCLASS_PWM,13)
#define PWM14  MAKE_VPIN(PINCLASS_PWM,14)
#define PWM15  MAKE_VPIN(PINCLASS_PWM,15)
#define PWM16  MAKE_VPIN(PINCLASS_PWM,16)
#define PWM17  MAKE_VPIN(PINCLASS_PWM,17)
#define PWM18  MAKE_VPIN(PINCLASS_PWM,18)
#define PWM19  MAKE_VPIN(PINCLASS_PWM,19)
#define PWM20  MAKE_VPIN(PINCLASS_PWM,20)
#define PWM21  MAKE_VPIN(PINCLASS_PWM,21)
#define PWM22  MAKE_VPIN(PINCLASS_PWM,22)
#define PWM23  MAKE_VPIN(PINCLASS_PWM,23)
#define PWM24  MAKE_VPIN(PINCLASS_PWM,24)
#define PWM25  MAKE_VPIN(PINCLASS_PWM,25)
#define PWM26  MAKE_VPIN(PINCLASS_PWM,26)
#define PWM27  MAKE_VPIN(PINCLASS_PWM,27)
#define PWM28  MAKE_VPIN(PINCLASS_PWM,28)
#define PWM29  MAKE_VPIN(PINCLASS_PWM,29)


#define ICU0  MAKE_VPIN(PINCLASS_ICU,0)
#define ICU1  MAKE_VPIN(PINCLASS_ICU,1)
#define ICU2  MAKE_VPIN(PINCLASS_ICU,2)
#define ICU3  MAKE_VPIN(PINCLASS_ICU,3)
#define ICU4  MAKE_VPIN(PINCLASS_ICU,4)
#define ICU5  MAKE_VPIN(PINCLASS_ICU,5)
#define ICU6  MAKE_VPIN(PINCLASS_ICU,6)
#define ICU7  MAKE_VPIN(PINCLASS_ICU,7)
#define ICU8  MAKE_VPIN(PINCLASS_ICU,8)
#define ICU9  MAKE_VPIN(PINCLASS_ICU,9)
#define ICU10  MAKE_VPIN(PINCLASS_ICU,10)
#define ICU11  MAKE_VPIN(PINCLASS_ICU,11)
#define ICU12  MAKE_VPIN(PINCLASS_ICU,12)
#define ICU13  MAKE_VPIN(PINCLASS_ICU,13)
#define ICU14  MAKE_VPIN(PINCLASS_ICU,14)
#define ICU15  MAKE_VPIN(PINCLASS_ICU,15)
#define ICU16  MAKE_VPIN(PINCLASS_ICU,16)
#define ICU17  MAKE_VPIN(PINCLASS_ICU,17)
#define ICU18  MAKE_VPIN(PINCLASS_ICU,18)
#define ICU19  MAKE_VPIN(PINCLASS_ICU,19)
#define ICU20  MAKE_VPIN(PINCLASS_ICU,20)
#define ICU21  MAKE_VPIN(PINCLASS_ICU,21)
#define ICU22  MAKE_VPIN(PINCLASS_ICU,22)
#define ICU23  MAKE_VPIN(PINCLASS_ICU,23)
#define ICU24  MAKE_VPIN(PINCLASS_ICU,24)
#define ICU25  MAKE_VPIN(PINCLASS_ICU,25)
#define ICU26  MAKE_VPIN(PINCLASS_ICU,26)
#define ICU27  MAKE_VPIN(PINCLASS_ICU,27)
#define ICU28  MAKE_VPIN(PINCLASS_ICU,28)
#define ICU29  MAKE_VPIN(PINCLASS_ICU,29)


#define RX0  MAKE_VPIN(PINCLASS_SER,0)
#define TX0  MAKE_VPIN(PINCLASS_SER,1)
#define RX1  MAKE_VPIN(PINCLASS_SER,2)
#define TX1  MAKE_VPIN(PINCLASS_SER,3)
#define RX2  MAKE_VPIN(PINCLASS_SER,4)
#define TX2  MAKE_VPIN(PINCLASS_SER,5)
#define RX3  MAKE_VPIN(PINCLASS_SER,6)
#define TX3  MAKE_VPIN(PINCLASS_SER,7)
#define RX4  MAKE_VPIN(PINCLASS_SER,8)
#define TX4  MAKE_VPIN(PINCLASS_SER,9)
#define RX5  MAKE_VPIN(PINCLASS_SER,10)
#define TX5  MAKE_VPIN(PINCLASS_SER,11)
#define RX6  MAKE_VPIN(PINCLASS_SER,12)
#define TX6  MAKE_VPIN(PINCLASS_SER,13)
#define RX7  MAKE_VPIN(PINCLASS_SER,14)
#define TX7  MAKE_VPIN(PINCLASS_SER,15)
#define RX8  MAKE_VPIN(PINCLASS_SER,16)
#define TX8  MAKE_VPIN(PINCLASS_SER,17)
#define RX9  MAKE_VPIN(PINCLASS_SER,18)
#define TX9  MAKE_VPIN(PINCLASS_SER,19)


#define MOSI0  MAKE_VPIN(PINCLASS_SPI,0)
#define MISO0  MAKE_VPIN(PINCLASS_SPI,1)
#define SCLK0   MAKE_VPIN(PINCLASS_SPI,2)
#define MOSI1  MAKE_VPIN(PINCLASS_SPI,3)
#define MISO1  MAKE_VPIN(PINCLASS_SPI,4)
#define SCLK1   MAKE_VPIN(PINCLASS_SPI,5)
#define MOSI2  MAKE_VPIN(PINCLASS_SPI,6)
#define MISO2  MAKE_VPIN(PINCLASS_SPI,7)
#define SCLK2   MAKE_VPIN(PINCLASS_SPI,8)
#define MOSI3  MAKE_VPIN(PINCLASS_SPI,9)
#define MISO3  MAKE_VPIN(PINCLASS_SPI,10)
#define SCLK3   MAKE_VPIN(PINCLASS_SPI,11)
#define MOSI4  MAKE_VPIN(PINCLASS_SPI,12)
#define MISO4  MAKE_VPIN(PINCLASS_SPI,13)
#define SCLK4   MAKE_VPIN(PINCLASS_SPI,14)
#define MOSI5  MAKE_VPIN(PINCLASS_SPI,15)
#define MISO5  MAKE_VPIN(PINCLASS_SPI,16)
#define SCLK5   MAKE_VPIN(PINCLASS_SPI,17)
#define MOSI6  MAKE_VPIN(PINCLASS_SPI,18)
#define MISO6  MAKE_VPIN(PINCLASS_SPI,19)
#define SCLK6   MAKE_VPIN(PINCLASS_SPI,20)
#define MOSI7  MAKE_VPIN(PINCLASS_SPI,21)
#define MISO7  MAKE_VPIN(PINCLASS_SPI,22)
#define SCLK7   MAKE_VPIN(PINCLASS_SPI,23)
#define MOSI8  MAKE_VPIN(PINCLASS_SPI,24)
#define MISO8  MAKE_VPIN(PINCLASS_SPI,25)
#define SCLK8   MAKE_VPIN(PINCLASS_SPI,26)
#define MOSI9  MAKE_VPIN(PINCLASS_SPI,27)
#define MISO9  MAKE_VPIN(PINCLASS_SPI,28)
#define SCLK9   MAKE_VPIN(PINCLASS_SPI,29)


#define SDA0  MAKE_VPIN(PINCLASS_I2C,0)
#define SCL0  MAKE_VPIN(PINCLASS_I2C,1)
#define SDA1  MAKE_VPIN(PINCLASS_I2C,2)
#define SCL1  MAKE_VPIN(PINCLASS_I2C,3)
#define SDA2  MAKE_VPIN(PINCLASS_I2C,4)
#define SCL2  MAKE_VPIN(PINCLASS_I2C,5)
#define SDA3  MAKE_VPIN(PINCLASS_I2C,6)
#define SCL3  MAKE_VPIN(PINCLASS_I2C,7)
#define SDA4  MAKE_VPIN(PINCLASS_I2C,8)
#define SCL4  MAKE_VPIN(PINCLASS_I2C,9)
#define SDA5  MAKE_VPIN(PINCLASS_I2C,10)
#define SCL5  MAKE_VPIN(PINCLASS_I2C,11)
#define SDA6  MAKE_VPIN(PINCLASS_I2C,12)
#define SCL6  MAKE_VPIN(PINCLASS_I2C,13)
#define SDA7  MAKE_VPIN(PINCLASS_I2C,14)
#define SCL7  MAKE_VPIN(PINCLASS_I2C,15)
#define SDA8  MAKE_VPIN(PINCLASS_I2C,16)
#define SCL8  MAKE_VPIN(PINCLASS_I2C,17)
#define SDA9  MAKE_VPIN(PINCLASS_I2C,18)
#define SCL9  MAKE_VPIN(PINCLASS_I2C,19)

#endif  //!defined(Z_VHAL_SKIP_PIN_DEFS)


#endif
