import enum
import abc
from dataclasses import dataclass
from typing import Any, Optional

class Type(abc.ABC):
    pass

class PrimitiveType(enum.Enum):
    Integer = 'int'
    Char = 'char'
    Boolean = 'bool'

@dataclass
class ArrayType:
    type: Type

class FunctionHeader(abc.ABC):
    pass

class ProcedureHeader(abc.ABC):
    pass

class Body(abc.ABC):
    pass

class Statement(abc.ABC):
    pass

class Expression(abc.ABC):
    pass

@dataclass
class IntExpression(Expression):
    value: Any

@dataclass
class BinOperatorExpression(Expression):
    left: Expression
    operator: str
    right: Expression

@dataclass
class UnaryOperatorExpression(Expression):
    operator: str
    expression: Expression

@dataclass
class VarDeclaration:
    name: Any
    expression: Optional[Expression]

@dataclass
class DeclarationStatement(Statement):
    type: Type or PrimitiveType
    var_declarations: list[VarDeclaration]

@dataclass
class AssignmentStatement(Statement):
    left: Any
    right: Any

@dataclass
class FunctionCallStatement(Statement):
    name: Any
    args: list[Expression]

@dataclass
class ElseBlock:
    condition: Optional[Expression] # elseif
    statements: list[Statement]

@dataclass
class IfStatement(Statement):
    condition: Expression
    statements: list[Statement]
    elseblocks: list[ElseBlock]


@dataclass
class WarningStatement(Statement):
    expr: Expression

@dataclass
class WhileDoStatement(Statement):
    condition: Expression
    statements: list[Statement]




@dataclass
class ForDoVarDeclaration:
    name: Any
    value: Any
    type: Optional[Type]

@dataclass
class ForCycleHead:
    var_declaration: ForDoVarDeclaration
    to: Any
    step: Optional[Any]

@dataclass
class ForDoStatement(Statement):
    head: ForCycleHead
    statements: list[Statement]

@dataclass
class RepeatUntilStatement(Statement):
    statements: list[Statement]
    condition: Expression

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
class Section:
    type: Type or PrimitiveType
    parameters: list[Any]

@dataclass
class Function:
    header: FunctionHeader
    body: Body

@dataclass
class Procedure:
    header: ProcedureHeader
    body: Body

@dataclass
class FunctionHeaderFull:
    name: Any
    sections: list[Section]
    return_type: Type or PrimitiveType

@dataclass
class ProcedureHeaderFull(ProcedureHeader):
    name: Any
    sections: list[Section]
