class Main : Object {
    compute:and:and: "<- definice metody - selektor se třemi parametry"
        [ |
            _ := ((self vysl) greaterThan: 0)
            ifTrue: [|u := self vysl: 1.].
        ]
}
