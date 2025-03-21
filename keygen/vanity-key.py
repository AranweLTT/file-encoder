# Libraries
import sys
import numpy as np


# Defenitions
def mod_inverse(A: int, M: int) -> int:
    for X in range(1, M):
        if ((A % M) * (X % M)) % M == 1:
            return X
    return -1


def find_key(key_a):
    assert key_a.shape == (3, 3), "key should be [3,3] shape"
    # det = (key_a[0][0] * key_a[1][1]) - (key_a[1][0] * key_a[0][1])
    det = np.round(np.linalg.det(key_a)).astype(int)
    alpha = mod_inverse(det, 256)

    # Compute key
    try:
        key_b = np.round(np.linalg.inv(key_a) * det * alpha).astype(int) % 256
    except Exception:
        key_b = np.zeros((3, 3))
    res = (np.matmul(key_a, key_b)).astype(int) % 256
    if (res == np.identity(3)).all:
        det = np.round(np.linalg.det(key_b)).astype(int)
        alpha = mod_inverse(det, 256)
        if alpha == -1:
            return key_b
    return -1


def keytohex(key):
    hexkey = ""
    for element in np.nditer(key):
        hexkey += f"{int(element):02x}"
    return hexkey


def generate_key_pair():
    while True:
        key_a = np.random.randint(1, 256, size=(3, 3))
        key_b = find_key(key_a)
        if not np.isscalar(key_b):
            return key_a, key_b


def find_vanity_key_pair(substring):
    while True:
        private_key, public_key = generate_key_pair()
        public_key_hex = keytohex(public_key)
        if public_key_hex.startswith(substring):
            return private_key, public_key


# Main
if __name__ == "__main__":
    marker = sys.argv[1]
    # Generate keys
    print(f"Searching for public key starting with {marker}...")
    # np.random.seed(0)
    key_a, key_b = find_vanity_key_pair(marker)

    # Test keys
    print("Testing key pair...")
    data = np.random.randint(0, 256, size=(3))
    encrypted = (np.matmul(data, key_a)).astype(int) % 256
    decrypted = (np.matmul(encrypted, key_b)).astype(int) % 256

    # Display results
    if (encrypted == decrypted).all:
        print("Keys valid")
        print(f"Private key: {keytohex(key_a)}")
        print(f"Public key: {keytohex(key_b)}")
    else:
        print("Keys not valid, please retry")
