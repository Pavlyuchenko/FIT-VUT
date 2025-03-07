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


keywords = ["class"]
class_names = ["Object", "Nil", "True", "False", "Integer", "String", "Block"]
user_class_names = []

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


first_comment_seen = False
first_comment = ""
# tokenizer, lexical analysis
# reads stdin character by characted
# and based on a finite automaton it returns either
#   Token or throws an error to the stderr
def get_next_token() -> Token:
    curr_ch = get_char()
    token = Token(TokenType.UNKNOWN, "")

    while curr_ch.isspace():
        curr_ch = get_char()

    # handles INTEGER input
    if curr_ch.isdigit() or curr_ch in ("+", "-"):
        token.lexeme = curr_ch
        token.type = TokenType.INTEGER

        if curr_ch in ("+", "-"):
            if not (curr_ch := get_char()).isdigit():
                throw_error(ERRORS["LEXICAL_ERR"])
            token.lexeme += curr_ch 

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
        elif token.lexeme in class_names:
            token.type = TokenType.IDENTIFIER
        else:
            token.type = TokenType.IDENTIFIER

    # COMMENT
    elif curr_ch == '"':
        comment = ""
        while (curr_ch := get_char()) != '"':
            if not curr_ch:  # Unclosed comment
                throw_error(ERRORS["LEXICAL_ERR"])
            comment += curr_ch

        global first_comment_seen
        global first_comment
        if not first_comment_seen:
            first_comment_seen = True
            first_comment = comment
            
        return get_next_token()

    # STRING
    elif curr_ch == "'":
        token.type = TokenType.STRING
        # TODO: Handle escape sequences??
        while (curr_ch := get_char()) != "'":
            # check ascii int value
            if ord(curr_ch) < 32:
                throw_error(ERRORS["LEXICAL_ERR"])

            if curr_ch == "\\":
                token.lexeme += curr_ch
                curr_ch = get_char()
                if curr_ch not in ("\\", "n", "\'"):
                    throw_error(ERRORS["LEXICAL_ERR"])
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
        token.type = TokenType.PARENTHESIS_LEFT
        token.lexeme = "("
    elif curr_ch == ")":
        token.type = TokenType.PARENTHESIS_RIGHT
        token.lexeme = ")"

    # EOF
    elif not curr_ch:
        token.type = TokenType.TOKEN_EOF

    else:
        throw_error(ERRORS["LEXICAL_ERR"])

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
    def __init__(self, selector, block):
        self.selector = selector  # List of selectors
        self.block = block  # Block object

    def accept(self, visitor, parent=None):
        return visitor.visit_method(self, parent)

    def __str__(self):
        return f"Method(selector={self.selector}, block={self.block})"

    def __repr__(self):
        return self.__str__()


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

    def accept(self, visitor, parent=None, count=1):
        return visitor.visit_statement(self, parent, count)

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

    def accept(self, visitor, parent=None, order=1):
        return visitor.visit_sel(self, parent, order)

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
            return Method(data["selector"], data["block"])
        elif node_type == "expression":
            return Expression(data["base"], data["tail"])
        elif node_type == "block":
            return Block(data["parameters"], data["statements"])
        elif node_type == "statement":
            return Statement(data["name"], data["expression"])
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
            print(self.current_token)
            print(expected_type)
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

        if self.current_token.type != TokenType.KEYWORD:
            throw_error(ERRORS["SYNTAX_ERR"])
        while self.current_token.type == TokenType.KEYWORD:
            classes.append(self._class())

        return NodeFactory.create_node("program", {"classes": classes})

    def _class(self):
        tst = self._consume(TokenType.KEYWORD, "class")
        class_name = self._consume(TokenType.IDENTIFIER).lexeme
        user_class_names.append(class_name)
        self._consume(TokenType.COLON)
        parent = self._consume(TokenType.IDENTIFIER).lexeme

        if not (parent in class_names or parent in user_class_names):
            throw_error(ERRORS["SEM_UNDEFINED_ERR"])

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
        methods = []
        while self.current_token.type == TokenType.IDENTIFIER:
            selector = self._selector()
            block = self._block()
            methods.append(
                NodeFactory.create_node("method", {
                    "type": "method",
                    "selector": selector,
                    "block": block
                })
            )
        return methods

    def _selector(self):
        selectors = ""
        while self.current_token.type == TokenType.IDENTIFIER:
            selectors += (self._consume(TokenType.IDENTIFIER).lexeme)
            if (self.current_token.type == TokenType.COLON):
                self._consume(TokenType.COLON)
                selectors += ":"

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
            print(parameters)

        return parameters

    def _block_stat(self):
        statements = []
        while self.current_token.type == TokenType.IDENTIFIER:
            name = self._consume(TokenType.IDENTIFIER).lexeme
            self._consume(TokenType.EQUALS)
            expression = self._expression()
            self._consume(TokenType.DOT)

            statements.append(NodeFactory.create_node("statement", {
                "name": name,
                "expression": expression 
            }))

        return statements

    def _expression(self):
        base = self._expr_base()
        tail = self._expr_tail()

        if isinstance(base, Expression) and base.tail is None:
            return base

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

        id = [self._consume(TokenType.IDENTIFIER).lexeme]
        if (self.current_token.type == TokenType.COLON):
            sel = self._expr_sel(id)
            return NodeFactory.create_node("tail", data={
                    "id": ':'.join(id) + ':',
                    "sel": sel
                })

        # unary message
        return NodeFactory.create_node("tail", data={"id": id[0] })

    def _expr_sel(self, id):
        self._consume(TokenType.COLON)
        base = self._expr_base()

        # recursive check for more sel
        sel = None
        if self.current_token.type == TokenType.IDENTIFIER:
            next_id = self._consume(TokenType.IDENTIFIER)
            if self.current_token.type == TokenType.COLON:
                id.append(next_id.lexeme)
                sel = self._expr_sel(id)

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


