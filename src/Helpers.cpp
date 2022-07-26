#include "stdafx.h"

// the precompiled directive above most likely means
// that many of the includes below are no longer required
#include <stdio.h>
#include <string.h>

#include <rose.h>
#include <expx/expx.h>

// Forward function declarations
void PopulateBaseCircle();

#define strcasecmp(a,b) strcmp(strtolower(a),strtolower(b))

extern SchemaMap * sm;			// current schema map, one of above

// Convert boolean to string.
void
boolean_to_string()
{
  static char b[10];
  RoseBoolean x = sm->fetchBoolean(0);
  if( x ) strcpy( b, "true" );
  else	  strcpy( b, "false" );
  sm->returnString(strdup(b));
}

// Convert enumeration attribute to an integer
void enum_to_integer()
{
  RoseObject * o = sm->fetchObject(0);
  char * att = sm->fetchString(1); // Attribute must be a enumeration type.
  if (o)
  {
    RoseAttribute * a = o->getAttribute(att);
    if (!a)
      printf("enum_to_integer: No such attribute '%s'\n", att);
    else
      sm->returnInteger(o->getInteger(a));
  }
  else
    printf("enum_to_integer: Object is NULL\n");
}

// Convert integer to string.
void
integer_to_string()
{
  static char s[100];
  int x = sm->fetchInteger(0);
  sprintf(s, "%d", x);
  sm->returnString(strdup(s));
}

// Delete an instantiated object
void
delete_obj()
{
  RoseObject * o = sm->fetchObject(0);
  if (!o)
    printf("Object is NULL\n");
  else
    delete o;
}

// Set enumeration attribute to indeterminate.
void set_null_enum()
{
  RoseObject * o = sm->fetchObject(0);
  char * att = sm->fetchString(1); // Attribute must be a enumeration type.
  if (o)
  {
    RoseAttribute * a = o->getAttribute(att);
    if (!a)
      printf("set_null_enum: No such attribute '%s'\n", att);
    else
      o->putInteger(NULL_ENUM, a);
  }
  else
    printf("set_null_enum: Object is NULL\n");
}

// Set complex attribute to indeterminate.
void set_null_complex_attribute()
{
//  unsigned i,sz;
  RoseObject * o = sm->fetchObject(0);
  char * dom = sm->fetchString(1); // sub type.
  char * att = sm->fetchString(2); // Attribute name.
  if (o)
  {
    RoseDomain * domain = o->domain()->findTypeSuperType(dom);
    if( domain )
    {
      RoseAttribute * a = domain->findTypeAttribute(att);
      if( a )
      {
        if( a->isString() ) o->putString( "", a );
        else
          printf("set_null_complex_attribute: Type '%s' not handled yet\n", a->slotRoseType()->name() );
      }
      else
        printf("set_null_complex_attribute: No such attribute '%s'\n", att);
    }
    else
      printf("set_null_complex_attribute: Domain '%s' is not a super type of the object\n", dom);
  }
  else
    printf("set_null_complex_attribute: Object is NULL\n");
}

// Set complex string attribute
void set_complex_string_attribute()
{
//  unsigned i,sz;
  RoseObject * o = sm->fetchObject(0);
  char * dom = sm->fetchString(1); // Entity sub-type.
  char * att = sm->fetchString(2); // Attribute name.
  char * value = sm->fetchString(3); // String Value.
  if (o)
  {
    RoseDomain * domain = o->domain()->findTypeSuperType(dom);
    if( domain )
    {
      RoseAttribute * a = domain->findTypeAttribute(att);
      if( a )
      {
        if( a->isString() ) o->putString( strdup(value), a );
        else
          printf("set_complex_string_attribute: Type '%s' is not a string type\n", a->slotRoseType()->name() );
      }
      else
        printf("set_complex_string_attribute: No such attribute '%s'\n", att);
    }
    else
      printf("set_complex_string_attribute: Domain '%s' is not a super type of the object\n", dom);
  }
  else
    printf("set_complex_string_attribute: Object is NULL\n");
}

