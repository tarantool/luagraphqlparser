### Luagraphqlparser

Lua-binding of [libgraphqlparser](https://github.com/graphql/libgraphqlparser)
compatible with [graphql-lua](https://github.com/bjornbytes/graphql-lua).

Differences from graphql-lua parser:
  * Improved error messages
  * Null support
  
Usage:
```lua
parse = require('luagraphqlparser').parse
parse([[
    query HeroComparison($first: Int = 3) {
      leftComparison: hero(episode: EMPIRE) {
        ...comparisonFields
      }
      rightComparison: hero(episode: JEDI) {
        ...comparisonFields
      }
    }
    
    fragment comparisonFields on Character {
      name
      friendsConnection(first: $first) {
        totalCount
        edges {
          node {
            name
          }
        }
      }
    }
]])
---
- definitions:
  - operation: query
    kind: operation
    variableDefinitions:
    - type:
        name:
          kind: name
          value: Int
        kind: namedType
      kind: variableDefinition
      variable:
        name:
          kind: name
          value: first
        kind: variable
      defaultValue:
        kind: int
        value: '3'
    name:
      kind: name
      value: HeroComparison
    selectionSet:
      selections:
      - selectionSet:
          selections:
          - name:
              kind: name
              value: comparisonFields
            kind: fragmentSpread
          kind: selectionSet
        kind: field
        alias:
          name:
            kind: name
            value: leftComparison
          kind: alias
        name:
          kind: name
          value: hero
        arguments:
        - kind: argument
          name:
            kind: name
            value: episode
          value:
            kind: enum
            value: EMPIRE
      - selectionSet:
          selections:
          - name:
              kind: name
              value: comparisonFields
            kind: fragmentSpread
          kind: selectionSet
        kind: field
        alias:
          name:
            kind: name
            value: rightComparison
          kind: alias
        name:
          kind: name
          value: hero
        arguments:
        - kind: argument
          name:
            kind: name
            value: episode
          value:
            kind: enum
            value: JEDI
      kind: selectionSet
  - typeCondition:
      name:
        kind: name
        value: Character
      kind: namedType
    selectionSet:
      selections:
      - name:
          kind: name
          value: name
        kind: field
      - selectionSet:
          selections:
          - name:
              kind: name
              value: totalCount
            kind: field
          - selectionSet:
              selections:
              - selectionSet:
                  selections:
                  - name:
                      kind: name
                      value: name
                    kind: field
                  kind: selectionSet
                name:
                  kind: name
                  value: node
                kind: field
              kind: selectionSet
            name:
              kind: name
              value: edges
            kind: field
          kind: selectionSet
        arguments:
        - kind: argument
          name:
            kind: name
            value: first
          value:
            name:
              kind: name
              value: first
            kind: variable
        name:
          kind: name
          value: friendsConnection
        kind: field
      kind: selectionSet
    name:
      kind: name
      value: comparisonFields
    kind: fragmentDefinition
  kind: document
...
```
