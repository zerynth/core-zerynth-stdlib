//#define ZERYNTH_PRINTF 1
#include "zerynth.h"


/**
* @brief strucure for single format entries
*/
typedef struct _pack_entry {
    uint8_t type;
    uint8_t has_count;
	uint8_t padding;
    uint16_t count;
    uint16_t size;
} PackEntry;


//Some macros for formats
#define PACK_x 0
#define PACK_c 1
#define PACK_b 2
#define PACK_B 3
#define PACK_bool 4
#define PACK_h 5
#define PACK_H 6
#define PACK_i 7
#define PACK_I 8
#define PACK_l 9
#define PACK_L 10
#define PACK_q 11
#define PACK_Q 12
#define PACK_n 13
#define PACK_N 14
#define PACK_e 15
#define PACK_f 16
#define PACK_d 17
#define PACK_s 18
#define PACK_p 19
#define PACK_P 20
#define PACKS 21


#if defined(BYTECODE_ACCESS_ALIGNED_4)
uint8_t packtypes[] __attribute__ ((section (".data")))= "xcbB?hHiIlLqQnNefdspP";
uint8_t packsize[] __attribute__ ((section (".data")))= {0,1,1,1,1,2,2,4,4,4,4,8,8,4,4,2,4,8,0,0,4};
uint8_t packmods[] __attribute__ ((section (".data")))= "@=<>!";
uint8_t packbige[] __attribute__ ((section (".data")))= {0,0,0,1,1};
#else
uint8_t *packtypes = "xcbB?hHiIlLqQnNefdspP";
uint8_t packsize[] = {0,1,1,1,1,2,2,4,4,4,4,8,8,4,4,2,4,8,0,0,4};
uint8_t *packmods = "@=<>!";
uint8_t packbige[] = {0,0,0,1,1};
#endif

static inline reverse_even_buf(uint8_t *buf, int size) {
    uint8_t tmp;
    // size can be 4 or 8
    for (int i=0; i < size/2; i++) {
        tmp = buf[i];
        buf[i] = buf[size-i-1];
        buf[size-i-1] = tmp;
    }
}


/**
* @brief saves a char into buf. Bytes are taken from tuple[idx]
*
* @param tuple
* @param idx
* @param buf
*
* @return -1 in case of error, 1 otherwise
*/
int pack_get_char(PObject *tuple, int idx, uint8_t *buf){
    PObject *item = PTUPLE_ITEM(tuple,idx);
    int tt = PTYPE(item);
    if(tt==PBYTES||tt==PBYTEARRAY||tt==PSTRING){
        if(PSEQUENCE_ELEMENTS(item)!=1) return -1;
        *buf = PSEQUENCE_BYTES(item)[0];
        return 1;
    }
    return -1;
}

/**
* @brief saves an integer of size 1,2,4 into buf according to size. PObject is taken from tuple[idx]
*
* @param tuple
* @param idx
* @param buf
* @param mode
* @param size
*
* @return -1 on error, written size otherwise
*/
int pack_get_integer(PObject *tuple, int idx, uint8_t *buf, int mode, int size){
    PObject *item = PTUPLE_ITEM(tuple,idx);
    int tt = PTYPE(item);
    if(tt==PSMALLINT || tt==PINTEGER){
        uint64_t v = (uint64_t)(INTEGER_VALUE(item));
        printf("%x\n",v);
        uint8_t t2[8];
        if(size==1){
            t2[0] = v&0xff;
        } else if(size==2){
            if(!packbige[mode]){
                //little
                t2[0] = v&0xff;
                t2[1] = (v>>8)&0xff;
            } else {
                t2[1] = v&0xff;
                t2[0] = (v>>8)&0xff;
            }
        } else if(size==4){
            if(!packbige[mode]){
                //little
                t2[0] = v&0xff;
                t2[1] = (v>>8)&0xff;
                t2[2] = (v>>16)&0xff;
                t2[3] = (v>>24)&0xff;
            } else {
                //big
                t2[3] = v&0xff;
                t2[2] = (v>>8)&0xff;
                t2[1] = (v>>16)&0xff;
                t2[0] = (v>>24)&0xff;
            }
        } else if(size==8){
            if(!packbige[mode]){
                //little
                t2[0] = v&0xff;
                t2[1] = (v>>8)&0xff;
                t2[2] = (v>>16)&0xff;
                t2[3] = (v>>24)&0xff;
                t2[4] = (v>>32)&0xff;
                t2[5] = (v>>40)&0xff;
                t2[6] = (v>>48)&0xff;
                t2[7] = (v>>52)&0xff;
            } else {
                //big
                t2[7] = v&0xff;
                t2[6] = (v>>8)&0xff;
                t2[5] = (v>>16)&0xff;
                t2[4] = (v>>24)&0xff;
                t2[3] = (v>>32)&0xff;
                t2[2] = (v>>40)&0xff;
                t2[1] = (v>>48)&0xff;
                t2[0] = (v>>52)&0xff;
            }
        }
        memcpy(buf,t2,size);
        return size;
    } else if (tt==PBOOL){
        if(item==PBOOL_TRUE()) *buf=1;
        else *buf=0;
        return 1;
    }
    return -1;
}

