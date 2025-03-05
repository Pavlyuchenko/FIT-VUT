# IPP 2025, FIT VUT Brno, Projekt 1
# Autor: Michal Pavlíček xpavlim00

from enum import Enum, auto
import sys
import json
import xml.etree.ElementTree as ET

# Chyby
ERRORS = { 
    "PARAMETER_ERR": 10,
    "FILE_OPEN_ERR": 11,
    "FILE_WRITE_ERR": 12,
    "LEXICAL_ERR": 21,
    "SYNTAX_ERR": 22,
    "SEM_MAIN_ERR": 31,
    "SEM_UNDEFINED_ERR": 32,
    "SEM_ARITY_ERR": 33,
    "SEM_COLISION_ERR": 34,
    "SEM_OTHER_ERR": 35
}

def throw_error(ret_code):
    print(f"Exiting with error {ret_code}")
    sys.exit(ret_code)


keywords = ["class", "nil", "true", "false"]
class_keywords = ["Object", "Nil", "True", "False", "Integer", "String", "Block"]

class TokenType(Enum):
    IDENTIFIER = auto()
    KEYWORD = auto()
    CLASS_NAME = auto()
    INTEGER = auto()
    STRING = auto()
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


    def change_type(self, type: TokenType):
        self.type = type


    def append_lexeme(self, chr: str):
        self.lexeme += chr


    def __str__(self):
        return f"Token({self.type.name}, {self.lexeme})"
    
    def __repr__(self):
        return self.__str__()


# reads char from stdin in buffer is empty,
# else return first char in buffer
# (sometimes lookahead is requied, but the char is not used yet)
buffer = []
def get_char() -> str:
    chr = buffer.pop(0) if len(buffer) > 0 else sys.stdin.read(1)
    return chr


# tokenizer, lexical analysis
# reads stdin character by characted
# and based on a finite automaton it returns either
#   Token or throws an error to the stderr
def get_next_token() -> Token:
    curr_ch = get_char()
    token = Token(TokenType.UNKNOWN, "")

    # handles INTEGER input
    if curr_ch.isdigit() or curr_ch in ("+", "-"):
        token.lexeme = curr_ch
        token.type = TokenType.INTEGER

        while (curr_ch := get_char()).isdigit():
            token.lexeme += curr_ch

        buffer.append(curr_ch)

    # handles IDENTIFIER input
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
    elif curr_ch in (" ", "\t", "\n"):
        return get_next_token()

    return token


class StrEncoder(json.JSONEncoder):
    def default(self, obj):
        return str(obj)


class AST_Node:
    def accept(self, visitor, parent=None):
        raise NotImplementedError("This method should be implemented")


class Program(AST_Node):
    def __init__(self, classes):
        self.classes = classes

    def accept(self, visitor, parent=None):
        return visitor.visit_program(self, parent)

    def __str__(self):
        return f"Program({self.classes})"


class Class(AST_Node):
    def __init__(self, name, parent, methods):
        self.name = name
        self.parent = parent
        self.methods = methods

    def accept(self, visitor, parent=None):
        return visitor.visit_class(self, parent)

    def __str__(self):
        return f"Class({self.name}, parent={self.parent}, {self.methods})"

    def __repr__(self):
        return self.__str__()


class Method(AST_Node):
    def __init__(self, selectors, block):
        self.selectors = selectors  # List of selectors
        self.block = block  # Block object

    def accept(self, visitor, parent=None):
        return visitor.visit_method(self, parent)

    def __str__(self):
        return f"Method(selectors={self.selectors}, block={self.block})"


class Block(AST_Node):
    def __init__(self, parameters, statements):
        self.parameters = parameters
        self.statements = statements

    def accept(self, visitor, parent=None):
        return visitor.visit_block(self, parent)

    def __str__(self):
        return f"Block(parameters={self.parameters}, statements={self.statements})"

    def __repr__(self):
        return self.__str__()


class Statement(AST_Node):
    def __init__(self, name, expression):
        self.name = name
        self.expression = expression

    def accept(self, visitor, parent=None):
        return visitor.visit_statement(self, parent)

    def __str__(self):
        return f"Statement(name={self.name}, expression={self.expression})"

    def __repr__(self):
        return self.__str__()


class Expression(AST_Node):
    def __init__(self, base, tail):
        self.base = base
        self.tail = tail

    def accept(self, visitor, parent=None):
        return visitor.visit_expression(self, parent)

    def __str__(self):
        return f"Expression(base={self.base}, tail={self.tail})"

    def __repr__(self):
        return self.__str__()


class Integer(AST_Node):
    def __init__(self, value):
        self.value = value

    def accept(self, visitor, parent=None):
        return visitor.visit_base(self, parent)

    def __str__(self):
        return f"Integer(value={self.value})"


class String(AST_Node):
    def __init__(self, value):
        self.value = value

    def accept(self, visitor, parent=None):
        return visitor.visit_base(self, parent)

    def __str__(self):
        return f"String(value={self.value})"


