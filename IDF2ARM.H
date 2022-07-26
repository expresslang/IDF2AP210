#ifndef IDF2ARM_H
#define IDF2ARM_H

// external function definitions
void bind_external_functions();

// functions in IDF2ARM.cpp
SchemaMap *initialize_system(char *map_name);
int CreateAP210Arm(SdaiModel myModel, char *dir, char *name);
void run_mapping(SchemaMap * the_sm, int view_cnt);


extern RoseDesign * src;
extern SchemaMap * sm;

#endif
