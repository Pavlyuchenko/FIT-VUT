class Main : Object {
    run "<- definice metody - bezparametrický selektor run"
        [ |
            "zaslání zprávy 'compute:and:and:' sobě samému - selektor se dvěma arg."
            x := self compute: 3 and: 2 and: 5.
            "zaslání zprávy 'plusOne:' sobě samému - selektor s jedním arg. Argumentem je výsledek po zaslání zprávy 'vysl' objektu self."
            x := self plusOne: (self vysl).
            "zaslání zprávy 'asString' objektu x - bezparam. selektor"
            y := x asString.
        ]
}
