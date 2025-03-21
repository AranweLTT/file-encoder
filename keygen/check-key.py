# Libraries
import numpy as np


# Defenitions
def hex_to_matrix(hex_key: str):
    """Converts a 3x3 hex string into a NumPy 3x3 matrix."""
    assert len(hex_key) == 18, "Hex key must be 18 characters (9 bytes)."

    # Convert hex string to an array of integers
    key_bytes = [int(hex_key[i : i + 2], 16) for i in range(0, 18, 2)]

    # Reshape into 3x3 matrix
    return np.array(key_bytes, dtype=int).reshape(3, 3)


def mod_inverse(A: int, M: int) -> int:
    for X in range(1, M):
        if ((A % M) * (X % M)) % M == 1:
            return X
    return -1


def print_matrix(matrix, label: str):
    """Prints a matrix with a label."""
    print(f"\n{label}:")
    for row in matrix:
        print(" ".join(f"{val:3}" for val in row))


# ** Main Execution **
if __name__ == "__main__":
    # Hex key pair to test
    private_key_hex = "06059b65948ad5cb20"
    public_key_hex = "deafbabe2f7d352149"

    # Convert hex keys to matrices
    private_matrix = hex_to_matrix(private_key_hex)
    public_matrix = hex_to_matrix(public_key_hex)

    # Compute modular inverses
    det = np.round(np.linalg.det(private_matrix)).astype(int)
    alpha = mod_inverse(det, 256)
    private_inv = (
        np.round(np.linalg.inv(private_matrix) * det * alpha).astype(int) % 256
    )

    det = np.round(np.linalg.det(private_matrix)).astype(int)
    alpha = mod_inverse(det, 256)
    public_inv = np.round(np.linalg.inv(public_matrix) * det * alpha).astype(int) % 256

    # Print results
    print_matrix(private_matrix, "Private Key Matrix")
    print_matrix(public_matrix, "Public Key Matrix")
    print_matrix(private_inv, "Inverse of Private Key Matrix")
    print_matrix(public_inv, "Inverse of Public Key Matrix")

    print_matrix(
        (np.matmul(private_matrix, public_matrix)).astype(int) % 256, "private x public"
    )
    print_matrix(
        (np.matmul(public_matrix, private_matrix)).astype(int) % 256, "public x private"
    )
    print_matrix(
        (np.matmul(private_matrix, private_inv)).astype(int) % 256,
        "private x private_inv",
    )
    print_matrix(
        (np.matmul(public_matrix, public_inv)).astype(int) % 256, "public x public_inv"
    )
