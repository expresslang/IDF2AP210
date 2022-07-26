#include "stdafx.h"

// the precompiled directive above most likely means
// that many of the includes below are no longer required
#include <stdio.h>
#include <string.h>

//#include <rose.h>
#include <expx/expx.h>

#include "idf2arm.h"

#include "../IOAP210.h"
#include "../Ap210Viewer.h"

// JDV
// Need the following new includes:
#include "express.h"
#include "load.h"
//#include "load_types.h"
//#include "vm.h"
#include "merge.h"
#include "resolve.h"
#include "error.h"
#include "checks.h"
#include "express_parse_schema_ROSE_LOAD.h"
#include "ignorestruct.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define strcasecmp(a,b) strcmp(strtolower(a),strtolower(b))


//SchemaMap * sm_idf2arm;
//SchemaMap * sm_arm2aim;
SchemaMap * sm;			// current schema map, one of above


//RoseDesign * arm_targ;
//RoseDesign * aim_targ;
RoseDesign * src;

// JDV
// Intialize for separate compilation.
SchemaMap * 
initialize_system(char * map_name)
{
  int c, nschemas;

  EXPRESSinitialize();
  RoseDesign * des = ROSE.findDesign(map_name);
  if (!des)
    return NULL;

  EXPRESSdesigns.add(des);
  des->findObjects(ROSE_DOMAIN(exp_schema), &EXPRESSschemas);
  for(c = 0, nschemas = EXPRESSschemas.size(); c < nschemas; ++c)
    EXPRESSenvironment->contents()->add(EXPRESSschemas[c]);

  load_all_schemas();
  all_schemas_resolved();
  init_local_symbols(EXPRESSenvironment);
  merge_use();
  merge_reference();
  resolve_hierarchy();

  for(c = 0, nschemas = EXPRESSschemas.size(); c < nschemas; ++c)
  {
    EXPRESScurrent_working_design = EXPRESSschemas[c]->design();
    resolve(EXPRESSschemas[c]);
  }
  detect_definedtype_cyclic_inheritance();

  return new SchemaMap(des);
}
int WriteIDFStep(SdaiModel IDFModel, char *dir, char *name) {
   // Get source schema instance. IDF MODEL PER DOC
   RoseDesign *src = _sdaiGetRoseDesign(IDFModel);

   // Save IDF Model
   src->format(IDFSTEPEXT);
   src->name(name);
   src->fileDirectory(dir);
   sdaiSaveChanges(IDFModel);
   return (0);
   }
int WriteAP210ArmFile(RoseDesign *AP210ArmModel, char *dir, char *name) {
   // Save the AP210 Arm schema instance as a Part 21 file.
   AP210ArmModel->format(AP210ARMEXT);
   AP210ArmModel->name(name);

   // build header
//   AP210ArmModel->initialize_header();

   // add file description information
   AP210ArmModel->header_description()->description()->add("IDF/AP210 ARM file");
   AP210ArmModel->header_description()->description()->add("EMPilot IDF to AP210 Translator Version 1.0");
       
   // add file name information
   AP210ArmModel->header_name()->author()->add("Empilot Team");
   AP210ArmModel->header_name()->organization()->add("PDES Inc. EMPilot");
   AP210ArmModel->header_name()->originating_system("IDF Version 2.0");

   // save the ARM
   AP210ArmModel->fileDirectory(dir);
   AP210ArmModel->save();
   return (0);
   }

int WriteAP210AimFile(RoseDesign *AP210AimModel, char *dir, char *name) {
   AP210AimModel->format(AP210EXT);
   AP210AimModel->name(name);

   // build header
//   AP210AimModel->initialize_header();

   // add file description information
   AP210AimModel->header_description()->description()->add("IDF/AP210 AIM file");
   AP210AimModel->header_description()->description()->add("EMPilot IDF to AP210 Translator Version 1.0");
        
   // add file name information
   AP210AimModel->header_name()->author()->add("Empilot Team");
   AP210AimModel->header_name()->organization()->add("PDES Inc. EMPilot");
   AP210AimModel->header_name()->originating_system("IDF Version 2.0");

   // save the AIM
   AP210AimModel->fileDirectory(dir);
   AP210AimModel->save();

   return (0);
   }
