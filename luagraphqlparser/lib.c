#include <lua.h>
#include <lauxlib.h>
#include <c/GraphQLAstVisitor.h>
#include <c/GraphQLParser.h>
#include <c/GraphQLAstNode.h>
#include <c/GraphQLAst.h>

static const struct GraphQLAstVisitorCallbacks LuaAstVisitorCallbacks;

static int
visit_document(const struct GraphQLAstDocument *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	// kind: document
	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "document");
	lua_settable(L, -3);

	// definitions: [
	lua_pushliteral(L, "definitions");
	lua_newtable(L);
	return 1;
}

static void
end_visit_document(const struct GraphQLAstDocument *def, void *arg)
{
	(void)def;
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
	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "operation");
	lua_settable(L, -3);

	// operation: "query/mutation/..."
	lua_pushliteral(L, "operation");
	const char *operation = GraphQLAstOperationDefinition_get_operation(def);
	lua_pushstring(L, operation);
	lua_settable(L, -3);

	int var_def_size = GraphQLAstOperationDefinition_get_variable_definitions_size(def);
	if (var_def_size > 0) {
		lua_pushliteral(L, "variableDefinitions");
		lua_createtable(L, var_def_size, 0);
		lua_settable(L, -3);
	}

	return 1;
}

static void
end_visit_operation_definition(const struct GraphQLAstOperationDefinition *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	int directive_size = GraphQLAstOperationDefinition_get_directives_size(def);
	const struct GraphQLAstSelectionSet *sel_set = GraphQLAstOperationDefinition_get_selection_set(def);
	if (sel_set != NULL) {
		lua_pushliteral(L, "selectionSet");
		lua_insert(L, -2);
		lua_settable(L, -3 - directive_size);
	}

	if (directive_size > 0) {
		lua_createtable(L, directive_size, 0);
		lua_insert(L, - directive_size - 1);
		for (int i = 0; i < directive_size; i++) {
			lua_pushnumber(L, directive_size - i);
			lua_insert(L, -2);
			lua_settable(L, -3 - directive_size + i + 1);
		}

		lua_pushliteral(L, "directives");
		lua_insert(L, -2);
		lua_settable(L, -3);
	}

	// definitions[len + 1]: }
	lua_settable(L, -3);
}

static int
visit_selection_set(const struct GraphQLAstSelectionSet *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	// selectionSet: {
	lua_newtable(L);

	// kind: selectionSet
	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "selectionSet");
	lua_settable(L, -3);

	// selections: [
	lua_pushliteral(L, "selections");
	lua_newtable(L);

	return 1;
}

static void
end_visit_selection_set(const struct GraphQLAstSelectionSet *def, void *arg)
{
	int size = GraphQLAstSelectionSet_get_selections_size(def);

	struct lua_State *L = arg;

	for (int i = 0; i < size; i++) {
		lua_pushnumber(L, size - i);
		lua_insert(L, -2);
		lua_settable(L, -3 - size + i + 1);
	}

	// selections: ]
	lua_settable(L, -3);
}

static int
visit_field(const struct GraphQLAstField *def, void *arg)
{
	int arg_size = GraphQLAstField_get_arguments_size(def);

	struct lua_State *L = arg;

	lua_newtable(L);

	// kind: field
	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "field");
	lua_settable(L, -3);

	if (arg_size > 0) {
		// arguments: []
		lua_pushliteral(L, "arguments");
		lua_newtable(L);
		lua_settable(L, -3);
	}

	return 1;
}

static void
end_visit_field(const struct GraphQLAstField *def, void *arg)
{
	struct lua_State *L = arg;

	int directive_size = GraphQLAstField_get_directives_size(def);
	const struct GraphQLAstSelectionSet *sel_set = GraphQLAstField_get_selection_set(def);
	if (sel_set != NULL) {
		lua_pushliteral(L, "selectionSet");
		lua_insert(L, -2);
		lua_settable(L, -3 - directive_size);
	}

	if (directive_size > 0) {
		lua_createtable(L, directive_size, 0);
		lua_insert(L, - directive_size - 1);
		for (int i = 0; i < directive_size; i++) {
			lua_pushnumber(L, directive_size - i);
			lua_insert(L, -2);
			lua_settable(L, -3 - directive_size + i + 1);
		}

		lua_pushliteral(L, "directives");
		lua_insert(L, -2);
		lua_settable(L, -3);
	}

	const struct GraphQLAstName *alias_node = GraphQLAstField_get_alias(def);
	if (alias_node != NULL) {
		lua_pushliteral(L, "alias");
		lua_newtable(L);

		lua_pushliteral(L, "kind");
		lua_pushliteral(L, "alias");
		lua_settable(L, -3);

		graphql_node_visit((const struct GraphQLAstNode *)alias_node,
				&LuaAstVisitorCallbacks, L);

		lua_settable(L, -3);
	}
}

