// #define ZERYNTH_PRINTF 1
#include "zerynth.h"
#include "cbor.h"

/** Sets the memory management routines to use.
 *
 * Only available when CBOR_CUSTOM_ALLOC is truthy
 *
 * \rst
 * .. warning:: This function modifies the global state and should therefore be used accordingly. Changing the memory handlers while allocated items exist will result in a ``free``/``malloc`` mismatch. This function is not thread safe with respect to both itself and all the other *libcbor* functions that work with the heap.
 * .. note:: `realloc` implementation must correctly support `NULL` reallocation
 * \endrst
 *
 * @param custom_malloc malloc implementation
 * @param custom_realloc realloc implementation
 * @param custom_free free implementation
 */
// void cbor_set_allocs(_cbor_malloc_t custom_malloc, _cbor_realloc_t custom_realloc, _cbor_free_t custom_free);

int nm=0;
void *cbor_malloc(size_t n){
    nm++;
    return gc_malloc(n);
}

void * cbor_realloc(void *pnt, size_t n){
    if(!pnt) nm++;
    return gc_realloc(pnt,n);
}
void cbor_free(void *pnt){
    if(pnt) nm--;
    return gc_free(pnt);;
}

PObject *TagClass;
PObject *UndefinedClass;
int tagname,valuename;

C_NATIVE(_cbor_init)
{
    NATIVE_UNWARN();
    TagClass = args[0];
    UndefinedClass = args[1];
    tagname = PSMALLINT_VALUE(args[2]);
    valuename = PSMALLINT_VALUE(args[3]);

    cbor_set_allocs(cbor_malloc,cbor_realloc,cbor_free);
    *res = MAKE_NONE();
    return ERR_OK;
}

// uint32_t _array_indef_elements(cbor_item_t *a){
//     uint32_t sz = 0;
//     uint32_t nitems = cbor_array_size(a);
//     cbor_item_t **items = cbor_array_handle(a);
//     for(int i=0;i<nitems;i++){
//         if (cbor_typeof(items[i])==4){
//             if(cbor_array_is_definite(items[i])){
//                 sz+=cbor_array_size(items[i]);
//             } else { 
//                 sz+= _array_indef_elements(items[i]);
//             }
//         } else sz++;
//     }
//     return sz;
// }