/**
* @brief saves a float or double into buf according to size. PObject taken from tuple[idx]
*
* @param tuple
* @param idx
* @param buf
* @param size
*
* @return -1 on error, written size otherwise
*/
int pack_get_float(PObject *tuple, int idx, uint8_t *buf, int size, int bigendian){
    PObject *item = PTUPLE_ITEM(tuple,idx);
    int tt = PTYPE(item);
    if(tt==PFLOAT){
        if(size==4){
            float f = FLOAT_VALUE(item);
            memcpy(buf,&f,size);
        } else if(size==8) {
            double d = (double)FLOAT_VALUE(item);
            memcpy(buf,&d,size);
        } else {
            //short float not supported!
            return -1;
        }
        //TODO: support big endian mcu! At the moment all mcu are little endian
        if (bigendian) {
           reverse_even_buf(buf,size);
        }
        return size;
    }
    return -1;
}

/**
* @brief saves a string into buf with padding. String is taken from tuple[idx]
*
* @param tuple
* @param idx
* @param buf
* @param size
*
* @return -1 on error, written size otherwise
*/
int pack_get_string(PObject *tuple, int idx, uint8_t *buf, int size){
    PObject *item = PTUPLE_ITEM(tuple,idx);
    int tt = PTYPE(item);
    if(tt==PBYTES||tt==PBYTEARRAY||tt==PSTRING){
        int32_t len = PSEQUENCE_ELEMENTS(item);
        uint8_t *seq = PSEQUENCE_BYTES(item);
        if(size<=0) return size;
        if(size>len){
            memcpy(buf,seq,len);
            buf+=len;
            memset(buf,0,size-len);
        } else {
            memcpy(buf,seq,size);
        }
        return size;
    }
    return -1;
}

PObject* unpack_make_integer(uint8_t *buf,int size, int is_signed, int bigendian){
    uint8_t t2[8];
    uint64_t ii = 0;
    int64_t ss = 0;

    memcpy(t2,buf,size);
    
    //TODO: support big endian mcu! At the moment all mcu are little endian
    if (bigendian) {
        if(size==1){
            ii|= t2[0] & 0xff;
        } else if (size==2){
            ii|=((t2[0]<<8) | t2[1]) & 0xffff;
        } else if (size==4){
            ii|=((t2[0]<<24)|(t2[1]<<16)|(t2[2]<<8)|(t2[3])) & 0xffffffff;
        } else if (size==8){
            ii|=(t2[0]<<56)|(t2[1]<<48)|(t2[2]<<40)|(t2[3]<<32)|(t2[4]<<24)|(t2[5]<<16)|(t2[6]<<8)|(t2[7]);
        }
    } else {
        if (size==1){
            ii|= (t2[0] & 0xff);
        } else if (size==2){
            ii|=((t2[1]<<8) | t2[0]) & 0xffff;
        } else if (size==4){
            ii|=((t2[3]<<24)|(t2[2]<<16)|(t2[1]<<8)|(t2[0])) & 0xffffffff;
        } else if (size==8){
            ii|=(t2[7]<<56)|(t2[6]<<48)|(t2[5]<<40)|(t2[4]<<32)|(t2[3]<<24)|(t2[2]<<16)|(t2[1]<<8)|(t2[0]);
        }
    }
    if (is_signed){
        ss = ii;
        if(size==1) {
            if(ii>127) ss = -(256-ii);
        } else if (size==2){
            if(ii>32767) ss = -(65536-ii);
        } else if (size==4){
            if(ii>2147483647) ss = -(4294967296-ii);
        } else {
            ss = (int64_t)ii;
        }
        return pinteger_new(ss);
    } else {
        return pinteger_new_u(ii);
    }
}

