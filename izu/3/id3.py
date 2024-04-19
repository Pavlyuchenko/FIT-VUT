import math

attributes = {
    "teplota": ["nizka", "stredni", "vysoka"],
    "den": ["pracovni", "vikend", "svatek"],
    "obdobi": ["jaro", "leto", "podzim", "zima"],
    "vkhost": ["mala", "velka"],
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

    if value is not None:
        objects = [obj for obj in objects if obj[attribute] == value]

    for c in classes:
        p = len([obj for obj in objects if obj["classes"] == c]) / len(objects)
        if p != 0:
            entropy -= p * math.log2(p)

    return entropy, len(objects)


total_entropy = 1.3709505944546687

e_t1, c_t1 = entropy(objects, "teplota", "nizka")
e_t2, c_t2 = entropy(objects, "teplota", "stredni")
e_t3, c_t3 = entropy(objects, "teplota", "vysoka")

e_d1, c_d1 = entropy(objects, "den", "pracovni")
e_d2, c_d2 = entropy(objects, "den", "vikend")
e_d3, c_d3 = entropy(objects, "den", "svatek")

e_o1, c_o1 = entropy(objects, "obdobi", "jaro")
e_o2, c_o2 = entropy(objects, "obdobi", "leto")
e_o3, c_o3 = entropy(objects, "obdobi", "podzim")
e_o4, c_o4 = entropy(objects, "obdobi", "zima")

e_v1, c_v1 = entropy(objects, "vhkost", "mala")
e_v2, c_v2 = entropy(objects, "vhkost", "velka")


print()
# info_gain
e = total_entropy - c_t1 / 15 * e_t1 - c_t2 / 15 * e_t2 - c_t3 / 15 * e_t3
print("I_G(teplota) = " + str(e))

e = total_entropy - c_d1 / 15 * e_d1 - c_d2 / 15 * e_d2 - c_d3 / 15 * e_d3
print("I_G(den) = " + str(e))

e = (
    total_entropy
    - c_o1 / 15 * e_o1
    - c_o2 / 15 * e_o2
    - c_o3 / 15 * e_o3
    - c_o4 / 15 * e_o4
)
print("I_G(obdobi) = " + str(e))

e = total_entropy - c_v1 / 15 * e_v1 - c_v2 / 15 * e_v2
print("I_G(vhkost) = " + str(e))

# I_G(den) has the highest information gain => split by den
