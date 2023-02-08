import numpy as np
from sys import argv


def main():
    if len(argv) != 2:
        print("Give filename as argument.")
        exit(0)

    filename = argv[1]

    img = np.load(filename)

    img.tofile(filename.replace(".npy", ".csv"), sep=";", format="%10.5f")


if __name__ == "__main__":
    main()