class Identifier(AST_Node):
    def __init__(self, id):
        self.id = id

    def accept(self, visitor, parent=None):
        return visitor.visit_base(self, parent)

    def __str__(self):
        return f"Identifier(id={self.id})"


class ClassName(AST_Node):
    def __init__(self, name):
        self.name = name

    def accept(self, visitor, parent=None):
        return visitor.visit_base(self, parent)

    def __str__(self):
        return f"ClassName(name={self.name})"


class Tail(AST_Node):
    def __init__(self, id, sel=None):
        self.id = id
        self.sel = sel

    def accept(self, visitor, parent=None):
        return visitor.visit_tail(self, parent)

    def __str__(self):
        return f"Tail(id={self.id}, sel={self.sel})"


class Sel(AST_Node):
    def __init__(self, base, sel=None):
        self.base = base
        self.sel = sel

    def accept(self, visitor, parent=None):
        return visitor.visit_sel(self, parent)

    def __str__(self):
        return f"Sel(base={self.base}, sel={self.sel})"


class NodeFactory:
    @staticmethod
    def create_node(node_type: str, data):
        if node_type == "program":
            return Program(data["classes"])
        elif node_type == "class":
            return Class(data["name"], data["parent"], data["methods"])
        elif node_type == "method":
            return Method(data["selectors"], data["block"])
        elif node_type == "expression":
            return Expression(data["base"], data["tail"])
        elif node_type == "block":
            return Block(data["parameters"], data["statements"])
        elif node_type == "integer":
            return Integer(data["value"])
        elif node_type == "string":
            return String(data["value"])
        elif node_type == "identifier":
            return Identifier(data["id"])
        elif node_type == "class_name":
            return ClassName(data["name"])
        elif node_type == "tail":
            return Tail(data["id"], data.get("sel"))
        elif node_type == "sel":
            return Sel(data["base"], data.get("sel"))

class Parser:
    def __init__(self):
        # load first token
        self.current_token = get_next_token()


    # checks that current token has right TokenType
    # loads another token and returns the checked one
    def _consume(self, expected_type: TokenType, expected_lex: str = ""):
        if self.current_token.type != expected_type:
            throw_error(ERRORS["SYNTAX_ERR"])

        if (
            len(expected_lex) == 0 or
            expected_lex == self.current_token.lexeme
        ):
            consumed = self.current_token
            self.current_token = get_next_token()

            return consumed

    def program(self):
        classes = []
        while self.current_token.type == TokenType.KEYWORD:
            classes.append(self._class())

        return NodeFactory.create_node("program", {"classes": classes})

    def _class(self):
        self._consume(TokenType.KEYWORD, "class")
        class_name = self._consume(TokenType.IDENTIFIER).lexeme
        self._consume(TokenType.COLON)
        parent = self._consume(TokenType.CLASS_NAME).lexeme
        self._consume(TokenType.BRACE_LEFT)
        methods = self._method()
        self._consume(TokenType.BRACE_RIGHT)

        return NodeFactory.create_node("class", {
            "type": "class",
            "name": class_name,
            "parent": parent,
            "methods": methods
        })

    def _method(self):
        selectors = []
        while self.current_token.type == TokenType.IDENTIFIER:
            selectors.append(self._selector())
            block = self._block()

        return NodeFactory.create_node("method", {
            "type": "method",
            "selectors": selectors,
            "block": block
        })

    def _selector(self):
        selectors = []
        while self.current_token.type == TokenType.IDENTIFIER:
            selectors.append(self._consume(TokenType.IDENTIFIER).lexeme)
            if (self.current_token.type == TokenType.COLON):
                self._consume(TokenType.COLON)

        return selectors

    def _block(self):
        self._consume(TokenType.BRACKET_LEFT)
        parameters = self._block_par()
        self._consume(TokenType.PIPE)
        statements = self._block_stat()
        self._consume(TokenType.BRACKET_RIGHT)

        return NodeFactory.create_node("block", {
            "type": "block",
            "parameters": parameters,
            "statements": statements
        })

    def _block_par(self):
        parameters = []
        while self.current_token.type == TokenType.COLON:
            self._consume(TokenType.COLON)
            parameters.append(self._consume(TokenType.IDENTIFIER).lexeme)

        return parameters

    def _block_stat(self):
        statements = []
        while self.current_token.type == TokenType.IDENTIFIER:
            statement = {}

            statement["name"] = self._consume(TokenType.IDENTIFIER).lexeme
            self._consume(TokenType.EQUALS)
            statement["expression"] = self._expression()
            self._consume(TokenType.DOT)

            statements.append(statement)

        return statements

    def _expression(self):
        base = self._expr_base()
        tail = self._expr_tail()

        return NodeFactory.create_node("expression", {
            "base": base,
            "tail": tail
        })

    def _expr_base(self):
        if (self.current_token.type == TokenType.INTEGER):
            val = self._consume(TokenType.INTEGER)
            return NodeFactory.create_node("integer", data={"value": val.lexeme })
        elif (self.current_token.type == TokenType.STRING):
            val = self._consume(TokenType.STRING)
            return NodeFactory.create_node("string", data={"value": val.lexeme })
        elif (self.current_token.type == TokenType.IDENTIFIER):
            id = self._consume(TokenType.IDENTIFIER)
            return NodeFactory.create_node("identifier", data={"id": id.lexeme })
        elif (self.current_token.type == TokenType.CLASS_NAME):
            class_name = self._consume(TokenType.CLASS_NAME)
            return NodeFactory.create_node("class_name", data={"name": class_name })
        elif (self.current_token.type == TokenType.BRACKET_LEFT):
            block = self._block()
            return block
        elif (self.current_token.type == TokenType.PARENTHESIS_LEFT):
            self._consume(TokenType.PARENTHESIS_LEFT)
            expression = self._expression()
            self._consume(TokenType.PARENTHESIS_RIGHT)
            return expression

    def _expr_tail(self):
        if self.current_token.type != TokenType.IDENTIFIER:
            return None

        id = self._consume(TokenType.IDENTIFIER)
        if (self.current_token.type == TokenType.COLON):
            sel = self._expr_sel(id.lexeme)
            return NodeFactory.create_node("tail", data={
                    "id": id.lexeme,
                    "sel": sel
                })

        # unary message
        return NodeFactory.create_node("tail", data={"id": id.lexeme })

    def _expr_sel(self, id):
        self._consume(TokenType.COLON)
        base = self._expr_base()

        # recursive check for more sel
        sel = None
        if self.current_token.type == TokenType.IDENTIFIER:
            next_id = self._consume(TokenType.IDENTIFIER)
            if self.current_token.type == TokenType.COLON:
                sel = self._expr_sel(next_id.lexeme)

        return NodeFactory.create_node("sel", data={"base": base, "sel": sel})


