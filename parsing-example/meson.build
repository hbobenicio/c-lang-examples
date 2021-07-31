project('parsing-example', 'c',
    version : '0.1.0',
    default_options : [
        'warning_level=3',
        'c_std=c11',
        'default_library=both'
    ]
)

natural_parser_sources = [
    'natural-parser/src/natural-parser/string-view.c',
    'natural-parser/src/natural-parser/utils.c',
    'natural-parser/src/natural-parser/token/token.c',
    'natural-parser/src/natural-parser/token/queue.c',
    'natural-parser/src/natural-parser/lexer.c',
    'natural-parser/src/natural-parser/ast/ast.c',
    'natural-parser/src/natural-parser/parser.c',
]

# TODO create a public header directory and differentiate between them here
natural_parser_impl_include_path = include_directories('natural-parser/src')
natural_parser_public_include_path = include_directories('natural-parser/include')
natural_parser_include_paths = [
    natural_parser_impl_include_path,
    natural_parser_public_include_path,
]

natural_parser = library('natural-parser', natural_parser_sources,
    version: '0.1.0',
    soversion: '0',
    install: false,
    include_directories: natural_parser_include_paths,
)

# Unit Tests
fixtures_path = '../natural-parser/test/unit/fixtures'

natural_parser_unit_test_lexer = executable(
    'natural-parser-unit-test-lexer',
    'natural-parser/src/natural-parser/lexer.test.c',
    include_directories: natural_parser_include_paths,
    link_with: [
        natural_parser,
    ],
)
test('natural-parser - lexer', natural_parser_unit_test_lexer,
    args: [
        fixtures_path,
    ],
)

natural_parser_unit_test_parser = executable(
    'natural-parser-unit-test-parser',
    'natural-parser/src/natural-parser/parser.test.c',
    include_directories: natural_parser_include_paths,
    link_with: [
        natural_parser,
    ],
)
test('natural-parser - parser', natural_parser_unit_test_parser,
    args: [
        fixtures_path,
    ],
)
