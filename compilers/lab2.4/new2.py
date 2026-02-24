from collections import defaultdict

from Lexer import *
from abstractTree import *


class Parser:
    def __init__(self, lexer):
        self.lexer = lexer
        self.initialize_min_first_set()
        self.sym = lexer.nextToken()
        self.tree = self.NProgram()

    def print_tree(self):
        self.tree.print()

    def initialize_min_first_set(self):
        self.first = defaultdict(set)
        self.first["NFunctionDeclaration"].update(
            {DomainTag.KW_INT, DomainTag.KW_CHAR, DomainTag.KW_BOOL, DomainTag.KW_VOID})
        self.first["NType"].update({DomainTag.KW_INT, DomainTag.KW_CHAR, DomainTag.KW_BOOL})
        self.first["NPrim"].update(
            {DomainTag.VARNAME, DomainTag.LB, DomainTag.INTEGER, DomainTag.CHAR_CONST, DomainTag.BOOLEAN,
             DomainTag.KW_NULL})
        self.first["NCmpOp"].update({DomainTag.COMPARE_OP})
        self.first["NExpr"].update(
            {DomainTag.VARNAME, DomainTag.LB, DomainTag.INTEGER, DomainTag.CHAR_CONST, DomainTag.BOOLEAN,
             DomainTag.KW_NULL, DomainTag.STRING, DomainTag.MINUS, DomainTag.NOT_OP, DomainTag.KW_INT,
             DomainTag.KW_CHAR, DomainTag.KW_BOOL})

    def report_error(self):
        raise RuntimeError(str(self.sym.get_fragment_position().get_simplified_string()))

    # NProgram ::= NFunctions
    def NProgram(self):
        function_declarations = self.NFuncions()
        return AbstractTree(Program(function_declarations))

    # NFunctions ::= (NFunction)+
    def NFuncions(self):
        function_declarations = []
        while self.first["NFunctionDeclaration"].__contains__(self.sym.get_tag()):
            function_declarations.append(self.NFuncion())
        return function_declarations


    # NStatement ::= NType NDeclarationStatements
    #              | NExpr
    #                (
    #                  ':=' NExpr
    #                | KW_THEN NStatements (KW_ELSE NStatements)? '.'
    #                | KW_LOOP NStatements '.'
    #                | '~' NExpr KW_LOOP IDENTIFIER NStatements '.'
    #                )?
    #              | KW_LOOP NStatements KW_WHILE NExpr '.'
    #              | KW_RETURN (NExpr)?
    def NStatement(self):
        statement = None

        if self.sym.get_tag() in self.first["NType"]:
            type = self.NType()
            declaration_assignments = self.NDeclarationAssignments()
            statement = DeclarationStatement(type, declaration_assignments)
        elif self.sym.get_tag() == DomainTag.KW_LOOP:
            self.sym = lexer.nextToken()
            body = self.NStatements()
            if self.sym.get_tag() == DomainTag.DOT:
                self.sym = lexer.nextToken()
            else:
                self.report_error()
            condition = self.NExpr()
            if self.sym.get_tag() == DomainTag.DOT:
                self.sym = lexer.nextToken()
            else:
                self.report_error()
            statement = PostWhileStatement(condition, body)
        elif self.sym.get_tag() == DomainTag.KW_RETURN:
            self.sym = lexer.nextToken()
            expr = None
            if self.sym.get_tag() in self.first["NExpr"]:
                expr = self.NExpr()
            statement = ReturnStatement(expr)
        else:
            expr = self.NExpr()
            if self.sym.get_tag() == DomainTag.ASSIGN:
                self.sym = lexer.nextToken()
                expr2 = self.NExpr()
                statement = AssignmentStatement(expr, expr2)
            elif self.sym.get_tag() == DomainTag.KW_THEN:
                self.sym = lexer.nextToken()
                then_branch = self.NStatements()
                else_branch = []
                if self.sym.get_tag() == DomainTag.KW_ELSE:
                    self.sym = lexer.nextToken()
                    else_branch = self.NStatements()
                if self.sym.get_tag() == DomainTag.DOT:
                    self.sym = lexer.nextToken()
                else:
                    self.report_error()
                statement = IfStatement(expr, then_branch, else_branch)
            elif self.sym.get_tag() == DomainTag.KW_LOOP:
                self.sym = lexer.nextToken()
                body = self.NStatements()
                if self.sym.get_tag() == DomainTag.DOT:
                    self.sym = lexer.nextToken()
                else:
                    self.report_error()
                statement = WhileStatement(expr, body)
            elif self.sym.get_tag() == DomainTag.TILDE:
                self.sym = lexer.nextToken()
                expr2 = self.NExpr()
                if self.sym.get_tag() == DomainTag.KW_LOOP:
                    self.sym = lexer.nextToken()
                else:
                    self.report_error()
                variable = None
                if self.sym.get_tag() == DomainTag.VARNAME:
                    variable = self.sym.get_value()
                    self.sym = lexer.nextToken()
                else:
                    self.report_error()
                body = self.NStatements()
                if self.sym.get_tag() == DomainTag.DOT:
                    self.sym = lexer.nextToken()
                else:
                    self.report_error()
                statement = ForStatement(expr, expr2, variable, body)

        return statement

    # NFunction ::= NFunctionHeader '=' NStatements '.'

    def NFuncion(self):
        header = self.NFunctionHeader()
        if self.sym.get_tag() == DomainTag.EQUAL:
            self.sym = self.lexer.nextToken()
        else:
            self.report_error()
        statements = self.NStatements()
        if self.sym.get_tag() == DomainTag.DOT:
            self.sym = self.lexer.nextToken()
        else:
            self.report_error()
        return FunctionDeclaration(header, statements)

    # NFunctionHeader ::= NFunctionHeaderTypeName ('<-' NFuncParams)?
    def NFunctionHeader(self):
        type_and_name = self.NFunctionHeaderTypeName()
        formal_parameters = []
        if self.sym.get_tag() == DomainTag.LEFT_ARROW:
            self.sym = self.lexer.nextToken()
            formal_parameters = self.NFunctionParams()
        return FunctionHeader(type_and_name.type, type_and_name.name, formal_parameters)

    # NStatements ::= NStatement (';' NStatement)*
    def NStatements(self):
        statements = [self.NStatement()]
        while self.sym.get_tag() == DomainTag.SEMICOLON:
            self.sym = self.lexer.nextToken()
            statements.append(self.NStatement())
        return statements

    # NFunctionHeaderTypeName ::= (NType | KW_VOID) VARNAME
    def NFunctionHeaderTypeName(self):
        type_ = None
        name = None
        if self.first["NType"].__contains__(self.sym.get_tag()):
            type_ = self.NType()
        else:
            if self.sym.get_tag() == DomainTag.KW_VOID:
                self.sym = self.lexer.nextToken()
            else:
                self.report_error()
        if self.sym.get_tag() == DomainTag.VARNAME:
            name = self.sym.get_value()
            self.sym = self.lexer.nextToken()
        else:
            self.report_error()
        return FunctionHeaderTypeName(type_, name)

    # NFuncParams ::= NFuncParam (',' NFuncParam)*
    def NFunctionParams(self):
        formal_parameters = [self.NFuncionParam()]
        while self.sym.get_tag() == DomainTag.COMMA:
            self.sym = self.lexer.nextToken()
            formal_parameters.append(self.NFuncionParam())
        return formal_parameters

    # NFuncParam ::= NType VARNAME
    def NFuncionParam(self):
        type_ = self.NType()
        name = None
        if self.sym.get_tag() == DomainTag.VARNAME:
            name = self.sym.get_value()
            self.sym = self.lexer.nextToken()
        else:
            self.report_error()
        return FuncionParam(type_, name)

    # NType ::= (KW_INT | KW_CHAR | KW_BOOL) ('[]')*
    def NType(self):
        prim = None
        if self.sym.get_tag() == DomainTag.KW_INT:
            prim = BasicType.Integer
            self.sym = lexer.nextToken()
        elif self.sym.get_tag() == DomainTag.KW_CHAR:
            prim = BasicType.Char
            self.sym = lexer.nextToken()
        elif self.sym.get_tag() == DomainTag.KW_BOOL:
            prim = BasicType.Boolean
            self.sym = lexer.nextToken()
        else:
            self.report_error()

        n = 0
        while self.sym.get_tag() == DomainTag.LBRBCC:
            n += 1
            self.sym = lexer.nextToken()

        return Type(prim, n)

    # NDeclarationStatement ::= VARNAME (':=' NArithmeticExpression)?
    def NDeclarationAssignment(self):
        name = None
        expr = None
        if self.sym.get_tag() == DomainTag.VARNAME:
            name = self.sym.get_value()
            self.sym = lexer.nextToken()
        else:
            self.report_error()

        if self.sym.get_tag() == DomainTag.ASSIGN:
            self.sym = lexer.nextToken()
            expr = self.NArithmeticExpression()

        return DeclarationAssignment(name, expr)

    # NDeclarationStatements::= NDeclarationStatement (',' NDeclarationStatement)*
    def NDeclarationAssignments(self):
        declaration_assignments = [self.NDeclarationAssignment()]
        while self.sym.get_tag() == DomainTag.COMMA:
            self.sym = lexer.nextToken()
            declaration_assignments.append(self.NDeclarationAssignment())
        return declaration_assignments


    # NExpr ::= NLogicalExpression ('|' NLogicalExpression | '@' NLogicalExpression)*
    def NExpr(self):
        expr = self.NLogicalExpression()
        bin_op_expr = None
        while self.sym.get_tag() == DomainTag.OR_XOR_OP:
            op = self.sym.get_value()
            self.sym = lexer.nextToken()
            expr2 = self.NLogicalExpression()
            if bin_op_expr is not None:
                bin_op_expr = BinOpExpr(bin_op_expr, op, expr2)
            else:
                bin_op_expr = BinOpExpr(expr, op, expr2)
        if bin_op_expr is None:
            return expr
        return bin_op_expr

    # NLogicalExpression ::= NCompareExpression ('&' NCompareExpression)*
    def NLogicalExpression(self):
        expr = self.NCompareExpression()
        bin_op_expr = None
        while self.sym.get_tag() == DomainTag.AND_OP:
            op = self.sym.get_value()
            self.sym = lexer.nextToken()
            expr2 = self.NCompareExpression()
            if bin_op_expr is not None:
                bin_op_expr = BinOpExpr(bin_op_expr, op, expr2)
            else:
                bin_op_expr = BinOpExpr(expr, op, expr2)
        if bin_op_expr is None:
            return expr
        return bin_op_expr

    # NCompareExpression ::= NCallExpression (NCompareOperator NCallExpression)*

    def NCompareExpression(self):
        expr = self.NCallExpression()
        bin_op_expr = None
        while self.sym.get_tag() in self.first["NCmpOp"]:
            op = self.NCompareOperator()
            expr2 = self.NCallExpression()
            if bin_op_expr is not None:
                bin_op_expr = BinOpExpr(bin_op_expr, op, expr2)
            else:
                bin_op_expr = BinOpExpr(expr, op, expr2)
        if bin_op_expr is None:
            return expr
        return bin_op_expr

    # NCompareOperator '==' | '!=' | '<' | '>' | '<=' | '=>'
    def NCompareOperator(self):
        op = ""
        if self.sym.get_tag() == DomainTag.COMPARE_OP:
            op = self.sym.get_value()
            self.sym = lexer.nextToken()
        else:
            self.report_error()
        return op

    # NCallExpression ::= NArithmeticExpression ('<-' NArgs)?
    def NCallExpression(self):
        expr = self.NArithmeticExpression()
        args = []
        if self.sym.get_tag() == DomainTag.LEFT_ARROW:
            self.sym = lexer.nextToken()
            args = self.NArgs()
        if not args:
            return expr
        return CallExpression(expr, args)

    # NArgs ::= NArithmeticExpression (',' NArithmeticExpression)*
    def NArgs(self):
        args = [self.NArithmeticExpression()]
        while self.sym.get_tag() == DomainTag.COMMA:
            self.sym = lexer.nextToken()
            args.append(self.NArithmeticExpression())
        return args

    # NArithmeticExpression ::= NTerm (NAdditiveOperator NTerm)*
    def NArithmeticExpression(self):
        expr = self.NTerm()
        bin_op_expr = None
        # NAdditiveOperator ::= '+' | '-'
        while self.sym.get_tag() == DomainTag.PLUS or self.sym.get_tag() == DomainTag.MINUS:
            op = self.sym.get_value()
            self.sym = lexer.nextToken()
            expr2 = self.NTerm()
            if bin_op_expr is not None:
                bin_op_expr = BinOpExpr(bin_op_expr, op, expr2)
            else:
                bin_op_expr = BinOpExpr(expr, op, expr2)
        if bin_op_expr is None:
            return expr
        return bin_op_expr

    # NTerm ::= NFactor (NMultiplicativeOperator NFactor)*
    def NTerm(self):
        expr = self.NFactor()
        bin_op_expr = None
        # NMultiplicativeOperator ::= '*' | '/' | '%'
        while self.sym.get_tag() == DomainTag.MUL_OP:
            op = self.sym.get_value()
            self.sym = lexer.nextToken()
            expr2 = self.NFactor()
            if bin_op_expr is not None:
                bin_op_expr = BinOpExpr(bin_op_expr, op, expr2)
            else:
                bin_op_expr = BinOpExpr(expr, op, expr2)
        if bin_op_expr is None:
            return expr
        return bin_op_expr

    # NFactor ::= NPower (NPowerOperator NFactor)?
    def NFactor(self):
        expr = self.NPower()
        bin_op_expr = None
        # NPowerOperator ::= '^'
        if self.sym.get_tag() == DomainTag.POWER_OP:
            op = self.sym.get_value()
            self.sym = lexer.nextToken()
            expr2 = self.NFactor()
            bin_op_expr = BinOpExpr(expr, op, expr2)
        if bin_op_expr is None:
            return expr
        return bin_op_expr


    # NPower ::= NArrayExpr | (Nneg NPower) | (NType NPrim)
    def NPower(self):
        expr = None
        # Nneg ::= '!' | '-'

        if self.sym.get_tag() == DomainTag.NOT_OP or self.sym.get_tag() == DomainTag.MINUS:
            op = self.sym.get_value()
            self.sym = lexer.nextToken()
            expr = UnOpExpr(op, self.NPower())
        elif self.sym.get_tag() in self.first["NType"]:
            type_ = self.NType()
            expr2 = self.NPrim()
            expr = AllocExpr(type_, expr2)
        else:
            expr = self.NArrayExpr()
        return expr

    # NArrayExpr ::= NPrimExpr | (NArrayExpr NPrimExpr) | NSTRING
    def NArrayExpr(self):
        expr = None
        if self.sym.get_tag() == DomainTag.STRING:
            expr = self.NStringConstant()
        elif self.sym.get_tag() in self.first["NPrim"]:
            expr = self.NPrim()
        else:
            expr1 = self.NArrayExpr()
            expr2 = self.NPrim()
            expr = BinOpExpr(expr1, "at", expr2)
        return expr

    # NPrim ::= VARNAME | NConst | '(' NExpr ')'
    def NPrim(self):
        expr = None
        if self.sym.get_tag() == DomainTag.VARNAME:
            name = self.sym.get_value()
            self.sym = lexer.nextToken()
            expr = Var(name)
        elif self.sym.get_tag() == DomainTag.LB:
            self.sym = lexer.nextToken()
            expr = self.NExpr()
            if self.sym.get_tag() == DomainTag.RB:
                self.sym = lexer.nextToken()
            else:
                self.report_error()
        else:
            expr = self.NConst()
        return expr

    def NStringConstant(self):
        return String(self.sym.get_value)

    # NConst ::= INTEGER | CHAR | REF_CONST | STRING | KW_TRUE | KW_FALSE
    def NConst(self):
        expr = None
        if self.sym.get_tag() in [DomainTag.INTEGER, DomainTag.BOOLEAN, DomainTag.CHAR_CONST, DomainTag.KW_NULL]:
            value = self.sym.get_value()
            if self.sym.get_tag() == DomainTag.INTEGER:
                expr = ConstExpr(Type(BasicType.Integer, 0), value)
            elif self.sym.get_tag() == DomainTag.CHAR_CONST:
                expr = ConstExpr(Type(BasicType.Char, 0), value)
            elif self.sym.get_tag() == DomainTag.BOOLEAN:
                expr = ConstExpr(Type(BasicType.Boolean, 0), value)
            elif self.sym.get_tag() == DomainTag.KW_NULL:
                expr = ConstExpr(Type(None, 0), value)
            self.sym = lexer.nextToken()
        else:
            self.report_error()
        return expr


if __name__ == "__main__":
    input_file = "input2.txt"
    with open(input_file, "r") as file:
        input_text = file.read()
    lexer = Lexer(input_text)
    # tokens = []
    # while True:
    #     tokens.append(lexer.nextToken())
    #     if tokens[-1].tag == DomainTag.END:
    #         break
    #
    # for i in tokens:
    #     print(i.value, i.tag, i.fragment.get_simplified_string())
    parser = Parser(lexer)
    parser.print_tree()
