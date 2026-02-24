import sys
from pprint import pprint

import parser_edsl.parser_edsl as pe
from model import *

# lexical structure
VARNAME = pe.Terminal('VARNAME', '[a-zA-Z_\\?\\$\\@\\.\\#\\~][a-zA-Z0-9_\\?\\$\\@\\.\\#\\~]*', str)
REF_CONST = pe.Terminal('REF_CONST', 'nil', str)
INT_CONST = pe.Terminal('INT_CONST', '(0x([0-9a-fA-F]{1,8}))|(\d+)', str, priority=7)
CHAR_CONST = pe.Terminal('CHAR_CONST', '(\'.?\')|(\\$.+\\$)', str)
STRING_CONST = pe.Terminal('STRING_CONST', '\".*\"', str)


def make_keyword(image):
    return pe.Terminal(image, image, lambda name: None, priority=10)


KW_INT, KW_CHAR, KW_BOOL = \
    map(make_keyword, 'int char bool'.split())
KW_XOR, KW_OR, KW_MOD, KW_AND, KW_NOT, KW_TRUE, KW_FALSE = \
    map(make_keyword, '^^ || % && ! tt ff'.split())

NProgram, NFunction, NProcedure, NFunctionHeader, NProcedureHeader = \
    map(pe.NonTerminal, 'Program Func Procedure FunctionHeader ProcedureHeader'.split())

NSections, NSection, NParameters, NType, NPrimitiveType = \
    map(pe.NonTerminal, 'Sections Section Parameters Type PrimitiveType'.split())

NStatements, NStatement, NDeclarationStatement, NAssignmentStatement, NArrayAssignmentStatement, NArrayCall, NFunctionCallStatement = \
    map(pe.NonTerminal, 'Statements Statement DeclarationStatement AssignmentStatement NArrayAssignmentStatement NArrayCall FunctionCallStatement'.split())

NIfStatement, NElseBlock, NLoopWithPreconditionStatements, NLoopWithPostconditionStatement = \
    map(pe.NonTerminal, 'IfStatement ElseBlock LoopWithPreconditionStatements LoopWithPostconditionStatement'.split())

NVarDeclarations, NVarDeclaration, NExpression, NLogicalExpression = \
    map(pe.NonTerminal, 'VarDeclarations VarDeclaration Expression LogicalExpression'.split())

NCompareExpression, NCompareOperator, NArithmeticExpression, NPowExpression, NAdditiveOperator, NMultiplicativeOperator = \
    map(pe.NonTerminal, 'CompareExpression NCompareOperator ArithmeticExpression PowExpression AdditiveOperator MultiplicativeOperator'.split())

NTerm, NFactor, NSpec, NArgs, NExtendedVar, NConst = \
    map(pe.NonTerminal, 'Term Factor Spec Args ExtendedVar Const'.split())

NForCycleVarDeclaration, NForCycleTo, NForCycleHead, NMultiplicativeExpression = \
    map(pe.NonTerminal, 'ForCycleVarDeclaration ForCycleTo ForCycleHead MultiplicativeExpression'.split())

# grammar
NProgram |= NFunction, lambda fn: [fn]
NProgram |= NProcedure, lambda pr: [pr]
NProgram |= NProgram, NFunction, lambda _, fn: _ + [fn]
NProgram |= NProgram, NProcedure, lambda _, pr: _ + [pr]

NFunction |= NFunctionHeader, NStatements, 'endfunc', Function
NProcedure |= NProcedureHeader, NStatements, 'endproc', Procedure
NFunctionHeader |= 'func', VARNAME, '(', NSections, ')', '->', NType, FunctionHeaderFull
NProcedureHeader |= 'proc', VARNAME, '(', NSections, ')', ProcedureHeaderFull

