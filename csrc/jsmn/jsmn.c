#include "jsmn.h"

/**
 * Allocates a fresh unused token from the token pull.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser,
        jsmntok_t *tokens, size_t num_tokens) {
    jsmntok_t *tok;
    if (parser->toknext >= num_tokens) {
        return NULL;
    }
    tok = &tokens[parser->toknext++];
    tok->start = tok->end = -1;
    tok->size = 0;
#ifdef JSMN_PARENT_LINKS
    tok->parent = -1;
#endif
    return tok;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, jsmntype_t type,
                            int start, int end) {
    token->type = type;
    token->start = start;
    token->end = end;
    token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js,
        size_t len, jsmntok_t *tokens, size_t num_tokens) {
    jsmntok_t *token;
    int start;

    start = parser->pos;

    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
        switch (js[parser->pos]) {
#ifndef JSMN_STRICT
            /* In strict mode primitive must be followed by "," or "}" or "]" */
            case ':':
#endif
            case '\t' : case '\r' : case '\n' : case ' ' :
            case ','  : case ']'  : case '}' :
                goto found;
        }
        if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
            parser->pos = start;
            return JSMN_ERROR_INVAL;
        }
    }
#ifdef JSMN_STRICT
    /* In strict mode primitive must be followed by a comma/object/array */
    parser->pos = start;
    return JSMN_ERROR_PART;
#endif

found:
    if (tokens == NULL) {
        parser->pos--;
        return 0;
    }
    token = jsmn_alloc_token(parser, tokens, num_tokens);
    if (token == NULL) {
        parser->pos = start;
        return JSMN_ERROR_NOMEM;
    }
    jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
#ifdef JSMN_PARENT_LINKS
    token->parent = parser->toksuper;