PObject *unpack_make_float(uint8_t* buf,int size, int bigendian){
    //size 2 is not supported
    //TODO: support big endian mcu! At the moment all mcu are little endian
    if (bigendian) {
        reverse_even_buf(buf,size);
    }
    if (size==4){
        float f;
        memcpy(&f,buf,4);
        return pfloat_new(f);
    } else {
        double d;
        memcpy(&d,buf,8);
        return pfloat_new(d);
    }
}


PackEntry* pack_calc_size(uint8_t *fmt, int fmtl, int *endianess, int* total_items, int* total_entries, int* total_size){
    int pos = 0;
    int entries = 0;
    int items = 0;
    int mode = 0;
    int entry = 0;
    int gsize = 0;

    while(pos<fmtl){
        uint8_t *ee = memchr(packtypes,fmt[pos++],PACKS);
        if (ee) entries++;
    }

    printf("entries %i\n",entries);

    PackEntry *pes = (PackEntry*) gc_malloc(sizeof(PackEntry)*entries);
    uint8_t *pp = memchr(packmods,fmt[0],5);
    if (pp) {
        //found mod
        mode = (int)(pp-packmods);
        pos = 1;
    } else {
        mode = 0;
        pos = 0;
    }

    //parse fmt
    for(;pos<fmtl;pos++){
        uint8_t c = fmt[pos];
        PackEntry *ee = &pes[entry];

		if (c==' ' || c=='\t'){
			//skip whitespaces
			if(ee->has_count) {
				//no space between counts!
				gc_free(pes);
				return NULL;
			}
			continue;
		}
        //check count
        if (c>='0' && c<='9') {
            //found count
            printf("found count\n");
            ee->has_count = 1;
            ee->count = ee->count*10 +(c-'0');
            continue;
        }

        pp = memchr(packtypes,c,PACKS);
        if (!pp) {
            //fail!
            printf("bad format\n");
            gc_free(pes);
            return NULL;
        }

        ee->type =(int)(pp-packtypes);
        if (packsize[ee->type]) {
            //standard size
            if (ee->has_count){
                ee->size = ee->count*packsize[ee->type];
                items+=ee->count; 
            } else {
                ee->size = packsize[ee->type];
                items++;
                ee->count=1;
            }
        } else {
            //variable size
            if (ee->has_count){
                ee->size = ee->count;
            } else {
                ee->count = 1;
                ee->size = 1; //default count
            }
            if(ee->type!=PACK_x) items++;
            //if s or p, increase by one because item is expected!
        }
        entry++;
    }

    //now loop through entries and find exact size for padding and native mode
    for(entry=0;entry<entries;entry++){
        PackEntry *ee = &pes[entry];
		if (mode==0) {
			//native mode
			if (ee->type>=PACK_b && ee->type<=PACK_d){
				//integers
				int psize = packsize[ee->type];
				if(gsize%psize!=0){
					ee->padding = psize-(gsize%psize);
				} else {
					ee->padding=0;
				}
				printf("padding %i\n",ee->padding);
			} else  if (ee->type==PACK_x){
				if(entry!=entries-1){
					//last pad? -> don't touch size
					PackEntry *en = &pes[entry+1];
					if (packsize[en->type]){
						//we have a padding size reference
						int rm = (gsize+ee->size)%packsize[en->type];
						if(rm){
							//need to pad
							printf("x generates padding %i\n",rm);
							ee->size = ee->size+ (packsize[en->type]-rm);
						}
					}
				}
			}
		} else {
			//standard mode
			ee->padding=0;
		}
        gsize+=ee->size+ee->padding;
    }

    if(total_size) *total_size = gsize;
    if(total_entries) *total_entries = entries;
    if(total_items) *total_items = items;
    if(endianess) *endianess = mode;
    return pes;
}

