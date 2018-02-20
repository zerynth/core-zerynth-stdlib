#include "vosal.h"
#include "vhal.h"
#include "vbl.h"
#include "lang.h"


//#define printf(...) vbl_printf_stdout(__VA_ARGS__)
#define printf(...)


err_t _adc_ctl(int nargs, PObject *self, PObject **args, PObject **res) {
	(void)self;
	int32_t code;
	int32_t drvid;
	*res = MAKE_NONE();
	uint16_t *pins = NULL;
	PARSE_PY_ARGCODE(code);
	PARSE_PY_INT(drvid);

	switch (code) {
		case DRV_CMD_INIT: {
			vhalAdcConf conf;
			printf( "adc init: %i %i %i\n", drvid, conf.samples_per_second, nargs);
			int hh=parse_py_args("i", nargs, args, &conf.samples_per_second);
			printf( "adc init: %i\n", hh);
			if ( hh!= 1) goto ret_err_type;

			printf("before ADC Init\n");
			if (vhalAdcInit(drvid, &conf)) {
				return ERR_UNSUPPORTED_EXC;
			}
		}
		break;

		case DRV_CMD_DONE: {
			vhalAdcDone(drvid);
		}
		break;
		case DRV_CMD_READ: {
			vhalAdcCaptureInfo nfo;
			uint16_t pin;
			uint16_t samples=0;
			nfo.capture_mode =  ADC_CAPTURE_SINGLE;
			int tt = PTYPE(args[0]);
			if (tt == PSMALLINT) {
				pin = PSMALLINT_VALUE(args[0]);
				nfo.pins = &pin;
				nfo.npins = 1;
			} else if (tt == PLIST || tt == PTUPLE) {
				nfo.npins = PSEQUENCE_ELEMENTS(args[0]);
				pins = gc_malloc(sizeof(uint16_t) * nfo.npins);
				PObject **vpins = PSEQUENCE_OBJECTS(args[0]);
				debug( "multi adc %i\n", nfo.npins);
				for (pin = 0; pin < nfo.npins; pin++) {
					if (!(IS_PSMALLINT(vpins[pin]))) {
						goto ret_err_type;
					}
					pins[pin] = PSMALLINT_VALUE(vpins[pin]);
					debug( "pin %i\n", pins[pin]);
				}
				nfo.pins = pins;
			}
			if (!IS_PSMALLINT(args[1])) {
				goto ret_err_type;
			} else {
				nfo.samples = PSMALLINT_VALUE(args[1]);
				samples=nfo.samples;
			}
			code = vhalAdcPrepareCapture(drvid, &nfo);
			printf( "after prepare capture %i %i %i\n", code, nfo.samples, nfo.npins);
			if (code < 0)
				goto ret_unsup;
			if (code <= 2) {
				nfo.buffer = &pin;
			} else {
				nfo.buffer = gc_malloc(code);
			}
			RELEASE_GIL();
			vhalAdcRead(drvid, &nfo);
			ACQUIRE_GIL();
			printf( "after prepare capture %i %x\n", pin, &pin);
			if (samples == 1 && nfo.npins == 1) {
				*res = PSMALLINT_NEW(pin);
			} else {
				if (nfo.npins == 1) {
					PTuple *tpl = pshorts_new(samples, (uint16_t *)nfo.buffer);
					*res = (PObject *)tpl;
					gc_free(nfo.buffer);
				} else {
					PObject *gbl;
					if (samples == 1) {
						gbl = (PObject *)pshorts_new(nfo.npins, (uint16_t *)nfo.buffer);
					} else {
						gbl = (PObject *) ptuple_new(nfo.npins, NULL);
						for (pin = 0; pin < nfo.npins; pin++) {
							PObject *tpl = (PObject *)pshorts_new(samples, NULL);
							for (drvid = 0; drvid < (int32_t)samples; drvid++) {
								code = *(((uint16_t *)nfo.buffer) + (nfo.npins * drvid) + pin);
								PSHORTS_SET_ITEM(tpl, drvid, code);
							}
							PTUPLE_SET_ITEM(gbl, pin, tpl);
						}
					}
					*res = (PObject *)gbl;
					gc_free(nfo.buffer);
				}
			}
			goto ret_ok;
		}
		break;
		default:
			goto ret_unsup;
	}
ret_ok:
	if (pins) gc_free(pins);
	return ERR_OK;
ret_err_type:
	if (pins) gc_free(pins);
	return ERR_TYPE_EXC;
ret_unsup:
	if (pins) gc_free(pins);
	return ERR_UNSUPPORTED_EXC;

}

const VBLDriver adcdriver = {
    PRPH_ADC,
    _adc_ctl
};



err_t _vbl_adc_init(int nargs, PObject *self, PObject **args, PObject **res){
    *res = MAKE_NONE();
    vbl_install_driver(&adcdriver);
    vhalInitADC(NULL);
    return ERR_OK;
}

