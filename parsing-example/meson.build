project('parsing-example', 'c',
    version : '0.1.0',
    default_options : [
        'warning_level=3',
        'c_std=c11',
        'default_library=both'
    ]
)

aqls_parser_sources = [
    'aqls-parser/src/aqls-parser/string-view.c',
    'aqls-parser/src/aqls-parser/utils.c',
    'aqls-parser/src/aqls-parser/token/token.c',
    'aqls-parser/src/aqls-parser/token/queue.c',
    'aqls-parser/src/aqls-parser/lexer.c',
    'aqls-parser/src/aqls-parser/ast/ast.c',
    'aqls-parser/src/aqls-parser/parser.c',
]

# TODO create a public header directory and differentiate between them here
aqls_parser_impl_include_path = include_directories('aqls-parser/src')
aqls_parser_public_include_path = include_directories('aqls-parser/include')
aqls_parser_include_paths = [
    aqls_parser_impl_include_path,
    aqls_parser_public_include_path,
]

aqls_parser = library('aqls-parser', aqls_parser_sources,
    version: '0.1.0',
    soversion: '0',
    install: false,
    include_directories: aqls_parser_include_paths,
)

# Unit Tests
fixtures_path = '../aqls-parser/test/unit/fixtures'

aqls_parser_unit_test_lexer = executable(
    'aqls-parser-unit-test-lexer',
    'aqls-parser/src/aqls-parser/lexer.test.c',
    include_directories: aqls_parser_include_paths,
    link_with: [
        aqls_parser,
    ],
)
test('aqls-parser - lexer', aqls_parser_unit_test_lexer,
    args: [
        fixtures_path,
    ],
)

aqls_parser_unit_test_parser = executable(
    'aqls-parser-unit-test-parser',
    'aqls-parser/src/aqls-parser/parser.test.c',
    include_directories: aqls_parser_include_paths,
    link_with: [
        aqls_parser,
    ],
)
test('aqls-parser - parser', aqls_parser_unit_test_parser,
    args: [
        fixtures_path,
    ],
)
