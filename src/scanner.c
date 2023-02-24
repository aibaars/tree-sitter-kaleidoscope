#include "tree_sitter/parser.h"
#include <wctype.h>

enum TokenType {
  OPERATOR_PRECEDENCE_0,
  OPERATOR_PRECEDENCE_1,
  OPERATOR_PRECEDENCE_2,
  OPERATOR_PRECEDENCE_3,
  OPERATOR_PRECEDENCE_4, 
  OPERATOR_PRECEDENCE_5,
  OPERATOR_PRECEDENCE_6, 
  OPERATOR_PRECEDENCE_7,
  OPERATOR_PRECEDENCE_8,
  OPERATOR_PRECEDENCE_9,
  OPERATOR_PRECEDENCE_10,
  OPERATOR_PRECEDENCE_11,
  OPERATOR_PRECEDENCE_12,
  OPERATOR_PRECEDENCE_13,
  OPERATOR_PRECEDENCE_14, 
  OPERATOR_PRECEDENCE_15,
  OPERATOR_PRECEDENCE_16, 
  OPERATOR_PRECEDENCE_17,
  OPERATOR_PRECEDENCE_18,
  OPERATOR_PRECEDENCE_19,
  OPERATOR_PRECEDENCE_20,
  OPERATOR_PRECEDENCE_21,
  OPERATOR_PRECEDENCE_22,
  OPERATOR_PRECEDENCE_23,
  OPERATOR_PRECEDENCE_24, 
  OPERATOR_PRECEDENCE_25,
  OPERATOR_PRECEDENCE_26, 
  OPERATOR_PRECEDENCE_27,
  OPERATOR_PRECEDENCE_28,
  OPERATOR_PRECEDENCE_29,
  OPERATOR_PRECEDENCE_30,
  OPERATOR_PRECEDENCE_31,
  OPERATOR_PRECEDENCE_32,
  OPERATOR_PRECEDENCE_33,
  OPERATOR_PRECEDENCE_34, 
  OPERATOR_PRECEDENCE_35,
  OPERATOR_PRECEDENCE_36, 
  OPERATOR_PRECEDENCE_37,
  OPERATOR_PRECEDENCE_38,
  OPERATOR_PRECEDENCE_39,
  OPERATOR_PRECEDENCE_40,
  OPERATOR_PRECEDENCE_41,
  OPERATOR_PRECEDENCE_42,
  OPERATOR_PRECEDENCE_43,
  OPERATOR_PRECEDENCE_44, 
  OPERATOR_PRECEDENCE_45,
  OPERATOR_PRECEDENCE_46, 
  OPERATOR_PRECEDENCE_47,
  OPERATOR_PRECEDENCE_48,
  OPERATOR_PRECEDENCE_49,
  OPERATOR_PRECEDENCE_50,
  OPERATOR_PRECEDENCE_51,
  OPERATOR_PRECEDENCE_52,
  OPERATOR_PRECEDENCE_53,
  OPERATOR_PRECEDENCE_54, 
  OPERATOR_PRECEDENCE_55,
  OPERATOR_PRECEDENCE_56, 
  OPERATOR_PRECEDENCE_57,
  OPERATOR_PRECEDENCE_58,
  OPERATOR_PRECEDENCE_59,
  OPERATOR_PRECEDENCE_60,
  OPERATOR_PRECEDENCE_61,
  OPERATOR_PRECEDENCE_62,
  OPERATOR_PRECEDENCE_63,
  OPERATOR_PRECEDENCE_64, 
  OPERATOR_PRECEDENCE_65,
  OPERATOR_PRECEDENCE_66, 
  OPERATOR_PRECEDENCE_67,
  OPERATOR_PRECEDENCE_68,
  OPERATOR_PRECEDENCE_69,
  OPERATOR_PRECEDENCE_70,
  OPERATOR_PRECEDENCE_71,
  OPERATOR_PRECEDENCE_72,
  OPERATOR_PRECEDENCE_73,
  OPERATOR_PRECEDENCE_74, 
  OPERATOR_PRECEDENCE_75,
  OPERATOR_PRECEDENCE_76, 
  OPERATOR_PRECEDENCE_77,
  OPERATOR_PRECEDENCE_78,
  OPERATOR_PRECEDENCE_79,
  OPERATOR_PRECEDENCE_80,
  OPERATOR_PRECEDENCE_81,
  OPERATOR_PRECEDENCE_82,
  OPERATOR_PRECEDENCE_83,
  OPERATOR_PRECEDENCE_84, 
  OPERATOR_PRECEDENCE_85,
  OPERATOR_PRECEDENCE_86, 
  OPERATOR_PRECEDENCE_87,
  OPERATOR_PRECEDENCE_88,
  OPERATOR_PRECEDENCE_89,
  OPERATOR_PRECEDENCE_90,
  OPERATOR_PRECEDENCE_91,
  OPERATOR_PRECEDENCE_92,
  OPERATOR_PRECEDENCE_93,
  OPERATOR_PRECEDENCE_94, 
  OPERATOR_PRECEDENCE_95,
  OPERATOR_PRECEDENCE_96, 
  OPERATOR_PRECEDENCE_97,
  OPERATOR_PRECEDENCE_98,
  OPERATOR_PRECEDENCE_99,
  OPERATOR_PRECEDENCE_100,
  OPERATOR_PRECEDENCE_DEF,
};
const int NUM_OPERATORS = 128;