PObject *_cbor_to_py(cbor_item_t *cbo){
    PObject *o = MAKE_NONE();
    switch (cbor_typeof(cbo)) {
        case 0: {
            //positive integers
            switch (cbor_int_get_width(cbo)) {
                case CBOR_INT_8:  o=PSMALLINT_NEW(cbor_get_uint8(cbo)); break;
                case CBOR_INT_16: o=PSMALLINT_NEW(cbor_get_uint16(cbo)); break;
                case CBOR_INT_32: o=pinteger_new(cbor_get_uint32(cbo)); break;
                case CBOR_INT_64: o=pinteger_new(cbor_get_uint64(cbo)); break;
            }
        }
        break;
        case 1: {
            //negative integers
            switch (cbor_int_get_width(cbo)) {
                case CBOR_INT_8:  o=PSMALLINT_NEW(-(cbor_get_uint8(cbo)+1)); break;
                case CBOR_INT_16: o=PSMALLINT_NEW(-(cbor_get_uint16(cbo)+1)); break;
                case CBOR_INT_32: o=pinteger_new(-(cbor_get_uint32(cbo)+1)); break;
                case CBOR_INT_64: o=pinteger_new(-(cbor_get_uint64(cbo)+1)); break;
            }
        }
        break;
        case 2: {
            //byte strings
            if (cbor_bytestring_is_definite(cbo)){
                //definite
                int sz = cbor_bytestring_length(cbo);
                o = pbytes_new(sz,cbor_bytestring_handle(cbo));

            } else {
                //indefinite
                int ns = cbor_bytestring_chunk_count(cbo);
                int sz = 0;
                int i;
                cbor_item_t **strs = cbor_bytestring_chunks_handle(cbo);
                for(i=0;i<ns;i++){
                    sz+=cbor_bytestring_length(strs[i]); //TODO: this may not be definite
                }
                o = pbytes_new(sz,NULL);
                uint8_t *buf = PSEQUENCE_BYTES(o);
                for(i=0;i<ns;i++){
                    sz=cbor_bytestring_length(strs[i]);
                    memcpy(buf,cbor_bytestring_handle(strs[i]),sz);
                    buf+=sz;
                }
            }

        }
        break;
        case 3: {
            //strings
            if (cbor_string_is_definite(cbo)){
                //definite
                int sz = cbor_string_length(cbo);
                o = pstring_new(sz,cbor_string_handle(cbo));

            } else {
                //indefinite
                int ns = cbor_string_chunk_count(cbo);
                int sz = 0;
                int i;
                cbor_item_t **strs = cbor_string_chunks_handle(cbo);
                for(i=0;i<ns;i++){
                    sz+=cbor_string_length(strs[i]);  //this is definite by definition in rfc7049
                }
                o = pstring_new(sz,NULL);
                uint8_t *buf = PSEQUENCE_BYTES(o);
                for(i=0;i<ns;i++){
                    sz=cbor_string_length(strs[i]);
                    memcpy(buf,cbor_string_handle(strs[i]),sz);
                    buf+=sz;
                }
            }

        }
        break;
        case 4: {
            //arrays
            int sz = cbor_array_size(cbo);
            int i;
            o = plist_new(sz,NULL);
            cbor_item_t **items = cbor_array_handle(cbo);
            for(i=0;i<sz;i++){
                cbor_item_t *item = items[i];
                PLIST_SET_ITEM(o,i,_cbor_to_py(item));
            }
        }
        break;
        case 5: {
            //maps
            //definite
            int sz = cbor_map_size(cbo);
            int i;
            o = pdict_new(sz);
            struct cbor_pair* pairs = cbor_map_handle(cbo);
            for(i=0;i<sz;i++){
                cbor_item_t *key = pairs[i].key;
                cbor_item_t *val = pairs[i].value;
                phash_put(o,_cbor_to_py(key),_cbor_to_py(val));
            }

        }
        break;
        case 6: {
            //tags
            o = pinstance_new(TagClass);
            PInstance *oo = (PInstance *)o;
            phash_put(oo->dict, PSMALLINT_NEW(tagname), pinteger_new(cbor_tag_value(cbo)));
            PObject *ooo = _cbor_to_py(cbor_tag_item(cbo));
            phash_put(oo->dict, PSMALLINT_NEW(valuename), ooo);
        }
        break;
        case 7: {
            //special: float & ctrls
            if (cbor_float_ctrl_is_ctrl(cbo)) {
                uint8_t val = cbor_ctrl_value(cbo);
                if (val==20) o = PBOOL_FALSE();
                else if (val==21) o = PBOOL_TRUE();
                else if (val==22) o = MAKE_NONE();
                else if (val==23) {
                    o = pinstance_new(UndefinedClass);
                } else {
                    //return None for all other unassigned/reserved values
                    o = MAKE_NONE();
                }
            } else {
                double v = cbor_float_get_float(cbo);
                o = pfloat_new(v);
            }
        }
        break;
        default:
        //TODO: error
        break;
    }
    return o;
}

C_NATIVE(_cbor_loads)
{
    NATIVE_UNWARN();
    uint8_t *buf;
    uint32_t len;
    *res = MAKE_NONE();
    if (parse_py_args("s", nargs, args, &buf, &len) != 1) {
        return ERR_TYPE_EXC;
    }
    struct cbor_load_result lr;
    cbor_item_t *rc = cbor_load(buf,len, &lr);
    if (lr.error.code!=CBOR_ERR_NONE){
        printf("ERR %i\n",lr.error.code);
        // cbor_decref(&rc);
        return ERR_VALUE_EXC;
    }

    *res = _cbor_to_py(rc);
    cbor_decref(&rc);
    printf("==========> NM %i\n",nm);
    return ERR_OK;
}