NType |= NPrimitiveType
NType |= '[', NType, ']', ArrayType
NPrimitiveType |= KW_INT, lambda: PrimitiveType.Integer
NPrimitiveType |= KW_CHAR, lambda: PrimitiveType.Char
NPrimitiveType |= KW_BOOL, lambda: PrimitiveType.Boolean

NSections |= NSection, lambda sec: [sec]
NSections |= NSections, ';', NSection, lambda secs, sec: secs + [sec]
NSection |= VARNAME, '->', NType, lambda p, t: Section(t, [p])
NSection |= NParameters, ',', VARNAME, '->', NType, lambda ps, p, t: Section(t, ps + [p])
NParameters |= VARNAME, lambda p: [p]
NParameters |= NParameters, ',', VARNAME, lambda ps, p: ps + [p]

# statements
NStatements |= NStatement, lambda st: [st]
NStatements |= NStatements, ';', NStatement, lambda stmts, st: stmts + [st]
NStatement |= NDeclarationStatement
NStatement |= NAssignmentStatement
NStatement |= NArrayAssignmentStatement
NStatement |= NFunctionCallStatement
NStatement |= NIfStatement
NStatement |= NLoopWithPreconditionStatements
NStatement |= NLoopWithPostconditionStatement
NStatement |= 'check', NExpression, WarningStatement
# end statements

NDeclarationStatement |= NVarDeclaration, '->', NType, lambda dec, t: DeclarationStatement(t, [dec])
NDeclarationStatement |= NVarDeclarations, ',', NVarDeclaration, '->', NType, lambda decls, dec, t: DeclarationStatement(t, decls + [dec])
NVarDeclarations |= NVarDeclaration, lambda dec: [dec]
NVarDeclarations |= NVarDeclarations, ',', NVarDeclaration, lambda decls, dec: decls + [dec]
NVarDeclaration |= VARNAME, lambda name: VarDeclaration(name, None)
NVarDeclaration |= VARNAME, '=', NExpression, lambda name, expr: VarDeclaration(name, expr)
#NVarDeclaration |= NAssignmentStatement # VARNAME, '=', NExpression, lambda name, expr: VarDeclaration(name, expr)

NAssignmentStatement |= VARNAME, '=', NExpression, lambda l, r: AssignmentStatement(l, r)
NArrayAssignmentStatement |= NArrayCall, '=', NExpression, lambda l, r: AssignmentStatement(l, r)
NFunctionCallStatement |= VARNAME, '(', NArgs, ')', FunctionCallStatement
NArgs |= NExpression, lambda arg: [arg]
NArgs |= NArgs, ',', NExpression, lambda args, arg: args + [arg]

NExtendedVar |= VARNAME, ExtendedVar
NArrayCall |= NSpec, '[', NExpression, ']', ArrayCall

NSpec |= NFunctionCallStatement
NSpec |= '[', NType, VARNAME, ']', ArrayAllocation
NSpec |= '[', NType, INT_CONST, ']', ArrayAllocation
NSpec |= NExtendedVar
NSpec |= NArrayCall
NSpec |= NConst

NConst |= INT_CONST, Const
NConst |= CHAR_CONST, Const
NConst |= STRING_CONST, Const
NConst |= REF_CONST, Const
NConst |= KW_TRUE, Const
NConst |= KW_FALSE, Const

NExpression |= NLogicalExpression
NExpression |= NExpression, KW_OR, NLogicalExpression, BinOperatorExpression
NExpression |= NExpression, KW_XOR, NLogicalExpression, BinOperatorExpression

NLogicalExpression |= NCompareExpression
NLogicalExpression |= NLogicalExpression, KW_AND, NCompareExpression, BinOperatorExpression

def make_op_lambda(op):
    return lambda: op


for op in ('==', '!=', '<', '>', '<=', '>='):
    NCompareOperator |= op, make_op_lambda(op)

NCompareExpression |= NArithmeticExpression
NCompareExpression |= NCompareExpression, NCompareOperator, NArithmeticExpression, BinOperatorExpression

