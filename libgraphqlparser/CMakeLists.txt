SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fPIC")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(graphqlparser STATIC
  Ast.h
  Ast.cpp
  AstVisitor.h
  c/GraphQLAst.h
  c/GraphQLAst.cpp
  c/GraphQLAstForEachConcreteType.h
  parser.tab.cpp
  parser.tab.hpp
  location.hh
  position.hh
  stack.hh
  lexer.cpp
  lexer.h
  c/GraphQLAstNode.cpp
  c/GraphQLAstVisitor.h
  c/GraphQLAstVisitor.cpp
  c/GraphQLParser.cpp
  GraphQLParser.cpp)