class Semantic_visitor():
    def __init__(self):
        self.classes = {}
        self.current_class = None
        self.predefined_classes = ["Object", "Nil", "True", "False", "Integer", "String", "Block"]

    def visit_program(self, node, parent=None):
        for cls in node.classes:
            cls.accept(self)
        if "Main" not in self.classes:
            throw_error(ERRORS["SEM_MAIN_ERR"])

    def visit_class(self, node, parent=None):
        if node.name in self.classes:
            self.throw_error(ERRORS["SEM_COLISION_ERR"])
        self.classes[node.name] = node
        if node.parent not in self.predefined_classes and node.parent not in self.classes:
            self.throw_error(ERRORS["SEM_UNDEFINED_ERR"])
        self.current_class = node.name
        for method in node.methods:
            method.accept(self)
        self.current_class = None

    def visit_method(self, node, parent=None):
        # print method name
        print(f"Method: {node.selector}")
        node.block.accept(self)

    def visit_block(self, node, parent=None):
        self.current_block_vars = node.parameters
        # node.statements.accept(self)


class XML_generator_visitor(AST_visitor):
    def visit_program(self, node, parent=None):
        root = ET.Element("program")
        root.set("language", "SOL25")

        global first_comment_seen
        global first_comment
        if first_comment_seen:
            root.set("description", first_comment)
        for cls in node.classes:
            cls.accept(self, root)
        return root

    def visit_class(self, node, parent):
        class_el = ET.SubElement(parent, "class")
        class_el.set("name", node.name)
        class_el.set("parent", node.parent)
        for method in node.methods:
            method.accept(self, class_el)

    def visit_method(self, node: AST_Node, parent):
        method_el = ET.SubElement(parent, "method")
        method_el.set("selector", node.selector)
        node.block.accept(self, method_el)

    def visit_block(self, node, parent):
        block_el = ET.SubElement(parent, "block")
        block_el.set("arity", str(len(node.parameters)))
        count = 1
        for param in node.parameters:
            param_el = ET.SubElement(block_el, "parameter")
            param_el.set("order", str(count))
            param_el.set("name", param)
            count += 1
        count = 1
        for stmt in node.statements:
            stmt.accept(self, block_el, count)
            count += 1

    def visit_statement(self, node, parent, order=1):
        assign_el = ET.SubElement(parent, "assign")
        assign_el.set("order", str(order))
        var_el = ET.SubElement(assign_el, "var")
        var_el.set("name", node.name)
        node.expression.accept(self, assign_el)

    def visit_expression(self, node, parent):
        expr_el = ET.SubElement(parent, "expr")
        if node.tail:
            test = ET.SubElement(expr_el, "send")
            test.set("selector", node.tail.id)

            if isinstance(node.base, Expression):
                node.base.accept(self, test)
                node.tail.accept(self, test)
            else:
                expr2_el = ET.SubElement(test, "expr")
                node.base.accept(self, expr2_el)
                node.tail.accept(self, test)

        else:
            node.base.accept(self, expr_el)

    def visit_base(self, node, parent):
        if isinstance(node, Integer):
            val_el = ET.SubElement(parent, "literal")
            val_el.set("class", "Integer")
            val_el.set("value", node.value)
        elif isinstance(node, String):
            val_el = ET.SubElement(parent, "literal")
            val_el.set("class", "String")
            val_el.set("value", node.value)

        elif isinstance(node, Identifier):
            class_names_lower = {name.lower() for name in class_names}
            if node.id in class_names:
                var_el = ET.SubElement(parent, "literal")
                var_el.set("class", "class")
            elif node.id.lower() in class_names_lower:
                var_el = ET.SubElement(parent, "literal")
                var_el.set("class", node.id.capitalize())
            elif node.id in user_class_names:
                var_el = ET.SubElement(parent, "literal")
                var_el.set("class", "class")
            else:
                var_el = ET.SubElement(parent, "var")
                var_el.set("name", node.id)
                return
            var_el.set("value", node.id)
        elif isinstance(node, ClassName):
            class_el = ET.SubElement(parent, "literal")
            class_el.set("class", "class")
            class_el.set("name", node.name)


    def visit_tail(self, node, parent):
        if node.sel:
            node.sel.accept(self, parent)

    def visit_sel(self, node, parent, order=1):
        arg_el = ET.SubElement(parent, "arg")
        arg_el.set("order", str(order))
        if isinstance(node.base, Block):
            expr_el = ET.SubElement(arg_el, "expr")
            node.base.accept(self, expr_el)
        else:
            if isinstance(node.base, Expression):
                node.base.accept(self, arg_el)
            else:
                expr_el = ET.SubElement(arg_el, "expr")
                node.base.accept(self, expr_el)

        if node.sel:
            node.sel.accept(self, parent, order+1)


parser = Parser()
ast = parser.program()

# SEMANTICS
semantic_checker = Semantic_visitor()
ast.accept(semantic_checker)


# XML
visitor = XML_generator_visitor()
root = ast.accept(visitor)
tree = ET.ElementTree(root)
ET.indent(tree, space="  ")

import xml.sax.saxutils as saxutils
xml_str = ET.tostring(root, encoding="unicode")
xml_str = saxutils.unescape(xml_str)
with open("output.xml", "w", encoding="utf-8") as f:
    # add xml declaration
    f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
    f.write(xml_str)

# write the tree to stdout
ET.dump(tree.getroot())
























