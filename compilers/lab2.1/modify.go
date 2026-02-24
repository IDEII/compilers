package main

import (
	"fmt"
	"go/ast"
	"go/format"
	"go/parser"
	"go/token"
	"os"
	"strconv"
)

var functions = make(map[string]int)

func replaceStringLitWithConst(file *ast.File) {
	ast.Inspect(file, func(n ast.Node) bool {
		for _, x := range file.Decls {
			if Decl, ok := x.(*ast.FuncDecl); ok {
				functions[Decl.Name.Name] = 0
			}
		}
		return false
	})

	ast.Inspect(file, func(n ast.Node) bool {
		for _, x := range file.Decls {
			if funcDecl, ok := x.(*ast.FuncDecl); ok {
				body := funcDecl.Body
				for _, bodyList := range body.List {
					if exprStmt, err := bodyList.(*ast.ExprStmt); err {
						if ident, err := exprStmt.X.(*ast.CallExpr).Fun.(*ast.Ident); err {
							functions[ident.Name] += 1
						}

					}
				}
			}
		}
		return false
	})

	keys := make([]string, len(functions))

	i := 0
	for k := range functions {
		keys[i] = k
		i++
	}

	result := ""

	for i = 0; i < len(keys)-1; i++ {
		result += keys[i] + " " + strconv.Itoa(functions[keys[i]]) + ", "
	}
	result += keys[len(keys)-1] + " " + strconv.Itoa(functions[keys[len(keys)-1]])

	var before, after []ast.Decl
	if len(file.Decls) > 0 {
		hasImport := false
		if genDecl, ok := file.Decls[0].(*ast.GenDecl); ok {
			hasImport = genDecl.Tok == token.IMPORT
		}

		if hasImport {
			before, after = []ast.Decl{file.Decls[0]}, file.Decls[1:]
		} else {
			after = file.Decls
		}
	}

	file.Decls = append(before,
		&ast.GenDecl{
			Tok: token.CONST,
			Specs: []ast.Spec{
				&ast.ValueSpec{
					Names: []*ast.Ident{ast.NewIdent("RESULT")},
					Type:  ast.NewIdent("string"),
					Values: []ast.Expr{
						&ast.BasicLit{
							Kind:  token.STRING,
							Value: "\"" + result + "\"",
						},
					},
				},
			},
		},
	)
	file.Decls = append(file.Decls, after...)
}

func main() {

	if len(os.Args) != 2 {
		return
	}

	fset := token.NewFileSet()
	if file, err := parser.ParseFile(fset, os.Args[1], nil, parser.ParseComments); err == nil {
		replaceStringLitWithConst(file)

		if format.Node(os.Stdout, fset, file) != nil {
			fmt.Printf("Formatter error: %v\n", err)
		}
	} else {
		fmt.Printf("Errors in %s\n", os.Args[1])
	}
}