struct Scanner {
    // char -> TokenType
    int BinopPrecedence[NUM_OPERATORS];
};

bool isOperator(int32_t character) {
   return character > 0 && character < NUM_OPERATORS && iswprint(character) && !iswspace(character) && !isalnum(character) &&
    character != '(' && character != ')' && character != ',' && character != ';' && character != '#';
}

bool tree_sitter_kaleidoscope_external_scanner_scan(
  void *payload,
  TSLexer *lexer,
  const bool *valid_symbols
) {
  struct Scanner *scanner = (struct Scanner*)payload;
  if (isOperator(lexer->lookahead)) {
    int32_t operator = lexer->lookahead;
    if (valid_symbols[OPERATOR_PRECEDENCE_DEF]) {
      lexer->result_symbol = OPERATOR_PRECEDENCE_DEF;
      lexer->advance(lexer, false);
      lexer->mark_end(lexer);
      while(!lexer->eof(lexer)) {
        if (iswspace(lexer->lookahead)) {
            lexer->advance(lexer, false);
        } else if (lexer->lookahead == '#') {
            lexer->advance(lexer, false);
            while(!lexer->eof(lexer) && lexer->lookahead != '\n' && lexer->lookahead != '\r') {
                lexer->advance(lexer, false);
            }
        } else {
            break;
        }
      }
      int precedence = iswdigit(lexer->lookahead) ? 0 : OPERATOR_PRECEDENCE_30;
      for(int i = 0; i < 3 && iswdigit(lexer->lookahead); i++) {
          precedence = precedence * 10 + (lexer->lookahead - '0');
          lexer->advance(lexer, false);
      }
      if (precedence >= OPERATOR_PRECEDENCE_1 && precedence <= OPERATOR_PRECEDENCE_100) {
          scanner->BinopPrecedence[operator] = precedence;
      }
      return true;
    }

    enum TokenType token_type = scanner->BinopPrecedence[operator];
    if (valid_symbols[token_type]) {
      lexer->result_symbol = token_type;
      lexer->advance(lexer, false);
      return true;
    }
  }
  return false;
}

void tree_sitter_kaleidoscope_external_scanner_init(struct Scanner *scanner) {
  for (int i = 0; i < NUM_OPERATORS; i++) {
    scanner->BinopPrecedence[i] = OPERATOR_PRECEDENCE_0;
  }
  scanner->BinopPrecedence['='] = OPERATOR_PRECEDENCE_2;
  scanner->BinopPrecedence['<'] = OPERATOR_PRECEDENCE_10;
  scanner->BinopPrecedence['+'] = OPERATOR_PRECEDENCE_20;
  scanner->BinopPrecedence['-'] = OPERATOR_PRECEDENCE_20;
  scanner->BinopPrecedence['*'] = OPERATOR_PRECEDENCE_40;
}

void * tree_sitter_kaleidoscope_external_scanner_create() {
      struct Scanner *scanner = malloc(sizeof(struct Scanner));
      tree_sitter_kaleidoscope_external_scanner_init(scanner);
      return scanner;
}

void tree_sitter_kaleidoscope_external_scanner_destroy(void *payload) {
  free(payload);
}

unsigned tree_sitter_kaleidoscope_external_scanner_serialize(
  void *payload,
  char *buffer
) {
    struct Scanner *scanner = (struct Scanner*)payload;
    enum TokenType* data = (enum TokenType*)buffer;
    for(int i = 0; i < NUM_OPERATORS; i++) {
        data[i] = scanner->BinopPrecedence[i];
    }
    return NUM_OPERATORS * sizeof(enum TokenType);
}

void tree_sitter_kaleidoscope_external_scanner_deserialize(
  void *payload,
  const char *buffer,
  unsigned length
) {
    struct Scanner *scanner = (struct Scanner*)payload;
    if(length != NUM_OPERATORS * sizeof(enum TokenType)) {
        tree_sitter_kaleidoscope_external_scanner_init(scanner);
        return;
    }
    enum TokenType* data = (enum TokenType*)buffer;
    for(int i = 0; i < NUM_OPERATORS; i++) {
        scanner->BinopPrecedence[i] = data[i];
    }
}