// Set date_and_time from IDF date string
//    yyyy/mm/dd.hh:mm:ss
//
//  Known Bugs:
//     Does not set time_zone which is not in input string.
void
PopulateDateAndTime()
{
  int count;
  int year, month, day, hour, minute, second, temp;
  RoseObject *dat = sm->fetchObject(0);  
  char *date = sm->fetchString(1);
//  printf("\nDate: %s\n", date);
  count = sscanf(date, "%d/%d/%d.%d:%d:%d", &year, &month, &day, &hour, &minute, &second);

  if (count == 6) {
     // check for screwy date in format:
     //   mm/dd/yy.hh:mm:ss
     if (year <= 12)
        {
        temp = day;
        day = month;
        month = year;
        // WARNING: year 2000 problem
        year = temp + 1900;
        if (year < 1956)
           {
           year += 100;
           }
        }
//printf("New Date: %d/%d/%d.%d:%d:%d", year, month, day, hour, minute, second);
 
     dat->putInteger(year,  "year");
     dat->putInteger(month, "month");
     dat->putInteger(day,   "day");
     dat->putInteger(hour,  "hour");
     dat->putInteger(minute,"minute");
     dat->putInteger(second,"second");
     }
  else {
     fprintf(stderr, "\nERROR: '%s' is not a valid date.\n", date);
     }
}
// Functions to store and retrieve
// source/target and target/source object associations.

// Manager object that will hold associations.
class association_manager : public BaseManager 
{
public:
  RoseObject * originated_from;
  RoseObject * created;

  // Manager boilerplate code (see Rose Library Reference Manaual).
  static RoseManagerType class_type();
  virtual RoseManagerType manager_type() { return class_type(); }
  association_manager() { originated_from = created = NULL; }
};

RoseManagerType association_manager::class_type()
{
  static RoseManagerType mytype = BaseManager::new_manager_type();
  return mytype;
}

// Get the manager; create if doesn't exist.
association_manager *
get_manager(RoseObject * o)
{
  association_manager * mgr
    = (association_manager *) o->find_manager(association_manager::class_type());
  if (!mgr)
  {
    mgr = new association_manager();
    o->add_manager(mgr);
  }
  return mgr;
}

void
set_originated_from()
{
  RoseObject * src = sm->fetchObject(0);
  RoseObject * targ = sm->fetchObject(1);
  association_manager * tm = get_manager(targ);
  tm->originated_from = src;
}

void
set_created()
{
  RoseObject * src = sm->fetchObject(0);
  RoseObject * targ = sm->fetchObject(1);
  association_manager * m = get_manager(src);
  m->created = targ;
}

void
get_source()
{
  RoseObject * targ = sm->fetchObject(0);
  association_manager * tm = get_manager(targ);
  sm->returnObject(tm->originated_from);
}

void
get_target()
{
  RoseObject * src = sm->fetchObject(0);
  association_manager * m = get_manager(src);
  sm->returnObject(m->created);
}


// Find RoseDesign bound to a given schema instance.
RoseDesign *
find_schema_binding(char * schema_instance)
{
  ListOfString * all_schema_instances = sm->schemaInstanceNames();
  int sz = all_schema_instances->size();
  for (int i=0; i < sz; ++i)
  {
    if (!strcmp(schema_instance, all_schema_instances->_element(i)))
      return sm->boundSchemaInstance(i);
  }
  return NULL;
}

// Create a complex entity instance of the 
// given type in the given schema instance.
void
create_complex()
{
  char * typ = sm->fetchString(0);
  char * schema_instance = sm->fetchString(1);
  RoseObject * o = NULL;
  RoseDesign * d = find_schema_binding(schema_instance);
  if (!d)
  {
    fprintf(stderr, "\ncreate_complex: '%s' not a valid schema instance!\n", 
	    schema_instance);
    exit(1);
  }
  o = d->pnewInstance(typ);
  if (!o)
  {
    fprintf(stderr,
	    "\ncreate_complex: '%s' not a valid complex type; check working set\n", 
	    typ);
    exit(1);
  }
  sm->returnObject(o);
}