cbor_item_t *_py_to_cbor(PObject *o){
    cbor_item_t *root=NULL;

    printf("PTYPE %i\n",PTYPE(o));
    switch(PTYPE(o)){
        case PSMALLINT:
        case PINTEGER:{
            int64_t v = INTEGER_VALUE(o);
            int is_signed = (v<0) ? 1:0;
            if (is_signed) v=-v-1;
            
            if(v<=0xff) {
                root = cbor_new_int8();
                cbor_set_uint8(root,v);
            } else if(v<=0xffff) {
                root = cbor_new_int16();
                cbor_set_uint16(root,v);
            } else if(v<=0xffffffff){
                root = cbor_new_int32();
                cbor_set_uint32(root,v);
            } else {
                root = cbor_new_int64();
                cbor_set_uint64(root,v);
            }
            if(is_signed) cbor_mark_negint(root);
        }
        break;
        case PFLOAT:{
            double v = FLOAT_VALUE(o);
            root = cbor_new_float8();
            cbor_set_float8(root,v);
        }
        break;
        case PBOOL:{
            int v = (o==PBOOL_FALSE())?0:1;
            root = cbor_build_bool(v);
        }
        break;
        case PSTRING:{
            root = cbor_new_definite_string();
            uint8_t *data = cbor_malloc(PSEQUENCE_ELEMENTS(o));
            memcpy(data,PSEQUENCE_BYTES(o),PSEQUENCE_ELEMENTS(o));
            cbor_string_set_handle(root,data,PSEQUENCE_ELEMENTS(o));
        }
        break;
        case PBYTES:
        case PBYTEARRAY:{
            root = cbor_new_definite_bytestring();
            uint8_t *data = cbor_malloc(PSEQUENCE_ELEMENTS(o));
            memcpy(data,PSEQUENCE_BYTES(o),PSEQUENCE_ELEMENTS(o));
            cbor_bytestring_set_handle(root,data,PSEQUENCE_ELEMENTS(o));
        }
        break;
        case PSHORTS:
        case PSHORTARRAY:{
            root = cbor_new_definite_bytestring();
            uint8_t *data = cbor_malloc(PSEQUENCE_ELEMENTS(o)*2);
            memcpy(data,PSEQUENCE_BYTES(o),PSEQUENCE_ELEMENTS(o)*2);
            cbor_bytestring_set_handle(root,data,PSEQUENCE_ELEMENTS(o)*2);
        }
        break;
        case PTUPLE:
        case PLIST:{
            int elements = PSEQUENCE_ELEMENTS(o);
            int i;
            root = cbor_new_definite_array(elements);
            for(i=0;i<elements;i++){
                PObject *item = (PTYPE(o)==PLIST)? (PLIST_ITEM(o,i)):(PTUPLE_ITEM(o,i));
                cbor_item_t *ii = _py_to_cbor(item);
                cbor_array_set(root,i,ii);
                cbor_decref(&ii);
            }
        }
        break;
        case PDICT:{
            PHash *dict = (PHash *)o;
            int i;
            root = cbor_new_definite_map(dict->elements);
            for (i = 0; i < dict->elements; i++) {
                HashEntry *h = phash_getentry(dict, i);
                cbor_item_t *kk = _py_to_cbor(h->key);
                cbor_item_t *vv = _py_to_cbor(h->value);
                struct cbor_pair pair;
                pair.key = kk;
                pair.value = vv;
                cbor_map_add(root,pair);
                cbor_decref(&kk);
                cbor_decref(&vv);
            }
        }
        break;
        case PINSTANCE:{
            PInstance *p = (PInstance*)o;
            if (p->base==UndefinedClass) {
                root = cbor_new_undef();
            } else if(p->base==TagClass){
                PObject *tag = phash_get(p->dict,PSMALLINT_NEW(tagname));
                PObject *vv = phash_get(p->dict,PSMALLINT_NEW(valuename));
                root = cbor_new_tag(INTEGER_VALUE(tag));
                cbor_item_t * val = _py_to_cbor(vv);
                cbor_tag_set_item(root,val);
                cbor_decref(&val);
            } else {
                //error!
                root = cbor_new_undef();
            }
        }
        break;
        case PNONE:{
            root = cbor_new_null();
        }
        break;
        default:
            //return undef!
            root = cbor_new_undef();
            break;

    }
    return root;
}

C_NATIVE(_cbor_dumps)
{
    NATIVE_UNWARN();
    PObject *o = args[0];
    *res = MAKE_NONE();
    int32_t err = ERR_OK;

    cbor_item_t *cobj = _py_to_cbor(o);
    uint8_t * buffer;
    int buffer_size, length = cbor_serialize_alloc(cobj, &buffer, &buffer_size);
    if(buffer) {
        *res = pbytes_new(length,buffer);
        cbor_free(buffer);
    } else {
        err = ERR_RUNTIME_EXC;
    }
    cbor_decref(&cobj);
    printf("==========> NM %i\n",nm);
    return err;
}




