# IPP 2025, FIT VUT Brno, Projekt 1
# Autor: Michal Pavlíček xpavlim00

from enum import Enum, auto
import sys

# Chyby
ERRORS = { 
    "LEXICAL_ERR": 21,
    "SYNTAX_ERR": 22,
    "SEM_MAIN_ERR": 31,
    "SEM_UNDEFINED_ERR": 32,
    "SEM_ARITY_ERR": 33,
    "SEM_COLISION_ERR": 34,
    "SEM_OTHER_ERR": 35
}

keywords = ["class", "self", "super", "nil", "true", "false"]
class_keywords = ["Object", "Nil", "True", "False", "Integer", "String", "Block"]

class TokenType(Enum):
    IDENTIFIER = auto()
    KEYWORD = auto()
    CLASS_NAME = auto()
    BOOLEAN = auto()
    INTEGER = auto()
    STRING = auto()
    BLOCK = auto()
    NIL = auto()
    MESSAGE = auto()
    UNKNOWN = auto()
    BRACKET_LEFT = auto() # [
    BRACKET_RIGHT = auto() # ]
    EQUALS = auto()
    COLON = auto()
    PIPE = auto()
    BRACE_LEFT = auto() # {
    BRACE_RIGHT = auto() # }
    DOT = auto()
    PARENTHESIS_LEFT = auto() # (
    PARENTHESIS_RIGHT = auto() # )
    TOKEN_EOF = auto()


class Token:
    def __init__(self, type: TokenType, lexeme: str) -> None:
        self.type = type
        self.lexeme = lexeme

    def __str__(self):
        return f"Token({self.type.name}, {self.lexeme})"

    
    def __repr__(self):
        return self.__str__()


buffer = []
def get_char() -> str:
    chr = buffer.pop(0) if len(buffer) > 0 else sys.stdin.read(1)
    return chr


def get_next_token() -> Token:
    curr_ch = get_char()
    token = Token(TokenType.UNKNOWN, "")

    # INTEGER
    if curr_ch.isdigit() or curr_ch in ("+", "-"):
        token.lexeme = curr_ch
        token.type = TokenType.INTEGER

        while (curr_ch := get_char()).isdigit():
            token.lexeme += curr_ch

        buffer.append(curr_ch)

    # IDENTIFIER
    elif curr_ch.isalpha() or curr_ch == "_":
        token.lexeme += curr_ch
        while (curr_ch := get_char()).isalnum() or curr_ch == "_":
            token.lexeme += curr_ch
        buffer.append(curr_ch)

        if token.lexeme in keywords:
            token.type = TokenType.KEYWORD
        elif token.lexeme in class_keywords:
            token.type = TokenType.CLASS_NAME
        else:
            token.type = TokenType.IDENTIFIER

    # COMMENT
    elif curr_ch == '"':
        while (curr_ch := get_char()) != '"':
            pass
        return get_next_token()

    # STRING
    elif curr_ch == "'":
        token.type = TokenType.STRING
        # TODO: Handle escape sequences??
        while (curr_ch := get_char()) != "'":
            token.lexeme += curr_ch

    # SYMBOLS
    elif curr_ch == ":":
        curr_ch = get_char()
        if curr_ch == "=":
            token.type = TokenType.EQUALS
            token.lexeme = ":="
        else:
            token.type = TokenType.COLON
            token.lexeme = ":"
            buffer.append(curr_ch)
    elif curr_ch == "[":
        token.type = TokenType.BRACKET_LEFT
        token.lexeme = "["
    elif curr_ch == "]":
        token.type = TokenType.BRACKET_RIGHT
        token.lexeme = "]"
    elif curr_ch == "|":
        token.type = TokenType.PIPE
        token.lexeme = "|"
    elif curr_ch == "{":
        token.type = TokenType.BRACE_LEFT
        token.lexeme = "{"
    elif curr_ch == "}":
        token.type = TokenType.BRACE_RIGHT
        token.lexeme = "}"
    elif curr_ch == ".":
        token.type = TokenType.DOT
        token.lexeme = "."
    elif curr_ch == "(":
        token.type = TokenType.PARENTHESIS_LEFT;
        token.lexeme = "("
    elif curr_ch == ")":
        token.type = TokenType.PARENTHESIS_RIGHT;
        token.lexeme = ")"

    # EOF
    elif not curr_ch:
        token.type = TokenType.TOKEN_EOF;

    # space
    elif curr_ch == " " or curr_ch == "\n":
        return get_next_token()

    return token


# while (token := get_next_token()).type != TokenType.TOKEN_EOF:
#     print(token)

def throw_error(ret_code):
    print(f"Exiting with error {ret_code}")
    sys.exit(ret_code)


class Parser:
    def __init__(self):
        self.current_token = get_next_token()


    def consume(self, expected_type: TokenType, expected_lex: str = ""):
        if self.current_token.type == expected_type:
            if len(expected_lex) == 0 or expected_lex == self.current_token.lexeme:
                consumed = self.current_token
                self.current_token = get_next_token()
                return consumed

        print(self.current_token)
        throw_error(ERRORS["SYNTAX_ERR"])


    def program(self):
        ast = {"type": "Program", "classes": []}
        while self.current_token.type == TokenType.KEYWORD:
            ast["classes"].append(self.class_())

        print(ast)


    def class_(self):
        self.consume(TokenType.KEYWORD, "class")
        class_name = self.consume(TokenType.IDENTIFIER)
        self.consume(TokenType.COLON)
        parent = self.consume(TokenType.CLASS_NAME)
        self.consume(TokenType.BRACE_LEFT)
        method = self.method()
        self.consume(TokenType.BRACE_RIGHT)
        
        return {
            "type": "class",
            "class_name": class_name,
            "parent": parent,
            "method": method
        }


    def method(self):
        print("here")
        selectors = self.selector()
        blocks = self.block()
        # TODO: Can be another method
        return {
            "type": "method",
            "selectors": selectors,
            "blocks": blocks
        }


    def selector(self):
        selectors = []
        print(self.current_token)
        while self.current_token.type == TokenType.IDENTIFIER:
            selectors.append(self.consume(TokenType.IDENTIFIER))

        return selectors


    def block(self):
        self.consume(TokenType.BRACKET_LEFT)
        parameters = self.block_par()
        self.consume(TokenType.PIPE)
        statements = self.block_stat()
        self.consume(TokenType.BRACKET_RIGHT)

        return {
            "type": "block",
            "parameters": parameters,
            "statements": statements
        }


    def block_par(self):
        parameters = []
        while self.current_token.type == TokenType.COLON:
            self.consume(TokenType.COLON)
            parameters.append(self.consume(TokenType.IDENTIFIER))

        return parameters


    def block_stat(self):
        statement_name = self.consume(TokenType.IDENTIFIER)
        self.consume(TokenType.EQUALS)
        expression = self.expression()
        self.consume(TokenType.DOT)
        # TODO: Can be more statements

        return {
            "type": "statements",
            "expressions": expression
        }

    def expression(self):
        # TODO: This is only placeholder
        self.consume(TokenType.INTEGER)

        return {
            "type": "1"
        }


parser = Parser()
parser.program()












