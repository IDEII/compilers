import abc
import parser_edsl.parser_edsl as pe
from pprint import pprint



class Type(abc.ABC):
    pass


import enum
import abc
from dataclasses import dataclass
from typing import Any, Optional


class Type(abc.ABC):
    pass


# лексическая структура
INTEGER = pe.Terminal('INTEGER', '[A-Za-z0-9]+\\{[0-9]+\\}\\$|[0-9]+', str, priority=7)
CHAR = pe.Terminal('CHAR', '\\$\\"[A-Za-z0-9]\\"', str)
STRING = pe.Terminal('STRING', '\\".*\\"', str)
VARNAME = pe.Terminal('VARNAME', '\\{[A-Za-z0-9]*\\}', str)
REF_CONST = ('REFCONST', 'null', str)


class BasicType(enum.Enum):
    Integer = 'int'
    Char = 'char'
    Boolean = 'bool'


@dataclass
class ArrayType:
    type: Type


class FunctionHeader(abc.ABC):
    pass


class Body(abc.ABC):
    pass


class Statement(abc.ABC):
    pass


class Expr(abc.ABC):
    pass


@dataclass
class BinOperatorExpression(Expr):
    left: Expr
    operator: str
    right: Expr


@dataclass
class UnaryOperatorExpression(Expr):
    operator: str
    expr: Expr


@dataclass
class VarDeclaration:
    name: Any
    expr: Optional[Expr]


@dataclass
class DeclarationStatement(Statement):
    type: Type or BasicType
    var_declarations: list[VarDeclaration]


@dataclass
class AssignmentStatement(Statement):
    left: Any
    right: Any


@dataclass
class FunctionCallStatement(Statement):
    name: Any
    args: list[Expr]


@dataclass
class ElseBlock:
    statements: list[Statement]


@dataclass
class IfStatement(Statement):
    condition: Expr
    statements: list[Statement]
    elseblocks: list[ElseBlock]


@dataclass
class WhileStatement(Statement):
    condition: Expr
    statements: list[Statement]


@dataclass
class CycleVar:
    name: Any
    value: Any
    type: Optional[Type]


@dataclass
class ForCycleHead:
    var_declaration: CycleVar
    forfrom: Any
    to: Any
    step: Optional[Any]


@dataclass
class ForStatement:
    head: ForCycleHead
    body: list[Statement]


@dataclass
class PostLoopStatement(Statement):
    statements: list[Statement]
    condition: Expr


@dataclass
class ExtendedVar:
    value: Any


@dataclass
class ArrayCall:
    array: Any
    ref: Any


@dataclass
class ArrayAllocation:
    type: Type
    size: Any


@dataclass
class Const:
    value: Any


@dataclass
class Function:
    header: FunctionHeader
    body: Body


@dataclass
class FunctionParameter:
    param_name: VARNAME
    param_type: Any


@dataclass
class FunctionHeaderFull:
    name: Any
    params: list[FunctionParameter]
    return_type: Type or BasicType


@dataclass
class ReturnStatement:
    return_value: Expr


@dataclass
class FunctionHeaderShort:
    name: Any


def make_keyword(image):
    return pe.Terminal(image, image, lambda name: None, priority=10)


KW_INT, KW_CHAR, KW_BOOL = \
    map(make_keyword, 'int char bool'.split())

KW_XOR, KW_OR, KW_MOD, KW_AND, KW_NOT, KW_TRUE, KW_FALSE = \
    map(make_keyword, '@ | % & ! TRUE FALSE'.split())

NProgram, NFunction, NFunctionHeader, NFuncBody = \
    map(pe.NonTerminal, 'Program Func FunctionHeader FuncBody'.split())

NFuncParams, NFuncParam, NType, NBasicType = \
    map(pe.NonTerminal, 'FuncParams FuncParam Type BasicType'.split())

NStatements, NStatement, NDeclarationStatement, NAssignmentStatement, NArrayAssignmentStatement, NArrayCall, NFunctionCallStatement, NVarDeclarationDown, NFuncParamStatements, NArrayFuncCall, NFunctionCallStatement2 = \
    map(pe.NonTerminal,
        'Statements Statement DeclarationStatement AssignmentStatement NArrayAssignmentStatement NArrayCall FunctionCallStatement VarDeclarationDown FuncParamStatements ArrayFuncCall FunctionCallStatement2'.split())