static int
visit_name(const struct GraphQLAstName *def, void *arg)
{
	struct lua_State *L = arg;

	// "name": {
	lua_pushliteral(L, "name");
	lua_newtable(L);

	// value: value
	const char *value = GraphQLAstName_get_value(def);
	lua_pushliteral(L, "value");
	lua_pushstring(L, value);
	lua_settable(L, -3);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "name");
	lua_settable(L, -3);

	return 0;
}

static void
end_visit_name(const struct GraphQLAstName *def, void *arg)
{
	(void)def;
	// "name": }
	struct lua_State *L = arg;
	lua_settable(L, -3);
}

static int
visit_variable_definition(const struct GraphQLAstVariableDefinition *def, void *arg)
{
	struct lua_State *L = arg;

	lua_getfield(L, -1, "variableDefinitions");

	// len = #variableDefinitions
	size_t len = lua_objlen(L, -1);

	// arguments[len + 1]: {
	lua_pushinteger(L, len + 1);
	lua_newtable(L);

	return 1;
}

static void
end_visit_variable_definition(const struct GraphQLAstVariableDefinition *def, void *arg)
{
	struct lua_State *L = arg;

	const struct GraphQLAstValue *def_node =  GraphQLAstVariableDefinition_get_default_value(def);
	if (def_node != NULL) {
		lua_pushliteral(L, "defaultValue");
		lua_insert(L, -2);
		lua_settable(L, -5);
	}

	lua_pushliteral(L, "type");
	lua_insert(L, -2);
	lua_settable(L, -4);

	lua_pushliteral(L, "variable");
	lua_insert(L, -2);
	lua_settable(L, -3);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "variableDefinition");
	lua_settable(L, -3);

	lua_settable(L, -3);

	lua_pop(L, 1);
}

static int
visit_field_definition()
{
	return 1;
}

static void
end_visit_field_definition() { }

static int
visit_directive(const struct GraphQLAstDirective *def, void *arg)
{
	int size = GraphQLAstDirective_get_arguments_size(def);

	struct lua_State *L = arg;
	lua_newtable(L);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "directive");
	lua_settable(L, -3);

	if (size > 0) {
		lua_pushliteral(L, "arguments");
		lua_newtable(L);
		lua_settable(L, -3);
	}

	return 1;
}

static void
end_visit_directive() { }

static int
visit_argument(const struct GraphQLAstArgument *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_getfield(L, -1, "arguments");

	// len = #arguments
	size_t len = lua_objlen(L, -1);

	// arguments[len + 1]: {
	lua_pushinteger(L, len + 1);
	lua_newtable(L);

	return 1;
}

static void
end_visit_argument(const struct GraphQLAstArgument *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	// set value
	lua_pushliteral(L, "value");
	lua_insert(L, -2);
	lua_settable(L, -3);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "argument");
	lua_settable(L, -3);

	// arguments[len + 1]: }
	lua_settable(L, -3);

	lua_pop(L, 1);
}

static int
visit_variable(const struct GraphQLAstVariable *def, void *arg)
{
	(void)def;

	struct lua_State *L = arg;
	lua_newtable(L);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "variable");
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_variable() { }

static int
visit_input_value_definition()
{
	return 1;
}

static void
end_visit_input_value_definition() { }

static int
visit_object_value(const struct GraphQLAstObjectValue *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_newtable(L);

	// value: { values: [
	lua_pushliteral(L, "values");
	lua_newtable(L);

	return 1;
}

static void
end_visit_object_value(const struct GraphQLAstObjectValue *def, void *arg)
{
	int size = GraphQLAstObjectValue_get_fields_size(def);

	struct lua_State *L = arg;

	for (int i = 0; i < size; i++) {
		lua_pushnumber(L, size - i);
		lua_insert(L, -2);
		lua_settable(L, -3 - size + i + 1);
	}

	lua_settable(L, -3);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "inputObject");
	lua_settable(L, -3);
}

