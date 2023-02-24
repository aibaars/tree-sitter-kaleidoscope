const NUM_PRECEDENCE_LEVELS = 101;
module.exports = grammar({
    name: 'kaleidoscope',
    extras: $ => [
        $.lineComment,
        $._whiteSpace,
    ],
    externals: $ => {
        let result = Array.from({ length: NUM_PRECEDENCE_LEVELS }, (_, index) => $['_operator_' + index]);
        result.push($._operator_def)
        return result;
    },
    word: $ => $.identifier,
    supertypes: $ => [$._expression, $._primaryExpression, $._prototype, $._repl],
    rules: {
        program: $ => repeat(field('statement', $._repl)),

        lineComment: $ => /#[^\r\n]*/,
        _whiteSpace: $ => /[\t\r\n\f ]+/,

        identifier: $ => /[a-zA-Z][a-zA-Z0-9]*/,

        _operator: $ => choice(...Array.from({ length: NUM_PRECEDENCE_LEVELS }, (_, index) => $['_operator_' + index])),

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
        functionCallExpression: $ => prec(1000, seq(field('callee', $.identifier), '(', optional(seq(field('argument', $._expression), repeat(seq(',', field('argument', $._expression))))), ')')),
        varInExpression: $ => seq('var', field('initializer', $.initializer), repeat(seq(',', field('initializer', $.initializer))), 'in', field('expr', $._expression)),
        conditionalExpression: $ => seq('if', field('condition', $._expression), 'then', field('then', $._expression), 'else', field('else', $._expression)),
        forExpression: $ => seq('for', field('initializer', $.initializer), ',', field('condition', $._expression), optional(seq(',', field('update', $._expression))), 'in', field('body', $._expression)),
        unaryOpExpression: $ => prec(900, seq(field('operator', alias($._operator, $.identifier)), field('operand', $._expression))),
        variableExpression: $ => field('name', $.identifier),

        _expression: $ => choice($._primaryExpression, $.binaryOpExpression),
        binaryOpExpression: $ => {
            const operators = Array.from({ length: NUM_PRECEDENCE_LEVELS }, (_, index) => $['_operator_' + index]);
            const operatorPrecedence = operators.map((operator, index) => prec.left(10 + index, seq(field('lhs', $._expression), field('operator', alias(operator, $.identifier)), field('rhs', $._expression))));
            return choice(...operatorPrecedence);
        },

        number: $ => /(0|[1-9][0-9]*)(\.[0-9]+)?/,
        _prototype: $ => choice(
            $.functionPrototype,
            $.unaryPrototype,
            $.binaryPrototype),
        functionPrototype: $ => seq(field('name', $.identifier), '(', repeat(field('argument', $.identifier)), ')'),
        unaryPrototype: $ => seq('unary', field('name', alias($._operator, $.identifier)), '(', field('argument', $.identifier), ')'),
        binaryPrototype: $ => seq('binary', field('name', alias($._operator_def, $.identifier)), optional(field('precedence', alias($._prec_number, $.number))), '(', field('argument', $.identifier), field('argument', $.identifier), ')'),
        _prec_number: $ => /[1-9][0-9]?(\.[0-9]+)?|100(\.0+)?/,
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