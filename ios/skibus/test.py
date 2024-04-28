import os

script_name = "./proj2"
script_skiers = 100
script_bus_stops = 8
script_bus_capacity = 10
script_time_skier = 1000
script_time_bus = 1000
script_args = (
    str(script_skiers)
    + " "
    + str(script_bus_stops)
    + " "
    + str(script_bus_capacity)
    + " "
    + str(script_time_skier)
    + " "
    + str(script_time_bus)
)


def run_and_open_file():
    """Helper function to run the script and open the file"""

    # run c script
    os.system(script_name + " " + script_args)

    # load proj2.out file
    f = open("proj2.out", "r")
    lines = f.readlines()

    # close file
    f.close()

    return lines


def test_log_numbering():
    """Test if the log numbering is correct -> goes from 1 to whatever"""

    lines = run_and_open_file()

    curr = 1
    for line in lines:
        # get first number
        num = line.split(":")[0].strip()

        # check if the numbers go ascending
        if int(num) != curr:
            print("Error: Expected", curr, "but got", num)
            return False

        curr += 1

    return True


def test_all_passengers_boarded():
    """Test if all passengers have boarded the bus
    -> if the number of boarding lines is equal to the number of skiers"""

    lines = run_and_open_file()

    count_boarded = 0
    for line in lines:
        if "boarding" in line:
            count_boarded += 1

    return count_boarded == script_skiers


def test_file_end():
    """Test if the last two line of the file are correct"""

    lines = run_and_open_file()

    # get last line
    last_line = lines[-1].strip()

    if "BUS: finish" not in last_line:
        return False

    # get second last line
    second_last_line = lines[-2].strip()

    if "BUS: leaving final" not in second_last_line:
        return False

    return True


def test_boarding_at_stop():
    """Tests that boarding is between BUS: arrived and BUS: leaving"""

    lines = run_and_open_file()

    for line in lines:
        if "BUS: arrived" in line:
            arrived = True
        if "BUS: leaving" in line:
            arrived = False
        if "boarding" in line and not arrived:
            return False

    return True


def test_going_to_ski_final():
    """Ensure going to ski is between BUS: arrived to final and BUS: leaving final"""

    lines = run_and_open_file()

    for line in lines:
        if "BUS: arrived to final" in line:
            arrived = True
        if "BUS: leaving final" in line:
            arrived = False
        if "going to ski" in line and not arrived:
            return False

    return True


def main():
    iterations_count = 50

    print("=====================================")
    print(
        "Running test 1: "
        + str(iterations_count)
        + " iterations of log numbering test..."
    )
    test_1_passed = True
    for i in range(iterations_count):
        print(i, end=" ")
        test_1_passed = test_1_passed and test_log_numbering()

    print()
    if not test_1_passed:
        print("ERROR: Test log_numbering failed")
    else:
        print("✅ SUCCESS: Test log_numbering passed")

    print()
    print("=====================================")

    print("Running test 2: all_passengers_boarded")
    test_2_passed = True
    for i in range(iterations_count):
        print(i, end=" ")
        test_2_passed = test_2_passed and test_all_passengers_boarded()

    print()
    if not test_2_passed:
        print("ERROR: Test all_passengers_boarded failed")
    else:
        print("✅ SUCCESS: Test all_passengers_boarded passed")

    print()
    print("=====================================")

    print("Running test 3: test_file_end")
    test_3_passed = True
    for i in range(iterations_count):
        print(i, end=" ")
        test_3_passed = test_3_passed and test_file_end()

    print()
    if not test_3_passed:
        print("ERROR: Test test_file_end failed")
    else:
        print("✅ SUCCESS: Test test_file_end passed")

    print()
    print("=====================================")

    print("Running test 4: test_boarding_at_stop")
    test_4_passed = True
    for i in range(iterations_count):
        print(i, end=" ")
        test_4_passed = test_4_passed and test_boarding_at_stop()

    print()
    if not test_4_passed:
        print("ERROR: Test test_boarding_at_stop failed")
    else:
        print("✅ SUCCESS: Test test_boarding_at_stop passed")

    print()
    print("=====================================")

    print("Running test 5: test_going_to_ski_final")
    test_5_passed = True
    for i in range(iterations_count):
        print(i, end=" ")
        test_5_passed = test_5_passed and test_going_to_ski_final()

    print()
    if not test_5_passed:
        print("ERROR: Test test_going_to_ski_final failed")
    else:
        print("✅ SUCCESS: Test test_going_to_ski_final passed")


if __name__ == "__main__":
    main()
