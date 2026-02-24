import re
from others import *


class Lexer:
    def __init__(self, input):
        self.input = input
        self.pos = 0

    def nextToken(self):
        VARNAME = r'(\{[A-Za-z0-9]*\})'
        CHAR = r'(\$\"[A-Za-z0-9]\")'
        STRING = r'\"[A-Za-z0-9 ]*\"'
        REF_CONST = r'null'
        INTEGER = r'([A-Za-z0-9]+\{[0-9]+\}\$)|([0-9]+)'
        BOOLEAN = r'true|false'
        OR_XOR = r'(\|)|(\@)'
        AND = r'(\&)'
        ASSIGN = r'(\:\=)'
        KW_LOOP = r'loop'
        KW_WHILE = r'while'
        tilda = r'(\~)'
        KW_ELSE = r'else'
        COMMENT = r'(\#\#.*\n)|(\#.*\#)'
        SPACES = r'[ \n\t]'
        LB = r'(\()'
        RB = r'(\))'
        LBRBCC = r'(\[\])'
        COMPARE_OPS = r'(\=\=)|(\!\=)|(\<\=)|(\>\=)|\>|(\<(?!\-))'
        POWER = r'(\^)'
        MULT_OPS = r'(\*)|(\/)|(\%)'
        LEFT_ARROW = r'\<\-'
        PLUS = r'\+'
        MINUS = r'(?<!\<)\-'
        NOT = r'(\!)'
        DOT = r'(\.)'
        KW_INT = r'int'
        KW_CHAR = r'char'
        KW_BOOL = r'bool'
        KW_RETURN = r'return'
        SEMICOLON = r'(\;)'
        KW_VOID = r'void'
        KW_THEN = r'then'
        COMMA = r'(\,)'
        EQUAL = r'(\=)'

        if self.pos >= len(self.input):
            return Token("$", DomainTag.END, Fragment(self.findPosition(self.input, len(self.input)),
                                                      self.findPosition(self.input, len(self.input))))

        matcher = re.compile(
            VARNAME + '|' +
            CHAR + '|' +
            STRING + '|' +
            REF_CONST + '|' +
            INTEGER + '|' +
            BOOLEAN + '|' +
            COMMA + '|' +
            OR_XOR + '|' +
            AND + '|' +
            ASSIGN + '|' +
            KW_LOOP + '|' +
            KW_WHILE + '|' +
            tilda + '|' +
            KW_ELSE + '|' +
            COMMENT + '|' +
            SPACES + '|' +
            LB + '|' +
            RB + '|' +
            LBRBCC + '|' +
            COMPARE_OPS + '|' +
            POWER + '|' +
            MULT_OPS + '|' +
            LEFT_ARROW + '|' +
            PLUS + '|' +
            MINUS + '|' +
            NOT + '|' +
            DOT + '|' +
            KW_INT + '|' +
            KW_CHAR + '|' +
            KW_BOOL + '|' +
            KW_RETURN + '|' +
            SEMICOLON + '|' +
            KW_VOID + '|' +
            EQUAL + '|' +
            KW_THEN
        ).match(self.input, self.pos)

        if matcher:
            # print(self.pos)
            # print(matcher)
            token = matcher.group()
            endPos = matcher.end()
            fragment = Fragment(self.findPosition(self.input, self.pos), self.findPosition(self.input, endPos - 1))
            self.pos = endPos
            return self.determineToken(token, fragment)
        raise Exception("LEX_ERROR: " + Fragment(self.findPosition(self.input, self.pos),
                                                 self.findPosition(self.input, self.pos)).get_simplified_string())

    def determineToken(self, token, fragment):
        VARNAME = r'(\{[A-Za-z0-9]*\})'
        CHAR = r'(\$\"[A-Za-z0-9]\")'
        STRING = r'\"[A-Za-z0-9 ]*\"'
        REF_CONST = r'null'
        INTEGER = r'([A-Za-z0-9]+\{[0-9]+\}\$)|([0-9]+)'
        BOOLEAN = r'true|false'
        OR_XOR = r'(\|)|(\@)'
        AND = r'(\&)'
        ASSIGN = r'(\:\=)'
        KW_LOOP = r'loop'
        KW_WHILE = r'while'
        tilda = r'(\~)'
        KW_ELSE = r'else'
        COMMENT = r'(\#\#.*\n)|(\#.*\#)'
        SPACES = r'[ \n\t]'
        LB = r'(\()'
        RB = r'(\))'
        LBRBCC = r'(\[\])'
        COMPARE_OPS = r'(\=\=)|(\!\=)|(\<\=)|(\>\=)|(\<)|(\>)'
        POWER = r'(\^)'
        MULT_OPS = r'(\*)|(\/)|(\%)'
        LEFT_ARROW = r'\<\-'
        PLUS = r'\+'
        MINUS = r'\-'
        NOT = r'(\!)'
        DOT = r'(\.)'
        KW_INT = r'int'
        KW_CHAR = r'char'
        KW_BOOL = r'bool'
        KW_RETURN = r'return'
        SEMICOLON = r'(\;)'
        KW_VOID = r'void'
        KW_THEN = r'then'
        COMMA = r'(\,)'
        EQUAL = r'(\=)'

        if re.match(VARNAME, token):
            return Token(token, DomainTag.VARNAME, fragment)
        elif re.match(LEFT_ARROW, token):
            return Token(token, DomainTag.LEFT_ARROW, fragment)
        elif re.match(EQUAL, token):
            return Token(token, DomainTag.EQUAL, fragment)
        elif re.match(INTEGER, token):
            return Token(token, DomainTag.INTEGER, fragment)
        elif re.match(CHAR, token):
            return Token(token, DomainTag.CHAR_CONST, fragment)
        elif re.match(STRING, token):
            return Token(token, DomainTag.STRING, fragment)
        elif re.match(BOOLEAN, token):
            return Token(token, DomainTag.BOOLEAN, fragment)
        elif re.match(OR_XOR, token):
            return Token(token, DomainTag.OR_XOR_OP, fragment)
        elif re.match(AND, token):
            return Token(token, DomainTag.AND_OP, fragment)
        elif re.match(COMPARE_OPS, token):
            return Token(token, DomainTag.COMPARE_OP, fragment)
        elif re.match(PLUS, token):
            return Token(token, DomainTag.PLUS, fragment)
        elif re.match(MINUS, token):
            return Token(token, DomainTag.MINUS, fragment)
        elif re.match(MULT_OPS, token):
            return Token(token, DomainTag.MUL_OP, fragment)
        elif re.match(POWER, token):
            return Token(token, DomainTag.POWER_OP, fragment)
        elif re.match(NOT, token):
            return Token(token, DomainTag.NOT_OP, fragment)
        elif re.match(ASSIGN, token):
            return Token(token, DomainTag.ASSIGN, fragment)
        elif re.match(DOT, token):
            return Token(token, DomainTag.DOT, fragment)
        elif re.match(COMMA, token):
            return Token(token, DomainTag.COMMA, fragment)
        elif re.match(tilda, token):
            return Token(token, DomainTag.TILDE, fragment)
        elif re.match(LBRBCC, token):
            return Token(token, DomainTag.LBRBCC, fragment)
        elif re.match(LB, token):
            return Token(token, DomainTag.LB, fragment)
        elif re.match(RB, token):
            return Token(token, DomainTag.RB, fragment)
        elif re.match(SEMICOLON, token):
            return Token(token, DomainTag.SEMICOLON, fragment)
        elif re.match(KW_INT, token):
            return Token(token, DomainTag.KW_INT, fragment)
        elif re.match(KW_BOOL, token):
            return Token(token, DomainTag.KW_BOOL, fragment)
        elif re.match(KW_RETURN, token):
            return Token(token, DomainTag.KW_RETURN, fragment)
        elif re.match(KW_VOID, token):
            return Token(token, DomainTag.KW_VOID, fragment)
        elif re.match(KW_CHAR, token):
            return Token(token, DomainTag.KW_CHAR, fragment)
        elif re.match(KW_LOOP, token):
            return Token(token, DomainTag.KW_LOOP, fragment)
        elif re.match(KW_THEN, token):
            return Token(token, DomainTag.KW_THEN, fragment)
        elif re.match(KW_ELSE, token):
            return Token(token, DomainTag.KW_ELSE, fragment)
        elif re.match(REF_CONST, token):
            return Token(token, DomainTag.KW_NULL, fragment)
        elif re.match(KW_WHILE, token):
            return Token(token, DomainTag.KW_WHILE, fragment)
        elif re.match(KW_LOOP, token):
            return Token(token, DomainTag.KW_LOOP, fragment)
        elif re.match(COMMENT, token):
            return self.nextToken()
        elif re.match(SPACES, token):
            return self.nextToken()
        raise Exception("LEX_ERROR:" + fragment.get_simplified_string())

    def findPosition(self, text, index):
        line = 1
        col = 1
        for i in range(index):
            if text[i] == '\n':
                line += 1
                col = 1
            else:
                col += 1
        return Position(line, col, index)
