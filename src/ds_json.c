
#include "ds_json.h"
#include "ds_array.h"
#include "ds_hmap.h"
#include "ds_tree.h"
#include "ds_str.h"


/* ************************************************************************
 * Minimal JSON parser
 *
 * Each `read()` returns a JSON literal, one of:
 * 1. Object
 * 2. Array
 * 3. String
 * 4. Symbol
 * 5. Number (starts with either `-` or a digit)
 * 6. RD - Record delimiter (comma)
 * 7. FD - Field delimiter (colon)
 * 8. ND - Number delimiter (`.`)
 *
 * A value is read using the main `read()` function as the entrypoint.
 * The main `read()` function dispatches a `read_*()` function based on the
 * non-whitespace character read.
 *    Within `read_object()` , we attempt to read a name, a FD and a value, in that
 *    order. Reading a RD after that loops. Each name/value pair is stored as a tree
 *    with the specified name and value. Tree type is set to `object`.
 *
 *    Within `read_array()`, we attempt to read a value. Reading a RD after that
 *    loops. Each value read is stored as an element in an array. The value of the
 *    treenode is a ds_array_t and there is no name set. Tree type is set to
 *    `array`.
 *
 *    Within `read_string()` we accumulate characters until we get to the
 *    terminating double-quote. Escape characters are handled like this:
 *       \n:      insert a newline
 *       \r:      insert a carriage return
 *       \t:      insert a tab
 *       \b:      insert a backspace
 *       \f:      insert a formfeed
 *       \":      insert a double-quote
 *       \\:      insert a slash
 *       \uXXXX:  Insert the unicode combination as UTF8
 *    For every other combination of `\`, register a warning. Tree type is set to
 *    `string` and the value is stored without a name as a nul-terminated string.
 *
 *    Within `read_symbol()`, we accumulate characters until non-symbol character is
 *    reached. The only valid symbols in JSON are `null`, `true` and `false`. No
 *    name is stored and the value is stored as a nul-terminated string. Tree
 *    type is set to `symbol`.
 *
 *    Within `read_number()`, we first attempt to read a sign, then we accumulate
 *    digits, then we attempt to read an `e`, and if successful, we attempt to read
 *    the optional sign and then we accumulate a third set of digits.
 *
 *    We accumlate sets of digits twice: accumulate before a ND and
 *    accumulate after ND. We expect a single period to separate the sets of digits.
 *    If the ND is not present we skip reading the second set of digits.
 *
 *    Finally, we check for the suffix `e` and if present we check for a sign. We
 *    store both of these and accumulate the third and final set of digits.
 *
 *    The value is stored as a record consisting of:
 *       A sign (either empty or '-')
 *       Major digits (A sequence or string of digits **OR** a single `0` character)
 *       Minor digits (A sequence or string of digits)
 *       The exponent sign (either '+' or '-')
 *       The exponent (A sequence or string of digits)
 *    Tree type is set to `number`.
 */

// Function to free hashmap of kvpairs
void kvpair_del (const void *key, size_t key_len,
                 void *value, size_t value_len,
                 void *param)
{
   (void)key;
   (void)key_len;
   ds_json_del ((ds_json_t *)value);
   (void)value_len;
   (void)param;
}


// Store a JSON Number object
struct ds_json_number_t {
   char      sign;            // Either 0 or ascii '-'
   char     *major_digits;    // Major digits
   char     *minor_digits;    // Minor digits
   char      exp_sign;        // Sign of the exponent, one of 0, '+' or '-'
   char     *exp_digits;      // Exponent digits
};

// The opaque pointer for callers
struct ds_json_t {
   enum ds_json_object_type_t type;
   union {
      char              *_string;
      char              *_symbol;
      ds_array_t        *_array;    // array of ds_json_t pointers
      ds_hmap_t         *_kvpairs;  // hmap of key/value pairs [string:ds_json_t*]
      ds_json_number_t   _number;
   } value;
};

