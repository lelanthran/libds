#include <stdarg.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>

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

static ds_array_t *g_messages = NULL;

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
   bool error;
   enum ds_json_object_type_t type;
   union {
      char              *_string;
      char              *_symbol;
      ds_array_t        *_array;    // array of ds_json_t pointers
      ds_hmap_t         *_kvpairs;  // hmap of key/value pairs [string:ds_json_t*]
      ds_json_number_t   _number;
   } value;
   ds_array_t *msgs;                // Store error and warning messages
};

static void fake_json_del (void *p, void *extra)
{
   ds_json_t *json = p;
   (void)extra;
   ds_json_del (json);
}

static void fake_free (void *p, void *extra)
{
   (void)extra;
   free (p);
}

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
         ds_array_iterate (json->value._array, fake_json_del, NULL);
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
   // Free the msgs
   ds_array_iterate (json->msgs, fake_free, NULL);
   ds_array_del (json->msgs);

   // Finally, free the object
   free (json);
}


static ds_json_t *json_new (enum ds_json_object_type_t type)
{
   ds_json_t *ret = calloc (1, sizeof *ret);
   if (!ret || !(ret->msgs = ds_array_new ())) {
      free (ret);
      return NULL;
   }
   ret->type = type;
   return ret;
}

#define ERROR(fname,line,cpos,...)     do {\
   if (!(message(__FILE__, __LINE__, "ERROR",fname,line,cpos,__VA_ARGS__))) {\
      fprintf (stderr, "%s:%i: Fatal, OOM reading [%s:%zu]\n",\
            __FILE__, __LINE__, fname, line);\
   }\
} while (0)

#define WARN(fname,line,cpos,...)     do {\
   if (!(message(__FILE__, __LINE__, "WARN",fname,line,cpos,__VA_ARGS__))) {\
      fprintf (stderr, "%s:%i: Fatal, OOM reading [%s:%zu]\n",\
            __FILE__, __LINE__, fname, line);\
   }\
} while (0)


static const char *messagev (const char *srcfile, int srcline,
                             const char *type, const char *fname,
                             size_t line, size_t cpos,
                             const char *fmts,
                             va_list ap)
{
   bool error = true;
   char *ret = NULL;
   char *prefix = NULL;
   char *msg = NULL;
   uint64_t now = time(NULL);

   if ((ds_str_printf (&prefix, "%"PRIu64":%s:%i:%s:%s:%zu:%zu:",
                                now, srcfile, srcline, type, fname, line, cpos)) == 0)
      goto cleanup;

   if ((ds_str_vprintf (&msg, fmts, ap)) == 0)
      goto cleanup;

   if (!(ret = ds_str_cat (prefix, msg, NULL)))
      goto cleanup;

   if (!g_messages) {
      if (!(g_messages = ds_array_new ())) {
         fprintf (stderr, "FATAL: Failed to create message structure\n");
         return NULL;
      }
   }

   if (!(ds_array_ins_head (g_messages, ret)))
      goto cleanup;

   error = false;
cleanup:
   free (prefix);
   free (msg);
   if (error) {
      free (ret);
      ret = NULL;
   }
   return ret;
}

