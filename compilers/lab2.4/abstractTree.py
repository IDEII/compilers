from dataclasses import dataclass
from typing import List


class BasicType:
    Integer = 'int'
    Char = 'char'
    Boolean = 'bool'


@dataclass
class Type:
    base: BasicType or None
    arrayLevel: int

    def toString(self, indent: int) -> str:
        return str(self)


@dataclass
class FuncionParam:
    type: Type
    name: str

    def toString(self, indent: int) -> str:
        return str(self)


@dataclass
class FunctionHeaderTypeName:
    type: Type
    name: str



class Expr:
    pass


@dataclass
class Var(Expr):
    name: str

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("VariableExpr", indent)
        builder.append("name", self.name)
        return builder.toString()


@dataclass
class ConstExpr(Expr):
    type: Type
    value: str

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("ConstExpr", indent)
        builder.append("type", self.type).append("value", self.value)
        return builder.toString()


@dataclass
class String(Expr):
    value: str

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("StringConstExpr", indent)
        builder.append("value", str(self.value))
        return builder.toString()


@dataclass
class FunctionCallExpr(Expr):
    expr: Expr
    actualParameters: List[Expr]

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("FunctionInvocationExpr", indent)
        builder.append("Expr", self.expr).append("actualParameters", self.actualParameters)
        return builder.toString()


@dataclass
class BinOpExpr(Expr):
    left: Expr
    op: str
    right: Expr

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("BinOpExpr", indent)
        builder.append("left", self.left).append("op", self.op).append("right", self.right)
        return builder.toString()


@dataclass
class AllocExpr(Expr):
    type: Type
    expr: Expr

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("AllocExpr", indent)
        builder.append("type", self.type).append("expr", self.expr)
        return builder.toString()


@dataclass
class UnOpExpr(Expr):
    op: str
    expr: Expr

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("UnOpExpr", indent)
        builder.append("op", self.op).append("expr", self.expr)
        return builder.toString()


class Statement:
    pass


@dataclass
class DeclarationAssignment:
    name: str
    expr: Expr

    def toString(self, indent: int) -> str:
        return str(self)

@dataclass
class DeclarationStatement(Statement):
    type: Type
    declarationAssignments: list[DeclarationAssignment]

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("DeclarationStatement", indent)
        builder.append("type", self.type).append("declarationAssignments", self.declarationAssignments)
        return builder.toString()


@dataclass
class AssignmentStatement(Statement):
    left: Expr
    right: Expr

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("AssignmentStatement", indent)
        builder.append("left", self.left).append("right", self.right)
        return builder.toString()


@dataclass
class CallExpression(Statement):
    name: str
    actualParameters: list[Expr]

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("InvocationStatement", indent)
        builder.append("name", self.name).append("actualParameters", self.actualParameters)
        return builder.toString()


@dataclass
class IfStatement(Statement):
    condition: Expr
    thenBranch: list[Statement]
    elseBranch: list[Statement]

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("IfStatement", indent)
        builder.append("condition", self.condition).append("thenBranch", self.thenBranch).append("elseBranch",
                                                                                                 self.elseBranch)
        return builder.toString()


@dataclass
class WhileStatement(Statement):
    condition: Expr
    body: list[Statement]

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("PreWhileStatement", indent)
        builder.append("condition", self.condition).append("body", self.body)
        return builder.toString()


@dataclass
class PostWhileStatement(Statement):
    condition: Expr
    body: list[Statement]

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("PostWhileStatement", indent)
        builder.append("condition", self.condition).append("body", self.body)
        return builder.toString()


@dataclass
class ForStatement(Statement):
    start: Expr
    end: Expr
    variable: str
    body: list[Statement]

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("ForStatement", indent)
        builder.append("start", self.start).append("end", self.end).append("variable", self.variable).append("body",
                                                                                                             self.body)
        return builder.toString()


@dataclass
class ReturnStatement(Statement):
    expr: Expr

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("ReturnStatement", indent)
        builder.append("expr", self.expr)
        return builder.toString()




@dataclass
class FunctionHeader:
    type: Type
    name: str
    FuncParams: list[FuncionParam]

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("FunctionHeader", indent)
        builder.append("type", self.type).append("name", self.name).append("formalParameters", self.FuncParams)
        return builder.toString()

@dataclass
class FunctionDeclaration:
    header: FunctionHeader
    body: list[Statement]

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("FunctionDeclaration", indent)
        builder.append("header", self.header).append("body", self.body)
        return builder.toString()

@dataclass
class Program:
    functionDeclarations: list[FunctionDeclaration]

    def toString(self, indent: int) -> str:
        builder = PrettyBuilder("Program", 0)
        builder.append("functionDeclarations", self.functionDeclarations)
        return builder.toString()



class AbstractTree:
    def __init__(self, program: Program):
        self.program = program

    def getProgram(self) -> Program:
        return self.program

    def __str__(self) -> str:
        return str(self.program)

    def print(self) -> None:
        print(self.program.toString(0))



class PrettyBuilder:
    def __init__(self, name, indent):
        self.string_builder = []
        self.string_builder.append(f"{name}(")
        self.indent = indent + len(self.string_builder[0])
        self.count = 0

    def append(self, name, value):
        builder = []
        if self.count > 0:
            builder.append(",\n" + " " * max(0, self.indent))
        builder.append(f"{name}=")
        if isinstance(value, list):
            builder.append("[")
            size = self.indent + len(name) + 2
            if len(value) > 0:
                if value[0]:
                    builder.append(value[0].toString(size))
                    for item in value[1:]:
                        if item is not None:
                            builder.append(",\n" + " " * max(0, size) + item.toString(size))
                        else:
                            builder.append(",\n" + " " * max(0, size) + "null")
            builder.append("]")
        elif hasattr(value, 'toString'):
            size = self.indent + len(name) + 1
            builder.append(value.toString(size))
        else:
            builder.append(value)

        self.string_builder.append("".join(builder))
        self.count += 1
        return self

    def toString(self):
        return "".join(self.string_builder) + ")"
