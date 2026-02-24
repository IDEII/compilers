from lexer import Lexer
from lexer import Token

GRAMMAR = 'GRAMMAR'
DECLARATION = 'DECLARATION'

NTERMS = 'NTERMS'
AFTERAX = 'AFTERAX'
B4AX = 'B4AX'
NTERM = 'NTERM'

RULE = 'RULE'
RULES = 'RULES'
SEQUENCE = 'SEQUENCE'

LB = 'LB'
RB = 'RB'
RLB = 'RLB'
RRB = 'RRB'
LCBR = 'LCBR'
RCBR = 'RCBR'

SYMBOL = 'SYMBOL'
SIGN = 'SIGN'
SOBACHKA = 'SOBACHKA'
AT = 'AT'
COMMA = 'COMMA'
COLON = 'COLON'
COL = 'COL'

LETTER = 'LETTER'

END = "END"


class TreeNode:
    num = 0

    def __init__(self, content):
        TreeNode.num += 1
        self.num = TreeNode.num
        self.content = content
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
        self.terminals = [COMMA, LCBR, RCBR, LB, RB, COL, LETTER, RLB, RRB, SYMBOL, AT]
        self.nonterminals = [GRAMMAR, NTERM, NTERMS, AFTERAX, DECLARATION, B4AX, RULE, RULES, SEQUENCE, SIGN, SOBACHKA]
        self.tokens = token_iterator
        self.table = {
            (GRAMMAR, LCBR): [B4AX, LCBR, NTERM, RCBR, COMMA, DECLARATION, RULE, RULES],
            (GRAMMAR, LETTER): [B4AX, LCBR, NTERM, RCBR, COMMA, DECLARATION, RULE, RULES],
            (GRAMMAR, RLB): [B4AX, LCBR, NTERM, RCBR, COMMA, DECLARATION, RULE, RULES],

            (DECLARATION, LETTER): [NTERM, AFTERAX],
            (DECLARATION, RLB): [NTERM, AFTERAX],

            (AFTERAX, COMMA): [COMMA, NTERM, AFTERAX],
            (AFTERAX, LB): [],

            (B4AX, LCBR): [],
            (B4AX, LETTER): [NTERM, COMMA, B4AX],
            (B4AX, RLB): [NTERM, COMMA, B4AX],

            (RULES, LB): [RULE, RULES],
            (RULES, END): [],

            (RULE, LB): [LB, NTERM, COL, SEQUENCE, RB],

            (NTERM, LETTER): [LETTER],
            (NTERM, RLB): [RLB, LETTER, RRB],

            (SEQUENCE, LETTER): [SIGN, NTERM, NTERMS, SOBACHKA],
            (SEQUENCE, RLB): [SIGN, NTERM, NTERMS, SOBACHKA],
            (SEQUENCE, SYMBOL): [SIGN, NTERM, NTERMS, SOBACHKA],

            (NTERMS, COL): [],
            (NTERMS, RB): [],
            (NTERMS, LETTER): [NTERM, NTERMS],
            (NTERMS, RLB): [NTERM, NTERMS],

            (SIGN, LETTER): [],
            (SIGN, RLB): [],
            (SIGN, SYMBOL): [SYMBOL, COLON],

            (SOBACHKA, COL): [COL, AT],
            (SOBACHKA, RB): [],

            (COLON, COL): [COL],
            (COLON, LETTER): [],
            (COLON, RLB): []
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
                    if a.token_type in ['COMMA', 'LCBR', 'RCBR', 'LB', 'RB', 'COL', 'LETTER', 'RLB', 'RRB', 'SYMBOL', 'AT']:
                        if a.value in ["\")\"", "\"n\"", "\"+\"", "\"*\"", "\"(\""]:
                            x.content = a
                            x.content.value = '\\\"' + a.value[1] + '\\\"'
                        else:
                            x.content = a
                        # x.content = a
                    self.magazine.pop()
                    a = next(self.tokens)
                else:
                    raise ValueError(f"Проблема с токеном {a.token_type}: {a.value}")
            elif (x.content.value, a.token_type) in self.table: # akjfjfhaslkfj
                self.magazine.pop()
                new_nodes = []
                for i in range(len(self.table[(x.content.value, a.token_type)])):
                    new_nodes.append(TreeNode(Token(self.table[(x.content.value, a.token_type)][i], self.table[(x.content.value, a.token_type)][i], 0, 0)))
                if len(new_nodes) == 0:
                    x.add_child(TreeNode(Token("", "", 0, 0)))
                for y in new_nodes:
                    cur_x = x
                    x.add_child(y)
                for y in new_nodes[::-1]:
                    self.magazine.append(y)
                result.append((x.content.value, self.table[(x.content.value, a.token_type)]))
            else:
                raise ValueError(f"Проблема с токеном {a.token_type}: {a.value}")
        return root


if __name__ == "__main__":
    with open('test.txt', 'r') as f:
        text = f.read()
    lexer = Lexer(text + "$")
    try:
        iterator = lexer.tokenize()
        # for token in iterator:
        #    print(token)
        predicter = Predicter(iterator)
        root = predicter.top_down_parse()
        with open('graph.dot', 'w') as f:
            f.write('digraph {\n')
            root.print_graph(f)
            f.write('}')
    except ValueError as v:
        print(v)


# GRAMMAR -> B4AX LCBR NTERM RCBR COMMA DECLARATION RULE RULES .
# DECLARATION -> NTERM AFTERAX .
# AFTERAX -> COMMA NTERM AFTERAX | .
# B4AX -> NTERM COMMA B4AX | .
# RULES -> RULE RULES | .
# RULE -> LB NTERM COL SEQUENCE RB .
# NTERM -> LETTER | RLB LETTER RRB.
# SEQUENCE -> SIGN NTERM NTERMS SOBACHKA .
# NTERMS -> NTERM NTERMS | .
# SIGN -> SYMB COLON | .
# SOBACHKA -> COL AT| .
# COLON -> COL | .
