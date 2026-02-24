import os
from pathlib import Path
from typing import List
import re
from collections import defaultdict


class Parser:
    def __init__(self, scanner):
        self.scanner = scanner

    def print_tree(self):
        pass


class Parser:
    def __init__(self, scanner):
        self.scanner = scanner
        self.initialize_min_first_set()
        self.sym = scanner.next_token()
        self.tree = AbstractTree(self.n_program())

    def print_tree(self):
        self.tree.print()

    def initialize_min_first_set(self):
        self.first = defaultdict(set)
        self.first["NFunctionDeclaration"].update({DomainTag.KW_INT, DomainTag.KW_CHAR, DomainTag.KW_BOOL, DomainTag.KW_VOID})
        self.first["NType"].update({DomainTag.KW_INT, DomainTag.KW_CHAR, DomainTag.KW_BOOL})
        self.first["NBottomExpr"].update({DomainTag.IDENTIFIER, DomainTag.LEFT_PAR, DomainTag.DECIMAL_INTEGER_CONSTANT, DomainTag.NON_DECIMAL_INTEGER_CONSTANT, DomainTag.SYMBOLIC_CONSTANT, DomainTag.BOOLEAN_CONSTANT, DomainTag.KW_NULL})
        self.first["NCmpOp"].update({DomainTag.EQ_OP, DomainTag.ORD_OP})
        self.first["NExpr"].update({DomainTag.IDENTIFIER, DomainTag.LEFT_PAR, DomainTag.DECIMAL_INTEGER_CONSTANT, DomainTag.NON_DECIMAL_INTEGER_CONSTANT, DomainTag.SYMBOLIC_CONSTANT, DomainTag.BOOLEAN_CONSTANT, DomainTag.KW_NULL, DomainTag.STRING_SECTION, DomainTag.MINUS_OP, DomainTag.NOT_OP, DomainTag.KW_INT, DomainTag.KW_CHAR, DomainTag.KW_BOOL})

    def report_error(self):
        raise RuntimeError(str(self.sym.get_fragment_position()))

    def n_program(self):
        function_declarations = self.n_function_declarations()
        return AbstractTree.Program(function_declarations)

    def n_function_declarations(self):
        function_declarations = []
        while self.first["NFunctionDeclaration"].__contains__(self.sym.get_tag()):
            function_declarations.append(self.n_function_declaration())
        return function_declarations

    def n_function_declaration(self):
        header = self.n_function_header()
        if self.sym.get_tag() == DomainTag.EQUAL:
            self.sym = self.scanner.next_token()
        else:
            self.report_error()
        statements = self.n_statements()
        if self.sym.get_tag() == DomainTag.DOT:
            self.sym = self.scanner.next_token()
        else:
            self.report_error()
        return AbstractTree.FunctionDeclaration(header, statements)

    def n_function_header(self):
        type_and_name = self.n_function_header_type_name()
        formal_parameters = []
        if self.sym.get_tag() == DomainTag.LEFT_ARROW:
            self.sym = self.scanner.next_token()
            formal_parameters = self.n_formal_parameters()
        return AbstractTree.FunctionHeader(type_and_name.type, type_and_name.name, formal_parameters)

    def n_statements(self):
        statements = [self.n_statement()]
        while self.sym.get_tag() == DomainTag.SEMICOLON:
            self.sym = self.scanner.next_token()
            statements.append(self.n_statement())
        return statements

    def n_function_header_type_name(self):
        type_ = None
        name = None
        if self.first["NType"].__contains__(self.sym.get_tag()):
            type_ = self.n_type()
        else:
            if self.sym.get_tag() == DomainTag.KW_VOID:
                self.sym = self.scanner.next_token()
            else:
                self.report_error()
        if self.sym.get_tag() == DomainTag.IDENTIFIER:
            name = self.sym.get_value()
            self.sym = self.scanner.next_token()
        else:
            self.report_error()
        return AbstractTree.FunctionHeaderTypeAndName(type_, name)

    def n_formal_parameters(self):
        formal_parameters = [self.n_formal_parameter()]
        while self.sym.get_tag() == DomainTag.COMMA:
            self.sym = self.scanner.next_token()
            formal_parameters.append(self.n_formal_parameter())
        return formal_parameters

    def n_formal_parameter(self):
        type_ = self.n_type()
        name = None
        if self.sym.get_tag() == DomainTag.IDENTIFIER:
            name = self.sym.get_value()
            self.sym = self.scanner.next_token()
        else:
            self.report_error()
        return AbstractTree.FormalParameter(type_, name)