static int
visit_object_field(const struct GraphQLAstObjectField *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_newtable(L);

	return 1;
}

static void
end_visit_object_field(const struct GraphQLAstObjectField *def, void *arg)
{
	struct lua_State *L = arg;
	lua_pushliteral(L, "value");
	lua_insert(L, -2);
	lua_settable(L, -3);

	const struct GraphQLAstName *name_node = GraphQLAstObjectField_get_name(def);
	const char *name = GraphQLAstName_get_value(name_node);

	lua_pushliteral(L, "name");
	lua_pushstring(L, name);
	lua_settable(L, -3);
}

#define VISIT_SINGLE_VALUE(type, snake_type) 										\
static int																			\
visit_##type##_value(const struct GraphQLAst##snake_type##Value *def, void *arg) 	\
{																					\
	struct lua_State *L = arg;														\
	lua_newtable(L);																\
																					\
	const char *value = GraphQLAst##snake_type##Value_get_value(def);				\
																					\
	lua_pushliteral(L, "value");													\
	lua_pushstring(L, value);														\
	lua_settable(L, -3);															\
																					\
	lua_pushliteral(L, "kind");														\
	lua_pushliteral(L, #type);														\
	lua_settable(L, -3);															\
																					\
	return 1;																		\
}

#define END_VISIT_SINGLE_VALUE(type, snake_type) 									\
static void																			\
end_visit_##type##_value() { }

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
	struct lua_State *L = arg;

	lua_newtable(L);

	int value = GraphQLAstBooleanValue_get_value(def);
	lua_pushliteral(L, "value");
	lua_pushstring(L, value ? "true" : "false");
	lua_settable(L, -3);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "boolean");
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_boolean_value() { }

static int
visit_null_value(const struct GraphQLAstNullValue *def, void *arg)
{
	(void)def;

	struct lua_State *L = arg;

	lua_newtable(L);

	lua_pushliteral(L, "value");
	lua_pushliteral(L, "null");
	lua_settable(L, -3);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "null");
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_null_value() { }

static int
visit_list_value(const struct GraphQLAstListValue *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_newtable(L);

	// value: { values: [
	lua_pushliteral(L, "values");
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

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "list");
	lua_settable(L, -3);
}

static int
visit_named_type(const struct GraphQLAstNamedType *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_newtable(L);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "namedType");
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_named_type() { }

static int
visit_non_null_type(const struct GraphQLAstNonNullType *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_newtable(L);

	return 1;
}

static void
end_visit_non_null_type(const struct GraphQLAstNonNullType *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_pushliteral(L, "type");
	lua_insert(L, -2);
	lua_settable(L, -3);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "nonNullType");
	lua_settable(L, -3);
}

static int
visit_list_type(const struct GraphQLAstListType *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_newtable(L);

	return 1;
}

static void
end_visit_list_type(const struct GraphQLAstListType *def, void *arg)
{
	struct lua_State *L = arg;

	lua_pushliteral(L, "type");
	lua_insert(L, -2);
	lua_settable(L, -3);

	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "listType");
	lua_settable(L, -3);
}

static int
visit_fragment_definition(const struct GraphQLAstFragmentDefinition *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	// len = #definitions
	size_t len = lua_objlen(L, -1);

	// definitions[len + 1]: {
	lua_pushinteger(L, len + 1);
	lua_newtable(L);

	// kind: fragmentDefinition
	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "fragmentDefinition");
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_fragment_definition(const struct GraphQLAstFragmentDefinition *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_pushliteral(L, "selectionSet");
	lua_insert(L, -2);
	lua_settable(L, -4);

	lua_pushliteral(L, "typeCondition");
	lua_insert(L, -2);
	lua_settable(L, -3);

	lua_settable(L, -3);
}

static int
visit_fragment_spread(const struct GraphQLAstFragmentSpread *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_newtable(L);

	// kind: fragmentSpread
	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "fragmentSpread");
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_fragment_spread(const struct GraphQLAstFragmentSpread *def, void *arg)
{
	int directive_size = GraphQLAstFragmentSpread_get_directives_size(def);
	struct lua_State *L = arg;

	if (directive_size > 0) {
		lua_createtable(L, directive_size, 0);
		lua_insert(L, - directive_size - 1);
		for (int i = 0; i < directive_size; i++) {
			lua_pushnumber(L, directive_size - i);
			lua_insert(L, -2);
			lua_settable(L, -3 - directive_size + i + 1);
		}

		lua_pushliteral(L, "directives");
		lua_insert(L, -2);
		lua_settable(L, -3);
	}
}