// Display a debugging message
void debug_integer()
{
  int int_value = sm->fetchInteger(0);

  printf( "%d\n", int_value );
}

// Display a debugging message
void debug_message()
{
  char * message = sm->fetchString(0);

  printf( "%s\n", message );
}

// Part 21 file header helpers
// initialize the Part 21 file header.
void initialize_header()
{
  char * schema_instance = sm->fetchString(0);
  RoseObject * o = NULL;
  RoseDesign * d = find_schema_binding(schema_instance);
  if (!d)
  {
    fprintf(stderr, "\ncreate_complex: '%s' not a valid schema instance!\n", 
	    schema_instance);
    exit(1);
  }
  d->initialize_header();
}// Set the originating system in the part21 file header.
void
set_originating_system()
{
  char * os = sm->fetchString(0);
  char * schema_instance = sm->fetchString(1);
  RoseObject * o = NULL;
  RoseDesign * d = find_schema_binding(schema_instance);
  if (!d)
  {
    fprintf(stderr, "\ncreate_complex: '%s' not a valid schema instance!\n", 
	    schema_instance);
    exit(1);
  }
  d->header_name()->originating_system(os);
}

// add to the description in the part21 file header.
void
add_to_description()
{
  char * desc = sm->fetchString(0);
  char * schema_instance = sm->fetchString(1);
  RoseObject * o = NULL;
  RoseDesign * d = find_schema_binding(schema_instance);
  if (!d)
  {
    fprintf(stderr, "\ncreate_complex: '%s' not a valid schema instance!\n", 
	    schema_instance);
    exit(1);
  }
  d->header_description()->description()->add(desc);
}
// add to the author in the part21 file header.
void
add_to_author()
{
  char * auth = sm->fetchString(0);
  char * schema_instance = sm->fetchString(1);
  RoseObject * o = NULL;
  RoseDesign * d = find_schema_binding(schema_instance);
  if (!d)
  {
    fprintf(stderr, "\ncreate_complex: '%s' not a valid schema instance!\n", 
	    schema_instance);
    exit(1);
  }
  d->header_name()->author()->add(auth);
}
// add to the organization in the part21 file header.
void
add_to_organization()
{
  char * desc = sm->fetchString(0);
  char * schema_instance = sm->fetchString(1);
  RoseObject * o = NULL;
  RoseDesign * d = find_schema_binding(schema_instance);
  if (!d)
  {
    fprintf(stderr, "\ncreate_complex: '%s' not a valid schema instance!\n", 
	    schema_instance);
    exit(1);
  }
  d->header_name()->organization()->add(desc);
}
// Bind external functions.
void
bind_external_functions()
{
  // IDF specific functions
  sm->bindFunction("boolean_to_string", boolean_to_string);
  sm->bindFunction("integer_to_string", integer_to_string);
  sm->bindFunction("delete_obj", delete_obj);
  sm->bindFunction("set_complex_string_attribute", set_complex_string_attribute);
  sm->bindFunction("set_null_complex_attribute", set_null_complex_attribute);
  sm->bindFunction("set_null_enum", set_null_enum);
  sm->bindFunction("enum_to_integer", enum_to_integer);
  sm->bindFunction("PopulateBaseCircle", PopulateBaseCircle);
  sm->bindFunction("PopulateDateAndTime", PopulateDateAndTime);

  // Function to create complex instances.
  sm->bindFunction("create_complex", create_complex);

  // functions to manage references
  sm->bindFunction("set_originated_from", set_originated_from);
  sm->bindFunction("set_created", set_created);
  sm->bindFunction("get_source", get_source);
  sm->bindFunction("get_target", get_target);

  // functions to help in debugging
  sm->bindFunction("debug_integer", debug_integer);
  sm->bindFunction("debug_message", debug_message);

  // functions to update Part 21 header
  sm->bindFunction("initialize_header", initialize_header);
  sm->bindFunction("set_originating_system", set_originating_system);
  sm->bindFunction("add_to_description", add_to_description);
  sm->bindFunction("add_to_author", add_to_author);
  sm->bindFunction("add_to_organization", add_to_organization);
}

