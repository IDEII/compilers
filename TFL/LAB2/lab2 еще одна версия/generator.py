import random
import check
import StateEliminator
import strgenerator

class Node:
    def __init__(self, data, stars, left=None, right=None):
        self.data = data
        self.star = stars
        self.left = left
        self.right = right


def generate(node, starlevel, lenght, everything):
    if lenght == 0:
        node.data = everything[random.randint(1, 5)]
        return
    elif lenght == 1:
        lenght -= 1
        node.data = everything[random.randint(1, 5)]
    else:
        if node.data is None:
            node.data = everything[random.randint(1, 9)]
        if node.data == '*':
            lenght -= 1
            node.left = Node(everything[random.randint(1, 9)], node.star + 1)
            generate(node.left, starlevel, lenght, everything)
        elif node.data == '#' or node.data == '+':
            if node.star < starlevel:
                node.left = Node(everything[random.randint(1, 9)], node.star)
                node.right = Node(everything[random.randint(1, 9)], node.star)
            else:
                node.left = Node(everything[random.randint(1, 8)], node.star)
                node.right = Node(everything[random.randint(1, 8)], node.star)
            lenght -= 2
            generate(node.left, starlevel, lenght, everything)
            generate(node.right, starlevel, lenght, everything)
        elif node.data == '|':
            if node.star < starlevel:
                node.left = Node(everything[random.randint(0, 9)], node.star)
                node.right = Node(everything[random.randint(0, 9)], node.star)
            else:
                node.left = Node(everything[random.randint(0, 8)], node.star)
                node.right = Node(everything[random.randint(0, 8)], node.star)
            lenght -= 2
            generate(node.left, starlevel, lenght, everything)
            generate(node.right, starlevel, lenght, everything)


def instr(root):
    if root is None:
        return ''
    if not root.data == "+":
        out = instr(root.left) + root.data + instr(root.right)
    else:
        out = instr(root.left) + instr(root.right)

    if root.data in '|#*':
        return '(' + out + ')'
    return out


def main():
    global reg
    print("ввод регулярки самостоятельно или нет - y/n")
    t = input()
    if t == 'y':
        reg = input()
        print("количество сгенерированных строк, '' = 10")
        cs = input()
        if cs == '':
            cs = 10
        else:
            cs = int(cs)
        array = [reg]
        return array, cs
    else:
        print("введите максимальную длину регулярного выражения в буквах")
        lenght = int(input())
        print("максимальную вложенность звезды")
        starlevel = int(input())
        print("количество тестов")
        count = int(input())
        print("количество сгенерированных строк")
        cs = int(input())
        everything = ["ε", 'a', 'b', 'c', 'd', 'e', '|', '#', '+', '*']
        array = []
        for i in range(count):
            node = Node(everything[random.randint(6, 9)], 0)
            generate(node, starlevel, lenght, everything)
            reg = instr(node)
            while "#" not in reg:
                generate(node, starlevel, lenght, everything)
                reg = instr(node)
            if reg not in array:
                array.append(reg)
        return array, cs


if __name__ == '__main__':
    regex, cs = main()
    print(regex)
    alphabet = ''
    for i in range(len(regex)):
        for j in regex[i]:
            if j in 'abcde':
                alphabet += j
        data = StateEliminator.main(alphabet, regex[i])

        strings = strgenerator.main(data)
        reg = data[3]
        print(reg)
        check.check(reg, strings, data[0], data[1], data[2], cs)