#endif
    parser->pos--;
    return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js,
        size_t len, jsmntok_t *tokens, size_t num_tokens) {
    jsmntok_t *token;

    int start = parser->pos;

    parser->pos++;

    /* Skip starting quote */
    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
        char c = js[parser->pos];

        /* Quote: end of string */
        if (c == '\"') {
            if (tokens == NULL) {
                return 0;
            }
            token = jsmn_alloc_token(parser, tokens, num_tokens);
            if (token == NULL) {
                parser->pos = start;
                return JSMN_ERROR_NOMEM;
            }
            jsmn_fill_token(token, JSMN_STRING, start+1, parser->pos);
#ifdef JSMN_PARENT_LINKS
            token->parent = parser->toksuper;
#endif
            return 0;
        }

        /* Backslash: Quoted symbol expected */
        if (c == '\\' && parser->pos + 1 < len) {
            int i;
            parser->pos++;
            switch (js[parser->pos]) {
                /* Allowed escaped symbols */
                case '\"': case '/' : case '\\' : case 'b' :
                case 'f' : case 'r' : case 'n'  : case 't' :
                    break;
                /* Allows escaped symbol \uXXXX */
                case 'u':
                    parser->pos++;
                    for(i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0'; i++) {
                        /* If it isn't a hex character we have an error */
                        if(!((js[parser->pos] >= 48 && js[parser->pos] <= 57) || /* 0-9 */
                                    (js[parser->pos] >= 65 && js[parser->pos] <= 70) || /* A-F */
                                    (js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */
                            parser->pos = start;
                            return JSMN_ERROR_INVAL;
                        }
                        parser->pos++;
                    }
                    parser->pos--;
                    break;
                /* Unexpected symbol */
                default:
                    parser->pos = start;
                    return JSMN_ERROR_INVAL;
            }
        }
    }
    parser->pos = start;
    
    return JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.
 */
int jsmn_parse(jsmn_parser *parser, const char *js, size_t len,
        jsmntok_t *tokens, unsigned int num_tokens) {
    int r;
    int i;
    jsmntok_t *token;
    int count = parser->toknext;

    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
        char c;
        jsmntype_t type;

        c = js[parser->pos];
        switch (c) {
            case '{': case '[':
                count++;
                if (tokens == NULL) {
                    break;
                }
                token = jsmn_alloc_token(parser, tokens, num_tokens);
                if (token == NULL)
                    return JSMN_ERROR_NOMEM;
                if (parser->toksuper != -1) {
                    tokens[parser->toksuper].size++;
#ifdef JSMN_PARENT_LINKS
                    token->parent = parser->toksuper;
#endif
                }
                token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
                token->start = parser->pos;
                parser->toksuper = parser->toknext - 1;
                break;
            case '}': case ']':
                if (tokens == NULL)
                    break;
                type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
                if (parser->toknext < 1) {
                    return JSMN_ERROR_INVAL;
                }
                token = &tokens[parser->toknext - 1];
                for (;;) {
                    if (token->start != -1 && token->end == -1) {
                        if (token->type != type) {
                            return JSMN_ERROR_INVAL;
                        }
                        token->end = parser->pos + 1;
                        parser->toksuper = token->parent;
                        break;
                    }
                    if (token->parent == -1) {
                        if(token->type != type || parser->toksuper == -1) {
                            return JSMN_ERROR_INVAL;
                        }
                        break;
                    }
                    token = &tokens[token->parent];
                }
#else
                for (i = parser->toknext - 1; i >= 0; i--) {
                    token = &tokens[i];
                    if (token->start != -1 && token->end == -1) {
                        if (token->type != type) {
                            return JSMN_ERROR_INVAL;
                        }
                        parser->toksuper = -1;
                        token->end = parser->pos + 1;
                        break;
                    }
                }
                /* Error if unmatched closing bracket */
                if (i == -1) return JSMN_ERROR_INVAL;
                for (; i >= 0; i--) {
                    token = &tokens[i];
                    if (token->start != -1 && token->end == -1) {
                        parser->toksuper = i;
                        break;
                    }
                }
#endif
                break;
            case '\"':
                r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
                if (r < 0) return r;
                count++;
                if (parser->toksuper != -1 && tokens != NULL)
                    tokens[parser->toksuper].size++;
                break;
            case '\t' : case '\r' : case '\n' : case ' ':
                break;
            case ':':
                parser->toksuper = parser->toknext - 1;
                break;
            case ',':
                if (tokens != NULL && parser->toksuper != -1 &&
                        tokens[parser->toksuper].type != JSMN_ARRAY &&
                        tokens[parser->toksuper].type != JSMN_OBJECT) {
#ifdef JSMN_PARENT_LINKS
                    parser->toksuper = tokens[parser->toksuper].parent;
#else
                    for (i = parser->toknext - 1; i >= 0; i--) {
                        if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT) {
                            if (tokens[i].start != -1 && tokens[i].end == -1) {
                                parser->toksuper = i;
                                break;
                            }
                        }
                    }
#endif
                }
                break;
#ifdef JSMN_STRICT
            /* In strict mode primitives are: numbers and booleans */
            case '-': case '0': case '1' : case '2': case '3' : case '4':
            case '5': case '6': case '7' : case '8': case '9':
            case 't': case 'f': case 'n' :
                /* And they must not be keys of the object */
                if (tokens != NULL && parser->toksuper != -1) {
                    jsmntok_t *t = &tokens[parser->toksuper];
                    if (t->type == JSMN_OBJECT ||
                            (t->type == JSMN_STRING && t->size != 0)) {
                        return JSMN_ERROR_INVAL;
                    }
                }
#else
            /* In non-strict mode every unquoted value is a primitive */
            default:
#endif
                r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
                if (r < 0) return r;
                count++;
                if (parser->toksuper != -1 && tokens != NULL)
                    tokens[parser->toksuper].size++;
                break;

#ifdef JSMN_STRICT
            /* Unexpected char in strict mode */
            default:
                return JSMN_ERROR_INVAL;
#endif
        }
    }

    if (tokens != NULL) {
        for (i = parser->toknext - 1; i >= 0; i--) {
            /* Unmatched opened object or array */
            if (tokens[i].start != -1 && tokens[i].end == -1) {
                return JSMN_ERROR_PART;
            }
        }
    }

    return count;
}

/**
 * Creates a new parser based over a given  buffer with an array of tokens
 * available.
 */
void jsmn_init(jsmn_parser *parser) {
    parser->pos = 0;
    parser->toknext = 0;
    parser->toksuper = -1;
}




//ZERYNTH WRAPPER
#include "zerynth.h"
//#define printf(...) vbl_printf_stdout(__VA_ARGS__)

int str_to_num(char *str,int size, int* ires, FLOAT_TYPE* fres){
    int i=0;
    int sign;
    uint32_t iacc=0;
    uint32_t macc=0;
    uint32_t dacc=0;
    FLOAT_TYPE facc=0;
    int is_float = 0;

    if(str[0]=='-'){
        sign = -1;
        i=1;
    } else {
        sign = 1;
    }

    for(;i<size;i++){
        if (str[i]=='.'){
            is_float=1;
            macc=0;
            dacc=1;
            continue;
        }
        if (is_float){
            macc=macc*10+(str[i]-'0');
            dacc=dacc*10;
        } else {
            iacc=iacc*10+(str[i]-'0');
        }
    }
    if (is_float){
        facc = (FLOAT_TYPE)iacc+(FLOAT_TYPE)macc/(FLOAT_TYPE)dacc;
        //printf("iacc %i, macc %i, dacc %i, is_float %i, f %g %x\n",iacc,macc,dacc,is_float,facc,facc);
    }
    *ires=sign*iacc;
    *fres=sign*facc;
    return is_float;
}

