from lexer import Lexer
from lexer import Token

# GRAMMAR -> non_Terms NTERM NTERMS terms TERM TERMS RULE SEMICOLON RULES AXIOM END.
# NTERMS -> COMMA NTERM NTERMS | SEMICOLON .
# TERMS -> COMMA TERM TERMS | SEMICOLON .
# NTERM -> LETTER .
# TERM -> SYMBOL .
# RULES -> RULE SEMICOLON RULES | .
# RULE -> NTERM EQV ELEMENT SEQUENCE .
# SEQUENCE -> OR ELEMENT SEQUENCE | .
# ELEMENT -> NTERM ELEMENT | TERM ELEMENT | EPSILON | .
# AXIOM -> axiom NTERM SEMICOLON .



GRAMMAR = 'GRAMMAR'

axiom = 'axiom'

non_Terms = 'non_Terms'
terms = 'terms'
RULE = 'RULE'
RULES = 'RULES'
SEQUENCE = 'SEQUENCE'
SEMICOLON = 'SEMICOLON'
AXIOM = 'AXIOM'

ELEMENT = 'ELEMENT'

SYMBOL = 'SYMBOL'
COMMA = 'COMMA'
LETTER = 'LETTER'

OR = 'OR'
EQV = 'EQV'
EPSILON = 'EPSILON'

NTERM = 'NTERM'
NTERMS = 'NTERMS'

TERM = 'TERM'
TERMS = 'TERMS'



END = "END"


class TreeNode:
    num = 0

    def __init__(self, content):
        TreeNode.num += 1
        self.num = TreeNode.num
        self.content = content
        self.pos = [content.line, content.position]
        self.children = []

    def add_child(self, child):
        self.children.append(child)

    def __repr__(self):
        return str(self.content)

    def print_graph(self, f):
        f.write(f'{self.num} [label = "{str(self.content.value)}"]\n')
        for child in self.children:
            f.write(f'{self.num} -> {child.num}\n')
        for child in self.children:
            child.print_graph(f)


class Predicter:
    def __init__(self, token_iterator):
        self.magazine = []
        self.terminals = [non_Terms, terms,axiom, EPSILON, SYMBOL, SEMICOLON, LETTER, SYMBOL, END, COMMA, EQV, OR]
        self.nonterminals = [GRAMMAR, NTERM, TERM, NTERMS, TERMS, SEQUENCE, ELEMENT, AXIOM, RULE, RULES]
        self.tokens = token_iterator
        self.table = {
            # (GRAMMAR, NTERM): [non_Terms, NTERM, NTERMS, terms, TERM, TERMS, RULE, SEMICOLON, RULES, AXIOM, END],
            (GRAMMAR, non_Terms): [non_Terms, NTERM, NTERMS, terms, TERM, TERMS, RULE, SEMICOLON, RULES, AXIOM, END],
            # (GRAMMAR, LETTER): [B4AX, LCBR, LETTER, RCBR, AFTERAX, RULE, RULES],
            (NTERMS, SEMICOLON):[SEMICOLON],
            (NTERMS, COMMA): [COMMA, NTERM, NTERMS],

            (TERMS, COMMA): [COMMA, TERM, TERMS],
            (TERMS, SEMICOLON): [SEMICOLON],

            (NTERM, LETTER):[LETTER],

            (TERM, SYMBOL):[SYMBOL],


            (RULES, LETTER):[RULE, SEMICOLON, RULES],
            (RULES, axiom):[],

            (RULE, LETTER):[NTERM, EQV, ELEMENT, SEQUENCE],

            (SEQUENCE, SEMICOLON):[],
            (SEQUENCE, OR): [OR, ELEMENT, SEQUENCE],

            (ELEMENT, LETTER):[NTERM, ELEMENT],
            (ELEMENT, SYMBOL):[TERM, ELEMENT],
            (ELEMENT, EPSILON):[EPSILON],
            (ELEMENT, SEMICOLON):[],
            (ELEMENT, OR):[],

            (AXIOM, axiom):[axiom, NTERM, SEMICOLON]

        }

    def top_down_parse(self):
        self.magazine.append(TreeNode(Token(END, 'END', 0, 0)))
        root = TreeNode(Token(GRAMMAR, 'GRAMMAR', 0, 0))
        self.magazine.append(root)
        a = next(self.tokens)

        result = []
        while True:
            x = self.magazine[-1]
            if x.content.token_type == END:
                break
            if x.content.token_type in self.terminals:
                if x.content.token_type == a.token_type:
                    if a.token_type in ['non_Terms', 'terms','axiom','EPSILON', 'SYMBOL', 'SEMICOLON', 'LETTER', 'SYMBOL', 'END', 'COMMA', 'EQV', 'OR']:
                        if a.value[0] ==  '\"':
                            x.content = a
                            x.content.value = '\\\"' + a.value[1] + '\\\"'
                            x.pos = [a.line, a.position]
                        else:
                            x.content = a
                            # x.pos = [a.line, a.position]
                    self.magazine.pop()
                    a = next(self.tokens)
                else:
                    raise ValueError(f"Проблема с токеном 1 {a.token_type}: {a.value}, {a.line, a.position}")
            elif (x.content.value, a.token_type) in self.table:
                self.magazine.pop()
                new_nodes = []
                for i in range(len(self.table[(x.content.value, a.token_type)])):
                    new_nodes.append(TreeNode(Token(self.table[(x.content.value, a.token_type)][i],
                                                    self.table[(x.content.value, a.token_type)][i], a.line, a.position)))

                    # print(x.content.position, x.content.line)
                if len(new_nodes) == 0:
                    x.add_child(TreeNode(Token("", "", a.line, a.position)))
                    # print(a.position, a.line)
                for y in new_nodes:
                    cur_x = x
                    x.add_child(y)
                for y in new_nodes[::-1]:
                    self.magazine.append(y)
                result.append((x.content.value, self.table[(x.content.value, a.token_type)]))
            else:
                print(x)
                raise ValueError(f"Проблема с токеном 2 {a.token_type}: {a.value}, {a.line, a.position}")
        return root


if __name__ == "__main__":
    with open('test.txt', 'r') as f:
        text = f.read()
    lexer = Lexer(text + "$")
    try:
        iterator = lexer.tokenize()
        predicter = Predicter(iterator)
        root = predicter.top_down_parse()

        with open('graph.dot', 'w') as f:
            f.write('digraph {')
            root.print_graph(f)
            f.write('}')
    except ValueError as v:
        print(v)

