package main

import "fmt"

func function1() string {
	s := "augabuga"
	return s
}

func funcs() int {
	i := 0
	return i
}

func main() {
	s := function1()
	t := funcs()
	fmt.Println(t)

	fmt.Println(s)
}
