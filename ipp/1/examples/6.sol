class Main : Object {
	run [|
		a := A new.
		"instance bloku si zapamatuje , že self odkazuje na tuto instanci Main"
		"blok navíc tuto referenci na konci vrací"
		b := [ :arg | y := self attr: arg. z := self. ].
		"zavoláme metodu 'foo' na instanci A a předáme jí objekt 'b' typu Block"
		c := a foo: b.
		"výsledkem přiřazeným do c je instance třídy Main s instančním atributem
		attr inicializovaným na 1"
	]
}
	class A : Object {
		foo: [ :x |
		"blok předaný v x je vyhodnocen a do instance Main je jím vytvořen
		instanční atribut attr s hodnotou 1"
		u := x value: 1.
	]
}

