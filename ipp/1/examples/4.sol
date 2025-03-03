class Main : Object {
    compute:and:and:
        [ :x :y :z |
            _ := ((self vysl) greaterThan: 0)
            ifTrue: [|u := self vysl: 1.]
            ifFalse: [|].
        ]
}
