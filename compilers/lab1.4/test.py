import re


class Automata:
    def __init__(self, start, finishes_data: list, transitions: dict) -> None:
        """
        start (int): Start state
        finishes_data (list(tuple)): List of finish states with their
        correspoding lexeme names - [(2, 'str'), (7, 'comment') ...]
        transitions: dict(list(tuple)): dictionary of automata transitions
        in format - dict[state] = [(state, regex_rule)]
        """

        self.start = start
        self.finishes = [i[0] for i in finishes_data]
        self.finish_lexemes = [i[1] for i in finishes_data]
        self.transitions = transitions
        if self.start in self.finishes: raise Exception('Finish states can not\
        contain Start state')
        self.s = self.start
        self.attr = ''

    def reset(self) -> None:
        # Setting automata in start condition
        self.s = self.start
        self.attr = ''

    def update(self, symbol: str) -> str:
        # print(self.s, symbol)
        """
        Updating automata by reading a symbol and returning lexema name
        corresponding to reached state: ('NONE' if not finial state)
        symbol (str): symbol to update an automata
        Return (str): lexema name
        """
        if self.s == self.start: self.reset()
        self.attr += symbol

        if self.s in self.transitions:
            rules = self.transitions[self.s]
            for rule in rules:
                if rule[1].match(symbol):
                    self.s = rule[0]
                    return 'NONE', self.attr

        if self.s in self.finishes:
            res = self.finish_lexemes[self.finishes.index(self.s)]
            attr = self.attr
            self.reset()
            return res, attr[:-1]

        self.s = -1  # transition into ERROR state (-1)
        return 'NONE', ''


class Symbolizer:
    def __init__(self, text):
        self.text = text

    def next_symbol(self):
        with open(self.text, 'r') as file:
            for line in file:
                for char in line: yield char
            yield '\n'


class Lexer:
    def __init__(self, text, automata):
        self.aut = automata
        self.symbolizer = Symbolizer(text)

    def next_token(self):
        line, pos = 1, 0
        for char in self.symbolizer.next_symbol():
            pos += 1
            r = self.aut.update(char)
            if r[0] != 'NONE':
                self.aut.reset()
                self.aut.update(char)
                yield r[0], r[1], line, pos - len(r[1]), pos
            if char == '\n':
                line += 1
                pos = 0


automata = Automata(
    start=0,
    finishes_data=[(8, 'LONGREAL'), (12, 'REAL'), (14, '>='), (16, ':='), \
                   (17, 'COMMENT'), (19, 'IDENT'), (21, 'NUM')],
    transitions={
        0: [(0, re.compile('[\s\r]')), (1, re.compile('l')), (9, re.compile('r')), \
            (13, re.compile('>')), (15, re.compile(':')), (19, re.compile('[a-qst-z]')), (21, re.compile('\d+'))],
        1: [(2, re.compile('o')), (19, re.compile('[b-z0-9]'))],
        2: [(3, re.compile('n')), (19, re.compile('[a-mo-z0-9]'))],
        3: [(4, re.compile('g')), (19, re.compile('[a-fh-z0-9]'))],
        4: [(5, re.compile('r')), (19, re.compile('[a-qs-z0-9]'))],
        5: [(6, re.compile('e')), (19, re.compile('[a-df-z0-9]'))],
        6: [(7, re.compile('a')), (19, re.compile('[b-z0-9]'))],
        7: [(8, re.compile('l')), (19, re.compile('[a-km-z0-9]'))],
        8: [(19, re.compile('[a-z0-9]'))],

        9: [(10, re.compile('e')), (19, re.compile('[a-df-z0-9]'))],
        10: [(11, re.compile('a')), (19, re.compile('[b-z0-9]'))],
        11: [(12, re.compile('l')), (19, re.compile('[a-km-z0-9]'))],
        12: [(19, re.compile('[a-z0-9]'))],

        13: [(14, re.compile('='))],

        15: [(16, re.compile('=')), (17, re.compile(':'))],

        17: [(17, re.compile('[^\n]'))],

        19: [(19, re.compile('[A-Za-z0-9]+'))],

        21: [(21, re.compile('\d+'))],

        -1: [(-1, re.compile('^\s'))],
        -1: [(0, re.compile('\s|\n'))]
    }
)

lex = Lexer('test.txt', automata)
# for i in lex.next_token():
#    print(i)
for i in lex.next_token(): print(f'{i[0]}{" " * (8 - len(i[0]))} line: {i[2]}, pos:\
     {i[3]}-{i[4]}{" " * (5 - len(str(i[3])) - len(str(i[4])))} val: {i[1]}')