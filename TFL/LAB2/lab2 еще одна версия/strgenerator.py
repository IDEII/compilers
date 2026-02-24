import random
import re


def find_all_paths(graph, start, end, matrix, path=[]):
    wordonpath = []
    path = path + [start]
    if start == end:
        st = ""
        for i in range(len(path) - 1):
            st += matrix[path[i]][path[i + 1]]
        if len(path) == 1:
            wordonpath.append(matrix[path[0]][path[0]])
        wordonpath.append(st)
        return wordonpath
    if start not in graph:
        return []
    paths = []
    for neighbor in graph[start]:
        if neighbor not in path:
            new_paths = find_all_paths(graph, neighbor, end, matrix, path)
            for new_path in new_paths:
                paths.append(new_path)
    return paths


def main(data):
    global strg
    strg = []
    final_states = data[0]
    met = dict(data[1])
    matrix = data[2]

    #for i, ch in enumerate(matrix):
    #    print(ch)

    #print("______________________")
    g = [''] * len(matrix)
    for i in range(len(matrix)):
        g[i] = [''] * len(matrix)

    wordsonpath = [''] * len(matrix)
    for i in range(len(matrix)):
        wordsonpath[i] = [''] * len(matrix)

    for i in range(len(matrix)):
        temp = []
        for j in range(len(matrix)):
            if matrix[i][j] != "":
                temp.append(j)
        g[i] = temp[:]


    graph = {i: g[i] for i in range(len(g))}

    if len(graph) == 1:
        wordsonpath[0][0] = matrix[0][0]
        wordsonpath[0][0].append('0')
    else:
        for i in range(len(matrix)):
            for j in range(len(matrix)):
                wordsonpath[i][j] = find_all_paths(graph, i, j, matrix)
                if wordsonpath[i][j] == []:
                    wordsonpath[i][j] = ['']


    #for i, ch in enumerate(wordsonpath):
    #    print(ch)
    #print("___________________")
    return wordsonpath
