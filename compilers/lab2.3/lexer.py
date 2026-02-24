import re

# # объявления
# non-terminal E, E1, T, T1, F;
# terminal '+', '*', '(', ')', n;
#
# # правила грамматики
# E ::= T E1;
# E1 ::= '+' T E1 | epsilon;
# T ::= F T1;
# T1 ::= '*' F T1 | epsilon;
# F ::= n | '(' E ')';
#
# axiom E;

class Token:
    def __init__(self, token_type, value, line, position):
        self.token_type = token_type
        self.value = value
        self.line = line
        self.position = position

    def __str__(self):
        return self.token_type + " " + self.value



class Lexer:
    def __init__(self, text):
        self.text = text
        self.line = 1
        self.position = 1
        self.tokens = []
        self.rules = [
            ('non_Terms', r'non-terminal'),
            ('terms', r'terminal'),
            ('axiom', r'axiom'),
            ('EPSILON', r'epsilon'),
            ('SYMBOL', r'([n])|(\'([A-Za-z\+\*\(\)])\')'),
            ('EQV', r'\:\:\='),
            ('OR', r'\|'),
            ('COMMA', r'\,'),
            ('SEMICOLON', r'\;'),
            ('LETTER', r'[A-Za-z][\d+]{0,5}\'?'),
            ('END', r'\$'),
            ('SPACE', r'\s+')
        ]

    def tokenize(self):
        lines = self.text.splitlines()
        for line_num, line in enumerate(lines):
            self.position = 1
            self.line = line_num + 1
            while self.position < len(line) + 1:
                for rule in self.rules:
                    match = re.match(rule[1], line[self.position - 1:])
                    if match:
                        if rule[0] != 'SPACE' or rule[0] == 'COMM':
                            value = match.group()
                            self.tokens.append(Token(rule[0], value, self.line, self.position))
                        self.position += match.end()
                        break
                else:
                    for token in self.tokens:
                        print(token)
                    print(f'lexer error ({self.line}, {self.position})\n')
                    raise ValueError

        return iter(self.tokens)


if __name__ == "__main__":
    with open('test.txt', 'r') as f:
        text = f.read()
    lexer = Lexer(text + "$")
    try:
        for token in lexer.tokenize():
            print(token.token_type, token.value, token.line, token.position)
    except ValueError as v:
        pass
