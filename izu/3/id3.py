import math

attributes = {
    "vhkost": ["nizka", "stredni", "vysoka"],
    "teplota": ["pracovni", "vikend", "svatek"],
    "den": ["jaro", "leto", "podzim", "zima"],
    "obdobi": ["mala", "velka"],
}

classes = {"Nizka", "Stredni", "Velka"}

objects = [
    {
        "id": 1,
        "classes": "Nizka",
        "teplota": "stredni",
        "den": "pracovni",
        "obdobi": "jaro",
        "vhkost": "mala",
    },
    {
        "id": 2,
        "classes": "Nizka",
        "teplota": "vysoka",
        "den": "svatek",
        "obdobi": "leto",
        "vhkost": "mala",
    },
    {
        "id": 3,
        "classes": "Stredni",
        "teplota": "nizka",
        "den": "vikend",
        "obdobi": "zima",
        "vhkost": "mala",
    },
    {
        "id": 4,
        "classes": "Stredni",
        "teplota": "stredni",
        "den": "pracovni",
        "obdobi": "podzim",
        "vhkost": "velka",
    },
    {
        "id": 5,
        "classes": "Nizka",
        "teplota": "stredni",
        "den": "pracovni",
        "obdobi": "zima",
        "vhkost": "mala",
    },
    {
        "id": 6,
        "classes": "Velka",
        "teplota": "nizka",
        "den": "svatek",
        "obdobi": "zima",
        "vhkost": "mala",
    },
    {
        "id": 7,
        "classes": "Velka",
        "teplota": "stredni",
        "den": "svatek",
        "obdobi": "leto",
        "vhkost": "velka",
    },
    {
        "id": 8,
        "classes": "Nizka",
        "teplota": "nizka",
        "den": "svatek",
        "obdobi": "leto",
        "vhkost": "velka",
    },
    {
        "id": 9,
        "classes": "Nizka",
        "teplota": "nizka",
        "den": "vikend",
        "obdobi": "zima",
        "vhkost": "velka",
    },
    {
        "id": 10,
        "classes": "Nizka",
        "teplota": "stredni",
        "den": "pracovni",
        "obdobi": "leto",
        "vhkost": "mala",
    },
    {
        "id": 11,
        "classes": "Stredni",
        "teplota": "vysoka",
        "den": "vikend",
        "obdobi": "leto",
        "vhkost": "velka",
    },
    {
        "id": 12,
        "classes": "Nizka",
        "teplota": "stredni",
        "den": "pracovni",
        "obdobi": "leto",
        "vhkost": "velka",
    },
    {
        "id": 13,
        "classes": "Nizka",
        "teplota": "nizka",
        "den": "vikend",
        "obdobi": "jaro",
        "vhkost": "mala",
    },
    {
        "id": 14,
        "classes": "Nizka",
        "teplota": "nizka",
        "den": "pracovni",
        "obdobi": "zima",
        "vhkost": "velka",
    },
    {
        "id": 15,
        "classes": "Velka",
        "teplota": "vysoka",
        "den": "svatek",
        "obdobi": "zima",
        "vhkost": "mala",
    },
]


def entropy(objects, attribute, value=None):
    entropy = 0

    num_classes = [0, 0, 0]
    for row in objects:
        if value is not None and row[attribute] != value:
            continue
        if row["classes"] == "Nizka":
            num_classes[0] += 1
        elif row["classes"] == "Stredni":
            num_classes[1] += 1
        else:
            num_classes[2] += 1

    print(num_classes)
    print(sum(num_classes))

    for _class in num_classes:
        entropy -= (_class / sum(num_classes)) * math.log2(_class / sum(num_classes))

    return entropy


print(entropy(objects, "teplota", "nizka"))
print("---------")
print(entropy(objects, "teplota", "stredni"))
print("---------")
print(entropy(objects, "teplota", "vysoka"))
print("---------")
print(entropy(objects, "den"))

e1 = entropy(objects, "teplota", "nizka")
e2 = entropy(objects, "teplota", "stredni")
e3 = entropy(objects, "teplota", "vysoka")
es = entropy(objects, "den")

# info_gain
e = es - 6 / 15 * e1 - 6 / 15 * e2 - 3 / 15 * e3
print(e)