void ds_json_del (ds_json_t *json)
{
   if (!json)
      return;

   // First free the value
   switch (json->type) {
      case ds_json_OBJECT:
         ds_hmap_iterate (json->value._kvpairs, kvpair_del, NULL);
         ds_hmap_del (json->value._kvpairs);
         break;

      case ds_json_ARRAY:
         ds_array_iterate (json->value._array, (void (*) (void *, void *))ds_json_del, NULL);
         ds_array_del (json->value._array);
         break;

      case ds_json_STRING:
         free (json->value._string);
         break;

      case ds_json_SYMBOL:
         free (json->value._symbol);
         break;

      case ds_json_NUMBER:
         free (json->value._number.major_digits);
         free (json->value._number.minor_digits);
         free (json->value._number.exp_digits);
         break;

      case ds_json_UNKNOWN:
         // Error!!!
         break;
   }

   // Finally, free the object
   free (json);
}


static ds_json_t *json_new (enum ds_json_object_type_t type)
{
   ds_json_t *ret = malloc (sizeof *ret);
   if (!ret)
      return NULL;
   ret->type = type;
   return ret;
}

size_t ds_json_nbuckets = 20;

static ds_json_t *json_new_object (void)
{
   ds_json_t *ret = json_new (ds_json_OBJECT);
   if (!ret || !(ret->value._kvpairs = ds_hmap_new (ds_json_nbuckets))) {
      ds_json_del (ret);
      ret = NULL;
   }
   return ret;
}

static ds_json_t *json_new_array (void)
{
   ds_json_t *ret = json_new (ds_json_ARRAY);
   if (!ret || !(ret->value._array = ds_array_new ())) {
      ds_json_del (ret);
      ret = NULL;
   }
   return ret;
}

static ds_json_t *json_new_string (enum ds_json_object_type_t type, const char *src)
{
   ds_json_t *ret = json_new (type);
   if (!ret || !(ret->value._string = ds_str_dup (src))) {
      ds_json_del (ret);
      ret = NULL;
   }
   return ret;
}

static ds_json_t *json_new_number (const char *sign,
                                   const char *major_digits,
                                   const char *minor_digits,
                                   const char *exp_sign,
                                   const char *exp_digits)
{
   ds_json_t *ret = json_new (ds_json_NUMBER);
   if (!ret || !(ret->value._number.major_digits = ds_str_dup (major_digits))
            || !(ret->value._number.minor_digits = ds_str_dup (minor_digits))
            || !(ret->value._number.exp_digits = ds_str_dup (exp_digits))) {
      ds_json_del (ret);
      return NULL;
   }
   ret->value._number.sign = (sign && *sign == '-') ? '-' : 0;
   if (exp_sign) {
      switch (*exp_sign) {
         case '-': ret->value._number.exp_sign = '-'; break;
         case '+': ret->value._number.exp_sign = '+'; break;
      }
   } else {
      exp_sign = 0;
   }

   return ret;
}

/* ************************************************************
 * Parser functions.
 */

typedef int (parser_getchar_t) (void *handle, void *extra);
typedef int (parser_ungetchar_t) (void *handle, void *extra);

static ds_json_t *json_parse (void *handle,
                              void *extra,
                              parser_getchar_t *fptr_getc,
                              parser_ungetchar_t *fptr_ungetc)
{
   int c = 0;
   while ((c = fptr_getc (handle, extra)) != EOF) {
      // TODO: Stopped here last
      printf ("[%c]\n", c);
   }
   (void)fptr_ungetc;
   return NULL;
}


static int parser_string_getchar (const char *src, size_t *index)
{
   if (src[*index] == 0) {
      return EOF;
   }
   int ret = src[*index];
   (*index)++;
   return ret;
}

static int parser_string_ungetchar (const char *src, size_t *index)
{
   return *index == 0 ? EOF : src[*index++];
}


static int parser_stream_getchar (FILE *infile, char *saved)
{
   return (*saved = fgetc (infile));
}

static int parser_stream_ungetchar (FILE *infile, char *saved)
{
   return ungetc (*saved, infile);
}


ds_json_t *ds_json_parse_string (const char *src)
{
   size_t index = 0;
   return json_parse ((void *)src, &index,
                      (parser_getchar_t *)parser_string_getchar,
                      (parser_ungetchar_t *)parser_string_ungetchar);
}


ds_json_t *ds_json_parse_stream (FILE *infile)
{
   return json_parse (infile, NULL,
                      (parser_getchar_t *)parser_stream_getchar,
                      (parser_ungetchar_t *)parser_stream_ungetchar);
}



