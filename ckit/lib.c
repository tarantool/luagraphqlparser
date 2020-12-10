#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <c/GraphQLAstVisitor.h>
#include <c/GraphQLParser.h>
#include <c/GraphQLAstNode.h>
#include <c/GraphQLAst.h>
#include <c/GraphQLAstToJSON.h>

static int
visit_document(const struct GraphQLAstDocument *def, void *arg)
{
	struct lua_State *L = arg;

	// kind: document
	lua_pushstring(L, "kind");
	lua_pushstring(L, "document");
	lua_settable(L, -3);

	// definitions: [
	lua_pushstring(L, "definitions");
	lua_newtable(L);
	return 1;
}

static void
end_visit_document(const struct GraphQLAstDocument *def, void *arg)
{
	struct lua_State *L = arg;
	// definitions: ]
	lua_settable(L, -3);
}

static int
visit_operation_definition(const struct GraphQLAstOperationDefinition *def, void *arg)
{
	struct lua_State *L = arg;

	// len = #definitions
	size_t len = lua_objlen(L, -1);

	// definitions[len + 1]: {
	lua_pushinteger(L, len + 1);
	lua_newtable(L);

	// kind: operation
	lua_pushstring(L, "kind");
	lua_pushstring(L, "operation");
	lua_settable(L, -3);

	// operation: "query/mutation/..."
	lua_pushstring(L, "operation");
	const char *operation = GraphQLAstOperationDefinition_get_operation(def);
	lua_pushstring(L, operation);
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_operation_definition(const struct GraphQLAstOperationDefinition *def, void *arg)
{
	struct lua_State *L = arg;
	// definitions[len + 1]: }
	lua_settable(L, -3);
}

static int
visit_selection_set(const struct GraphQLAstSelectionSet *def, void *arg)
{
	struct lua_State *L = arg;

	// selectionSet: {
	lua_pushstring(L, "selectionSet");
	lua_newtable(L);

	// kind: selectionSet
	lua_pushstring(L, "kind");
	lua_pushstring(L, "selectionSet");
	lua_settable(L, -3);

	// selections: [
	lua_pushstring(L, "selections");
	lua_newtable(L);

	return 1;
}

static void
end_visit_selection_set(const struct GraphQLAstSelectionSet *def, void *arg)
{
	struct lua_State *L = arg;

	// selections: ]
	lua_settable(L, -3);

	// selectionSet: }
	lua_settable(L, -3);
}

static int
visit_field(const struct GraphQLAstField *def, void *arg)
{
	printf("visit_field\n");
	int arg_size = GraphQLAstField_get_arguments_size(def);
	int directive_size = GraphQLAstField_get_directives_size(def);

	struct lua_State *L = arg;

	// len = #selections
	size_t len = lua_objlen(L, -1);

	// selections[len + 1]: {
	lua_pushinteger(L, len + 1);
	lua_newtable(L);

	// kind: field
	lua_pushstring(L, "kind");
	lua_pushstring(L, "field");
	lua_settable(L, -3);

	if (arg_size > 0) {
		// arguments: []
		lua_pushstring(L, "arguments");
		lua_newtable(L);
		lua_settable(L, -3);
	}

	if (directive_size > 0) {
		// directives: []
		lua_pushstring(L, "directives");
		lua_newtable(L);
		lua_settable(L, -3);
	}

	return 1;
}

static void
end_visit_field(const struct GraphQLAstField *def, void *arg)
{
	printf("end_visit_field\n");
	struct lua_State *L = arg;
	// selections[len + 1]: }
	lua_settable(L, -3);
}

static int
visit_name(const struct GraphQLAstName *def, void *arg)
{
	struct lua_State *L = arg;

	// "name": {
	lua_pushstring(L, "name");
	lua_newtable(L);

	// value: value
	const char *value = GraphQLAstName_get_value(def);
	printf("visit_name %s\n", value);
	lua_pushstring(L, "value");
	lua_pushstring(L, value);
	lua_settable(L, -3);

	lua_pushstring(L, "kind");
	lua_pushstring(L, "name");
	lua_settable(L, -3);

	return 0;
}

static void
end_visit_name(const struct GraphQLAstName *def, void *arg)
{
	printf("end_visit_name\n");
	// "name": }
	struct lua_State *L = arg;
	lua_settable(L, -3);
}

static int
visit_variable_definition(const struct GraphQLAstVariableDefinition *def, void *arg)
{
	printf("visit_field_definition\n");
	return 1;
}

static void
end_visit_variable_definition(const struct GraphQLAstVariableDefinition *def, void *arg)
{
	printf("end_visit_field_definition\n");
}

static int
visit_field_definition(const struct GraphQLAstFieldDefinition *def, void *arg)
{
	int size = GraphQLAstFieldDefinition_get_arguments_size(def);
	printf("visit_field_definition %d\n", size);
	return 1;
}

static void
end_visit_field_definition(const struct GraphQLAstFieldDefinition *def, void *arg)
{
	printf("end_visit_field_definition\n");
}

static int
visit_directive(const struct GraphQLAstDirective *def, void *arg)
{
	int size = GraphQLAstDirective_get_arguments_size(def);
	printf("visit_directive %d\n", size);
	return 1;
}

static void
end_visit_directive(const struct GraphQLAstDirective *def, void *arg)
{
	printf("end_visit_directive");
}

static int
visit_argument(const struct GraphQLAstArgument *def, void *arg)
{
	printf("visit_argument\n");

	struct lua_State *L = arg;

	lua_getfield(L, -1, "arguments");

	// len = #arguments
	size_t len = lua_objlen(L, -1);

	// arguments[len + 1]: {
	lua_pushinteger(L, len + 1);
	lua_newtable(L);

	// arguments[len + 1]: {"value":
//	lua_pushstring(L, "value");
//	lua_newtable(L);

	return 1;
}

static void
end_visit_argument(const struct GraphQLAstArgument *def, void *arg)
{
	printf("end_visit_argument\n");
	struct lua_State *L = arg;

	// set value
	lua_pushstring(L, "value");
	lua_insert(L, -2);
	lua_settable(L, -3);

	lua_pushstring(L, "kind");
	lua_pushstring(L, "argument");
	lua_settable(L, -3);

	// arguments[len + 1]: }
	lua_settable(L, -3);

	lua_pop(L, 1);
}

static int
visit_variable(const struct GraphQLAstVariable *def, void *arg)
{
	printf("visit_variable\n");

	struct lua_State *L = arg;

	// "value": {
	lua_pushstring(L, "value");
	lua_newtable(L);

	lua_pushstring(L, "kind");
	lua_pushstring(L, "variable");
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_variable(const struct GraphQLAstVariable *def, void *arg)
{
	printf("end_visit_variable\n");
	// "value": }
	struct lua_State *L = arg;
	lua_settable(L, -3);
}

static int
visit_input_value_definition(const struct GraphQLAstInputValueDefinition *def, void *arg)
{
	printf("visit_input_value_definition\n");
	return 1;
}

static void
end_visit_input_value_definition(const struct GraphQLAstInputValueDefinition *def, void *arg)
{
	printf("end_visit_input_value_definition\n");
}

static void
stackDump(lua_State *L) {
	int i = lua_gettop(L);
	printf(" ----------------  Stack Dump ----------------\n" );
	while (i > 0) {
		int t = lua_type(L, i);
		switch (t) {
			case LUA_TSTRING:
				printf("%d:`%s'\n", i, lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:
				printf("%d: %s\n",i,
					   lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				printf("%d: %g\n",  i, lua_tonumber(L, i));
				break;
			default:
				printf("%d: %s\n", i, lua_typename(L, t));
				break;
		}
		i--;
	}
	printf("--------------- Stack Dump Finished ---------------\n" );
}

static int
visit_object_value(const struct GraphQLAstObjectValue *def, void *arg)
{
	int fields_size = GraphQLAstObjectValue_get_fields_size(def);
	printf("visit_object_value %d\n", fields_size);

	struct lua_State *L = arg;

	lua_newtable(L);

	// value: { values: [
	lua_pushstring(L, "values");
	lua_newtable(L);

	return 1;
}

static void
end_visit_object_value(const struct GraphQLAstObjectValue *def, void *arg)
{
	int size = GraphQLAstObjectValue_get_fields_size(def);
	printf("end_visit_object_value %d\n", size);

	struct lua_State *L = arg;

	for (int i = 0; i < size; i++) {
		lua_pushnumber(L, size - i);
		lua_insert(L, -2);
		lua_settable(L, -3 - size + i + 1);
	}

	lua_settable(L, -3);

	lua_pushstring(L, "kind");
	lua_pushstring(L, "InputObject");
	lua_settable(L, -3);


	// "value": }
//	lua_settable(L, -3);
}

static int
visit_object_field(const struct GraphQLAstObjectField *def, void *arg)
{
	const struct GraphQLAstName *name_node = GraphQLAstObjectField_get_name(def);

	const char *name = GraphQLAstName_get_value(name_node);
	printf("visit_object_field %s\n", name);

	struct lua_State *L = arg;

	lua_newtable(L);

	return 1;
}

static void
end_visit_object_field(const struct GraphQLAstObjectField *def, void *arg)
{
	printf("end_visit_object_field\n");
	struct lua_State *L = arg;
	lua_pushstring(L, "value");
	lua_insert(L, -2);
	lua_settable(L, -3);
}

//
//void print_table(lua_State *L)
//{
//	if ((lua_type(L, -2) == LUA_TSTRING))
//		printf("%s\n", lua_tostring(L, -2));
//
//	lua_pushnil(L);
//	while(lua_next(L, -2) != 0) {
//		if(lua_isstring(L, -1))
//			printf("%s = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
//		else if(lua_isnumber(L, -1))
//			printf("%s = %d\n", lua_tostring(L, -2), lua_tonumber(L, -1));
//		else if(lua_istable(L, -1)) {
//			print_table(L);
//		}
//		lua_pop(L, 1);
//	}
//}

#define VISIT_SINGLE_VALUE(type, snake_type) 										\
static int																			\
visit_##type##_value(const struct GraphQLAst##snake_type##Value *def, void *arg) 	\
{																					\
    printf("visit_"#type"_value\n");												\
	struct lua_State *L = arg;														\
	lua_newtable(L);																\
																					\
	const char *value = GraphQLAst##snake_type##Value_get_value(def);				\
																					\
	lua_pushstring(L, "value");														\
	lua_pushstring(L, value);														\
	lua_settable(L, -3);															\
																					\
	lua_pushstring(L, "kind");														\
	lua_pushstring(L, #type);														\
	lua_settable(L, -3);															\
																					\
	return 1;																		\
}

#define END_VISIT_SINGLE_VALUE(type, snake_type) 									\
static void																			\
end_visit_##type##_value()															\
{																					\
	printf("end_visit_"#type"_value\n");											\
}

VISIT_SINGLE_VALUE(int, Int)
END_VISIT_SINGLE_VALUE(int, Int)
VISIT_SINGLE_VALUE(float, Float)
END_VISIT_SINGLE_VALUE(float, Float)
VISIT_SINGLE_VALUE(string, String)
END_VISIT_SINGLE_VALUE(string, String)
VISIT_SINGLE_VALUE(enum, Enum)
END_VISIT_SINGLE_VALUE(enum, Enum)


static int
visit_boolean_value(const struct GraphQLAstBooleanValue *def, void *arg)
{
	printf("visit_boolean_value\n");

	struct lua_State *L = arg;

	lua_newtable(L);

	int value = GraphQLAstBooleanValue_get_value(def);
	lua_pushstring(L, "value");
	lua_pushstring(L, value ? "true" : "false");
	lua_settable(L, -3);

	lua_pushstring(L, "kind");
	lua_pushstring(L, "boolean");
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_boolean_value(const struct GraphQLAstBooleanValue *def, void *arg)
{
	printf("end_visit_boolean_value\n");
}

static int
visit_null_value(const struct GraphQLAstNullValue *def, void *arg)
{
	printf("visit_null_value\n");

	struct lua_State *L = arg;

	lua_newtable(L);

	lua_pushstring(L, "value");
	lua_pushstring(L, "null");
	lua_settable(L, -3);

	lua_pushstring(L, "kind");
	lua_pushstring(L, "null");
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_null_value(const struct GraphQLAstNullValue *def, void *arg)
{
	printf("end_visit_null_value\n");
}

static int
visit_list_value(const struct GraphQLAstListValue *def, void *arg)
{
	printf("visit_list_value\n");

	struct lua_State *L = arg;

	lua_newtable(L);

	// value: { values: [
	lua_pushstring(L, "values");
	lua_newtable(L);

	return 1;
}

static void
end_visit_list_value(const struct GraphQLAstListValue *def, void *arg)
{
	int size = GraphQLAstListValue_get_values_size(def);

	struct lua_State *L = arg;

	for (int i = 0; i < size; i++) {
		lua_pushnumber(L, size - i);
		lua_insert(L, -2);
		lua_settable(L, -3 - size + i + 1);
	}

	lua_settable(L, -3);

	lua_pushstring(L, "kind");
	lua_pushstring(L, "list");
	lua_settable(L, -3);
}

static int
visit_common()
{
	printf("visit_common");
	return 1;
}

static void
end_visit_common()
{
	printf("end_visit_common");
}

static const struct GraphQLAstVisitorCallbacks LuaAstVisitorCallbacks = {
		.visit_document = visit_document,
		.end_visit_document = end_visit_document,
		.visit_operation_definition = visit_operation_definition,
		.end_visit_operation_definition = end_visit_operation_definition,
		.visit_variable_definition = visit_variable_definition,
		.end_visit_variable_definition = end_visit_variable_definition,
		.visit_selection_set = visit_selection_set,
		.end_visit_selection_set = end_visit_selection_set,
		.visit_field = visit_field,
		.end_visit_field = end_visit_field,
		.visit_argument = visit_argument,
		.end_visit_argument = end_visit_argument,
		.visit_fragment_spread = visit_common,
		.end_visit_fragment_spread = NULL,
		.visit_inline_fragment = visit_common,
		.end_visit_inline_fragment = NULL,
		.visit_fragment_definition = visit_common,
		.end_visit_fragment_definition = NULL,
		.visit_variable = visit_variable,
		.end_visit_variable = end_visit_variable,
		.visit_int_value = visit_int_value,
		.end_visit_int_value = end_visit_int_value,
		.visit_float_value = visit_float_value,
		.end_visit_float_value = end_visit_float_value,
		.visit_string_value = visit_string_value,
		.end_visit_string_value = end_visit_string_value,
		.visit_boolean_value = visit_boolean_value,
		.end_visit_boolean_value = end_visit_boolean_value,
		.visit_null_value = visit_null_value,
		.end_visit_null_value = end_visit_null_value,
		.visit_enum_value = visit_enum_value,
		.end_visit_enum_value = end_visit_enum_value,
		.visit_list_value = visit_list_value,
		.end_visit_list_value = end_visit_list_value,
		.visit_object_value = visit_object_value,
		.end_visit_object_value = end_visit_object_value,
		.visit_object_field = visit_object_field,
		.end_visit_object_field = end_visit_object_field,
		.visit_directive = visit_directive,
		.end_visit_directive = end_visit_directive,
		.visit_named_type = visit_common,
		.end_visit_named_type = NULL,
		.visit_list_type = visit_common,
		.end_visit_list_type = NULL,
		.visit_non_null_type = visit_common,
		.end_visit_non_null_type = NULL,
		.visit_name = visit_name,
		.end_visit_name = end_visit_name,
		.visit_schema_definition = visit_common,
		.end_visit_schema_definition = NULL,
		.visit_operation_type_definition = visit_common,
		.end_visit_operation_type_definition = NULL,
		.visit_scalar_type_definition = visit_common,
		.end_visit_scalar_type_definition = NULL,
		.visit_object_type_definition = visit_common,
		.end_visit_object_type_definition = NULL,
		.visit_field_definition = visit_field_definition,
		.end_visit_field_definition = end_visit_field_definition,
		.visit_input_value_definition = visit_input_value_definition,
		.end_visit_input_value_definition = end_visit_input_value_definition,
		.visit_interface_type_definition = visit_common,
		.end_visit_interface_type_definition = NULL,
		.visit_union_type_definition = visit_common,
		.end_visit_union_type_definition = NULL,
		.visit_enum_type_definition = visit_common,
		.end_visit_enum_type_definition = NULL,
		.visit_enum_value_definition = visit_common,
		.end_visit_enum_value_definition = NULL,
		.visit_input_object_type_definition = visit_common,
		.end_visit_input_object_type_definition = NULL,
		.visit_type_extension_definition = visit_common,
		.end_visit_type_extension_definition = NULL,
		.visit_directive_definition = visit_common,
		.end_visit_directive_definition = NULL,
};


static int
graphql_parse(struct lua_State *L)
{
	int top = lua_gettop(L);
	if (top != 1) {
		luaL_error(L, "Expected string as first argument");
	}

	if (lua_isstring(L, -1) == 0) {
		luaL_error(L, "Expected string as first argument");
	}

	const char *err = NULL;
	const char *str = lua_tostring(L, 1);
	struct GraphQLAstNode* node = graphql_parse_string(str, &err);
	if (node == NULL) {
		lua_pushnil(L);
		lua_pushstring(L, err);
		graphql_error_free(err);
		return 2;
	}

	lua_newtable(L);
	graphql_node_visit(node, &LuaAstVisitorCallbacks, L);
	graphql_node_free(node);
	return 1;
}

/* exported function */
LUA_API int
luaopen_ckit_lib(lua_State *L)
{
	/* result returned from require('ckit.lib') */
	lua_newtable(L);
	static const struct luaL_Reg meta [] = {
		{"parse", graphql_parse},
		{NULL, NULL}
	};
	luaL_register(L, NULL, meta);
	return 1;
}
/* vim: syntax=c ts=8 sts=8 sw=8 noet */