class AbstractTree:
    class Statement:
        pass

    class Type:
        def __init__(self, prim, n):
            self.prim = prim
            self.n = n

    class DeclarationAssignment:
        def __init__(self, name, expr):
            self.name = name
            self.expr = expr

    class DeclarationStatement(Statement):
        def __init__(self, type, declaration_assignments):
            self.type = type
            self.declaration_assignments = declaration_assignments

    class PostWhileStatement(Statement):
        def __init__(self, condition, body):
            self.condition = condition
            self.body = body

    class ReturnStatement(Statement):
        def __init__(self, expr):
            self.expr = expr

    class AssignmentStatement(Statement):
        def __init__(self, expr1, expr2):
            self.expr1 = expr1
            self.expr2 = expr2

    class IfStatement(Statement):
        def __init__(self, expr, then_branch, else_branch):
            self.expr = expr
            self.then_branch = then_branch
            self.else_branch = else_branch

    class PreWhileStatement(Statement):
        def __init__(self, expr, body):
            self.expr = expr
            self.body = body

    class ForStatement(Statement):
        def __init__(self, expr1, expr2, variable, body):
            self.expr1 = expr1
            self.expr2 = expr2
            self.variable = variable
            self.body = body

    class PrimType:
        INT = 'int'
        CHAR = 'char'
        BOOL = 'bool'


class DomainTag:
    KW_LOOP = 'KW_LOOP'
    KW_RETURN = 'KW_RETURN'
    KW_THEN = 'KW_THEN'
    KW_ELSE = 'KW_ELSE'
    DOT = 'DOT'
    ASSIGN = 'ASSIGN'
    TILDE = 'TILDE'
    IDENTIFIER = 'IDENTIFIER'
    COMMA = 'COMMA'
    KW_INT = 'KW_INT'
    KW_CHAR = 'KW_CHAR'
    KW_BOOL = 'KW_BOOL'
    BRACKETS = 'BRACKETS'


class Scanner:
    def nextToken(self):
        pass


class Symbol:
    def getTag(self):
        pass

    def getValue(self):
        pass


first = {
    "NType": {DomainTag.KW_INT, DomainTag.KW_CHAR, DomainTag.KW_BOOL},
    "NExpr": {DomainTag.IDENTIFIER}  # Example, adjust as needed
}

scanner = Scanner()
sym = Symbol()


def reportError():
    pass


def NType():
    prim = None
    if sym.getTag() == DomainTag.KW_INT:
        prim = AbstractTree.PrimType.INT
        sym = scanner.nextToken()
    elif sym.getTag() == DomainTag.KW_CHAR:
        prim = AbstractTree.PrimType.CHAR
        sym = scanner.nextToken()
    elif sym.getTag() == DomainTag.KW_BOOL:
        prim = AbstractTree.PrimType.BOOL
        sym = scanner.nextToken()
    else:
        reportError()

    n = 0
    while sym.getTag() == DomainTag.BRACKETS:
        n += 1
        sym = scanner.nextToken()

    return AbstractTree.Type(prim, n)


def NDeclarationAssignment():
    name = None
    expr = None
    if sym.getTag() == DomainTag.IDENTIFIER:
        name = sym.getValue()
        sym = scanner.nextToken()
    else:
        reportError()

    if sym.getTag() == DomainTag.ASSIGN:
        sym = scanner.nextToken()
        expr = NArithmExpr()

    return AbstractTree.DeclarationAssignment(name, expr)


