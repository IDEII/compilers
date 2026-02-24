import re


class Automata:
    def __init__(self, start, finishes_data: list, transitions: dict) -> None:
        self.start = start
        self.finishes = [i[0] for i in finishes_data]
        self.finish_lexemes = [i[1] for i in finishes_data]
        self.transitions = transitions
        if self.start in self.finishes:
            raise Exception('Finish states can not contain Start state')
        self.s = self.start
        self.attr = ''

    def reset(self):
        self.s = self.start
        self.attr = ''

    def update(self, symbol: str) -> str:
        if self.s == self.start:
            self.reset()
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

        self.s = -1
        return 'NONE', ''


class Symbolizer:
    def __init__(self, text):
        self.text = text

    def next_symbol(self):
        with open(self.text, 'r') as file:
            for line in file:
                for char in line:
                    yield char
            yield '\n'


class Lexer:
    def __init__(self, text, automata):
        self.aut = automata
        self.symbolizer = Symbolizer(text)

    def next_token(self):
        line, pos = 1, 0
        startline = line
        startpos = pos
        temp1 = [line, pos]
        temp2 = [line, pos]
        flag = False
        flag2 = False
        for char in self.symbolizer.next_symbol():
            pos += 1
            r = self.aut.update(char)
            if len(r[1]):
                if char in ('\n', ' ') and self.aut.s == -1:
                    self.aut.reset()
                    self.aut.update(char)

            if r[0] != 'NONE':
                self.aut.reset()
                self.aut.update(char)
                if flag == False and flag2 == False:
                    temp1 = [line, pos - len(r[1])]
                    temp2 = [line, pos - 1]
                elif flag == True:
                    flag = False
                elif flag2 == True:
                    temp2 = [line, pos - 1]
                    flag2 = False
                yield r[0], r[1], temp1, temp2

            if char == '\n':
                if self.aut.s not in (1, -1):
                    if self.aut.s not in (13, 14, 15, 20):
                        flag = True
                        temp1 = [line, pos - len(r[1]) + 1]
                        temp2 = [line, pos]
                    else:
                        flag2 = True
                        temp1 = [line, pos - len(r[1]) + 1]
                line += 1
                pos = 0


automata = Automata(
    start=1,
    finishes_data=[
        (2, 'IDENT'),
        (3, 'IDENT'),
        (4, 'IDENT'),
        (5, 'IDENT'),
        (6, 'IDENT'),
        (33, 'FOR'),
        (7, 'FORWARD'),
        (18, '&&'),
        (19, '||'),
        (50, 'STRING'),
        (51, 'IDENT'),
        (52, 'NUMBER'),
    ],
    transitions={
        1: [(1, re.compile(r'[\n\s\t ]')), (2, re.compile('F')), (9, re.compile(r'\&')), (11, re.compile(r'\|')),
            (13, re.compile(r'"')), (51, re.compile(r'[A-EG-Z]')), (52, re.compile(r'[0-9]'))],
        2: [(3, re.compile('O')), (51, re.compile(r'[A-NP-Z0-9]'))],
        3: [(33, re.compile('R')), (51, re.compile(r'[A-QS-Z0-9]'))],
        33: [(4, re.compile('W')), (51, re.compile(r'[A-VXYZ0-9]'))],
        4: [(5, re.compile('A')), (51, re.compile(r'[B-Z0-9]'))],
        5: [(6, re.compile('R')), (51, re.compile(r'[A-QS-Z0-9]'))],
        6: [(7, re.compile('D')), (51, re.compile(r'[ABCE-Z0-9]'))],
        7: [(51, re.compile(r'[A-Z0-9]'))],

        9: [(18, re.compile(r'\&'))],
        11: [(19, re.compile(r'\|'))],

        13: [(13, re.compile(r'[A-Za-z0-9 \s\n\t\|\&]')), (14, re.compile(r"\\")), (50, re.compile(r'"'))],
        14: [(13, re.compile(r'[A-Za-z0-9 \s\n\t\|\&]')), (15, re.compile(r'"'))],
        15: [(13, re.compile(r'[A-Za-z0-9 \s\n\t\|\&]'))],

        51: [(51, re.compile(r'[A-Z0-9]'))],
        52: [(52, re.compile(r'[0-9]'))],


        -1: [(-1, re.compile(r'[^\s\n\t]')), (1, re.compile(r'[ \s\n\t]'))],

    }
)

lexer = Lexer('input2.txt', automata)
for i in lexer.next_token():
    # print(len(i))
    if len(i) == 4:
        # print(i)
        print(f'{i[0]}{" " * (8 - len(i[0]))} fragment: ({i[2][0]}, {i[2][1]})-({i[3][0]}, {i[3][1]}) val: {i[1]}')