NIfStatement, NIfElseStatement, NLoopWhileStatement, NLoopPostStatement, NReturnStatement, NElseBlock = \
    map(pe.NonTerminal,
        'IfStatement IfElseStatement LoopWhileStatement LoopPostStatement ReturnStatement ElseBlock'.split())

NVarDeclarations, NVarDeclaration, NExpr, NLogicalExpression = \
    map(pe.NonTerminal, 'VarDeclarations VarDeclaration Expr LogicalExpression'.split())

NCompareExpression, NCompareOperator, NArithmeticExpression, NPowExpression, NAdditiveOperator, NMultiplicativeOperator = \
    map(pe.NonTerminal,
        'CompareExpression NCompareOperator ArithmeticExpression PowExpression AdditiveOperator MultiplicativeOperator'.split())

NTerm, NFactor, NSpec, NArgs, NExtendedVar, NConst = \
    map(pe.NonTerminal, 'Term Factor Spec Args ExtendedVar Const'.split())

NCallExpression, NPrim = \
    map(pe.NonTerminal, 'CallExpression Prim'.split())

NLoopForStatement, NCycle, NCycleVar, NForCycleHead, NMultiplicativeExpression = \
    map(pe.NonTerminal, 'LoopForStatement Cycle CycleVar ForCycleHead MultiplicativeExpression'.split())


NProgram |= NFunction, lambda st: [st]
NProgram |= NProgram, NFunction, lambda _, st: _ + [st]

NFunction |= NFunctionHeader, "=", NFuncBody, lambda header, body: Function(header=header, body=body)

NFunctionHeader |= NType, VARNAME, "<-", NFuncParamStatements, lambda t, name, param: FunctionHeaderFull(name=name, return_type=t, params=[param])
NFunctionHeader |= "void", VARNAME, "<-", NFuncParamStatements, lambda name, param: FunctionHeaderFull(name=name, return_type='void', params=[param])
NFunctionHeader |= "void", VARNAME, lambda name: FunctionHeaderShort(name=name)

NType |= NBasicType
NType |= NType, "[]", ArrayType
NBasicType |= KW_INT, lambda: BasicType.Integer
NBasicType |= KW_CHAR, lambda: BasicType.Char
NBasicType |= KW_BOOL, lambda: BasicType.Boolean

NFuncBody |= NStatements, '.'

NFuncParamStatements |= NFuncParams
NFuncParams |= NFuncParams, ',', NFuncParam, lambda ps, p: ps + [p]
NFuncParams |= NFuncParam, lambda p: [p]

NFuncParam |= NType, VARNAME, lambda type, name: FunctionParameter(param_type=type, param_name=name)

NStatements |= NStatements, ';', NStatement, lambda stmts, st: stmts + [st]
NStatements |= NStatement, lambda st: [st]

NStatement |= NDeclarationStatement
NStatement |= NAssignmentStatement
NStatement |= NCallExpression
NStatement |= NIfStatement
NStatement |= NIfElseStatement
NStatement |= NLoopWhileStatement
NStatement |= NLoopForStatement
NStatement |= NReturnStatement
NStatement |= NLoopPostStatement

NDeclarationStatement |= NType, NVarDeclarations, lambda type, decls: DeclarationStatement(type, decls)

NVarDeclarations |= NVarDeclarations, ',', NVarDeclaration, lambda decls, decl:  decls + [decl]
NVarDeclarations |= NVarDeclaration, lambda decl: [decl]
NVarDeclaration |= VARNAME, ':=', NArithmeticExpression, lambda name, value: VarDeclaration(name, value)
NVarDeclaration |= VARNAME, lambda name: VarDeclaration(name, None)

NAssignmentStatement |= NSpec, ':=', NExpr, lambda l, r: AssignmentStatement(l, r)

NSpec |= '(', NType, NPrim, ')', ArrayAllocation
NSpec |= NSpec, NPrim, ArrayCall
NSpec |= NPrim

