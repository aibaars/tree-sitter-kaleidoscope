const OPERATORS = ['=', '*', '+', '-', '/', '<', '^', '!', '%', '&', '.', ':', '>', '?',
    '@', '\\', '_', '|', '==', '!=', '++', '--']
module.exports = grammar({
    name: 'kaleidoscope',
    extras: $ => [
        $.lineComment,
        $._whiteSpace,
    ],
    word: $ => $.identifier,
    supertypes: $ => [$._expression, $._primaryExpression, $._prototype, $._repl],
    rules: {
        program: $ => repeat(field('statement', $._repl)),

        lineComment: $ => /#[^\r\n]*/,
        _whiteSpace: $ => /[\t\r\n\f ]+/,

        identifier: $ => /[a-zA-Z][a-zA-Z0-9]*/,

        _operator: $ => choice(...OPERATORS),

        initializer: $ => seq(
            field('variable', $.identifier), optional(seq('=', field('expr', $._expression)))),

        _primaryExpression: $ => choice(
            $.parenExpression,
            $.functionCallExpression,
            $.varInExpression,
            $.conditionalExpression,
            $.forExpression,
            $.unaryOpExpression,
            $.variableExpression,
            $.number),
        parenExpression: $ => seq('(', field('expr', $._expression), ')'),
        functionCallExpression: $ => prec(100, seq(field('callee', $.identifier), '(', optional(seq(field('argument', $._expression), repeat(seq(',', field('argument', $._expression))))), ')')),
        varInExpression: $ => seq('var', field('initializer', $.initializer), repeat(seq(',', field('initializer', $.initializer))), 'in', field('expr', $._expression)),
        conditionalExpression: $ => seq('if', field('condition', $._expression), 'then', field('then', $._expression), 'else', field('else', $._expression)),
        forExpression: $ => seq('for', field('initializer', $.initializer), ',', field('condition', $._expression), optional(seq(',', field('update', $._expression))), 'in', field('body', $._expression)),
        unaryOpExpression: $ => prec(90, seq(field('operator', alias($._operator, $.identifier)), field('operand', $._expression))),
        variableExpression: $ => field('name', $.identifier),

        _expression: $ => choice($._primaryExpression, $.binaryOpExpression),
        // TODO: handle (user defined) precedence
        binaryOpExpression: $ => prec.left(5, seq(field('lhs', $._expression), field('operator', alias($._operator, $.identifier)), field('rhs', $._expression))),

        number: $ => /(0|[1-9][0-9]*)(\.[0-9]+)?/,
        _prototype: $ => choice(
            $.functionPrototype,
            $.unaryPrototype,
            $.binaryPrototype),
        functionPrototype: $ => seq(field('name', $.identifier), '(', repeat(field('argument', $.identifier)), ')'),
        unaryPrototype: $ => seq('unary', field('name', alias($._operator, $.identifier)), '(', field('argument', $.identifier), ')'),
        binaryPrototype: $ => seq('binary', field('name', alias($._operator, $.identifier)), optional(field('precedence', $.number)), '(', field('argument', $.identifier), field('argument', $.identifier), ')'),

        _repl: $ => choice(
            $.functionDefinition,
            $.externalDeclaration,
            $._topLevelExpression,
            $._topLevelSemiColon),
        functionDefinition: $ => seq('def', field('prototype', $._prototype), field('body', $._expression)),
        externalDeclaration: $ => seq('extern', field('prototype', $._prototype)),
        _topLevelExpression: $ => prec(-10, $._expression),
        _topLevelSemiColon: $ => ';',
    }
});