static const char *message (const char *srcfile, int srcline, const char *type, const char *fname,
                             size_t line, size_t cpos,
                            const char *fmts,
                            ...)
{
   va_list ap;
   va_start (ap, fmts);
   const char *ret = messagev (srcfile, srcline, type, fname, line, cpos, fmts, ap);
   va_end(ap);
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

static ds_json_t *json_new_number (char sign,
                                   const char *major_digits,
                                   const char *minor_digits,
                                   char exp_sign,
                                   const char *exp_digits)
{
   minor_digits = minor_digits ? minor_digits : "";
   exp_digits = exp_digits ? exp_digits : "";
   ds_json_t *ret = json_new (ds_json_NUMBER);
   if (!ret || !(ret->value._number.major_digits = ds_str_dup (major_digits))
            || !(ret->value._number.minor_digits = ds_str_dup (minor_digits))
            || !(ret->value._number.exp_digits = ds_str_dup (exp_digits))) {
      ds_json_del (ret);
      return NULL;
   }
   ret->value._number.sign = (sign == '-') ? '-' : 0;
   switch (exp_sign) {
      case '-': ret->value._number.exp_sign = '-'; break;
      case 0:
      case '+': ret->value._number.exp_sign = '+'; break;
      default:
         ds_json_del (ret);
         ret = NULL;
   }

   return ret;
}

static void make_utf8(char *dst, uint64_t codepoint)
{
   (void)dst;
   (void)codepoint;
   // TODO:
}

/* ************************************************************
 * Parser functions.
 */

typedef int (parser_getchar_t) (void *handle, void *extra, size_t *line, size_t *cpos);
typedef int (parser_ungetchar_t) (void *handle, void *extra, size_t *line, size_t *cpos);

static ds_json_t *json_read_value (void *handle,
                                   void *extra,
                                   const char *fname,
                                   size_t *line,
                                   size_t *cpos,
                                   parser_getchar_t *fptr_getc,
                                   parser_getchar_t *fptr_ungetc);


static void swallow_ws (void *handle,
                        void *extra,
                        const char *fname,
                        size_t *line,
                        size_t *cpos,
                        parser_getchar_t *fptr_getc,
                        parser_getchar_t *fptr_ungetc)
{
   (void)fname;
   int c = 0;
   while ((c = fptr_getc (handle, extra, line, cpos)) && isspace(c))
      ;
   fptr_ungetc (handle, extra, line, cpos);
}

static char read_char (void *handle,
                       void *extra,
                       const char *fname,
                       size_t *line,
                       size_t *cpos,
                       parser_getchar_t *fptr_getc,
                       parser_getchar_t *fptr_ungetc)
{
   swallow_ws (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   int ret = fptr_getc (handle, extra, line, cpos);
   return (ret == EOF) ? 0 : (char)ret;
}

static char *read_digits (void *handle, void *extra,
                          const char *fname, size_t *line, size_t *cpos,
                          parser_getchar_t *fptr_getc,
                          parser_getchar_t *fptr_ungetc)
{
   int c = 0;
   char *ret = NULL;
   swallow_ws (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   while ((c = fptr_getc (handle, extra, line, cpos)) != EOF && isdigit (c)) {
      char tmp[2] = { (char)c, 0 };
      if (!(ds_str_append (&ret, tmp, NULL))) {
         free (ret);
         return NULL;
      }
   }
   fptr_ungetc (handle, extra, line, cpos);

   return ret;
}


static bool read_number (ds_json_number_t *dst, void *handle, void *extra,
                         const char *fname, size_t *line, size_t *cpos,
                         parser_getchar_t *fptr_getc,
                         parser_getchar_t *fptr_ungetc)
{
   memset (dst, 0, sizeof *dst);
   swallow_ws (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);

   int c = fptr_getc (handle, extra, line, cpos);
   if (!(isdigit (c)) && c != '-')
      return false;

   if (c == '0') {
      int tmp = fptr_getc (handle, extra, line, cpos);
      if (isdigit (tmp)) {
         return false;
      }
      if (!(dst->major_digits = ds_str_dup ("0")))
         return false;
      return true;
   }

   fptr_ungetc (handle, extra, line, cpos);

   dst->sign = read_char (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   if (dst->sign != '-') {
      fptr_ungetc (handle, extra, line, cpos);
      dst->sign = 0;
   }
   if (!(dst->major_digits = read_digits (handle, extra, fname, line, cpos,
                                          fptr_getc, fptr_ungetc))) {
      return false;
   }
   if ((fptr_getc (handle, extra, line, cpos)) != '.') {
      fptr_ungetc (handle, extra, line, cpos);
      return true;
   }
   if (!(dst->minor_digits = read_digits (handle, extra, fname, line, cpos,
                                          fptr_getc, fptr_ungetc))) {
      return false;
   }
   if ((fptr_getc (handle, extra, line, cpos)) != 'e') {
      fptr_ungetc (handle, extra, line, cpos);
      return true;
   }
   dst->exp_sign = read_char (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   if (dst->exp_sign != '+' && dst->exp_sign != '-' && !(isdigit (dst->exp_sign))) {
      fptr_ungetc (handle, extra, line, cpos);
      return false;
   }
   if (isdigit (dst->exp_sign)) {
      fptr_ungetc (handle, extra, line, cpos);
   }
   if (!(dst->exp_digits = read_digits (handle, extra, fname, line, cpos,
                                        fptr_getc, fptr_ungetc))) {
      return false;
   }

   return true;
}


static char *read_string (void *handle,
                          void *extra,
                          const char *fname,
                          size_t *line,
                          size_t *cpos,
                          parser_getchar_t *fptr_getc,
                          parser_getchar_t *fptr_ungetc)
{

   bool error = true;

   swallow_ws (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   int c = fptr_getc (handle, extra, line, cpos);
   if (c != '"')
      return NULL;

   char *ret = NULL;

   while ((c = fptr_getc (handle, extra, line, cpos)) != EOF && c != '"') {
      bool escaped = c == '\\';
      char tmp[10] = { (char)c, 0 };
      if (escaped) {
         uint64_t ic = 0;
         char sc[5];
         if ((c = fptr_getc (handle, extra, line, cpos)) == EOF)
            goto cleanup;
         switch (c) {
            case 'n':   tmp[0] = '\n';    break;
            case 'r':   tmp[0] = '\r';    break;
            case 't':   tmp[0] = '\t';    break;
            case 'b':   tmp[0] = '\b';    break;
            case 'f':   tmp[0] = '\f';    break;
            case '"':   tmp[0] = '\"';    break;
            case '\\':  tmp[0] = '\\';    break;

            case 'u':
               memset (sc, 0, sizeof sc);
               for (size_t i=0; i<4; i++) {
                  int digit = fptr_getc (handle, extra, line, cpos);
                  if (digit == EOF)
                     goto cleanup;
                  sc[i] = (char)digit;
               }

               if ((sscanf (sc, "%" PRIu64, &ic)) != 1)
                  goto cleanup;

               make_utf8 (tmp, ic);
               break;

            default:
               goto cleanup;
         }
      }
      if (!(ds_str_append (&ret, tmp, NULL)))
         goto cleanup;
   }

   error = false;

cleanup:
   if (error) {
      free (ret);
      ret = NULL;
   }

   return ret;
}

static ds_json_t *json_read_object (void *handle,
                                    void *extra,
                                    const char *fname,
                                    size_t *line,
                                    size_t *cpos,
                                    parser_getchar_t *fptr_getc,
                                    parser_getchar_t *fptr_ungetc)
{
   bool error = true;
   ds_json_t *ret = json_new_object ();
   swallow_ws (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   int c = fptr_getc (handle, extra, line, cpos);
   if (c != '{') {
      ERROR(fname, *line, *cpos, "Expected '{', found '%c'\n", c);
      return ret;
   }
   char *name = NULL;
   char colon = 0;
   ds_json_t *value = NULL;
   char comma = 0;

   do {
      free (name);
#define READ_PARAMS handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc
      name = read_string (READ_PARAMS);
      colon = read_char (READ_PARAMS);
      value = json_read_value (READ_PARAMS);
      if (!name || colon != ':' || !value || value->type == ds_json_UNKNOWN) {
         ERROR(fname, *line, *cpos, "Failed to read name/value pair");
         free (name); name = NULL;
         ds_json_del (value);
         goto cleanup;
      }
      if (!(ds_hmap_set_str_ptr (ret->value._kvpairs, name, value))) {
         free (name); name = NULL;
         ds_json_del (value);
         goto cleanup;
      }
      free (name); name = NULL;
      comma = read_char (READ_PARAMS);
#undef READ_PARAMS
   } while (comma == ',');

   error = false;
cleanup:
   ret->error = error;
   free (name);
   return ret;
}

static ds_json_t *json_read_array (void *handle,
                                   void *extra,
                                   const char *fname,
                                   size_t *line,
                                   size_t *cpos,
                                   parser_getchar_t *fptr_getc,
                                   parser_getchar_t *fptr_ungetc)
{
   bool error = true;
   ds_json_t *ret = json_new_array ();
   if (!ret)
      return NULL;

   swallow_ws (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   int c = fptr_getc (handle, extra, line, cpos);
   if (c != '[') {
      ERROR(fname, *line, *cpos, "Expected '[', got '%c'\n", c);
      goto cleanup;
   }

   char comma = 0;
   do {
      ds_json_t *value = json_read_value (handle, extra, fname, line, cpos,
                                          fptr_getc, fptr_ungetc);
      if (!value) {
         ERROR (fname, *line, *cpos, "Expecting a literal value\n");
         goto cleanup;
      }
      if (!(ds_array_ins_tail (ret->value._array, value))) {
         ERROR (fname, *line, *cpos, "OOM error\n");
         goto cleanup;
      }
      comma = read_char (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   } while (comma == ',');

   fptr_ungetc (handle, extra, line, cpos);

   error = false;
cleanup:
   ret->error = error;
   return ret;
}

static ds_json_t *json_read_string (void *handle,
                                    void *extra,
                                    const char *fname,
                                    size_t *line,
                                    size_t *cpos,
                                    parser_getchar_t *fptr_getc,
                                    parser_getchar_t *fptr_ungetc)
{
   char *tmp = read_string (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   ds_json_t *ret = json_new_string (ds_json_STRING, tmp);
   if (!ret) {
      free (tmp);
      return NULL;
   }

   if (!tmp) {
      ret->error = true;
   }
   free (tmp);
   return ret;
}

static ds_json_t *json_read_symbol (void *handle,
                                    void *extra,
                                    const char *fname,
                                    size_t *line,
                                    size_t *cpos,
                                    parser_getchar_t *fptr_getc,
                                    parser_getchar_t *fptr_ungetc)
{
   char *tmp = read_string (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   ds_json_t *ret = json_new_string (ds_json_SYMBOL, tmp);
   if (!ret) {
      free (tmp);
      return NULL;
   }

   if (!tmp) {
      ret->error = true;
   }
   free (tmp);
   return ret;
}

static ds_json_t *json_read_number (void *handle,
                                    void *extra,
                                    const char *fname,
                                    size_t *line,
                                    size_t *cpos,
                                    parser_getchar_t *fptr_getc,
                                    parser_getchar_t *fptr_ungetc)
{
   ds_json_number_t number;
   bool error = read_number (&number, handle, extra, fname, line, cpos,
                             fptr_getc, fptr_ungetc);
   ds_json_t *ret = json_new_number (number.sign,
                                     number.major_digits, number.minor_digits,
                                     number.exp_sign,
                                     number.exp_digits);
   free (number.major_digits);
   free (number.minor_digits);
   free (number.exp_digits);

   if (ret) {
      ret->error = error;
   }
   return ret;
}

static ds_json_t *json_read_value (void *handle,
                                   void *extra,
                                   const char *fname,
                                   size_t *line,
                                   size_t *cpos,
                                   parser_getchar_t *fptr_getc,
                                   parser_getchar_t *fptr_ungetc)
{

   swallow_ws (handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc);
   int c = fptr_getc (handle, extra, line, cpos);

   fptr_ungetc (handle, extra, line, cpos);
#define READ_PARAMS     handle, extra, fname, line, cpos, fptr_getc, fptr_ungetc
   if (c == '{')
      return json_read_object (READ_PARAMS);

   if (c == '[')
      return json_read_array (READ_PARAMS);

   if (c == '"')
      return json_read_string (READ_PARAMS);

   if ((isdigit (c)) || c == '-')
      return json_read_number (READ_PARAMS);

   return json_read_symbol (READ_PARAMS);
#undef READ_PARAMS
}

static ds_json_t *json_parse (const char *fname, void *handle, void *extra,
                              parser_getchar_t *fptr_getc,
                              parser_ungetchar_t *fptr_ungetc)
{
   size_t line = 1;
   size_t cpos = 1;

   swallow_ws (handle, extra, fname, &line, &cpos, fptr_getc, fptr_ungetc);

   return json_read_object (handle, extra, fname, &line, &cpos, fptr_getc, fptr_ungetc);
}


static int parser_string_getchar (const char *src, size_t *index,
                                  size_t *line, size_t *cpos)
{
   if (src[*index] == 0)
      return EOF;

   int ret = src[*index];
   *index = *index + 1;
   if (ret == '\n') {
      *line = *line + 1;
      *cpos = 0;
   } else {
      *cpos = *cpos + 1;
   }

   return ret;
}

static int parser_string_ungetchar (const char *src, size_t *index,
                                    size_t *line, size_t *cpos)
{
   if (*index == 0)
      return EOF;

   *index = *index - 1;
   int ret = src[*index];
   if (ret == '\n') {
      *line = *line - 1;
      (void)cpos;    // cpos is essentially inaccurate now until the next linefeed
   }
   return ret;
}


static int parser_stream_getchar (FILE *infile, char *saved, size_t *line, size_t *cpos)
{
   // TODO: Redo these.
   (void)line;
   (void)cpos;
   int ret = fgetc (infile);
   *saved = (char)ret;
   return ret;
}

static int parser_stream_ungetchar (FILE *infile, char *saved, size_t *line, size_t *cpos)
{
   // TODO: Redo these.
   (void)line;
   (void)cpos;
   return ungetc (*saved, infile);
}


ds_json_t *ds_json_parse_string (const char *name, const char *src)
{
   size_t index = 0;
   return json_parse (name, (void *)src, &index,
                      (parser_getchar_t *)parser_string_getchar,
                      (parser_ungetchar_t *)parser_string_ungetchar);
}


ds_json_t *ds_json_parse_stream (const char *name, FILE *infile)
{
   return json_parse (name, infile, NULL,
                      (parser_getchar_t *)parser_stream_getchar,
                      (parser_ungetchar_t *)parser_stream_ungetchar);
}


char **ds_json_messages_get (void)
{
   if (!g_messages || (ds_array_length (g_messages) == 0))
      return NULL;

   size_t nmessages = ds_array_length (g_messages);
   char **ret = calloc (nmessages + 1, sizeof *ret);
   if (!ret)
      return NULL;
   for (size_t i=0; i<nmessages; i++) {
      const char *msg = ds_array_get (g_messages, i);
      if (!(ret[i] = ds_str_dup (msg))) {
         fprintf (stderr, "%s:%i: FATAL: OOM allocating message %zu [%s]\n",
                  __FILE__, __LINE__, i, msg);
      }
   }
   return ret;
}

void ds_json_messages_clear (void)
{
   if (!g_messages)
      return;

   ds_array_iterate (g_messages, fake_free, NULL);
   ds_array_del (g_messages);
   g_messages = NULL;
}

#define INDENT(o)    for (size_t i=0; i<o->depth; i++) {\
   ds_str_append (&o->output, " ", NULL);\
}

struct stringify_t {
   uint16_t depth;
   char *output;
};

static void stringify (ds_json_t *json, struct stringify_t *sobj);

static void stringify_object_keys (const void *key, size_t keylen,
                                   void *data, size_t datalen,
                                   void *sobj)
{
   const char *name = key;
   (void)keylen;
   ds_json_t *value = data;
   (void)datalen;
   struct stringify_t *obj = sobj;

   printf ("Processing key [%s]\n", name);

   ds_str_append (&obj->output, "\n", NULL);
   INDENT(obj);
   ds_str_append (&obj->output, "\"", name, "\": ", NULL);
   stringify (value, sobj);
}

void stringify_object (ds_json_t *json, struct stringify_t *sobj)
{
   (void)json;
   ds_str_append (&sobj->output, "{", NULL);
   sobj->depth++;
   ds_hmap_iterate (json->value._kvpairs, stringify_object_keys, sobj);
   sobj->depth--;
   ds_str_append (&sobj->output, "\n", NULL);
   INDENT(sobj);
   ds_str_append (&sobj->output, "} ", NULL);
}

void stringify_array (ds_json_t *json, struct stringify_t *sobj)
{
   (void)sobj;
   (void)json;
   // TODO: Stopped here last - iterate over the array
}

void stringify_string (ds_json_t *json, struct stringify_t *sobj)
{
   (void)sobj;
   ds_str_append (&sobj->output, "\"", json->value._string, "\"", NULL);
}

void stringify_symbol (ds_json_t *json, struct stringify_t *sobj)
{
   ds_str_append (&sobj->output, json->value._string, NULL);
}

void stringify_number (ds_json_t *json, struct stringify_t *sobj)
{
   // bool error = true;
   char sign[2] = { 0, 0 };
   char exp_sign[2] = { 0, 0 };
   sign[0] = json->value._number.sign;
   exp_sign[0] = json->value._number.exp_sign;

   if (!(ds_str_append (&sobj->output, sign, json->value._number.major_digits, NULL))) {
      goto cleanup;
   }

   if (json->value._number.minor_digits[0]) {
      if (!(ds_str_append (&sobj->output, ".", json->value._number.minor_digits, NULL))) {
         goto cleanup;
      }
   }

   if (json->value._number.exp_digits[0]) {
      if (!(ds_str_append (&sobj->output, "e", exp_sign, NULL))) {
         goto cleanup;
      }
      if (!(ds_str_append (&sobj->output, json->value._number.exp_digits, NULL))) {
         goto cleanup;
      }
   }

   // error = false;

cleanup:
   // TODO: Something something error handling
   return;
}


static void stringify (ds_json_t *json, struct stringify_t *sobj)
{
   if (!json)
      return;

   switch (json->type) {
      case ds_json_UNKNOWN: ds_str_dup ("ERROR");           break;
      case ds_json_OBJECT:  stringify_object (json, sobj);  break;
      case ds_json_ARRAY:   stringify_array (json, sobj);   break;
      case ds_json_STRING:  stringify_string (json, sobj);  break;
      case ds_json_SYMBOL:  stringify_symbol (json, sobj);  break;
      case ds_json_NUMBER:  stringify_number (json, sobj);  break;
   }

}


char *ds_json_stringify (ds_json_t *json)
{
   struct stringify_t *sobj = calloc (1, sizeof *sobj);
   if (!sobj)
      return NULL;
   stringify (json, sobj);
   char *ret = sobj->output;
   free (sobj);
   return ret;
}

