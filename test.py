data = "44; 58; 52; 50; 64; 53; 62; 48; 53; 53; 60; 63; 60; 53; 61; 53; 50; 68; 60; 49"

# remove "; " and turn data into arr of int
data = data.replace("; ", " ").split(" ")

str_hodnota = (1 / len(data)) * sum([int(x) for x in data])
rozptyl = (1 / (len(data) - 1)) * sum([(int(x) - str_hodnota) ** 2 for x in data])

print("Str. hodnota: " + str(str_hodnota))
print("Rozptyl: " + str(rozptyl))
print("Smerodatna odchylka: " + str(rozptyl**0.5))