C_NATIVE(__struct_pack) {
    NATIVE_UNWARN();
    CHECK_ARG(args[0], PSTRING);
    CHECK_ARG(args[3], PTUPLE); //vargs
    uint8_t *fmt = PSEQUENCE_BYTES(args[0]);
    int fmtl = PSEQUENCE_ELEMENTS(args[0]);
    PObject *ibuffer = args[1];
    PObject *ioffset = args[2];
    PObject *iitems = args[3];
    int bufsize = 0;
    PObject *item;
    int nitems = PSEQUENCE_ELEMENTS(iitems);
    int items=0;
    int citem=0;
    *res = MAKE_NONE();
    int pos=0;
    int entries=0;
    int entry = 0;
    int gsize = 0;
    int mode = 0;
    int i;
    int err=0;
    uint8_t *buf;
    PBytes *bres = NULL;


    PackEntry *pes = pack_calc_size(fmt,fmtl,&mode,&items,&entries,&gsize);
    if(!pes){
        return ERR_OK;
    }

    printf("Items %i of %i\n",items,nitems);
    if(items!=nitems){
        //fail
        printf("too many or too few argments\n");
        err=1;
        goto clean_up;
    }
    
    printf("gsize %i\n",gsize);
    //allocate result
    if (PTYPE(ibuffer)==PBYTEARRAY){
        //use external buffer
        bres = ibuffer;
        buf = PSEQUENCE_BYTES(ibuffer);
        int ofs = PSMALLINT_VALUE(ioffset);
        buf+=ofs;
        bufsize = PSEQUENCE_ELEMENTS(ibuffer)-ofs;
    } else {
        //allocate own buffer
        bres = pbytes_new(gsize,NULL);
        buf = PSEQUENCE_BYTES(bres);
        bufsize = 0xffff;
    }
    if(bufsize<gsize){
        //small buffer!
        err=1;
        goto clean_up;
    }
    pos = 0;

    for(entry=0;entry<entries;entry++){
        PackEntry *ee = &pes[entry];
		//add padding
		if(ee->padding) {
			for(i=0;i<ee->padding;i++){
				*buf++=0;
			}	
		}
        if (packsize[ee->type]){
            if (ee->type==PACK_c){
                //char
                i=0;
                while(i<ee->count){
                    if(citem>=nitems){
                        //no more items! fail
                        printf("out of arguments\n");
                        err=1;
                        goto clean_up;
                    }
                    if(pack_get_char(iitems,citem,buf)<0){
                        //fail!
                        err=1;
                        goto clean_up;
                    }
                    i++;
                    citem++;
                    buf++;
                }
            } else if ((ee->type>=PACK_b && ee->type<=PACK_N)||ee->type==PACK_P) {
                //integer
                i=0;
                while(i<ee->count){
                    if(citem>=nitems){
                        //no more items! fail
                        printf("out of arguments\n");
                        err=1;
                        goto clean_up;
                    }
                    pos = pack_get_integer(iitems,citem,buf,mode,packsize[ee->type]);
                    printf("parsed integer %i %i\n",citem,pos);
                    if(pos<0){
                        //fail!
                        err=1;
                        goto clean_up;
                    }
                    i++;
                    citem++;
                    buf+=pos;
                }
            } else if (ee->type>=PACK_e && ee->type<=PACK_d){
                //float
                i=0;
                while(i<ee->count){
                    if(citem>=nitems){
                        //no more items! fail
                        printf("out of arguments\n");
                        err=1;
                        goto clean_up;
                    }
                    pos = pack_get_float(iitems,citem,buf,packsize[ee->type],packbige[mode]);
                    printf("parsed float %i %i\n",citem,pos);
                    if(pos<0){
                        //fail!
                        err=1;
                        goto clean_up;
                    }
                    i++;
                    citem++;
                    buf+=pos;
                }
            }
        } else {
            if(ee->type==PACK_x){
                //pad
                memset(buf,0,ee->size);
                buf+=ee->size;
            } else if (ee->type==PACK_s){
                //str
                pos = pack_get_string(iitems,citem,buf,ee->size);
                printf("parsed string %i %i\n",citem,pos);
                if(pos<0){
                    //fail!
                    err=1;
                    goto clean_up;
                }
                buf+=pos;
                citem++;
            } else {
                //pascal str
                if(ee->size>255) {
                    err=1;
                    goto clean_up;
                }
                *buf++=ee->size;
                pos = pack_get_string(iitems,citem,buf,ee->size-1);
                if(pos<0){
                    //fail!
                    err=1;
                    goto clean_up;
                }
                buf+=pos;
                citem++;
            }
        }

    }

clean_up:

    gc_free(pes); 
    if (err) *res=MAKE_NONE();
    else *res = bres;
    return ERR_OK;
}


