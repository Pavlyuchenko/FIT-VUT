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

print(ERRORS["LEXICAL_ERR"])

keywords = ["class", "self", "super", "nil", "true", "false"]
class_keywords = ["Object", "Nil", "True", "False", "Integer", "String", "Block"]

class TokenType(Enum):
    IDENTIFIER = auto()
    KEYWORD = auto()
    CLASS_KEYWORD = auto()
    BOOLEAN = auto()
    INTEGER = auto()
    STRING = auto()
    BLOCK = auto()
    NIL = auto()
    MESSAGE = auto()
    UNKNOWN = auto()
    BRACKET_LEFT = auto()
    BRACKET_RIGHT = auto()
    EQUALS = auto()
    COLON = auto()
    PIPE = auto()
    BRACE_LEFT = auto() # {
    BRACE_RIGHT = auto() # }
    DOT = auto()
    PARENTHESIS_LEFT = auto()
    PARENTHESIS_RIGHT = auto()


class Token:
    def __init__(self, type: TokenType, lexeme: str) -> None:
        self.type = type
        self.lexeme = lexeme

    def __str__(self):
        return f"Token({self.type.name}, {self.lexeme})"

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
            token.type = TokenType.CLASS_KEYWORD
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
        return None

    # space
    elif curr_ch == " " or curr_ch == "\n":
        return get_next_token()

    return token


while True:
    token = get_next_token()
    if (token == None):
        print("EOF")
        break
    print(token)
