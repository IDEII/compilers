import os
import sys
import subprocess
import z3


def exprsplit(s):
    variables = []
    temp = []
    temp2 = []
    counter = 0
    for i, ch in enumerate(s):
        if ch == "(":
            counter += 1
            temp.append(i)
        elif ch == ")":
            counter -= 1
            temp2.append(i)
    for j in range(len(temp)):
        variables.append(s[temp[j] + 1:temp2[len(temp) - 1 - j]])
    return variables


def expr(array):  # почти не нужная функция
    s = "x"
    coefficient = []
    for i in range(1, len(array)):
        coefficient.append(''.join(array[i])[0])
        sample = "(+ (* " + ''.join(array[i])[0] + "0 x) " + ''.join(array[i])[0] + "1)"
        s = s.replace("x", sample)
    return s, coefficient


def func1(coefficient):
    m = [[coefficient[0] + "11", coefficient[0] + "12"],
         [coefficient[0] + "21", coefficient[0] + "22"]]
    for i in range(1, len(coefficient)):
        m2 = [[coefficient[i] + "11", coefficient[i] + "12"],
              [coefficient[i] + "21", coefficient[i] + "22"]]
        m = mul22(m, m2)
    x = m

    f = []
    while len(coefficient) > 0:
        if len(coefficient) == 1:
            m = [coefficient[0] + "1", coefficient[0] + "2"]
        else:
            m = [[coefficient[0] + "11", coefficient[0] + "12"],
                 [coefficient[0] + "21", coefficient[0] + "22"]]
            for i in range(1, len(coefficient)):
                if i == len(coefficient) - 1:
                    m2 = [coefficient[i] + "1", coefficient[i] + "2"]
                    m = mul21(m, m2)
                else:
                    m2 = [[coefficient[i] + "11", coefficient[i] + "12"],
                          [coefficient[i] + "21", coefficient[i] + "22"]]
                    m = mul22(m, m2)
        f.append(m)
        coefficient.pop()

    free = f[0]
    for i in range(1, len(f)):
        free = summ(free, f[i])

    return x, free


def mul22(m1, m2):
    res = [[0, 0],
           [0, 0]]
    res[0][0] = "(amax (asum " + m1[0][0] + " " + m2[0][0] + ") (asum " + m1[0][1] + " " + m2[1][0] + "))"
    res[0][1] = "(amax (asum " + m1[0][0] + " " + m2[0][1] + ") (asum " + m1[0][1] + " " + m2[1][1] + "))"
    res[1][0] = "(amax (asum " + m1[1][0] + " " + m2[0][0] + ") (asum " + m1[1][1] + " " + m2[1][0] + "))"
    res[1][1] = "(amax (asum " + m1[1][0] + " " + m2[0][1] + ") (asum " + m1[1][1] + " " + m2[1][1] + "))"
    return res


def mul21(m1, m2):
    res = [0, 0]
    res[0] = "(amax (asum " + m1[0][0] + " " + m2[0] + ") (asum " + m1[0][1] + " " + m2[1] + "))"
    res[1] = "(amax (asum " + m1[1][0] + " " + m2[0] + ") (asum " + m1[1][1] + " " + m2[1] + "))"
    return res


def summ(m1, m2):
    res = [0, 0]
    res[0] = "(asum " + m1[0] + " " + m2[0] + ")"
    res[1] = "(asum " + m1[1] + " " + m2[1] + ")"
    return res


def formSMT(x1, x2, free1, free2, coefficient):
    f = open("lab1.smt2", "w")
    wfile = "(set-logic QF_NIA)\n" \
            "(define-fun amax ((a Int) (b Int)) Int (ite (>= a b) a b))\n" \
            "(define-fun asum ((a Int) (b Int)) Int (ite (and (> a -1)  (> b -1)) (+ a b) (ite (<= a -1) b a )))\n" \
            "(define-fun ag ((a Int) (b Int)) Bool (ite (and (= a -1) (= b -1) ) true (> a b)))\n"
    for i in range(len(coefficient)):
        wfile += "(declare-fun " + coefficient[i] + "11 () Int)\n" \
                 "(declare-fun " + coefficient[i] + "12 () Int)\n" \
                 "(declare-fun " + coefficient[i] + "21 () Int)\n" \
                 "(declare-fun " + coefficient[i] + "22 () Int)\n" \
                 "(declare-fun " + coefficient[i] + "1 () Int)\n" \
                 "(declare-fun " + coefficient[i] + "2 () Int)\n"

    for i in range(len(coefficient)):
        wfile += "(assert (or (> " + coefficient[i] + "11 -1) (and (= " + coefficient[i] + "11 0) (= " + coefficient[i] + "1 0))))\n" \
                 "(assert (>= " + coefficient[i] + "12 -1))\n" \
                 "(assert (>= " + coefficient[i] + "21 -1))\n" \
                 "(assert (>= " + coefficient[i] + "22 -1))\n" \
                 "(assert (> " + coefficient[i] + "1 -1))\n" \
                 "(assert (>= " + coefficient[i] + "2 -1))\n"
    for i in range(len(x1)):
        wfile += "(assert (ag " + x1[i][0][0] + " " + x2[i][0][0] + "))\n" \
                 "(assert (ag " + x1[i][0][1] + " " + x2[i][0][1] + "))\n" \
                 "(assert (ag " + x1[i][1][0] + " " + x2[i][1][0] + "))\n" \
                 "(assert (ag " + x1[i][1][1] + " " + x2[i][1][1] + "))\n" \
                 "(assert (ag " + free1[i][0] + " " + free2[i][0] + "))\n" \
                 "(assert (ag " + free1[i][1] + " " + free2[i][1] + "))\n"
    wfile += "(check-sat)\n" \
             "(get-model)\n" \
             "(exit)"
    f.write(wfile)
    f.close()
    os.system('z3 -smt2 lab1.smt2 > output.txt')

def main():
    print("Пример ввода: f(g(x)) -> g(x). Пустая строка - это завершение.")
    left = []
    right = []
    while True:
        s = input()
        if s != "":
            left.append(s.split("->")[0].strip())
            right.append(s.split("->")[1].strip())
        else:
            break
    coefficient = []
    leftx, leftfree, rightx, rightfree = [], [], [], []
    for i in range(len(left)):
        x1 = list(reversed(exprsplit(left[i])))
        x2 = list(reversed(exprsplit(right[i])))
        x1.append(left[i])
        x2.append(right[i])
        x1, c1 = expr(x1)
        x2, c2 = expr(x2)
        for i, ch in enumerate(c1):
            if ch not in coefficient:
                coefficient.append(ch)
        for i, ch in enumerate(c2):
            if ch not in coefficient:
                coefficient.append(ch)
        x1, free1 = func1(c1)
        leftx.append(x1)
        leftfree.append(free1)
        x2, free2 = func1(c2)
        rightx.append(x2)
        rightfree.append(free2)
    formSMT(leftx, rightx, leftfree, rightfree, coefficient)


if __name__ == '__main__':
    main()