def NDeclarationAssignments():
    declaration_assignments = [NDeclarationAssignment()]
    while sym.getTag() == DomainTag.COMMA:
        sym = scanner.nextToken()
        declaration_assignments.append(NDeclarationAssignment())
    return declaration_assignments


def NStatement():
    statement = None

    if sym.getTag() in first["NType"]:
        type = NType()
        declaration_assignments = NDeclarationAssignments()
        statement = AbstractTree.DeclarationStatement(type, declaration_assignments)
    elif sym.getTag() == DomainTag.KW_LOOP:
        sym = scanner.nextToken()
        body = NStatements()
        if sym.getTag() == DomainTag.DOT:
            sym = scanner.nextToken()
        else:
            reportError()
        condition = NExpr()
        if sym.getTag() == DomainTag.DOT:
            sym = scanner.nextToken()
        else:
            reportError()
        statement = AbstractTree.PostWhileStatement(condition, body)
    elif sym.getTag() == DomainTag.KW_RETURN:
        sym = scanner.nextToken()
        expr = None
        if sym.getTag() in first["NExpr"]:
            expr = NExpr()
        statement = AbstractTree.ReturnStatement(expr)
    else:
        expr = NExpr()
        if sym.getTag() == DomainTag.ASSIGN:
            sym = scanner.nextToken()
            expr2 = NExpr()
            statement = AbstractTree.AssignmentStatement(expr, expr2)
        elif sym.getTag() == DomainTag.KW_THEN:
            sym = scanner.nextToken()
            then_branch = NStatements()
            else_branch = []
            if sym.getTag() == DomainTag.KW_ELSE:
                sym = scanner.nextToken()
                else_branch = NStatements()
            if sym.getTag() == DomainTag.DOT:
                sym = scanner.nextToken()
            else:
                reportError()
            statement = AbstractTree.IfStatement(expr, then_branch, else_branch)
        elif sym.getTag() == DomainTag.KW_LOOP:
            sym = scanner.nextToken()
            body = NStatements()
            if sym.getTag() == DomainTag.DOT:
                sym = scanner.nextToken()
            else:
                reportError()
            statement = AbstractTree.PreWhileStatement(expr, body)
        elif sym.getTag() == DomainTag.TILDE:
            sym = scanner.nextToken()
            expr2 = NExpr()
            if sym.getTag() == DomainTag.KW_LOOP:
                sym = scanner.nextToken()
            else:
                reportError()
            variable = None
            if sym.getTag() == DomainTag.IDENTIFIER:
                variable = sym.getValue()
                sym = scanner.nextToken()
            else:
                reportError()
            body = NStatements()
            if sym.getTag() == DomainTag.DOT:
                sym = scanner.nextToken()
            else:
                reportError()
            statement = AbstractTree.ForStatement(expr, expr2, variable, body)

    return statement


class AbstractTree:
    class Expr:
        pass

    class BinOpExpr(Expr):
        def __init__(self, left, op, right):
            self.left = left
            self.op = op
            self.right = right

    class UnOpExpr(Expr):
        def __init__(self, op, expr):
            self.op = op
            self.expr = expr

    class FunctionInvocationExpr(Expr):
        def __init__(self, expr, args):
            self.expr = expr
            self.args = args

    class VariableExpr(Expr):
        def __init__(self, name):
            self.name = name

    class ConstExpr(Expr):
        def __init__(self, type, value):
            self.type = type
            self.value = value

    class StringConstExpr(Expr):
        def __init__(self, type, sections):
            self.type = type
            self.sections = sections

    class AllocExpr(Expr):
        def __init__(self, type, expr):
            self.type = type
            self.expr = expr

    class Type:
        def __init__(self, prim_type, dim):
            self.prim_type = prim_type
            self.dim = dim

    class PrimType:
        INT = 0
        CHAR = 1
        BOOL = 2