class AST_visitor:
    def visit_program(self, node, parent=None): pass
    def visit_class(self, node, parent=None): pass
    def visit_method(self, node, parent=None): pass
    def visit_block(self, node, parent=None): pass
    def visit_statement(self, node, parent=None): pass
    def visit_expression(self, node, parent=None): pass
    def visit_base(self, node, parent=None): pass
    def visit_tail(self, node, parent=None): pass
    def visit_sel(self, node, parent=None): pass


class XML_generator_visitor(AST_visitor):
    def visit_program(self, node, parent=None):
        root = ET.Element("program")
        root.set("language", "SOL25")
        for cls in node.classes:
            cls.accept(self, root)
        return root

    def visit_class(self, node, parent):
        class_el = ET.SubElement(parent, "class")
        class_el.set("name", node.name)
        class_el.set("parent", node.parent)

    def visit_method(self, node, parent):
        for selector, block in zip(node.selectors, node.blocks):
            method_el = ET.SubElement(parent, "method")
            method_el.set("selector", selector)
            block.accept(self, method_el)

    def visit_block(self, node, parent):
        block_el = ET.SubElement(parent, "block")
        block_el.set("arity", str(len(node.parameters)))
        for param in node.parameters:
            param_el = ET.SubElement(block_el, "parameter")
            param_el.set("name", param)
        for stmt in node.statements:
            stmt.accept(self, block_el)

    def visit_statement(self, node, parent):
        assign_el = ET.SubElement(parent, "assign")
        var_el = ET.SubElement(assign_el, "var")
        var_el.set("name", node.name)
        node.expression.accept(self, assign_el)

    def visit_expression(self, node, parent):
        expr_el = ET.SubElement(parent, "expression")
        node.base.accept(self, expr_el)
        if node.tail:
            node.tail.accept(self, expr_el)

    def visit_base(self, node, parent):
        if isinstance(node, Integer):
            val_el = ET.SubElement(parent, "integer")
            val_el.text = node.value
        elif isinstance(node, String):
            val_el = ET.SubElement(parent, "string")
            val_el.text = node.value
        elif isinstance(node, Identifier):
            var_el = ET.SubElement(parent, "var")
            var_el.set("name", node.id)
        elif isinstance(node, ClassName):
            class_el = ET.SubElement(parent, "class_name")
            class_el.set("name", node.name)

    def visit_tail(self, node, parent):
        tail_el = ET.SubElement(parent, "message")
        tail_el.set("selector", node.id)
        if node.sel:
            node.sel.accept(self, tail_el)

    def visit_sel(self, node, parent):
        arg_el = ET.SubElement(parent, "argument")
        node.base.accept(self, arg_el)
        if node.sel:
            node.sel.accept(self, parent)
        




parser = Parser()
ast = parser.program()

print(str(ast))

# XML
visitor = XML_generator_visitor()
root = ast.accept(visitor)
tree = ET.ElementTree(root)
ET.indent(tree, space="    ")
tree.write("output.xml", encoding="UTF-8", xml_declaration=True)
























