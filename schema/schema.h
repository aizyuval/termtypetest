 /* Schema.h */
 /* This file declares schemas for use with libcyaml */

 /* Declare and Define structs to be used for schema defenitions:*/
#ifndef CONFIG_H
#define CONFIG_H
 /* stats yaml structs: */
struct stats_metrics {
	float wpm;
	float rwpm;
	float acc;
};
struct stats_cat { 
	struct stats_metrics best; 
	struct stats_metrics *tests; 
	unsigned int tests_count;
};
struct stats_file {
	struct stats_cat *times;
	unsigned int times_count;
};

 /* stats yaml structs: */
struct _strings {
	char ** words_member;
	unsigned int words_member_count; 
};

 /* Declare language yaml schemas: */

 extern const cyaml_schema_field_t stats_metrics_field_schema[];
 extern const cyaml_schema_value_t stats_metrics_schema;
 extern const cyaml_schema_field_t stats_cat_field_schema[];
 extern const cyaml_schema_value_t stats_cat_schema;


 extern const cyaml_schema_field_t stats_file_field_schema[];

 extern const cyaml_schema_value_t stats_file_schema_top;

 extern const cyaml_config_t config;
 


 /* Declare language yaml schemas: */

 extern const cyaml_schema_value_t strings_schema;
 extern const cyaml_schema_field_t strings_field_schema[];

 extern const cyaml_schema_value_t strings_schema_top;

#endif
