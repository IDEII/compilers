import random
import re
from transitions import Machine


class Matter(object):
    pass


def generate(mat, start, end, t): #возрождение генератора слов
    global strg, k
    k += 1
    if k > t:
        return
    if start in end and strg != "":
        return
    else:
        l = mat[start]
        ls = []
        for i in range(len(l)):
            if l[i][0] == "":
                ls.append(i)
        if len(ls) < len(l):
            state = random.randint(0, len(l) - 1)
            while state in ls:
                state = random.randint(0, len(l) - 1)
        else:
            return

        if len(l) > 0:
            if len(l[state]) > 1:
                state2 = random.randint(0, len(l[state]) - 1)
                strg += l[state][state2]
            else:
                strg += l[state][0]
        else:
            return
        if state in end:
            return
        else:
            generate(mat, state, end, t)
    return


def checkword(lump, states,transitions, v, strg, regex, final, f):
    machine = Machine(lump, states=states, transitions=transitions, initial=str(v))
    for r in strg:
        try:
            lump.trigger(r)
        except:
            f = 0
    if re.fullmatch(regex, strg) and lump.state in final and f != 0:
        return True
    else:
        return False


def check(regex, strings, final_states, all, matrix, cs):
    global strg, k, fl
    final = []
    for i in range(len(final_states)):
        final.append(str(all.get(final_states[i])))

    states = []
    lump = Matter()
    for i in range(len(matrix)):
        states.append(str(i))
    transitions = []
    for i in range(len(matrix)):
        for j in range(len(matrix[i])):
            if matrix[i][j] != "":
                transitions.append({'trigger': matrix[i][j], 'source': str(i), 'dest': str(j)})

    fl = []

    for i, ch in enumerate(strings):
       print(ch)

    for i, ch in enumerate(matrix):
        print(ch)

    print(final)
    strs = []
    for i in range(len(strings)):
        strs.append(['', i])
    for i in range(len(strings)):
        for j in range(len(strings[i])):
            for k in range(len(strings[i][j])):
                if strings[i][j][k] != '':
                    strs.append([strings[i][j][k], i])
    for i in range(len(strs)):
        machine = Machine(lump, states=states, transitions=transitions, initial=str(strs[i][1]))
        for r in strs[i][0]:
            try:
                lump.trigger(r)
            except:
                break
        if re.fullmatch(regex, strs[i][0]) and lump.state in final:
            if strs[i][0] not in fl:
                fl.append(strs[i][0])
                if strs[i][0] == '':
                    strs[i][0] = "ε"
                print("Проверка проедена стройкой:", strs[i][0])
    j = len(fl)
    for i in range(j, cs + 2):
        t = random.randint(0, 11) #максимальная вложенность слов на пути в слове
        f = 1
        strg = ''
        v = random.randint(0, len(all) - 1)
        k = 0
        generate(strings, v, final, t) #сгенерированные слова могут повторяться
        flag = checkword(lump, states, transitions, v, strg, regex, final, f)
        if flag and strg not in fl:
            fl.append(strg)
            print("Проверка пройдена стройкой:", strg)

    print("количество сгенерированных слов не прошедих проверку", cs - len(fl))
    if len(fl) == 0:
        print("Бедус")