static int
visit_inline_fragment(const struct GraphQLAstInlineFragment *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	lua_newtable(L);

	// kind: inlineFragment
	lua_pushliteral(L, "kind");
	lua_pushliteral(L, "inlineFragment");
	lua_settable(L, -3);

	return 1;
}

static void
end_visit_inline_fragment(const struct GraphQLAstInlineFragment *def, void *arg)
{
	(void)def;
	struct lua_State *L = arg;

	int directive_size = GraphQLAstInlineFragment_get_directives_size(def);

	const struct GraphQLAstSelectionSet *sel_set = GraphQLAstInlineFragment_get_selection_set(def);
	const struct GraphQLAstNamedType *named_type = GraphQLAstInlineFragment_get_type_condition(def);

	int named_typed_offset = 0;
	if (named_type != NULL) {
		named_typed_offset += 1;
	}

	if (sel_set != NULL) {
		lua_pushliteral(L, "selectionSet");
		lua_insert(L, -2);
		lua_settable(L, -3 - named_typed_offset - directive_size);
	}

	if (directive_size > 0) {
		lua_createtable(L, directive_size, 0);
		lua_insert(L, - directive_size - 1);
		for (int i = 0; i < directive_size; i++) {
			lua_pushnumber(L, directive_size - i);
			lua_insert(L, -2);
			lua_settable(L, -3 - directive_size + i + 1);
		}

		lua_pushliteral(L, "directives");
		lua_insert(L, -2);
		lua_settable(L, -3 - named_typed_offset);
	}

	if (named_type != NULL) {
		lua_pushliteral(L, "typeCondition");
		lua_insert(L, -2);
		lua_settable(L, -3);
	}
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
		.visit_fragment_spread = visit_fragment_spread,
		.end_visit_fragment_spread = end_visit_fragment_spread,
		.visit_inline_fragment = visit_inline_fragment,
		.end_visit_inline_fragment = end_visit_inline_fragment,
		.visit_fragment_definition = visit_fragment_definition,
		.end_visit_fragment_definition = end_visit_fragment_definition,
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
		.visit_named_type = visit_named_type,
		.end_visit_named_type = end_visit_named_type,
		.visit_list_type = visit_list_type,
		.end_visit_list_type = end_visit_list_type,
		.visit_non_null_type = visit_non_null_type,
		.end_visit_non_null_type = end_visit_non_null_type,
		.visit_name = visit_name,
		.end_visit_name = end_visit_name,
		.visit_schema_definition = NULL,
		.end_visit_schema_definition = NULL,
		.visit_operation_type_definition = NULL,
		.end_visit_operation_type_definition = NULL,
		.visit_scalar_type_definition = NULL,
		.end_visit_scalar_type_definition = NULL,
		.visit_object_type_definition = NULL,
		.end_visit_object_type_definition = NULL,
		.visit_field_definition = visit_field_definition,
		.end_visit_field_definition = end_visit_field_definition,
		.visit_input_value_definition = visit_input_value_definition,
		.end_visit_input_value_definition = end_visit_input_value_definition,
		.visit_interface_type_definition = NULL,
		.end_visit_interface_type_definition = NULL,
		.visit_union_type_definition = NULL,
		.end_visit_union_type_definition = NULL,
		.visit_enum_type_definition = NULL,
		.end_visit_enum_type_definition = NULL,
		.visit_enum_value_definition = NULL,
		.end_visit_enum_value_definition = NULL,
		.visit_input_object_type_definition = NULL,
		.end_visit_input_object_type_definition = NULL,
		.visit_type_extension_definition = NULL,
		.end_visit_type_extension_definition = NULL,
		.visit_directive_definition = NULL,
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

LUA_API int
luaopen_luagraphqlparser_lib(lua_State *L)
{
	/* result returned from require('luagraphqlparser.lib') */
	lua_newtable(L);
	static const struct luaL_Reg meta [] = {
		{"parse", graphql_parse},
		{NULL, NULL}
	};
	luaL_register(L, NULL, meta);
	return 1;
}
/* vim: syntax=c ts=8 sts=8 sw=8 noet */