C_NATIVE(__struct_calcsize) {
    NATIVE_UNWARN();
    CHECK_ARG(args[0], PSTRING);
    uint8_t * fmt = PSEQUENCE_BYTES(args[0]);
    int fmtl = PSEQUENCE_ELEMENTS(args[0]);
    int gsize=0;


    PackEntry *pes = pack_calc_size(fmt,fmtl,NULL,NULL,NULL,&gsize);
    if(!pes){
        *res = PSMALLINT_NEW(-1);
    } else {
        gc_free(pes);
        *res = PSMALLINT_NEW(gsize);
    }

    return ERR_OK;
}

C_NATIVE(__struct_unpack) {
    NATIVE_UNWARN();
    CHECK_ARG(args[0], PSTRING);
    CHECK_ARG(args[2],PSMALLINT);
    uint8_t * fmt = PSEQUENCE_BYTES(args[0]);
    int fmtl = PSEQUENCE_ELEMENTS(args[0]);
    uint8_t *buf = PSEQUENCE_BYTES(args[1]);
    int bufsize = PSEQUENCE_ELEMENTS(args[1]);
    int offset = PSMALLINT_VALUE(args[2]);
    int items;
    int citem;
    int entries;
    int mode;
    int gsize;
    int entry;
    int i;
    int err=0;

    PObject *tuple;
    PObject *o;
    bufsize-=offset;
    buf+=offset;
    *res = MAKE_NONE();

    PackEntry *pes = pack_calc_size(fmt,fmtl,&mode,&items,&entries,&gsize);
    if(!pes || gsize>bufsize){
        err=1;
        goto clean_up;
    } 

    tuple = ptuple_new(items,NULL);

    citem = 0;
    for(entry=0;entry<entries;entry++){
        PackEntry *ee = &pes[entry];
        switch(ee->type){
            case PACK_x:
                //skip bytes
                buf+=ee->padding; 
                buf+=ee->size;
                break;
            case PACK_c:
                for(i=0;i<ee->size;i++){
                    o = pbytes_new(1,buf);
                    PTUPLE_SET_ITEM(tuple,citem,o);
                    citem++;
                    buf++;
                }      
                break;
            case PACK_Q:
            case PACK_q:
            case PACK_b:
            case PACK_h:
            case PACK_i:
            case PACK_l:
            case PACK_n:
                //signed
                for(i=0;i<ee->count;i++){
                    buf+=ee->padding; 
                    o = unpack_make_integer(buf,packsize[ee->type],1,packbige[mode]);
                    buf+=packsize[ee->type];
                    PTUPLE_SET_ITEM(tuple,citem,o);
                    citem++;
                }
                break;
            case PACK_B:
            case PACK_H:
            case PACK_I:
            case PACK_L:
            case PACK_N:
            case PACK_P:
                //unsigned
                for(i=0;i<ee->count;i++){
                    buf+=ee->padding; 
                    o = unpack_make_integer(buf,packsize[ee->type],0,packbige[mode]);
                    buf+=packsize[ee->type];
                    PTUPLE_SET_ITEM(tuple,citem,o);
                    citem++;
                }
                break;
            case PACK_e:
                //not supported!
                err=1;
                goto clean_up;
            case PACK_f:
            case PACK_d:
                //unsigned
                for(i=0;i<ee->count;i++){ 
                    buf+=ee->padding; 
                    o = unpack_make_float(buf,packsize[ee->type],packbige[mode]);
                    buf+=packsize[ee->type];
                    PTUPLE_SET_ITEM(tuple,citem,o);
                    citem++;
                }
                break;
            case PACK_p:
            case PACK_s:
                i = (ee->type==PACK_s) ? 0:1;
                o = pstring_new(ee->size-i,buf+i);
                PTUPLE_SET_ITEM(tuple,citem,o);
                citem++;
                buf+=ee->size;
                break;
            case PACK_bool:
                buf+=ee->padding; 
                if (*buf) {
                    PTUPLE_SET_ITEM(tuple,citem,PBOOL_TRUE());
                } else {
                    PTUPLE_SET_ITEM(tuple,citem,PBOOL_FALSE());
                }
                citem++;
                buf++;
                break;
        }
    }

clean_up:

    gc_free(pes);
    if (err) *res=MAKE_NONE();
    else *res = tuple;
    return ERR_OK;
}


