import numpy as np
import sys
import os

# ----------------------
# GF(2) helpers (8x8)
# ----------------------
def gf2_inv(A: np.ndarray) -> np.ndarray:
    A = A.copy().astype(np.uint8)
    n = 8
    Iy = np.eye(n, dtype=np.uint8)
    aug = np.concatenate([A, Iy], axis=1)
    row = 0
    for col in range(n):
        pivot = None
        for r in range(row, n):
            if aug[r, col] == 1:
                pivot = r
                break
        if pivot is None:
            raise ValueError("Matrix not invertible over GF(2)")
        if pivot != row:
            aug[[pivot, row]] = aug[[row, pivot]]
        for r in range(n):
            if r != row and aug[r, col] == 1:
                aug[r] ^= aug[row]
        row += 1
    return aug[:, n:].astype(np.uint8)

def is_invertible(A: np.ndarray) -> bool:
    try:
        _ = gf2_inv(A)
        return True
    except ValueError:
        return False

def hex_to_mat(h: str) -> np.ndarray:
    if len(h) != 16:
        raise ValueError("public hex must be 16 hex chars (8 bytes)")
    arr = []
    for i in range(0, 16, 2):
        byte = int(h[i:i+2], 16)
        arr.append([(byte >> shift) & 1 for shift in reversed(range(8))])
    return np.array(arr, dtype=np.uint8)

# ----------------------
# Fixed generator matrix
# ----------------------
def fixed_G():
    G = np.array([
        [1,0,1,0,0,1,0,1],
        [0,1,1,0,1,0,1,0],
        [1,1,0,1,0,0,1,0],
        [0,0,1,1,1,0,0,1],
        [1,0,0,1,0,0,1,0],
        [0,1,0,0,1,1,0,1],
        [1,0,1,0,1,0,1,0],
        [0,1,0,1,0,1,0,1],
    ], dtype=np.uint8)
    if not is_invertible(G):
        raise RuntimeError("Chosen G is not invertible")
    return G, gf2_inv(G)

G, G_inv = fixed_G()

# ----------------------
# Encrypt / Decrypt
# ----------------------
def encrypt_chunk(pubhex: str, chunk: bytes) -> bytes:
    """Encrypt a chunk of bytes using public key matrix."""
    P = hex_to_mat(pubhex)  # 8x8
    # Convert bytes -> bits (shape: nbytes x 8), then transpose to shape 8 x nbytes
    bits = np.unpackbits(np.frombuffer(chunk, dtype=np.uint8)).reshape(-1, 8).T
    # Matrix multiply over GF(2)
    enc_bits = (P @ bits) % 2
    # Transpose back, pack bits -> bytes
    return np.packbits(enc_bits.T, axis=1).tobytes()

def decrypt_chunk(privhex: str, chunk: bytes) -> bytes:
    """Decrypt a chunk of bytes using private key matrix."""
    S = hex_to_mat(privhex)
    S_inv = gf2_inv(S)
    # Convert bytes -> bits
    bits = np.unpackbits(np.frombuffer(chunk, dtype=np.uint8)).reshape(-1, 8).T
    # Decrypt in one go: p = S @ (G_inv @ (S_inv @ c))
    y = (S_inv @ bits) % 2
    u = (G_inv @ y) % 2
    p = (S @ u) % 2
    return np.packbits(p.T, axis=1).tobytes()

def encrypt_file(infile: str, outfile: str, pubhex: str):
    with open(infile, "rb") as f_in, open(outfile, "wb") as f_out:
        while True:
            chunk = f_in.read(4096)
            if not chunk:
                break
            f_out.write(encrypt_chunk(pubhex, chunk))

def decrypt_file(infile: str, outfile: str, privhex: str):
    with open(infile, "rb") as f_in, open(outfile, "wb") as f_out:
        while True:
            chunk = f_in.read(4096)
            if not chunk:
                break
            f_out.write(decrypt_chunk(privhex, chunk))

# ----------------------
# Main CLI
# ----------------------
if __name__ == "__main__":
    if len(sys.argv) < 5:
        print(f"Usage: {sys.argv[0]} encrypt|decrypt <input> <output> <pub_or_priv_hex>")
        sys.exit(1)

    mode, infile, outfile, keyhex = sys.argv[1:5]

    if not os.path.exists(infile):
        print(f"Error: Input file '{infile}' not found.")
        sys.exit(1)

    if mode == "encrypt":
        encrypt_file(infile, outfile, keyhex)
        print(f"File encrypted to {outfile}")
    elif mode == "decrypt":
        decrypt_file(infile, outfile, keyhex)
        print(f"File decrypted to {outfile}")
    else:
        print("Mode must be 'encrypt' or 'decrypt'.")
        sys.exit(1)