C_NATIVE(jsmn_loads){
    C_NATIVE_UNWARN();
    uint8_t *jstr;
    uint32_t jlen;
    int r,i,c,ii;
    FLOAT_TYPE ff;

    if (parse_py_args("s", nargs, args, &jstr, &jlen) != 1)
        return ERR_TYPE_EXC;

    RELEASE_GIL();

    jsmn_parser parser;
    jsmn_init(&parser);

    r = jsmn_parse(&parser,jstr,jlen,NULL,0);
    printf("TOKENS %i %x %i\n",r,(int)jstr,jlen);

    if(r<0) {
        ACQUIRE_GIL();
        return ERR_VALUE_EXC;
    }
    jsmntok_t *tokens=gc_malloc(sizeof(jsmntok_t)*r);

    jsmn_init(&parser);
    jsmn_parse(&parser,jstr,jlen,tokens,r);
    printf("TOKENS2 %i\n",r);

    //check parse correctness
    for(i=0;i<r;i++){
        if (tokens[i].end<0){
            gc_free(tokens);
            ACQUIRE_GIL();
            return ERR_VALUE_EXC;
        }
    }

    jsmntok_t *token;

    for(i=0;i<r;i++){
        token =  &tokens[i];
        //printf("TOKEN %i %i %i ch %i p %i %x\n",i,token->start,token->end,token->size, token->parent,(int)token->obj);
        switch(token->type){
            case JSMN_OBJECT:
                token->obj = (void*)pdict_new(token->size);
                printf("OBJECT %i %i ch %i p %i %x\n",token->start,token->end,token->size, token->parent,(int)token->obj);
                break;
            case JSMN_ARRAY:
                token->obj = (void*)plist_new(token->size,NULL);
                PSEQUENCE_ELEMENTS_SET(token->obj,0);
                printf("ARRAY %i %i ch %i p %i %x\n",token->start,token->end,token->size, token->parent,(int)token->obj);
                break;
            case JSMN_STRING:
                token->obj = (void*)pstring_new(token->end-token->start,jstr+token->start);
                printf("STRING %i %i ch %i p %i %x\n",token->start,token->end,token->size, token->parent,(int)token->obj);
                break;
            case JSMN_PRIMITIVE:
                c = jstr[token->start];
                switch(c){
                    case 't':
                        token->obj = (void*)PBOOL_TRUE();
                        printf("TRUE %i %i ch %i p %i %x\n",token->start,token->end,token->size, token->parent,(int)token->obj);
                        break;
                    case 'f':
                        token->obj = (void*)PBOOL_FALSE();
                        printf("FALSE %i %i ch %i p %i %x\n",token->start,token->end,token->size, token->parent,(int)token->obj);
                        break;
                    case 'n':
                        token->obj = (void*)MAKE_NONE();
                        printf("NULL %i %i ch %i p %i %x\n",token->start,token->end,token->size, token->parent,(int)token->obj);
                        break;
                    default:
                        if ((c>='0' && c<='9') || c=='-') {
                            //it's a number
                            if (str_to_num(jstr+token->start,token->end-token->start,&ii,&ff)){
                                //is float
                                token->obj = (void*)pfloat_new(ff);
                                printf("FLOAT %i %i ch %i p %i %x\n",token->start,token->end,token->size, token->parent,(int)token->obj);
                            } else {
                                //is int
                                token->obj = (void*)PSMALLINT_NEW(ii);
                                printf("INT %i %i ch %i p %i %x\n",token->start,token->end,token->size, token->parent,(int)token->obj);
                            }
                        }
                }
                break;
        }
        if (token->parent>=0){
            jsmntok_t *ptoken = &tokens[token->parent];
            if (ptoken->type == JSMN_STRING){
                //when the parent is a string, token it's the value of a key (ptoken is the key)
                jsmntok_t *dtoken = &tokens[ptoken->parent];
                pdict_put(dtoken->obj,ptoken->obj,token->obj);
            } else if (ptoken->type==JSMN_OBJECT) {
                //skip, token it's a key, we will add it together with the key
            } else if (ptoken->type==JSMN_ARRAY){
                ii = PSEQUENCE_ELEMENTS(ptoken->obj);
                PLIST_SET_ITEM(ptoken->obj,ii,token->obj);
                PSEQUENCE_ELEMENTS_SET(ptoken->obj,ii+1);
            }
        }
    }

    *res = tokens[0].obj;
    gc_free(tokens);
    ACQUIRE_GIL();

    return ERR_OK;
}




