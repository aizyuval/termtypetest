 /* schema.c */
 /* This file *defines* schemas for use with libcyaml */

#include <cyaml/cyaml.h>
#include "schema.h"


 /* Define stats yaml schemas: */
 const cyaml_schema_field_t stats_metrics_field_schema[] = {
	 CYAML_FIELD_FLOAT("wpm",CYAML_FLAG_DEFAULT, struct stats_metrics, wpm),
	 CYAML_FIELD_FLOAT("rwpm",CYAML_FLAG_DEFAULT, struct stats_metrics, rwpm),
	 CYAML_FIELD_FLOAT("acc",CYAML_FLAG_DEFAULT, struct stats_metrics, acc),

	 CYAML_FIELD_END
 };
 const cyaml_schema_value_t stats_metrics_schema = {
	 CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, struct stats_metrics, stats_metrics_field_schema),
 };

 const cyaml_schema_field_t stats_cat_field_schema[] = {
	 CYAML_FIELD_MAPPING(
		 "best", CYAML_FLAG_DEFAULT, struct stats_cat, best, stats_metrics_field_schema),
	 CYAML_FIELD_SEQUENCE("tests", CYAML_FLAG_POINTER, struct stats_cat, tests,&stats_metrics_schema,0,CYAML_UNLIMITED), // tests is sequence without known size
	 CYAML_FIELD_END
 };
 const cyaml_schema_value_t stats_cat_schema = {
	 CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, struct stats_cat, stats_cat_field_schema),
 };


 const cyaml_schema_field_t stats_file_field_schema[] = {
	 CYAML_FIELD_SEQUENCE("times", CYAML_FLAG_POINTER, struct stats_file, times, &stats_cat_schema, 0,CYAML_UNLIMITED),
	 CYAML_FIELD_END
 };

 const cyaml_schema_value_t stats_file_schema_top = {
	 CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct stats_file, stats_file_field_schema),
 };

 const cyaml_config_t config = {
	 .log_fn = cyaml_log,
	 .mem_fn = cyaml_mem,
	 .log_level = CYAML_LOG_WARNING,
 };



 /* Define languages yaml schemas: */

 const cyaml_schema_value_t strings_schema = {
	 CYAML_VALUE_STRING(CYAML_FLAG_POINTER, char*, 0, CYAML_UNLIMITED),
 };
 const cyaml_schema_field_t strings_field_schema[] = { 
	 CYAML_FIELD_SEQUENCE("words", CYAML_FLAG_POINTER, struct _strings, words_member,  &strings_schema, 0, CYAML_UNLIMITED),
	 CYAML_FIELD_END
 };

 const cyaml_schema_value_t strings_schema_top = {
	 CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct _strings, strings_field_schema),
 };

