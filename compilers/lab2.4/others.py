class Position:
    def __init__(self, line, column, index):
        self.line = line
        self.column = column
        self.index = index

    def get_simplified_string(self):
        return f"({self.line}, {self.column})"


class Fragment:
    def __init__(self, start_position, end_position):
        self.start_position = start_position
        self.end_position = end_position

    def __str__(self):
        return f"{self.start_position}-{self.end_position}"

    def get_simplified_string(self):
        return f"{self.start_position.get_simplified_string()}-{self.end_position.get_simplified_string()}"


class Token:
    def __init__(self, value, tag, fragment):
        self.value = value
        self.tag = tag
        self.fragment = fragment

    def get_value(self):
        return self.value

    def get_tag(self):
        return self.tag

    def get_fragment_position(self):
        return self.fragment


class DomainTag:
    VARNAME = 1
    INTEGER = 2
    CHAR_CONST = 3
    STRING = 4
    OR_XOR_OP = 5
    AND_OP = 6
    PLUS = 7
    MINUS = 37
    COMPARE_OP = 8
    MUL_OP = 11
    POWER_OP = 12
    NOT_OP = 13
    EQUAL = 14
    ASSIGN = 15
    DOT = 16
    LEFT_ARROW = 17
    COMMA = 18
    TILDE = 19
    LBRBCC = 20
    LB = 21
    RB = 22
    SEMICOLON = 23
    KW_INT = 24
    KW_BOOL = 25
    KW_RETURN = 26
    KW_VOID = 27
    KW_CHAR = 28
    KW_LOOP = 29
    KW_THEN = 30
    KW_ELSE = 31
    KW_NULL = 32
    KW_WHILE = 33
    END = 34
    NONE = 35
    BOOLEAN = 36