def NExpr(scanner, sym, first):
    expr = NAndExpr(scanner, sym, first)
    bin_op_expr = None
    while sym.getTag() == DomainTag.OR_XOR_OP:
        op = sym.getValue()
        sym = scanner.nextToken()
        expr2 = NAndExpr(scanner, sym, first)
        if bin_op_expr is not None:
            bin_op_expr = AbstractTree.BinOpExpr(bin_op_expr, op, expr2)
        else:
            bin_op_expr = AbstractTree.BinOpExpr(expr, op, expr2)
    if bin_op_expr is None:
        return expr
    return bin_op_expr

def NAndExpr(scanner, sym, first):
    expr = NCmpExpr(scanner, sym, first)
    bin_op_expr = None
    while sym.getTag() == DomainTag.AND_OP:
        op = sym.getValue()
        sym = scanner.nextToken()
        expr2 = NCmpExpr(scanner, sym, first)
        if bin_op_expr is not None:
            bin_op_expr = AbstractTree.BinOpExpr(bin_op_expr, op, expr2)
        else:
            bin_op_expr = AbstractTree.BinOpExpr(expr, op, expr2)
    if bin_op_expr is None:
        return expr
    return bin_op_expr

def NCmpExpr(scanner, sym, first):
    expr = NFuncCallExpr(scanner, sym, first)
    bin_op_expr = None
    while sym.getTag() in first["NCmpOp"]:
        op = NCmpOp(scanner, sym)
        expr2 = NFuncCallExpr(scanner, sym, first)
        if bin_op_expr is not None:
            bin_op_expr = AbstractTree.BinOpExpr(bin_op_expr, op, expr2)
        else:
            bin_op_expr = AbstractTree.BinOpExpr(expr, op, expr2)
    if bin_op_expr is None:
        return expr
    return bin_op_expr

def NCmpOp(scanner, sym):
    op = ""
    if sym.getTag() == DomainTag.EQ_OP:
        op = sym.getValue()
        sym = scanner.nextToken()
    elif sym.getTag() == DomainTag.ORD_OP:
        op = sym.getValue()
        sym = scanner.nextToken()
    else:
        reportError()
    return op

def NFuncCallExpr(scanner, sym, first):
    expr = NArithmExpr(scanner, sym, first)
    args = []
    if sym.getTag() == DomainTag.LEFT_ARROW:
        sym = scanner.nextToken()
        args = NArgs(scanner, sym, first)
    if not args:
        return expr
    return AbstractTree.FunctionInvocationExpr(expr, args)

def NArgs(scanner, sym, first):
    args = [NArithmExpr(scanner, sym, first)]
    while sym.getTag() == DomainTag.COMMA:
        sym = scanner.nextToken()
        args.append(NArithmExpr(scanner, sym, first))
    return args

def NArithmExpr(scanner, sym, first):
    expr = NTerm(scanner, sym, first)
    bin_op_expr = None
    while sym.getTag() == DomainTag.PLUS_OP or sym.getTag() == DomainTag.MINUS_OP:
        op = sym.getValue()
        sym = scanner.nextToken()
        expr2 = NTerm(scanner, sym, first)
        if bin_op_expr is not None:
            bin_op_expr = AbstractTree.BinOpExpr(bin_op_expr, op, expr2)
        else:
            bin_op_expr = AbstractTree.BinOpExpr(expr, op, expr2)
    if bin_op_expr is None:
        return expr
    return bin_op_expr

def NTerm(scanner, sym, first):
    expr = NFactor(scanner, sym, first)
    bin_op_expr = None
    while sym.getTag() == DomainTag.MUL_DIV_REM_OP:
        op = sym.getValue()
        sym = scanner.nextToken()
        expr2 = NFactor(scanner, sym, first)
        if bin_op_expr is not None:
            bin_op_expr = AbstractTree.BinOpExpr(bin_op_expr, op, expr2)
        else:
            bin_op_expr = AbstractTree.BinOpExpr(expr, op, expr2)
    if bin_op_expr is None:
        return expr
    return bin_op_expr

def NFactor(scanner, sym, first):
    expr = NPower(scanner, sym, first)
    bin_op_expr = None
    if sym.getTag() == DomainTag.POWER_OP:
        op = sym.getValue()
        sym = scanner.nextToken()
        expr2 = NFactor(scanner, sym, first)
        bin_op_expr = AbstractTree.BinOpExpr(expr, op, expr2)
    if bin_op_expr is None:
        return expr
    return bin_op_expr

