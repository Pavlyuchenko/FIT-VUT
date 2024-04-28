import os

for i in range(50):
    # run c script
    os.system("./proj2 9 5 10 1000 1000")

    # load proj2.out file
    f = open("proj2.out", "r")

    # read the file line by line
    lines = f.readlines()

    curr = 1
    for line in lines:
        # get first number
        num = line.split(":")[0].strip()

        # check if the numbers go ascending
        if int(num) != curr:
            print("Error: Expected", curr, "but got", num)
            exit(1)

        curr += 1

    print("Test", i, "passed")