NAdditiveOperator |= '+', lambda: '+'
NAdditiveOperator |= '-', lambda: '-'

NArithmeticExpression |= NMultiplicativeExpression
NArithmeticExpression |= NMultiplicativeExpression, NAdditiveOperator, NMultiplicativeExpression, BinOperatorExpression
# NArithmeticExpression |= NPowExpression
# NArithmeticExpression |= NPowExpression, NAdditiveOperator, NPowExpression, BinOperatorExpression

NMultiplicativeExpression |= NTerm
NMultiplicativeExpression |= NMultiplicativeExpression, NMultiplicativeOperator, NTerm, BinOperatorExpression
# NPowExpression |= NTerm
# NPowExpression |= NTerm, '^', NPowExpression, lambda l, r: BinOperatorExpression(l, '^', r)

NMultiplicativeOperator |= '*', lambda: '*'
NMultiplicativeOperator |= '/', lambda: '/'
NMultiplicativeOperator |= '%', lambda: '%'

NTerm |= NFactor
NTerm |= NFactor, '^', NTerm, lambda l, r: BinOperatorExpression(l, '^', r)

NFactor |= KW_NOT, NSpec, lambda expr: UnaryOperatorExpression('!', expr)
NFactor |= '-', NSpec, lambda expr: UnaryOperatorExpression('-', expr)
NFactor |= NSpec

NIfStatement |= 'if', NExpression, 'then', NStatements, 'endif', lambda expr, stmts: IfStatement(condition=expr, statements=stmts, elseblocks=list())
NIfStatement |= 'if', NExpression, 'then', NStatements, NElseBlock, 'endif', IfStatement
NElseBlock |= 'else', NStatements, lambda stmts: [ElseBlock(condition=None, statements=stmts)]
NElseBlock |= 'elseif', NExpression, 'then', NStatements, NElseBlock, lambda expr, stmts, block: [ElseBlock(condition=expr, statements=stmts)] + block

NLoopWithPreconditionStatements |= 'while', NExpression, 'do', NStatements, 'endwhile', WhileDoStatement

NLoopWithPreconditionStatements |= NForCycleHead, 'do', NStatements, 'endfor', ForDoStatement

NForCycleHead |= 'for', NForCycleVarDeclaration, 'to', NForCycleTo, lambda decl, to: ForCycleHead(decl, to, step=None)
NForCycleHead |= 'for', NForCycleVarDeclaration, 'to', NForCycleTo, 'step', INT_CONST, lambda decl, to, step: ForCycleHead(decl, to, step=step)

NForCycleVarDeclaration |= VARNAME, '=', INT_CONST, '->', KW_INT, lambda name, expr: ForDoVarDeclaration(name=name, value=expr, type=PrimitiveType.Integer)
NForCycleVarDeclaration |= VARNAME, '=', CHAR_CONST, '->', KW_CHAR, lambda name, expr: ForDoVarDeclaration(name=name, value=expr, type=PrimitiveType.Char)
NForCycleVarDeclaration |= VARNAME, '=', NExpression, lambda name, expr: ForDoVarDeclaration(name=name, value=expr, type=None)


NForCycleTo |= NExpression
# NForCycleTo |= INT_CONST, lambda i: i
# NForCycleTo |= CHAR_CONST, lambda c: c

NLoopWithPostconditionStatement |= 'repeat', NStatements, 'until', NExpression, RepeatUntilStatement

parser = pe.Parser(NProgram)
# parser.print_table()
parser.add_skipped_domain('\s')
parser.add_skipped_domain('\\(\\*.*\\*\\)')
assert parser.is_lalr_one()

for filename in sys.argv[1:]:
    try:
        with open(filename) as f:
            tree = parser.parse(f.read())
            pprint(tree)
    except pe.Error as e:
        print(f'Ошибка {e.pos}: {e.message}')
    except Exception as e:
        print(e)