NPrim |= NConst
NPrim |= VARNAME
NPrim |= '(', NExpr, ')'

NConst |= INTEGER
NConst |= CHAR
NConst |= REF_CONST
NConst |= STRING
NConst |= KW_TRUE
NConst |= KW_FALSE

NExpr |= NLogicalExpression
NExpr |= NExpr, '|', NLogicalExpression, BinOperatorExpression
NExpr |= NExpr, '@', NLogicalExpression, BinOperatorExpression

NLogicalExpression |= NCompareExpression
NLogicalExpression |= NLogicalExpression, '&', NCompareExpression, BinOperatorExpression

NCompareOperator |= '==', lambda: '=='
NCompareOperator |= '!=', lambda: '!='
NCompareOperator |= '<', lambda: '<'
NCompareOperator |= '>', lambda: '>'
NCompareOperator |= '<=', lambda: '<='
NCompareOperator |= '=>', lambda: '=>'

NCompareExpression |= NCallExpression
NCompareExpression |= NCompareExpression, NCompareOperator, NCallExpression, BinOperatorExpression

NCallExpression |= NArithmeticExpression
NCallExpression |= VARNAME, '<-', NArgs, FunctionCallStatement
NArgs |= NArithmeticExpression, lambda arg: [arg]
NArgs |= NArgs, ',', NArithmeticExpression, lambda args, arg: args + [arg]

NAdditiveOperator |= '+', lambda: '+'
NAdditiveOperator |= '-', lambda: '-'

NArithmeticExpression |= NMultiplicativeExpression
NArithmeticExpression |= NArithmeticExpression, NAdditiveOperator, NMultiplicativeExpression, BinOperatorExpression

NMultiplicativeExpression |= NTerm
NMultiplicativeExpression |= NMultiplicativeExpression, NMultiplicativeOperator, NTerm, BinOperatorExpression

NMultiplicativeOperator |= '*', lambda: '*'
NMultiplicativeOperator |= '/', lambda: '/'
NMultiplicativeOperator |= '%', lambda: '%'

NTerm |= NFactor
NTerm |= NFactor, '^', NTerm, lambda l, r: BinOperatorExpression(l, '^', r)

NFactor |= '!', NSpec, lambda spec: UnaryOperatorExpression('!', spec)
NFactor |= '-', NSpec, lambda spec: UnaryOperatorExpression('-', spec)
NFactor |= NSpec

NIfElseStatement |= NExpr, 'then', NStatements, NElseBlock, '.', lambda expr, stmts, stmts2: IfStatement(condition=expr, statements=stmts, elseblocks=stmts2)

NIfStatement |= NExpr, 'then', NStatements, '.', lambda expr, stmts: IfStatement(condition=expr, statements=stmts, elseblocks=list())

NElseBlock |= 'else', NStatements, lambda stmts: [ElseBlock(statements=stmts)]

NLoopWhileStatement |= NExpr, 'loop', NStatements, '.', WhileStatement
NLoopForStatement |= NCycle, NStatements, '.', ForStatement
NLoopPostStatement |= 'loop', NStatements, 'while', NExpr, '.', lambda stmts, expr: PostLoopStatement(statements=stmts, condition=expr)

NCycle |= NExpr, '~', NExpr, 'loop', NCycleVar, lambda value1, value2, name: ForCycleHead(forfrom=value1, to=value2, step=None, var_declaration=name)
NCycleVar |= VARNAME, lambda name:  VarDeclaration(name, None)

NReturnStatement |= 'return', NExpr, lambda expr: ReturnStatement(return_value=expr)

parser = pe.Parser(NProgram)
# parser.print_table()
assert parser.is_lalr_one()

# пробельные символы
parser.add_skipped_domain('[\\s\\n]')
# комментарии вида # … #
# комментарии вида ##
parser.add_skipped_domain('\\#.*\\#')
parser.add_skipped_domain('\\#\\#.*')
filename = 'file.txt'
try:
    with open(filename) as f:
        tree = parser.parse(f.read())
        pprint(tree)
except pe.Error as e:
    print(f'Ошибка {e.pos}: {e.message}')
except Exception as e:
    print(e)
