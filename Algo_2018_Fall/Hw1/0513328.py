
# Global variables
TableCount = 0


def read_file():
    with open("input.txt", "r") as input:
        TableCount = input.readline()
        print(TableCount)

def print_output():
    with open("output.txt", "r") as output:
        output.write(result)


def main():
    """
        Run main function, and print result to output.txt
    """
    read_file()

if __name__ == "__main__":
    main()

