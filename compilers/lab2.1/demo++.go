package main

import (
	"fmt"
	"go/ast"
	"go/format"
	"go/parser"
	//"go/printer"
	"go/token"
	"os"
)

func changeFunc(node ast.Node) bool {
	if funcDecl, ok := node.(*ast.FuncDecl); ok {
		for i, stmt := range funcDecl.Body.List {
			if returnStmt, ok := stmt.(*ast.ReturnStmt); ok {
				if len(returnStmt.Results) > 0 {
					if funcDecl.Type.Results.List[0].Type.(*ast.Ident).Name == "int" {
						returnType := returnStmt.Results[0].(*ast.Ident).Name
						funcName := funcDecl.Name.Name
						callExpr := &ast.ExprStmt{
							X: &ast.CallExpr{
								Fun: &ast.SelectorExpr{
									X:   ast.NewIdent("fmt"),
									Sel: ast.NewIdent("Printf"),
								},
								Args: []ast.Expr{
									&ast.BasicLit{
										Kind:  token.STRING,
										Value: "\"Function Name: %s, Value: %d\\n\"",
									},
									&ast.BasicLit{
										Kind:  token.STRING,
										Value: fmt.Sprintf("\"%s\"", funcName),
									},
									&ast.Ident{
										Name: returnType,
									},
								},
							},
						}
						funcDecl.Body.List = append(funcDecl.Body.List[:i], append([]ast.Stmt{callExpr, stmt}, funcDecl.Body.List[i+1:]...)...)
					}
				}
			}
		}
	}
	return true
}

func inspect(file *ast.File) {
	ast.Inspect(file, changeFunc)
}

func main() {
	fset := token.NewFileSet()
	if file, err := parser.ParseFile(fset, "main.go", nil, parser.ParseComments); err == nil {
		ast.Fprint(os.Stdout, fset, file, nil)
		inspect(file)
		writer, err := os.OpenFile("main_rebuilded.go", os.O_CREATE|os.O_RDWR, 0777)
		defer writer.Close()
		if format.Node(writer, fset, file) != nil {
			fmt.Printf("Formatter error: %v\n", err)
		}

	} else {
		// в противном случае, выводим сообщение об ошибке
		fmt.Printf("Error: %v", err)
	}
}