def NPower(scanner, sym, first):
    expr = None
    if sym.getTag() == DomainTag.NOT_OP or sym.getTag() == DomainTag.MINUS_OP:
        op = sym.getValue()
        sym = scanner.nextToken()
        expr = AbstractTree.UnOpExpr(op, NPower(scanner, sym, first))
    elif sym.getTag() in first["NType"]:
        type = NType(scanner, sym, first)
        expr2 = NBottomExpr(scanner, sym, first)
        expr = AbstractTree.AllocExpr(type, expr2)
    else:
        expr = NArrExpr(scanner, sym, first)
    return expr

def NArrExpr(scanner, sym, first):
    expr = None
    if sym.getTag() == DomainTag.STRING_SECTION:
        expr = NStringConstant(scanner, sym)
    elif sym.getTag() in first["NBottomExpr"]:
        expr = NBottomExpr(scanner, sym, first)
    else:
        expr1 = NArrExpr(scanner, sym, first)
        expr2 = NBottomExpr(scanner, sym, first)
        expr = AbstractTree.BinOpExpr(expr1, "at", expr2)
    return expr

def NBottomExpr(scanner, sym, first):
    expr = None
    if sym.getTag() == DomainTag.IDENTIFIER:
        name = sym.getValue()
        sym = scanner.nextToken()
        expr = AbstractTree.VariableExpr(name)
    elif sym.getTag() == DomainTag.LEFT_PAR:
        sym = scanner.nextToken()
        expr = NExpr(scanner, sym, first)
        if sym.getTag() == DomainTag.RIGHT_PAR:
            sym = scanner.nextToken()
        else:
            reportError()
    else:
        expr = NConst(scanner, sym)
    return expr

def NStringConstant(scanner, sym):
    sections = []
    if sym.getTag() == DomainTag.STRING_SECTION:
        sections.append(sym.getValue())
        sym = scanner.nextToken()
    else:
        reportError()
    while sym.getTag() == DomainTag.STRING_SECTION:
        sections.append(sym.getValue())
        sym = scanner.nextToken()
    return AbstractTree.StringConstExpr(AbstractTree.Type(AbstractTree.PrimType.CHAR, 1), sections)

def NConst(scanner, sym):
    expr = None
    if sym.getTag() in [DomainTag.DECIMAL_INTEGER_CONSTANT, DomainTag.NON_DECIMAL_INTEGER_CONSTANT, DomainTag.SYMBOLIC_CONSTANT, DomainTag.BOOLEAN_CONSTANT, DomainTag.KW_NULL]:
        value = sym.getValue()
        if sym.getTag() == DomainTag.DECIMAL_INTEGER_CONSTANT or sym.getTag() == DomainTag.NON_DECIMAL_INTEGER_CONSTANT:
            expr = AbstractTree.ConstExpr(AbstractTree.Type(AbstractTree.PrimType.INT, 0), value)
        elif sym.getTag() == DomainTag.SYMBOLIC_CONSTANT:
            expr = AbstractTree.ConstExpr(AbstractTree.Type(AbstractTree.PrimType.CHAR, 0), value)
        elif sym.getTag() == DomainTag.BOOLEAN_CONSTANT:
            expr = AbstractTree.ConstExpr(AbstractTree.Type(AbstractTree.PrimType.BOOL, 0), value)
        elif sym.getTag() == DomainTag.KW_NULL:
            expr = AbstractTree.ConstExpr(AbstractTree.Type(None, 0), value)
        sym = scanner.nextToken()
    else:
        reportError()
    return expr

def NType(scanner, sym, first):
    # Implementation of NType function
    pass

def reportError():
    # Implementation of reportError function
    pass



if __name__ == "__main__":
    input_file = "input.txt"
    with open(input_file, "r") as file:
        input_text = file.read()
    lexer = input_text.splitlines()
    parser = Parser(lexer)
    parser.print_tree()

